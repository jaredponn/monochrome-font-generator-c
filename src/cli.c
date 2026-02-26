#include "cli.h"

#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_int(const char *str, const char *name, int *out) {
  char *end;
  errno = 0;
  long val = strtol(str, &end, 10);

  if (end == str || *end != '\0') {
    fprintf(stderr, "Error: -%s value '%s' is not a valid integer\n", name,
            str);
    return 1;
  }
  if (errno == ERANGE || val < INT_MIN || val > INT_MAX) {
    fprintf(stderr, "Error: -%s value '%s' is out of range\n", name, str);
    return 1;
  }

  *out = (int)val;
  return 0;
}

void print_usage(const char *program) {
  fprintf(stderr,
          "Usage: %s -w <width> -h <height> [-x <hdpi>] [-y <vdpi>] "
          "<input.ttf>\n"
          "  -w <width>   Glyph width in 1/64 of a point (a point is 1/72 of "
          "an inch)\n"
          "  -h <height>  Glyph height in 1/64 of point (a point is 1/72 of an "
          "inch)\n"
          "  -x <hdpi>    Horizontal device resolution (default: 72)\n"
          "  -y <vdpi>    Vertical device resolution (default: 72)\n",
          program);
}

int parse_args(int argc, char *argv[], ttf_to_bw_args_t *out) {
  out->width = 0;
  out->height = 0;
  out->hdpi = 72;
  out->vdpi = 72;
  out->input_ttf = NULL;

  int opt;
  while ((opt = getopt(argc, argv, "w:h:x:y:")) != -1) {
    switch (opt) {
    case 'w':
      if (parse_int(optarg, "w", &out->width))
        return 1;
      break;
    case 'h':
      if (parse_int(optarg, "h", &out->height))
        return 1;
      break;
    case 'x':
      if (parse_int(optarg, "x", &out->hdpi))
        return 1;
      break;
    case 'y':
      if (parse_int(optarg, "y", &out->vdpi))
        return 1;
      break;
    default:
      return 1;
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Error: missing input TTF file\n");
    return 1;
  }
  out->input_ttf = argv[optind];

  if (out->width <= 0 || out->height <= 0) {
    fprintf(stderr, "Error: width and height must be positive integers\n");
    return 1;
  }

  return 0;
}
