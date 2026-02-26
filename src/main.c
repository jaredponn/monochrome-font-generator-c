#include "cli.h"

#include <stdio.h>

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
  ft_error = FT_Set_Char_Size(face,       /* handle to face object         */
                              arg.width,  /* char_width in 1/64 of points  */
                              arg.height, /* char_height in 1/64 of points */
                              arg.hdpi,   /* horizontal device resolution  */
                              arg.vdpi);  /* vertical device resolution    */

  if (ft_error != FT_Err_Ok) {
    fprintf(stderr, "Failed to set char size for %s with ft_error=%s\n",
            args.input_ttf,
            FT_Error_String(ft_error) == NULL ? "<unknown error>"
                                              : FT_Error_String(ft_error));
    status = 1;
    goto ft_done_freetype;
  }

ft_done_face:
  FT_Done_Face(ft_face);

ft_done_freetype:
  FT_Done_FreeType(ft_library);
out:
  return 0;
}
