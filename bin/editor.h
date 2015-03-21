#ifndef _EDITOR_H_
#define _EDITOR_H_


#define EDITOR_ITEM_HUMAN       (1 << 0)
#define EDITOR_ITEM_ORC         (1 << 1)

struct _Editor
{
   Pud          *pud;
   Evas_Object  *win;
   Evas_Object  *menu;
   Evas_Object  *inwin;
   Evas_Object  *mainbox;
   /*Evas_Object  *scroller;*/
   Grid_Cell   **cells;

   Evas_Object  *glview;

   /* This part is managed by grid.c */
   struct {
      Evas_GL_API *api;
      GLuint       vbo;
      GLuint       vshader;
      GLuint       fshader;
      GLuint       prog;
      GLint        translation_mtx;
      GLint        scaling_mtx;

      GLfloat     *vertices;
      int          vertices_count;  /* How many vertices */
      int          vertices_size;   /* Allocated size */

      GLfloat      x;
      GLfloat      y;
      GLfloat      zoom;

      GLfloat      zoom_step;

      Eina_Bool    init_done;
   } gl;

   struct {
      Texture_Dictionary  dict;
      Eina_Hash          *hash;
   } tex;

   Elm_Object_Item *main_sel[4];
   Elm_Object_Item *hmn_sel[4];
   Elm_Object_Item *orc_sel[4];

   struct _mainconfig {
      Evas_Object *container;
      Evas_Object *img;
      Evas_Object *menu_size;
      Evas_Object *menu_era;
   } mainconfig;

   struct _menu_item {
      Elm_Object_Item *item;
      Evas_Object     *radio;
      unsigned char    active_for : 2;
   } tools_items[110], *tools_item_active;

   Pud_Dimensions size;
   int map_w;
   int map_h;
   Pud_Era era;
   Eina_Bool has_extension;
};

Eina_Bool editor_init(void);
void editor_shutdown(void);
void editor_free(Editor *ed);
void editor_close(Editor *ed);
Editor *editor_new(void);
void editor_mainconfig_show(Editor *ed);
void editor_mainconfig_hide(Editor *ed);
void editor_error(Editor *ed, const char *msg);

#define EDITOR_ERROR_RET(ed_, msg_, ...) \
   do { \
      CRI(msg_); \
      editor_error(ed_, msg_); \
      return __VA_ARGS__; \
   } while (0)

#define EDITOR_ERROR_GOTO(ed_, msg_, label_) \
   do { \
      CRI(msg_); \
      editor_error(ed_, msg_); \
      goto label_; \
   } while (0)

#endif /* ! _EDITOR_H_ */

