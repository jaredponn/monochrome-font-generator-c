#include "jet_brains_mono.tab.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief writes the provided canvas to a pbm file
 */
static int write_pbm(const char *path, const uint8_t *pixels, int w, int h) {
  FILE *f = fopen(path, "w");
  if (!f)
    return 1;

  fprintf(f, "P1\n%d %d\n", w, h);

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      fputc(pixels[y * w + x] ? '1' : '0', f);
      fputc(x + 1 < w ? ' ' : '\n', f);
    }
  }

  fclose(f);
  return 0;
}

/**
 * @brief Computes the minimum width and height (in pixels) to render the
 * provided text
 */
static int get_text_width_and_height(char const *text, int *out_width,
                                     int *out_height) {
  *out_width = 0;
  *out_height = 0;

  if (text[0] == '\0')
    return 0;

  int largest_line_length = 0;

  int line_length = 0;
  int line_count = 0;

  for (const char *c = text; *c; ++c) {
    if (*c == '\n') {
      line_length = 0;
      ++line_count;
    } else {
      line_length += jet_brains_mono_tab[*c].advance_x;
      largest_line_length =
          largest_line_length > line_length ? largest_line_length : line_length;
    }
  }

  *out_width = largest_line_length;
  *out_height = jet_brains_mono_ascender + jet_brains_mono_height * line_count -
                jet_brains_mono_descender;
}

/**
 * @brief Renders a string using the generated monochrome font table and writes
 * a PBM.
 *
 * Usage: ./a.out [text] [output.pbm]
 */
int main(int argc, char *argv[]) {
  int status = 0;

  const char *text = "Hello, World!";
  const char *output = "output.pbm";

  if (argc > 1)
    text = argv[1];
  if (argc > 2)
    output = argv[2];

  printf("Rendering text=`%s`\n", text);

  int canvas_w;
  int canvas_h;

  get_text_width_and_height(text, &canvas_w, &canvas_h);

  if (canvas_w <= 0)
    canvas_w = 1;
  if (canvas_h <= 0)
    canvas_h = 1;

  uint8_t *canvas = malloc(canvas_w * canvas_h);
  if (!canvas) {
    fprintf(stderr, "Failed to allocate %dx%d canvas\n", canvas_w, canvas_h);
    status = 1;
    goto out;
  }
  memset(canvas, 0x00, canvas_w * canvas_h);

  int pen_x = 0;
  int pen_y = 0;

  // See <https://freetype.org/freetype2/docs/glyphs/glyphs-5.html> for methods
  // of rendering text
  for (const char *p = text; *p; ++p) {
    uint8_t ch = (uint8_t)*p;

    if (ch == '\n') {
      pen_x = 0;
      pen_y += jet_brains_mono_height;
      continue;
    }
    jet_brains_mono_glyph_and_metrics_t const *slot = &jet_brains_mono_tab[ch];

    // It's always good to generally have the pictures
    // <https://freetype.org/freetype2/docs/glyphs/glyphs-3.html> in mind to
    // justify where we are setting the glyph_x/y
    int glyph_x = pen_x + slot->bitmap_left;
    int glyph_y = jet_brains_mono_ascender - slot->bitmap_top + pen_y;

    // See <https://freetype.org/freetype2/docs/glyphs/glyphs-7.html> for
    // rendering the pitch
    int pitch = slot->pitch;
    int abs_pitch = pitch >= 0 ? pitch : -pitch;

    for (unsigned int r = 0; r < slot->rows; ++r) {
      unsigned int buffer_row = pitch >= 0 ? r : (slot->rows - 1 - r);
      for (unsigned int c = 0; c < slot->width; ++c) {
        int byte_index = buffer_row * abs_pitch + c / 8;
        int bit_index = c % 8;
        int pixel_on = (slot->buffer[byte_index] >> (7 - bit_index)) & 1;

        int cx = glyph_x + (int)c;
        int cy = glyph_y + (int)r;

        if (cx >= 0 && cx < canvas_w && cy >= 0 && cy < canvas_h) {
          if (pixel_on) {
            canvas[cy * canvas_w + cx] = true;
          }
        }
      }
    }

    pen_x += (int)(slot->advance_x);
  }

  status = write_pbm(output, canvas, canvas_w, canvas_h);

  if (status != 0) {
    fprintf(stderr, "Failed to write PBM to %s\n", output);
    goto free_canvas;
  }

  fprintf(stderr, "Wrote %dx%d PBM to %s\n", canvas_w, canvas_h, output);

free_canvas:
  free(canvas);

out:
  return status;
}
