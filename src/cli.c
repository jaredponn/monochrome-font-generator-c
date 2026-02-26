#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(const char *program) {
  fprintf(stderr, "Usage: %s <input.ttf>\n", program);
}

int parse_args(int argc, char *argv[], ttf_to_bw_args_t *out) {
  if (argc != 2) {
    return 1;
  }

  out->input_ttf = argv[1];

  return 0;
}
