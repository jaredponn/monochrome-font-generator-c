#include "char_map.tab.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int write_pbm(const char *path, const uint8_t *pixels, int w, int h) {
  FILE *f = fopen(path, "w");
  if (!f)
    return 1;

  fprintf(f, "P1\n%d %d\n", w, h);

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      fputc(pixels[y * w + x] == 0x00 ? '1' : '0', f);
      fputc(x + 1 < w ? ' ' : '\n', f);
    }
  }

  fclose(f);
  return 0;
}

/**
 * Renders a string using the generated monochrome font table and writes a PBM.
 *
 * Usage: ./a.out [text] [output.pbm]
 */
int main(int argc, char *argv[]) {
  const char *text = "Hello, World!";
  const char *output = "output.pbm";

  if (argc > 1)
    text = argv[1];
  if (argc > 2)
    output = argv[2];

  int pen_x = 0;
  int max_ascent = 0;
  int max_descent = 0;

  for (const char *p = text; *p; ++p) {
    uint8_t ch = (uint8_t)*p;
    char_map_slot_t const *slot = &char_map_tab[ch];

    int top = slot->bitmap_top;
    int bottom = (int)slot->rows - top;

    if (top > max_ascent)
      max_ascent = top;
    if (bottom > max_descent)
      max_descent = bottom;

    pen_x += (int)(slot->advance_x >> 6);
  }

  int canvas_w = pen_x + 2;
  int canvas_h = max_ascent + max_descent + 2;

  if (canvas_w <= 0)
    canvas_w = 1;
  if (canvas_h <= 0)
    canvas_h = 1;

  uint8_t *canvas = malloc(canvas_w * canvas_h);
  if (!canvas) {
    fprintf(stderr, "Failed to allocate %dx%d canvas\n", canvas_w, canvas_h);
    return 1;
  }
  memset(canvas, 0xFF, canvas_w * canvas_h);

  pen_x = 0;
  for (const char *p = text; *p; ++p) {
    uint8_t ch = (uint8_t)*p;
    char_map_slot_t const *slot = &char_map_tab[ch];

    int glyph_x = pen_x + slot->bitmap_left;
    int glyph_y = max_ascent - slot->bitmap_top;

    int abs_pitch = slot->pitch < 0 ? -slot->pitch : slot->pitch;

    for (unsigned int r = 0; r < slot->rows; ++r) {
      for (unsigned int c = 0; c < slot->width; ++c) {
        int byte_index = r * abs_pitch + c / 8;
        int bit_index = 7 - (c % 8);
        int pixel_off = (slot->buffer[byte_index] >> bit_index) & 1;

        int cx = glyph_x + (int)c;
        int cy = glyph_y + (int)r;

        if (cx >= 0 && cx < canvas_w && cy >= 0 && cy < canvas_h) {
          if (pixel_off) {
            canvas[cy * canvas_w + cx] = 0x00;
          }
        }
      }
    }

    pen_x += (int)(slot->advance_x >> 6);
  }

  int rc = write_pbm(output, canvas, canvas_w, canvas_h);
  free(canvas);

  if (rc != 0) {
    fprintf(stderr, "Failed to write PBM to %s\n", output);
    return 1;
  }

  fprintf(stderr, "Wrote %dx%d PBM to %s\n", canvas_w, canvas_h, output);
  return 0;
}
