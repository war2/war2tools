/* libwar2/font.c */

#include "war2_private.h"

static Pud_Bool
_decode_glyph(const War2_Font_Decoder *const decoder, uint8_t glyph, const unsigned char *const mem, const Pud_Color *const palette)
{
  if (mem == NULL)
  {
    printf("Char 0x%x has no pixels\n", glyph);
    decoder->glyph_empty(decoder->data, glyph);
    return PUD_TRUE;
  }

  const size_t width = (size_t)(mem[0]);
  const size_t height = (size_t)(mem[1]);
  const size_t x_offset = (size_t)(mem[2]);
  const size_t y_offset = (size_t)(mem[3]);

  const size_t nb_pixels = width * height;
  const unsigned char *pixels = mem + 4;

  const Pud_Color empty_pixel = { .r = 0, .g = 0, .b = 0, .a = 0, };

  decoder->glyph_start(decoder->data, glyph, width, height, x_offset, y_offset);
  printf("---------------------------------: w=%zu, h=%zu\n",width,height);

  size_t decoded_pixels = 0;
  size_t x = 0;
  size_t y = 0;
  while (decoded_pixels < nb_pixels)
  {
    uint8_t byte = *pixels;
    while (byte >= 8)
    {
      decoded_pixels += 1;
      byte -= 8;
      decoder->glyph_pixel(decoder->data, glyph, x, y, empty_pixel);
      printf("_");
      x += 1;
      if (x >= width)
      {
        printf("\n");
        x = 0;
        y += 1;
      }
      if (decoded_pixels >= nb_pixels)
      { goto done_decoding; }
    }

    Pud_Color color = empty_pixel;
    switch (byte)
    {
      case 0x01: color = palette[0xc8]; break;
      case 0x02: color = palette[0xc7]; break;
      case 0x04: color = palette[0xc0]; break;
      case 0x05: color = palette[0x00]; break;
    }
    decoded_pixels += 1;
    decoder->glyph_pixel(decoder->data, glyph, x, y, color);
    printf("%c", byte + '0');
    x += 1;
    if (x >= width)
    {
        printf("\n");
      x = 0;
      y += 1;
    }
    pixels++;
  }

done_decoding:
  printf("\n");

  printf("--------------------------------- => %zu\n", decoded_pixels);
  fflush(stdout);
  decoder->glyph_end(decoder->data, glyph);

  return PUD_TRUE;
}

// See https://forum.war2.ru/index.php?topic=4701.msg76149#msg76149
PUDAPI Pud_Bool
war2_font_decode(
  War2_Data *w2,
  War2_Font font_to_decode,
  const War2_Font_Decoder *decoder)
{
  Pud_Bool status = PUD_FALSE;
  if (! decoder) { DIE_GOTO(end, "decoder must not be NULL"); };
  if (! decoder->glyph_start) { DIE_GOTO(end, "decoder->glyph_start must not be NULL"); };
  if (! decoder->glyph_end) { DIE_GOTO(end, "decoder->glyph_end must not be NULL"); };
  if (! decoder->glyph_pixel) { DIE_GOTO(end, "decoder->glyph_pixel must not be NULL"); };
  if (! decoder->glyph_empty) { DIE_GOTO(end, "decoder->glyph_empty must not be NULL"); };

  // Fonts lie between entries 279 and 283 (included). By carefully
  // crafting the value of War2_Font, we can immediately
  // deduce the entry from the enumeration value of the font.
  const unsigned int entry = (unsigned)font_to_decode + 279u;

  // Decode the entry. We expect a header of 8 bytes.
  size_t entry_size;
  unsigned char *const mem = war2_entry_extract(w2, entry, &entry_size);
  if (entry_size < 8) { DIE_GOTO(end, "Entry is too small"); }

  // This is indeed a FONT entry if the first four bytes are "FONT"
  uint32_t header;
  memcpy(&header, mem, sizeof(uint32_t));
  if (header != 0x544e4f46) /* "FONT" in ASCII */
  { DIE_GOTO(end_free, "Bad magic; expected \"FONT\""); }

  // Remaining four bytes of the header:
  // - low_index: lowest value contained in the font
  // - high_index: highest value contained in the font
  // - max width and height: maximum size of a glyph
  //
  // A "value" is probably a regular ASCII value... To handle non-english
  // languages, not sure how it's done...
  const uint8_t low_index = mem[4];
  const uint8_t high_index = mem[5];
  const size_t max_width = (size_t)(mem[6]);
  const size_t max_height = (size_t)(mem[7]);

  if (low_index >= high_index)
  { DIE_GOTO(end_free, "Incoherent font header"); }

  // We can use any color palette to decode fonts. Let's use the first one.
  const Pud_Color *const palette = w2->forest;

  // The remaining is a contiguous list of 4-bytes pointers. One pointer
  // per glyph. If a pointer is NULL, there are no data for the glyph
  // (typically: a space).
  const uint8_t nb_glyphs = high_index - low_index + 1;

  if (decoder->start) { decoder->start(decoder->data, nb_glyphs, max_width, max_height); }

  for (uint8_t i = 0; i < nb_glyphs; i++)
  {
    uint32_t pointer;
    const uint8_t value = low_index + i;
    const uint8_t *ptr = mem + 8 + (i * sizeof(uint32_t));
    memcpy(&pointer, ptr, sizeof(uint32_t));
    _decode_glyph(decoder, value, (pointer == 0) ? NULL : (mem + pointer), palette);
  }

  if (decoder->end) { decoder->end(decoder->data); }

  status = PUD_TRUE;
end_free:
  free(mem);
end:
  return status;
}
