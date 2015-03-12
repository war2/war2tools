#include "war2edit.h"

#define EDITOR_DATA_KEY "war2/editor"


/*============================================================================*
 *                                 Private API                                *
 *============================================================================*/

static Grid_Cell **
_grid_cells_new(Editor *ed)
{
   Grid_Cell **ptr;
   int i;

   /* Iliffe vector allocation */

   ptr = malloc(ed->map_h * sizeof(*ptr));
   EINA_SAFETY_ON_NULL_RETURN_VAL(ptr, NULL);

   ptr[0] = calloc(ed->map_w * ed->map_h, sizeof(**ptr));
   EINA_SAFETY_ON_NULL_GOTO(ptr[0], fail);

   for (i = 1; i < ed->map_h; ++i)
     ptr[i] = ptr[i - 1] + ed->map_w;

   return ptr;

fail:
   free(ptr);
   return NULL;
}

static inline Editor *
_editor_get(Evas_Object *glv)
{
   return evas_object_data_get(glv, EDITOR_DATA_KEY);
}

static GLuint
_shader_load(Evas_GL_API  *api,
             GLenum        type,
             const char *code)
{
   GLuint shader;
   GLint status;

   shader = api->glCreateShader(type);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(shader == 0, 0);

   api->glShaderSource(shader, 1, &code, NULL);
   api->glCompileShader(shader);
   api->glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

   /* Failed to compile */
   if (EINA_UNLIKELY(status == 0))
     {
        CRI("Failed to compile shader:\n%s", code);
        api->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &status);
        if (EINA_LIKELY(status > 1))
          {
             char *log;

             log = malloc(status * sizeof(*log));
             EINA_SAFETY_ON_NULL_RETURN_VAL(log, 0);
             api->glGetShaderInfoLog(shader, status, NULL, log);
             CRI("Shader Error: %s", log);
             free(log);
          }
        api->glDeleteShader(shader);
        return 0;
     }

   return shader;
}

static inline GLint
_prog_attrib_get(Editor     *ed,
                 const char *var)
{
    return ed->gl.api->glGetAttribLocation(ed->gl.prog, var);
}

/*============================================================================*
 *                              OpenGL Callbacks                              *
 *============================================================================*/

