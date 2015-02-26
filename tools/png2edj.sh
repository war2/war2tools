#!/bin/sh -e

PNG_BASE="../data/tiles/png"
EDJ_BASE="../data/tiles/edj"
EDC_BASE="../data/tiles/edc"
TILESETS="forest winter wasteland swamp"

mkdir -p "$EDJ_BASE"
mkdir -p "$EDC_BASE"

for ts in $TILESETS; do
   edc="$EDC_BASE/$ts.edc"
   edj="$EDJ_BASE/$ts.edj"

   echo "Generating \"$edc\" ..."
   echo "collections {"                                          > "$edc"

   for img in `ls $PNG_BASE/$ts/*.png`; do

      base=`basename "$img"`
      i="${base%.*}"
      echo "   group { name: \"/$ts/tile/$i\";"                    >> "$edc"
      echo "      images {"                                        >> "$edc"
      echo "         image: \"$i.png\" COMP;"                      >> "$edc"
      echo "      }"                                               >> "$edc"
      echo "      parts {"                                         >> "$edc"
      echo "         part { name: \"tile\"; type: IMAGE;"          >> "$edc"
      echo "            description { state: \"default\" 0.0;"     >> "$edc"
      echo "               image.normal: \"$i.png\";"              >> "$edc"
      echo "            }"                                         >> "$edc"
      echo "         }"                                            >> "$edc"
      echo "      }"                                               >> "$edc"
      echo "   }"                                                  >> "$edc"

   done

   echo "}"                                                     >> "$edc"

   echo "Generating \"$edj\" ..."
   edje_cc -id "$PNG_BASE/$ts" -no-lossy -a "Jean Guyomarc'h" "$edc" "$edj"

done

