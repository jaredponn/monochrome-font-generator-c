#ifndef CLI_H
#define CLI_H

typedef struct {
  const char *input_ttf;
} ttf_to_bw_args_t;

int parse_args(int argc, char *argv[], ttf_to_bw_args_t *out);
void print_usage(const char *program);

#endif
