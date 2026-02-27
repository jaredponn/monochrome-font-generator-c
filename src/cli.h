#ifndef CLI_H
#define CLI_H

typedef struct {
  const char *input_ttf;
  const char *file_prefix;
  const char *name_prefix;
  int width;
  int height;
  int hdpi;
  int vdpi;
} ttf_to_bw_args_t;

int parse_args(int argc, char *argv[], ttf_to_bw_args_t *out);
void print_usage(const char *program);

#endif
