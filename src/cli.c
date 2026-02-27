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
      "[--vdpi <vdpi>] <input.ttf>\n"
      "  --width <width>    Glyph width in 1/64 of a point (a point is "
      "1/72 of an inch)\n"
      "  --height <height>  Glyph height in 1/64 of point (a point is "
      "1/72 of an inch)\n"
      "  --hdpi <hdpi>      Horizontal device resolution (default: 72)\n"
      "  --vdpi <vdpi>      Vertical device resolution (default: 72)\n"
      "  --file-prefix <prefix>  Output file prefix (default: char_map)\n"
      "  --name-prefix <prefix>  C symbol name prefix (default: ttf_to_bw)\n",
      program);
}

int parse_args(int argc, char *argv[], ttf_to_bw_args_t *out) {
  out->width = 0;
  out->height = 0;
  out->hdpi = 72;
  out->vdpi = 72;
  out->input_ttf = NULL;
  out->file_prefix = "char_map";
  out->name_prefix = "ttf_to_bw";

  static struct option long_options[] = {
      {"width", required_argument, NULL, 'w'},
      {"height", required_argument, NULL, 'h'},
      {"hdpi", required_argument, NULL, 'x'},
      {"vdpi", required_argument, NULL, 'y'},
      {"file-prefix", required_argument, NULL, 'p'},
      {"name-prefix", required_argument, NULL, 'n'},
      {NULL, 0, NULL, 0},
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
    switch (opt) {
    case 'w':
      if (parse_int(optarg, "width", &out->width))
        return 1;
      break;
    case 'h':
      if (parse_int(optarg, "height", &out->height))
        return 1;
      break;
    case 'x':
      if (parse_int(optarg, "hdpi", &out->hdpi))
        return 1;
      break;
    case 'y':
      if (parse_int(optarg, "vdpi", &out->vdpi))
        return 1;
      break;
    case 'p':
      out->file_prefix = optarg;
      break;
    case 'n':
      out->name_prefix = optarg;
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
