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
    fprintf(stderr, "Error: --%s value '%s' is not a valid integer\n", name,
            str);
    return 1;
  }
  if (errno == ERANGE || val < INT_MIN || val > INT_MAX) {
    fprintf(stderr, "Error: --%s value '%s' is out of range\n", name, str);
    return 1;
  }

  *out = (int)val;
  return 0;
}

void print_usage(const char *program) {
  fprintf(
      stderr,
      "Usage: %s --width <width> --height <height> [--hdpi <hdpi>] "
      "[--vdpi <vdpi>] [--file-prefix <prefix>] [--name-prefix <prefix>] "
      "[--lsb-leftmost] [--invert] [--advance-x] [--advance-y] <input.ttf>\n"
      "  --width <width>    Glyph width in 1/64 of a point (a point is "
      "1/72 of an inch)\n"
      "  --height <height>  Glyph height in 1/64 of point (a point is "
      "1/72 of an inch)\n"
      "  --hdpi <hdpi>      Horizontal device resolution (default: 72)\n"
      "  --vdpi <vdpi>      Vertical device resolution (default: 72)\n"
      "  --file-prefix <prefix>  Output file prefix (default: char_map)\n"
      "  --name-prefix <prefix>  C symbol name prefix (default: char_map)\n"
      "  --lsb-leftmost          Least significant bit is the leftmost pixel "
      "(default: MSB is the leftmost pixel i.e., the left most pixel in a byte "
      "has value "
      "128)\n"
      "  --invert                Invert the bitmap foreground and background "
      "colors (default: 1 is the foreground and 0 is the background)\n"
      "  --advance-x             Include advance_x field in the generated "
      "struct (default: omitted)\n"
      "  --advance-y             Include advance_y field in the generated "
      "struct (default: omitted)\n",
      program);
}

typedef enum {
  WIDTH,
  HEIGHT,
  HDPI,
  VDPI,
  FILE_PREFIX,
  NAME_PREFIX,
  LSB_LEFTMOST,
  INVERT,
  ADVANCE_X,
  ADVANCE_Y,
} cli_option_t;

int parse_args(int argc, char *argv[], monochrome_font_generator_args_t *out) {
  out->width = 0;
  out->height = 0;
  out->hdpi = 72;
  out->vdpi = 72;
  out->input_ttf = NULL;
  out->file_prefix = "char_map";
  out->name_prefix = "char_map";
  out->lsb_leftmost = 0;
  out->invert = 0;
  out->advance_x = 0;
  out->advance_y = 0;

  static struct option long_options[] = {
      {"width", required_argument, NULL, WIDTH},
      {"height", required_argument, NULL, HEIGHT},
      {"hdpi", required_argument, NULL, HDPI},
      {"vdpi", required_argument, NULL, VDPI},
      {"file-prefix", required_argument, NULL, FILE_PREFIX},
      {"name-prefix", required_argument, NULL, NAME_PREFIX},
      {"lsb-leftmost", no_argument, NULL, LSB_LEFTMOST},
      {"invert", no_argument, NULL, INVERT},
      {"advance-x", no_argument, NULL, ADVANCE_X},
      {"advance-y", no_argument, NULL, ADVANCE_Y},
      {NULL, 0, NULL, 0},
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
    switch (opt) {
    case WIDTH:
      if (parse_int(optarg, "width", &out->width))
        return 1;
      break;
    case HEIGHT:
      if (parse_int(optarg, "height", &out->height))
        return 1;
      break;
    case HDPI:
      if (parse_int(optarg, "hdpi", &out->hdpi))
        return 1;
      break;
    case VDPI:
      if (parse_int(optarg, "vdpi", &out->vdpi))
        return 1;
      break;
    case FILE_PREFIX:
      out->file_prefix = optarg;
      break;
    case NAME_PREFIX:
      out->name_prefix = optarg;
      break;
    case LSB_LEFTMOST:
      out->lsb_leftmost = 1;
      break;
    case INVERT:
      out->invert = 1;
      break;
    case ADVANCE_X:
      out->advance_x = 1;
      break;
    case ADVANCE_Y:
      out->advance_y = 1;
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

  if (!(out->width >= 0 && out->height >= 0)) {
    fprintf(stderr, "Error: width and height must be non-negative integers\n");
    return 1;
  }

  return 0;
}
