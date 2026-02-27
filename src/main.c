/**
 * @brief Most of this is a code generator to comply with the method described
 * in <https://freetype.org/freetype2/docs/tutorial/step1.html>
 */
#include "cli.h"

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
  ttf_to_bw_args_t args = {0};

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
  FILE *header_file = fopen("ttf_to_bw.h", "w");
  if (header_file == NULL) {
    fprintf(stderr, "Failed to open ttf_to_bw.h for writing: %s\n",
            strerror(errno));
    status = 1;
    goto ft_done_face;
  }

  FILE *source_file = fopen("ttf_to_bw.c", "w");
  if (source_file == NULL) {
    fprintf(stderr, "Failed to open ttf_to_bw.c for writing: %s\n",
            strerror(errno));
    status = 1;
    goto close_header;
  }

  /////////////////////////
  // Creating the header
  /////////////////////////
  fprintf(header_file, "#ifndef TTF_TO_BW_H\n");
  fprintf(header_file, "#define TTF_TO_BW_H\n");

  fprintf(header_file, "#include <stdint.h>\n");

  fprintf(header_file,
          "/** See <https://freetype.org/freetype2/docs/tutorial/example1.c> "
          "as an example of\n");
  fprintf(header_file, " * the underlying freetype library with how to use\n");
  fprintf(header_file, " */\n");
  fprintf(header_file, "typedef struct {\n");
  fprintf(header_file, "  int bitmap_left; //\n");
  fprintf(header_file, "  int bitmap_top; //\n");
  fprintf(header_file, "  long int advance_x; //\n");
  fprintf(header_file, "  long int advance_y; //\n");
  fprintf(header_file, "  unsigned int rows; // number of bitmap rows\n");
  fprintf(header_file,
          "  unsigned int width; // number of pixels in bitmap row\n");
  fprintf(header_file, "  int pitch; // pitch is an offset to add to a bitmap "
                       "pointer in order to go down one row.\n");
  fprintf(header_file, "  unsigned char const *buffer;\n");
  fprintf(header_file, "} ttf_to_bw_slot_t;\n");
  fprintf(header_file, "\n");

  fprintf(header_file, "\n");

  fprintf(header_file, "/** The character mapping from ASCII characters to the "
                       "useful information to render the character");
  fprintf(header_file, " * the underlying freetype library with how to use\n");
  fprintf(header_file, " */\n");
  fprintf(header_file,
          "extern ttf_to_bw_slot_t const ttf_to_bw_character_map[UINT8_MAX];");

  fprintf(header_file, "#endif /* TTF_TO_BW_H */\n");

  /////////////////////////
  // Creating the source
  /////////////////////////
  fprintf(source_file, "#include \"ttf_to_bw.h\"\n");
  fprintf(source_file, "\n");

  // First, create buffers of all the bit packed data
  for (uint8_t character = 0; character < UINT8_MAX; ++character) {
    ft_error =
        FT_Load_Char(ft_face, character, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);

    if (ft_error != FT_Err_Ok) {
      fprintf(stderr, "loading char failed for %X with ft_error=%s\n",
              character,
              FT_Error_String(ft_error) == NULL ? "<unknown error>"
                                                : FT_Error_String(ft_error));
      continue;
    }

    FT_GlyphSlot ft_slot = ft_face->glyph;

    fprintf(source_file,
            "static unsigned char const char_buffer_%02" PRIu8 "[] = {",
            (int)character);
    size_t bitmap_index = 0;
    for (size_t bitmap_column = 0; bitmap_column < ft_slot->bitmap.width;
         ++bitmap_column) {
      for (size_t bitmap_row = 0; bitmap_row < ft_slot->bitmap.rows;
           ++bitmap_row) {
        fprintf(source_file, "0x%02X,", ft_slot->bitmap.buffer[bitmap_index++]);
      }
    }
    fprintf(source_file, "};\n");
  }

  // Then, we create the mapping
  fprintf(source_file,
          "ttf_to_bw_slot_t const ttf_to_bw_character_map[UINT8_MAX] = {\n");

  for (uint8_t character = 0; character < UINT8_MAX; ++character) {
    ft_error =
        FT_Load_Char(ft_face, character, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);

    if (ft_error != FT_Err_Ok) {
      fprintf(stderr, "loading char failed for %X with ft_error=%s\n",
              character,
              FT_Error_String(ft_error) == NULL ? "<unknown error>"
                                                : FT_Error_String(ft_error));

      fprintf(source_file, " { 0 },\n");
      continue;
    }

    FT_GlyphSlot ft_slot = ft_face->glyph;

    fprintf(source_file,
            " { .bitmap_left = %d, .bitmap_top = %d, .advance_x = %ld, "
            ".advance_y = %ld, .rows = %d, .width = %d, .pitch = %d, .buffer = "
            "char_buffer_%02" PRIu8 " },\n",
            ft_slot->bitmap_left, ft_slot->bitmap_top, ft_slot->advance.x,
            ft_slot->advance.y, ft_slot->bitmap.rows, ft_slot->bitmap.width,
            ft_slot->bitmap.pitch, character);
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
