#!/usr/bin/env bash

# DESCRIPTION
# ~~~~~~~~~~~
# Additional sanity checks to verify that the generated font code is
# valid C

set -ex

monochrome-font-generator-c \
  --width 1024 --height 1024 \
  --lsb-leftmost --invert \
  --advance-x \
  --file-prefix my_font --name-prefix my_font \
  "$TTF"

"$CC" -c -fsyntax-only "my_font.tab.c"

monochrome-font-generator-c \
  --width 1024 --height 1024 \
  --lsb-leftmost --invert \
  --advance-y \
  --file-prefix my_font --name-prefix my_font \
  "$TTF"

"$CC" -c -fsyntax-only "my_font.tab.c"

monochrome-font-generator-c \
  --width 1024 --height 1024 \
  --lsb-leftmost --invert \
  --advance-x \
  --advance-y \
  --file-prefix my_font --name-prefix my_font \
  "$TTF"

"$CC" -c -fsyntax-only "my_font.tab.c"
