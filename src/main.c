/**
 * @brief Most of this is a code generator to comply with the method described
 * in <https://freetype.org/freetype2/docs/tutorial/step1.html>
 */
#include "cli.h"

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H

int main(int argc, char *argv[]) {
  int status = 0;

  /////////////////////////
  // Parse the CLI arguments
  /////////////////////////
  monochrome_font_generator_args_t args = {0};

  status = parse_args(argc, argv, &args);

  if (!(status == 0)) {
    print_usage(argv[0]);
    status = 1;
    goto out;
  }

  fprintf(stderr, "Processing: %s (%dx%d @ %dx%d dpi)\n", args.input_ttf,
          args.width, args.height, args.hdpi, args.vdpi);

  /////////////////////////
  // Start the FreeType library
  /////////////////////////
  FT_Library ft_library;
  FT_Face ft_face;
  FT_Error ft_error;

  ft_error = FT_Init_FreeType(&ft_library);
  if (ft_error) {
    fprintf(stderr, "Failed to initialize FreeType with ft_error=%s\n",
            FT_Error_String(ft_error) == NULL ? "<unknown error>"
                                              : FT_Error_String(ft_error));

    status = 1;
    goto out;
  }

  ft_error = FT_New_Face(ft_library, args.input_ttf, 0, &ft_face);
  if (ft_error != FT_Err_Ok) {
    fprintf(stderr, "Failed to load face %s with ft_error=%s\n", args.input_ttf,
            FT_Error_String(ft_error) == NULL ? "<unknown error>"
                                              : FT_Error_String(ft_error));
    status = 1;
    goto ft_done_freetype;
  }

  /////////////////////////
  // Configure the face
  /////////////////////////
  ft_error = FT_Set_Char_Size(ft_face,     /* handle to face object         */
                              args.width,  /* char_width in 1/64 of points  */
                              args.height, /* char_height in 1/64 of points */
                              args.hdpi,   /* horizontal device resolution  */
                              args.vdpi);  /* vertical device resolution    */

  if (ft_error != FT_Err_Ok) {
    fprintf(stderr, "Failed to set char size for %s with ft_error=%s\n",
            args.input_ttf,
            FT_Error_String(ft_error) == NULL ? "<unknown error>"
                                              : FT_Error_String(ft_error));
    status = 1;
    goto ft_done_freetype;
  }

  /////////////////////////
  // Open output files
  /////////////////////////
  char header_path[512];
  char source_path[512];

  status =
      snprintf(header_path, sizeof(header_path), "%s.tab.h", args.file_prefix);
  if (status < 0 || (size_t)status >= sizeof(header_path)) {
    fprintf(stderr, "File prefix too long for header path\n");
    status = 1;
    goto ft_done_face;
  }

  status =
      snprintf(source_path, sizeof(source_path), "%s.tab.c", args.file_prefix);
  if (status < 0 || (size_t)status >= sizeof(source_path)) {
    fprintf(stderr, "File prefix too long for source path\n");
    status = 1;
    goto ft_done_face;
  }

  FILE *header_file = fopen(header_path, "w");
  if (header_file == NULL) {
    fprintf(stderr, "Failed to open %s for writing: %s\n", header_path,
            strerror(errno));
    status = 1;
    goto ft_done_face;
  }

  FILE *source_file = fopen(source_path, "w");
  if (source_file == NULL) {
    fprintf(stderr, "Failed to open %s for writing: %s\n", source_path,
            strerror(errno));
    status = 1;
    goto close_header;
  }

  /////////////////////////
  // Creating the header
  /////////////////////////
  fprintf(header_file, "#ifndef %s_TAB_H\n", args.name_prefix);
  fprintf(header_file, "#define %s_TAB_H\n", args.name_prefix);

  fprintf(header_file, "#include <stdint.h>\n");

  fprintf(header_file,
          "/** See <https://freetype.org/freetype2/docs/tutorial/example1.c> "
          "as an example of\n");
  fprintf(header_file, " * the underlying freetype library with how to use\n");
  fprintf(header_file, " */\n");
  fprintf(header_file, "typedef struct {\n");
  fprintf(header_file, "  int bitmap_left; // The bitmap's left bearing "
                       "expressed in integer pixels "
                       "<https://freetype.org/freetype2/docs/reference/"
                       "ft2-glyph_retrieval.html#ft_glyphslotrec>\n");
  fprintf(header_file,
          "  int bitmap_top; // The bitmap's top bearing expressed in integer "
          "pixels. This is the distance from the baseline to the top-most "
          "glyph scanline, upwards y coordinates being positive.  "
          "<https://freetype.org/freetype2/docs/reference/"
          "ft2-glyph_retrieval.html#ft_glyphslotrec>\n");
  if (args.advance_x)
    fprintf(header_file,
            "  long int advance_x; // The horizontal distance to increment "
            "(for left-to-right writing) or decrement (for right-to-left "
            "writing) the pen position after a glyph has been rendered when "
            "processing text in pixels "
            "<https://freetype.org/freetype2/docs/glyphs/"
            "glyphs-3.html#section-3>\n");
  if (args.advance_y)
    fprintf(header_file, "  long int advance_y; // The vertical distance to "
                         "decrement (for top-to-bottom writing) or increment "
                         "the pen position after a glyph has been rendered in "
                         "pixels<https://freetype.org/freetype2/docs/glyphs/"
                         "glyphs-3.html#section-3>\n");
  fprintf(header_file, "  unsigned int rows; // number of bitmap rows\n");
  fprintf(header_file,
          "  unsigned int width; // number of pixels in bitmap row\n");
  fprintf(header_file, "  int pitch; // pitch is an offset to add to a bitmap "
                       "pointer in order to go down one row.\n");
  fprintf(header_file, "  unsigned char const *buffer;\n");
  fprintf(header_file, "} %s_glyph_and_metrics_t;\n", args.name_prefix);
  fprintf(header_file, "\n");

  fprintf(header_file, "\n");

  fprintf(header_file, "/** The character mapping from ASCII characters to the "
                       "useful information to render the character");
  fprintf(header_file, " * the underlying freetype library with how to use\n");
  fprintf(header_file, " */\n");
  fprintf(header_file,
          "extern %s_glyph_and_metrics_t const %s_tab[UINT8_MAX + 1];\n",
          args.name_prefix, args.name_prefix);

  fprintf(header_file, "#endif /* %s_TAB_H */\n", args.name_prefix);

  /////////////////////////
  // Creating the source
  /////////////////////////
  fprintf(source_file, "#include \"%s\"\n", header_path);
  fprintf(source_file, "\n");

  // First, create buffers of all the bit packed data
  for (int character = 0; character <= UINT8_MAX; ++character) {
    ft_error =
        FT_Load_Char(ft_face, character,
                     FT_LOAD_RENDER | FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO);

    if (ft_error != FT_Err_Ok) {
      fprintf(stderr, "loading char failed for character %d with ft_error=%s\n",
              character,
              FT_Error_String(ft_error) == NULL ? "<unknown error>"
                                                : FT_Error_String(ft_error));
      continue;
    }

    FT_GlyphSlot ft_slot = ft_face->glyph;
    int abs_pitch = ft_slot->bitmap.pitch < 0 ? -ft_slot->bitmap.pitch
                                              : ft_slot->bitmap.pitch;
    size_t buffer_size = (size_t)ft_slot->bitmap.rows * (size_t)abs_pitch;

    fprintf(source_file,
            "static unsigned char const char_buffer_%" PRIu8 "[] = {",
            (int)character);
    for (size_t i = 0; i < buffer_size; ++i) {
      unsigned char byte = ft_slot->bitmap.buffer[i];
      if (args.lsb_leftmost) {
        // Magic trick to reverse the bits in a byte
        // <https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith32Bits>
        byte = ((byte * 0x0802LU & 0x22110LU) | (byte * 0x8020LU & 0x88440LU)) *
                   0x10101LU >>
               16;
      }
      if (args.invert) {
        byte = ~byte;
      }
      fprintf(source_file, "0x%02X,", byte);
    }
    fprintf(source_file, "};\n");
  }

  // Then, we create the mapping
  fprintf(source_file,
          "%s_glyph_and_metrics_t const %s_tab[UINT8_MAX + 1] = {\n",
          args.name_prefix, args.name_prefix);

  for (int character = 0; character <= UINT8_MAX; ++character) {
    ft_error =
        FT_Load_Char(ft_face, character,
                     FT_LOAD_RENDER | FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO);

    if (ft_error != FT_Err_Ok) {
      fprintf(stderr, "loading char failed for character %d with ft_error=%s\n",
              character,
              FT_Error_String(ft_error) == NULL ? "<unknown error>"
                                                : FT_Error_String(ft_error));

      fprintf(source_file, " { 0 },\n");
      continue;
    }

    FT_GlyphSlot ft_slot = ft_face->glyph;

    fprintf(source_file, " { .bitmap_left = %d, .bitmap_top = %d, ",
            ft_slot->bitmap_left, ft_slot->bitmap_top);
    // Divide by 64 to convert these into pixels from FreeType's 26.6
    // fixed-point numbers format
    if (args.advance_x)
      fprintf(source_file, ".advance_x = %ld, ", ft_slot->advance.x / 64);
    if (args.advance_y)
      fprintf(source_file, ".advance_y = %ld, ", ft_slot->advance.y / 64);
    fprintf(source_file,
            ".rows = %d, .width = %d, .pitch = %d, .buffer = "
            "char_buffer_%" PRIu8 " },\n",
            ft_slot->bitmap.rows, ft_slot->bitmap.width, ft_slot->bitmap.pitch,
            character);
  }
  fprintf(source_file, "};\n");

  /////////////////////////
  // Cleanup
  /////////////////////////
close_source:
  fclose(source_file);

close_header:
  fclose(header_file);

ft_done_face:
  FT_Done_Face(ft_face);

ft_done_freetype:
  FT_Done_FreeType(ft_library);
out:
  return 0;
}
