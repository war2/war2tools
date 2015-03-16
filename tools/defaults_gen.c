/*
 * defaults_gen.c
 * defaults_gen
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include <pud.h>

#define W(m, ...) fprintf(stdout, m "\n", ## __VA_ARGS__)
#define WN(m, ...) fprintf(stdout, m, ## __VA_ARGS__)

#define GEN_STRUCT(type_, struct_) \
   W("struct _"#type_" "#type_"_"#struct_" = {"); \
   WN("     { "); \
   for (i = 0; i < 7; i++) WN("%u, ", pud->struct_.players[i]); \
   WN("%u", pud->struct_.players[7]); \
   W(" },"); \
   WN("     { "); \
   for (i = 0; i < 6; i++) WN("%u, ", pud->struct_.unusable[i]); \
   WN("%u", pud->struct_.unusable[6]); \
   W(" },"); \
   W("     %u", pud->struct_.neutral); \
   W("};\n")

typedef enum
{
   UDTA,
   ALOW,
   UGRD
} Type;

static void
_usage(void)
{
   fprintf(stderr, "*** Usage: defaults_gen <type> <file.pud>\n");
   fprintf(stderr, "***   Type values: udta,alow,ugrd\n");
}


int
main(int    argc,
     char **argv)
{
   Pud *pud;
   int i;
   Type t;
   const char *file, *type;

   /* Getopt */
   if (argc != 3)
     {
        _usage();
        return 1;
     }
   type = argv[1];
   file = argv[2];
   if (!strcasecmp(type, "udta"))
     t = UDTA;
   else if (!strcasecmp(type, "alow"))
     t = ALOW;
   else if (!strcasecmp(type, "ugrd"))
     t = UGRD;
   else
     {
        fprintf(stderr, "*** Invalid type [%s]\n", type);
        _usage();
        return 1;
     }

   /* Open and parse */
   pud = pud_open(file, PUD_OPEN_MODE_R);
   if (!pud)
     {
        fprintf(stderr, "*** Failed to open [%s]\n", file);
        return 2;
     }
   i = pud_parse(pud);
   if (!i)
     {
        fprintf(stderr, "*** Failed to parse [%s]\n", argv[1]);
        pud_close(pud);
        return 3;
     }

   switch (t)
     {
      case UDTA:

         W("struct _unit_data udata[110] = {");
         for (i = 0; i < 110; i++)
           {
              W("  [%i] = {", i);

#define GEN(field) W("   ."#field" = %u,", pud->unit_data[i].field)
#define GEN_STOP(field) W("   ."#field" = %u", pud->unit_data[i].field)
              GEN(overlap_frames);
              GEN(sight);
              GEN(hp);
              GEN(build_time);
              GEN(gold_cost);
              GEN(lumber_cost);
              GEN(oil_cost);
              GEN(size_w);
              GEN(size_h);
              GEN(box_w);
              GEN(box_h);
              GEN(range);
              GEN(computer_react_range);
              GEN(human_react_range);
              GEN(armor);
              GEN(priority);
              GEN(basic_damage);
              GEN(piercing_damage);
              GEN(missile_weapon);
              GEN(type);
              GEN(decay_rate);
              GEN(annoy);
              GEN(mouse_right_btn);
              GEN(point_value);
              GEN(can_target);
              GEN(flags);
              GEN(rect_sel);
              GEN(has_magic);
              GEN(weapons_upgradable);
              GEN_STOP(armor_upgradable);
#undef GEN_STOP
#undef GEN

              if (i < 109) W("  },");
              else         W("  }");
           }

         W("};");
         W("");
         break; // End UDTA


      case ALOW:

#define GEN(name) GEN_STRUCT(alow, name)
         GEN(unit_alow);
         GEN(spell_start);
         GEN(spell_alow);
         GEN(spell_acq);
         GEN(up_alow);
         GEN(up_acq);
#undef GEN

         break; // End ALOW


      case UGRD:

         W("struct _ugrd ugrd[52] = {");

         for (i = 0; i < 52; i++)
           {
              W("  [%i] = {", i);

#define GEN(field) W("   ."#field" = %u,", pud->upgrade[i].field)
#define GEN_STOP(field) W("   ."#field" = %u", pud->upgrade[i].field)
              GEN(time);
              GEN(gold);
              GEN(lumber);
              GEN(oil);
              GEN(icon);
              GEN(group);
              GEN_STOP(flags);
#undef GEN_STOP
#undef GEN
              if (i < 51) W("  },");
              else        W("  }");
           }

         W("};");
         W("");
         break; // End UGRD

     }
   pud_close(pud);

   return 0;
}

