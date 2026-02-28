# monochrome-font-generator-c

A command-line tool that converts TrueType (`.ttf`) font files into C source
code containing bit-packed monochrome glyph bitmaps alongside suitable metrics
to render text. More precisely, the generated `.tab.c` and `.tab.h` files
provide a lookup table mapping each of the 256 ASCII byte values to its
rendered glyph data and metrics to permit drawing text in framebuffer
environments without including FreeType.

## Getting started

Execute the following command to generate the `.tab.c` and `.tab.h` files
containing the monochrome glyph bitmaps and suitable metrics to render text.

```sh
$ monochrome-font-generator-c --width 1024 \
  --height 1024 \
  --advance-x \
  --lsb-leftmost \
  myfont.ttf
```

Then, one can include the `.tab.c` and `.tab.h` files in one's project to render
text. For example, see [testsuites/goldens/main.c](testsuites/goldens/main.c)
which demonstrates rendering text to a `.pbm`.

## Limitations

- **No kerning data.** The generated tables do not include kerning information
  (to keep code size smaller), so glyph spacing relies solely on each glyph's
  `advance_x` value. This works well for monospace fonts but will produce uneven
  spacing with proportional fonts. For best results, use a monospace typeface.

## Usage

```sh
monochrome-font-generator-c \
  --width <width> --height <height> \
  [--hdpi <hdpi>] [--vdpi <vdpi>] \
  [--file-prefix <prefix>] [--name-prefix <prefix>] \
  [--lsb-leftmost] [--invert] [--advance-x] [--advance-y] \
  <input.ttf>
```

| Flag | Description | Default |
|------------------|--------------------------------------------------|------------|
| `--width` | Glyph width in 1/64 of a point (required) | — |
| `--height` | Glyph height in 1/64 of a point (required) | — |
| `--hdpi` | Horizontal device resolution in DPI | 72 |
| `--vdpi` | Vertical device resolution in DPI | 72 |
| `--file-prefix` | Output file path prefix (produces `<prefix>.tab.c` and `<prefix>.tab.h`) | `char_map` |
| `--name-prefix` | C symbol prefix for generated types and tables | `char_map` |
| `--lsb-leftmost` | Least significant bit is the leftmost pixel | Disabled, so MSB is the leftmost pixel |
| `--invert` | Invert bitmap foreground and background colors | Disabled, so 1 = foreground, 0 = background |
| `--advance-x` | Include `advance_x` field in the generated struct | Omitted |
| `--advance-y` | Include `advance_y` field in the generated struct | Omitted |

## Installing

### With Nix (recommended)

The `flake.nix` provides the `default` overlay, which contains the derivation
for the code generator in the key `monochrome-font-generator-c`.

### With CMake

Requires CMake >= 3.20 and FreeType 2.

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build
```