static void
_init_gl(Evas_Object *glv)
{
   Editor *ed;
   Evas_GL_API *api;
   GLint status;
   int i, j, x, y;

   /*
    * Use version 120 (OpenGL 2.1, GLSL 1.20) because dear OSX (with X11)
    * aka GLX, does not support more recent...
    * OSX with native openGL (CGL) is fine though.
    *
    * Thanks a lot to:
    * http://www.morethantechnical.com/2013/11/09/
    *         vertex-array-objects-with-shaders-on-opengl-2-1-glsl-1-2-wcode/
    */

   const char vshader_code[] =
      "#version 120\n"
      "\n"
      "attribute vec3 vert;\n"
      "attribute vec2 vertTexCoord;\n"
      "\n"
      "void main()\n"
      "{\n"
      "   gl_Position = vec4(vert, 1);\n"
      "   gl_TexCoord[0].st = vertTexCoord;\n"
      "}\n";

   const char fshader_code[] =
      "#version 120\n"
      "\n"
      "#ifdef GL_ES\n"
      "precision mediump float;\n"
      "#endif\n"
      "\n"
      "uniform sampler2D tex;\n"
      "\n"
      "void main()\n"
      "{\n"
      "   gl_FragColor = texture2D(tex, gl_TexCoord[0].st);\n"
      "}\n";

   ed = _editor_get(glv);
   EINA_SAFETY_ON_NULL_RETURN(ed);

   api = ed->gl.api;


   ed->gl.vertices_count = 3;
   ed->gl.vertices_size = ed->gl.vertices_count * 5 * sizeof(GLfloat);
   ed->gl.vertices = malloc(ed->gl.vertices_size);
   EINA_SAFETY_ON_NULL_RETURN(ed->gl.vertices);

   ed->gl.vertices[ 0] = -0.5f;  // x
   ed->gl.vertices[ 1] = -0.5f;  // y
   ed->gl.vertices[ 2] =  0.0f;  // z
   ed->gl.vertices[ 3] =  0.0f;  // u
   ed->gl.vertices[ 4] =  1.0f;  // v

   ed->gl.vertices[ 5] =  0.5f;  // x
   ed->gl.vertices[ 6] = -0.5f;  // y
   ed->gl.vertices[ 7] =  0.0f;  // z
   ed->gl.vertices[ 8] =  1.0f;  // u
   ed->gl.vertices[ 9] =  1.0f;  // v

   ed->gl.vertices[10] =  0.0f;  // x
   ed->gl.vertices[11] =  0.5f;  // y
   ed->gl.vertices[12] =  0.0f;  // z
   ed->gl.vertices[13] =  0.5f;  // u
   ed->gl.vertices[14] =  0.0f;  // v

#if 0
   /* 6 vertices (1 vertex = 3 GLfloats) per tile (= 2 triangles) (count = map_w * map_h) */
   ed->gl.vertices_count = (ed->map_w * ed->map_h) * 6;
   ed->gl.vertices_size = ed->gl.vertices_count * 3 * sizeof(GLfloat);
   ed->gl.vertices = malloc(ed->gl.vertices_size);
   EINA_SAFETY_ON_NULL_RETURN(ed->gl.vertices);

   /* Fraction of screen per tile (map_w == map_h) */
   const GLfloat step = 1.0f / (GLfloat)(ed->map_w);

   /*      ^
    *      |
    * <----o---->
    *      |XXXXX
    *      vXXXXX
    *  ~~~~~~~~^
    *  Area used
    */
   for (i = 0; i < ed->map_h; ++i)
     {
        y = i * ed->map_w * 3 * 6;
        const GLfloat yf = (GLfloat)i;

        for (j = 0, x = 0; x < ed->map_w; j += (3 * 6), ++x)
          {
             const GLfloat xf = (GLfloat)x;

             /* Top-left vertex (x,y,z) */
             ed->gl.vertices[j + y +  0] = xf * step;
             ed->gl.vertices[j + y +  1] = -1.0f * yf * step;
             ed->gl.vertices[j + y +  2] = 0.0f;
             /* Top-right vertex (x,y,z) */
             ed->gl.vertices[j + y +  3] = (xf + 1.0f) * step;
             ed->gl.vertices[j + y +  4] = -1.0f * yf * step;
             ed->gl.vertices[j + y +  5] = 0.0f;
             /* Bottom-left vertex (x,y,z) */
             ed->gl.vertices[j + y +  6] = xf * step;
             ed->gl.vertices[j + y +  7] = -1.0f * (yf + 1.0f) * step;
             ed->gl.vertices[j + y +  8] = 0.0f;

             /* Top-right vertex (x,y,z) */
             ed->gl.vertices[j + y +  9] = (xf + 1.0f) * step;
             ed->gl.vertices[j + y + 10] = -1.0f * yf * step;
             ed->gl.vertices[j + y + 11] = 0.0f;
             /* Bottom-right vertex (x,y,z) */
             ed->gl.vertices[j + y + 12] = (xf + 1.0f) * step;
             ed->gl.vertices[j + y + 13] = -1.0f * (yf + 1.0f) * step;
             ed->gl.vertices[j + y + 14] = 0.0f;
             /* Bottom-left vertex (x,y,z) */
             ed->gl.vertices[j + y + 15] = xf * step;
             ed->gl.vertices[j + y + 16] = -1.0f * (yf + 1.0f) * step;
             ed->gl.vertices[j + y + 17] = 0.0f;
          }
     }
#endif

   /* Enable 2D texturing */
   api->glEnable(GL_TEXTURE_2D);
   api->glEnable(GL_BLEND);

   /* Shaders */
   ed->gl.vshader = _shader_load(api, GL_VERTEX_SHADER, vshader_code);
   ed->gl.fshader = _shader_load(api, GL_FRAGMENT_SHADER, fshader_code);
   EINA_SAFETY_ON_TRUE_RETURN((ed->gl.vshader == 0) || (ed->gl.fshader == 0));
   ed->gl.prog = api->glCreateProgram();
   EINA_SAFETY_ON_TRUE_RETURN(ed->gl.prog == 0);
   api->glAttachShader(ed->gl.prog, ed->gl.vshader);
   api->glAttachShader(ed->gl.prog, ed->gl.fshader);

   /* Get handle for the 2D sampler */
   // ed->gl.tid = api->glGetUniformLocation(ed->gl.prog, "s_2D");

   /* Link program */
   api->glLinkProgram(ed->gl.prog);
   api->glGetProgramiv(ed->gl.prog, GL_LINK_STATUS, &status);
   if (EINA_UNLIKELY(status == 0)) /* Not linked */
     {
        api->glGetProgramiv(ed->gl.prog, GL_INFO_LOG_LENGTH, &status);
        if (EINA_LIKELY(status > 1))
          {
             char *log;

             log = malloc(status * sizeof(*log));
             EINA_SAFETY_ON_NULL_RETURN(log);
             api->glGetProgramInfoLog(ed->gl.prog, status, NULL, log);
             CRI("Linking Error: %s", log);
             free(log);
          }
        else
          CRI("Failed to link program (unkown error)");
        api->glDeleteProgram(ed->gl.prog);
        EDITOR_ERROR_RET(ed, "OpenGL initialization failed: shader linking error");
     }

   api->glGenBuffers(1, &(ed->gl.vbo));
   api->glBindBuffer(GL_ARRAY_BUFFER, ed->gl.vbo);
   api->glBufferData(GL_ARRAY_BUFFER, ed->gl.vertices_size, ed->gl.vertices, GL_STATIC_DRAW);

   status = _prog_attrib_get(ed, "vert");
   api->glEnableVertexAttribArray(status);
   api->glVertexAttribPointer(status, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);

   status = _prog_attrib_get(ed, "vertTexCoord");
   api->glEnableVertexAttribArray(status);
   api->glVertexAttribPointer(status, 3, GL_FLOAT, GL_FALSE,
                              5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));

   ed->gl.tid = texture_load(api, "122");
   ed->gl.init_done = EINA_TRUE;
}

