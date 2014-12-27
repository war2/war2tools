#include <pud.h>

#define W(m, ...) fprintf(stdout, m "\n", ## __VA_ARGS__)

int
main(int    argc,
     char **argv)
{
   Pud *pud;
   int i;

   if (argc != 2)
     {
        fprintf(stderr, "*** Usage: %s <file.pud>\n", argv[0]);
        return 1;
     }

   pud = pud_open(argv[1], PUD_OPEN_MODE_R);
   if (!pud)
     {
        fprintf(stderr, "*** Failed to open [%s]\n", argv[1]);
        return 2;
     }

   i = pud_parse(pud);
   if (!i)
     {
        fprintf(stderr, "*** Failed to parse [%s]\n", argv[1]);
        pud_close(pud);
        return 3;
     }

   W("struct _unit_data udata[110] = {");

#define GEN(field) W("   ."#field" = %u,", pud->unit_data[i].field)
#define GEN_STOP(field) W("   ."#field" = %u", pud->unit_data[i].field)

   for (i = 0; i < 110; i++)
     {
        W("  [%i] = {", i);

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

        if (i < 109) W("  },");
        else         W("  }");
     }

#undef U

   W("};");
   W("");

   pud_close(pud);

   return 0;
}

