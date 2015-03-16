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

static inline GLint
_prog_uniform_get(Editor     *ed,
                  const char *var)
{
   return ed->gl.api->glGetUniformLocation(ed->gl.prog, var);
}

static void
_translate(Editor  *ed,
           GLfloat  x,
           GLfloat  y,
           GLfloat  z)
{
   const GLfloat mat[16] = {
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 1.0f,
         x,    y,    z, 1.0f
   };
   ed->gl.api->glUniformMatrix4fv(ed->gl.translation_mtx, 1, GL_FALSE, mat);
}

static void
_scale(Editor  *ed,
       GLfloat  x,
       GLfloat  y,
       GLfloat  z)
{
   const GLfloat mat[16] = {
         x, 0.0f, 0.0f, 0.0f,
      0.0f,    y, 0.0f, 0.0f,
      0.0f, 0.0f,    z, 1.0f,
      0.0f, 0.0f, 0.0f, 1.0f
   };
   ed->gl.api->glUniformMatrix4fv(ed->gl.scaling_mtx, 1, GL_FALSE, mat);
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
   GLfloat *v;

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
      "uniform mat4 translation_mtx;\n"
      "uniform mat4 scaling_mtx;\n"
      "\n"
      "void main()\n"
      "{\n"
      "   mat4 transform = scaling_mtx * translation_mtx;\n"
      "   vec4 pos = vec4(vert, 1);\n"
      "   gl_Position = transform * pos;\n"
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

   /* 6 vertices (1 vertex = 3 GLfloats) per tile (= 2 triangles) (count = map_w * map_h) */
   ed->gl.vertices_count = (ed->map_w * ed->map_h) * 6;
   ed->gl.vertices_size = ed->gl.vertices_count * 5 * sizeof(GLfloat);
   ed->gl.vertices = malloc(ed->gl.vertices_size);
   EINA_SAFETY_ON_NULL_RETURN(ed->gl.vertices);
   v = ed->gl.vertices; /* Shortcut + saves a lot of indirections */

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
        y = i * ed->map_w * 5 * 6;
        const GLfloat yf = (GLfloat)i;

        for (j = 0, x = 0; x < ed->map_w; j += (5 * 6), ++x)
          {
             const GLfloat xf = (GLfloat)x;

             /* Top-left vertex (x,y,z,u,v) */
             v[j + y +  0] = xf * step;                 // X
             v[j + y +  1] = -1.0f * yf * step;         // Y
             v[j + y +  2] = 0.0f;                      // Z
             v[j + y +  3] = 0.0f;                      // U
             v[j + y +  4] = 1.0f;                      // V
             /* Top-right vertex (x,y,z,u,v) */
             v[j + y +  5] = (xf + 1.0f) * step;        // X
             v[j + y +  6] = -1.0f * yf * step;         // Y
             v[j + y +  7] = 0.0f;                      // Z
             v[j + y +  8] = 1.0f;                      // U
             v[j + y +  9] = 1.0f;                      // V
             /* Bottom-left vertex (x,y,z,u,v) */
             v[j + y + 10] = xf * step;                 // X
             v[j + y + 11] = -1.0f * (yf + 1.0f) * step;// Y
             v[j + y + 12] = 0.0f;                      // Z
             v[j + y + 13] = 0.0f;                      // U
             v[j + y + 14] = 0.0f;                      // V

             /* Top-right vertex (x,y,z,u,v) */
             v[j + y + 15] = (xf + 1.0f) * step;        // X
             v[j + y + 16] = -1.0f * yf * step;         // Y
             v[j + y + 17] = 0.0f;                      // Z
             v[j + y + 18] = 1.0f;                      // U
             v[j + y + 19] = 1.0f;                      // V
             /* Bottom-right vertex (x,y,z,u,v) */
             v[j + y + 20] = (xf + 1.0f) * step;        // X
             v[j + y + 21] = -1.0f * (yf + 1.0f) * step;// Y
             v[j + y + 22] = 0.0f;                      // Z
             v[j + y + 23] = 0.0f;                      // U
             v[j + y + 24] = 1.0f;                      // V
             /* Bottom-left vertex (x,y,z,u,v) */
             v[j + y + 25] = xf * step;                 // X
             v[j + y + 26] = -1.0f * (yf + 1.0f) * step;// Y
             v[j + y + 27] = 0.0f;                      // Z
             v[j + y + 28] = 0.0f;                      // U
             v[j + y + 29] = 0.0f;                      // V
          }
     }

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

   ed->gl.translation_mtx = _prog_uniform_get(ed, "translation_mtx");
   ed->gl.scaling_mtx = _prog_uniform_get(ed, "scaling_mtx");

   ed->gl.x = -0.5f;
   ed->gl.y =  0.5f;
   ed->gl.z = -0.5f;

   api->glActiveTexture(GL_TEXTURE0);

   ed->gl.init_done = EINA_TRUE;
}

static void
_del_gl(Evas_Object *glv)
{
   Editor *ed;
   Evas_GL_API *api;

   ed = _editor_get(glv);
   EINA_SAFETY_ON_NULL_RETURN(ed);
   if (EINA_UNLIKELY(ed->gl.init_done == EINA_FALSE)) return;

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
//   Editor *ed;
//   int w, h;
//   Evas_GL_API *api;
//
//   ed = _editor_get(glv);
//   if (EINA_UNLIKELY(ed->gl.init_done == EINA_FALSE)) return;
//
//   elm_glview_size_get(glv, &w, &h);
//   api = ed->gl.api;
//   api->glViewport(0, 0, w, h);
}

static void
_render_gl(Evas_Object *glv)
{
   Editor *ed;
   Evas_GL_API *api;
   GLuint tid;
   int w, h;
   int x, y, k = 0;
   GLfloat wf, hf;
   GLfloat scale_x, scale_y;

   ed = _editor_get(glv);
   if (EINA_UNLIKELY(ed->gl.init_done == EINA_FALSE)) return;

   /* Determine zooming factor */
   elm_glview_size_get(glv, &w, &h);
   wf = (GLfloat)w;
   hf = (GLfloat)h;
   if (w < h)
     {
        scale_x = 1.0f;
        scale_y = wf / hf;
     }
   else
     {
        scale_x = hf / wf;
        scale_y = 1.0f;
     }

   api = ed->gl.api;
   api->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   api->glClear(GL_COLOR_BUFFER_BIT);

   api->glUseProgram(ed->gl.prog);

   _translate(ed, ed->gl.x, ed->gl.y, ed->gl.z);
   _scale(ed, scale_x, scale_y, 1.0f);

   api->glBindBuffer(GL_ARRAY_BUFFER, ed->gl.vbo);

   for (y = 0; y < ed->map_h; ++y)
     {
        for (x = 0; x < ed->map_w; ++x)
          {
             tid = texture_tile_access(ed, x, y);
             api->glBindTexture(GL_TEXTURE_2D, tid);
             api->glDrawArrays(GL_TRIANGLES, k, 6);
             k += 6;
          }
     }

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
   int x, y;
   const int min = texture_dictionary_min(ed);
   const int max = texture_dictionary_max(ed);

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

   for (y = 0; y < ed->map_h; ++y)
     {
        for (x = 0; x < ed->map_w; ++x)
          {
             ed->cells[y][x].tile = (rand() % (max - min + 1)) + min;
          }
     }

   ed->glview = glv;
   ed->gl.api = elm_glview_gl_api_get(glv);

   return EINA_TRUE;
}