static void
_del_gl(Evas_Object *glv)
{
   Editor *ed;
   Evas_GL_API *api;

   ed = _editor_get(glv);
   EINA_SAFETY_ON_NULL_RETURN(ed);
   if (ed->gl.init_done == EINA_FALSE) return;

   api = ed->gl.api;

   api->glDeleteBuffers(1, &(ed->gl.vbo));
   api->glDeleteShader(ed->gl.vshader);
   api->glDeleteShader(ed->gl.fshader);
   api->glDeleteProgram(ed->gl.prog);

   free(ed->gl.vertices);
   evas_object_data_del(glv, EDITOR_DATA_KEY);
}

static void
_resize_gl(Evas_Object *glv EINA_UNUSED)
{
}

static void
_render_gl(Evas_Object *glv)
{
   DBG("Rendering....");
   Editor *ed;
   Evas_GL_API *api;
   int w, h;

   ed = _editor_get(glv);
   EINA_SAFETY_ON_NULL_RETURN(ed);
   api = ed->gl.api;

   elm_glview_size_get(glv, &w, &h);

   api->glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
   api->glClear(GL_COLOR_BUFFER_BIT);

   api->glUseProgram(ed->gl.prog);
   api->glActiveTexture(GL_TEXTURE0);
   api->glBindTexture(GL_TEXTURE_2D, ed->gl.tid);

   api->glBindBuffer(GL_ARRAY_BUFFER, ed->gl.vbo);
   api->glDrawArrays(GL_TRIANGLES, 0, ed->gl.vertices_count);

   api->glUseProgram(0);

   api->glFinish();
}


/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

Eina_Bool
grid_add(Editor *ed)
{
   Evas_Object *glv;

   glv = elm_glview_add(ed->win);
   EINA_SAFETY_ON_NULL_RETURN_VAL(glv, EINA_FALSE);

   evas_object_size_hint_align_set(glv, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(glv, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   elm_glview_mode_set(glv, ELM_GLVIEW_ALPHA | ELM_GLVIEW_DEPTH);
   elm_glview_resize_policy_set(glv, ELM_GLVIEW_RESIZE_POLICY_RECREATE);
   elm_glview_render_policy_set(glv, ELM_GLVIEW_RENDER_POLICY_ON_DEMAND);
   elm_glview_init_func_set(glv, _init_gl);
   elm_glview_del_func_set(glv, _del_gl);
   elm_glview_resize_func_set(glv, _resize_gl);
   elm_glview_render_func_set(glv, _render_gl);

   evas_object_data_set(glv, EDITOR_DATA_KEY, ed);

   ed->cells = _grid_cells_new(ed);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ed->cells, EINA_FALSE);

   ed->glview = glv;
   ed->gl.api = elm_glview_gl_api_get(glv);

   return EINA_TRUE;
}

