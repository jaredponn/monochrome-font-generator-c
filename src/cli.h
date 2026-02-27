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
  int lsb;
} monochrome_font_generator_args_t;

int parse_args(int argc, char *argv[], monochrome_font_generator_args_t *out);
void print_usage(const char *program);

#endif
