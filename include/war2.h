/*
 * Copyright (c) 2014-2017 Jean Guyomarc'h
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file war2.h
 *
 * Libwar2 provides warcraft 2 data files manipulation.
 */

#ifndef __LIBWAR2_WAR2_H__
#define __LIBWAR2_WAR2_H__
#ifdef __cplusplus
extern "C" { /* } For dumb editors */
#endif

#include <pud.h>

/**
 * @defgroup War2_Types Libwar2 Types
 *
 * Types and enumeration values used by the libwar2
 * @{
 */

/**
 * @typedef War2_Data
 * Opaque type that handles warcraft 2 data
 * @since 1.0.0
 */
typedef struct _War2_Data War2_Data;

/**
 * @def WAR2_PALETTE_SIZE
 * The size of elements in a color palette
 * @since 1.0.0
 */
#define WAR2_PALETTE_SIZE 256

/**
 * @typedef War2_Sprites
 * Holds values for different types of sprites
 * @since 1.0.0
 */
typedef enum
{
   WAR2_SPRITES_UNITS     = 0x100, /**< Units sprites */
   WAR2_SPRITES_BUILDINGS = 0x101, /**< Buildings sprites */
   WAR2_SPRITES_ICONS     = 0x102, /**< Icons sprites */
   WAR2_SPRITES_SYSTEM    = 0x103  /**< System sprites (i.e. start locations) */
} War2_Sprites;

/**
 * Type that holds information about a current tileset decoding
 * @since 1.0.0
 */
typedef struct
{
   Pud_Era       era; /**< Current era */
   unsigned int  tiles; /**< Total amount of decoded tiles */
} War2_Tileset_Descriptor;

/**
 * Type to map a cursor to its associated entry
 * @since 1.0.0
 */
typedef enum
{
   WAR2_CURSOR_HUMAN_POINTER            = 301, /**< Human normal selector */
   WAR2_CURSOR_ORC_POINTER              = 302, /**< Orc normal selector */
   WAR2_CURSOR_HUMAN_POINTER_NOCLICK    = 303, /**< Human forbidden selector */
   WAR2_CURSOR_ORC_POINTER_NOCLICK      = 304, /**< Orc forbidden selector */
   WAR2_CURSOR_YELLOW_EAGLE             = 305, /**< Yellow eagle */
   WAR2_CURSOR_YELLOW_CROSSHAIRS        = 306, /**< Yellow crosshairs */
   WAR2_CURSOR_RED_EAGLE                = 307, /**< Red eagle */
   WAR2_CURSOR_RED_CROSSHAIRS           = 308, /**< Red crosshairs */
   WAR2_CURSOR_GREEN_EAGLE              = 309, /**< Green eagle */
   WAR2_CURSOR_GREEN_CROSSHAIRS         = 310, /**< Green crosshairs */
   WAR2_CURSOR_MAGNIFYING_GLASS         = 311, /**< Magnifying glass */
   WAR2_CURSOR_SELECTOR                 = 312, /**< Units selector */
   WAR2_CURSOR_HOURGLASS                = 313, /**< Hourglass */
   WAR2_CURSOR_CREDITS                  = 314, /**< Credits pointer */
   WAR2_CURSOR_ARROW_NORTH              = 315, /**< Scroll arrow (north) */
   WAR2_CURSOR_ARROW_NORTH_EAST         = 316, /**< Scroll arrow (north east) */
   WAR2_CURSOR_ARROW_EAST               = 317, /**< Scroll arraow (east) */
   WAR2_CURSOR_ARROW_SOUTH_EAST         = 318, /**< Scroll arrow (south east) */
   WAR2_CURSOR_ARROW_SOUTH              = 319, /**< Scroll arrow (south) */
   WAR2_CURSOR_ARROW_SOUTH_WEST         = 320, /**< Scroll arrow (south west) */
   WAR2_CURSOR_ARROW_WEST               = 321, /**< Scroll arrow (west) */
   WAR2_CURSOR_ARROW_NORTH_WEST         = 322, /**< Scroll arrow (north west) */

   __WAR2_CURSOR_LAST, /**< Sentinel. Can be used to terminate an iteration */
   __WAR2_CURSOR_FIRST                  = WAR2_CURSOR_HUMAN_POINTER /**< Alias to the first cursor */
} War2_Cursor;

/**
 * Type that holds information about a current sprite deconding
 * @since 1.0.0
 */
typedef struct
{
   Pud_Player   color; /**< Color of the sprite */
   Pud_Era      era; /**< Era of the sprite */
   Pud_Side     side; /**< Side (when appliable) */
   unsigned int object; /**< Decoded object */
   War2_Sprites sprite_type; /**< Sprite type */
} War2_Sprites_Descriptor;


/**
 * @typedef War2_Tileset_Decode_Func
 * Callback used for each tile to be decoded
 * @param data User provided data
 * @param tile The bitmap of the tile
 * @param w The width of the bitmap @c tile
 * @param h The height of the bitmap @c tile
 * @param ts Information about the decoding
 * @param tile_id The tile identifier
 * @since 1.0.0
 */
typedef void (*War2_Tileset_Decode_Func)(void *data,
                                         const Pud_Color *tile,
                                         unsigned int w,
                                         unsigned int h,
                                         const War2_Tileset_Descriptor *ts,
                                         uint16_t tile_id);

/**
 * @typedef War2_Sprites_Decode_Func
 * Callback used fir each sprite to be decoded
 * @param data User provided data
 * @param sprite The bitmap of the sprite
 * @param x X origin of the sprite
 * @param y Y origin of the sprite
 * @param w The width of the bitmap @c sprite
 * @param h The height of the bitmap @c sprite
 * @param sd Sprite descriptor of the current decoding
 * @param sprite_type Identifier of the currently decoded sprite
 * @since 1.0.0
 */
typedef void (*War2_Sprites_Decode_Func)(void *data,
                                         const Pud_Color *sprite,
                                         int x,
                                         int y,
                                         unsigned int w,
                                         unsigned int h,
                                         const War2_Sprites_Descriptor *sd,
                                         uint16_t sprite_id);


/**
 * @}
 */ /* End of War2_Types group */


/**
 * @defgroup War2_Core Core Functions
 *
 * Core Functions provided by libwar2
 * @{
 */

/**
 * Init libwar2 internal resources
 *
 * You should call this function when initializing your program.
 * Using functions of libwar2 without this call may result in
 * undefined behaviour.
 *
 * @return PUD_TRUE on success, PUD_FALSE on failure
 * @see war2_shutdown()
 * @since 1.0.0
 */
PUDAPI Pud_Bool war2_init(void);

/**
 * Release internal resources
 * @see war2_init()
 * @since 1.0.0
 */
PUDAPI void war2_shutdown(void);

/**
 * Open a Warcraft 2 data file (i.e. MAINDAT.WAR)
 *
 * @param file A valid path to MAINDAT.war
 * @return A valid handler on success, NULL otherwise
 * @see war2_close()
 * @since 1.0.0
 */
PUDAPI War2_Data *war2_open(const char *file);

/**
 * Close a Warcraft 2 data file
 *
 * @param w2 A valid handle to Warcraft 2 data file
 * @see war2_open()
 * @since 1.0.0
 */
PUDAPI void war2_close(War2_Data *w2);

/**
 * Set the verbosity level on the data handle
 *
 * @param w2 A valid handle to Warcraft 2 data file
 * @param level The verbosity level
 * @since 1.0.0
 */
PUDAPI void war2_verbosity_set(War2_Data *w2, int level);

/**
 * Extract the contents of a data entry
 *
 * The result MUST be manually freed by the caller with the free() function.
 *
 * @param w2 A valid handle to Warcraft 2 data file
 * @param entry The ID of the entry to extract
 * @param size_ret Used to return the size of the extracted data
 * @return The contents of the entry @c entry. NULL on failure
 * @since 1.0.0
 */
PUDAPI unsigned char *war2_entry_extract(War2_Data *w2, unsigned int entry, size_t *size_ret);

/**
 * Extract a palette from a data file
 *
 * @param[in] w2 A valid handle to Warcraft 2 data file
 * @param[in] era The era for the palette
 * @return The palette associated to @p era
 * @since 1.0.0
 */
PUDAPI const Pud_Color *war2_palette_get(const War2_Data *w2, Pud_Era era);

/**
 * Decode a tileset in the data file for a given era
 *
 * @param w2 A valid handle to Warcraft 2 data file
 * @param era The era of the tileset
 * @param func A user callback to be called for each decoded tile
 * @param data A user data passed to @c func
 * @return How many tiles were decoded
 * @since 1.0.0
 */
PUDAPI unsigned int war2_tileset_decode(War2_Data *w2, Pud_Era era, War2_Tileset_Decode_Func func, void *data);

/**
 * Decode sprites for a given object, color and era
 *
 * @param w2 A valid handle to Warcraft 2 data file
 * @param player_color The color of the sprites
 * @param era The era of the sprites
 * @param object The object to decode. To decode ICONS, pass WAR2_SPRITES_ICONS.
 * To decode units or buildings, pass the corresponding value of Pud_Unit.
 * @param func User callback to be called for each decoded sprite
 * @param data User data passed to @c func
 * @return PUD_TRUE on success, PUD_FALSE on failure
 * @since 1.0.0
 */
PUDAPI Pud_Bool
war2_sprites_decode(War2_Data                *w2,
                    Pud_Player                player_color,
                    Pud_Era                   era,
                    unsigned int              object,
                    War2_Sprites_Decode_Func  func,
                    void                     *data);

/**
 * Decode sprites in a given entry
 *
 * @param w2 A valid handle to Warcraft 2 data file
 * @param player_color The color of the sprites
 * @param entry The entry to decode
 * @param func User callback to be called for each decoded sprite
 * @param data User data passed to @c func
 * @return PUD_TRUE on success, PUD_FALSE on failure
 * @since 1.0.0
 */
PUDAPI Pud_Bool
war2_sprites_decode_entry(War2_Data                *w2,
                          Pud_Player                player_color,
                          unsigned int              entry,
                          War2_Sprites_Decode_Func  func,
                          void                     *data);

/**
 * Decode a cursor from an entry
 *
 * @param[in] w2 A valid handle to Warcract 2 data file
 * @param[in] entry An assumed valid entry to a cursor
 * @param[out] x The hot X position of the decoded cursor
 * @param[out] y The hot Y position of the decoded cursor
 * @param[out] w The width of the cursor
 * @param[out] h The height of the cursor
 * @return The bitmap of the cursor. NULL on failure. The caller MUST call
 *         free() on the returned value to release the memory.
 * @since 1.0.0
 */
PUDAPI Pud_Color *
war2_cursors_decode(War2_Data *w2,
                    unsigned int entry,
                    int *x,
                    int *y,
                    unsigned int *w,
                    unsigned int *h);

/**
 * Decode a user interface (UI element)
 *
 * @param[in] w2 A valid handle to Warcract 2 data file
 * @param[in] entry An assumed valid entry to an UI item
 * @param[out] w The width of the image
 * @param[out] h The height of the image
 * @return The decoded image for the UI element. NULL on failure.
 *         The caller must free() the returned value to release memory.
 * @since 1.0.0
 */
PUDAPI Pud_Color *
war2_ui_decode(War2_Data *w2,
               unsigned int entry,
               unsigned int *w,
               unsigned int *h);

/**
 * Write a bitmap as a PNG image on the filesystem.
 *
 * If libwar2 was NOT compiled with PNG support, this function will always
 * return PUD_FALSE.
 *
 * @param file The path where to save the png file
 * @param w The width of the bitmap
 * @param h The height of the bitmap
 * @param data The bitmap data
 * @return PUD_TRUE on success, PUD_FALSE on failure
 * @since 1.0.0
 */
PUDAPI Pud_Bool war2_png_write(const char          *file,
                               unsigned int         w,
                               unsigned int         h,
                               const unsigned char *data);

/**
 * Write a bitmap as a JPEG image on the filesystem.
 *
 * If libwar2 was NOT compiled with JPEG support, this function will always
 * return PUD_FALSE.
 *
 * @param file The path where to save the jpeg file
 * @param w The width of the bitmap
 * @param h The height of the bitmap
 * @param data The bitmap data
 * @return PUD_TRUE on success, PUD_FALSE on failure
 * @since 1.0.0
 */
PUDAPI Pud_Bool war2_jpeg_write(const char          *file,
                                unsigned int         w,
                                unsigned int         h,
                                const unsigned char *data);

/**
 * Write a bitmap as a PPM image on the filesystem.
 *
 * @param file The path where to save the ppm file
 * @param w The width of the bitmap
 * @param h The height of the bitmap
 * @param data The bitmap data
 * @return PUD_TRUE on success, PUD_FALSE on failure
 * @since 1.0.0
 */
PUDAPI Pud_Bool war2_ppm_write(const char          *file,
                               unsigned int         w,
                               unsigned int         h,
                               const unsigned char *data);

/**
 * Convert a color from one player to another
 *
 * @param from The source player color
 * @param to The wished player color
 * @param in_r The red component belonging to the player @c from
 * @param in_g The green component belonging to the player @c from
 * @param in_b The blue component belonging to the player @c from
 * @param out_r Memory where the red converted component will be stored
 * @param out_g Memory where the green converted component will be stored
 * @param out_b Memory where the blue converted component will be stored
 *
 * @warning In many functions, passing NULL in a return by address indicates
 * the return address will be ignored. It is NOT the case in this function.
 * If you pass NULL as @c out_r, @c out_g, @c out_b, your program will crash!
 * @since 1.0.0
 */
PUDAPI void
war2_sprites_color_convert(Pud_Player     from,
                           Pud_Player     to,
                           unsigned char  in_r,
                           unsigned char  in_g,
                           unsigned char  in_b,
                           unsigned char *out_r,
                           unsigned char *out_g,
                           unsigned char *out_b);

/**
 * @}
 */ /* End of War2_Core group */


#ifdef __cplusplus
}
#endif
#endif /* ! __LIBWAR2_WAR2_H__ */
