# monochrome-font-generator-c

A command-line tool that converts TrueType (`.ttf`) font files into C source
code containing bit-packed monochrome glyph bitmaps. The generated `.tab.c` and
`.tab.h` files provide a lookup table mapping each of the 256 ASCII byte values
to its rendered glyph data, so you can draw text in embedded or framebuffer
environments without linking FreeType at runtime.

## How it works

The generator uses FreeType 2 to rasterize every glyph in monochrome mode
(`FT_LOAD_MONOCHROME`). For each character it emits:

- A `static const` byte array holding the bit-packed bitmap.
- An entry in a `_slot_t` struct array containing the bitmap metrics
  (`bitmap_left`, `bitmap_top`, `advance_x`, `advance_y`, `rows`, `width`,
  `pitch`) and a pointer to the bitmap buffer.

The consuming application includes the generated header, iterates over a string,
and blits each glyph's bitmap onto a canvas using the provided metrics.

## Usage

```sh
monochrome-font-generator-c \
  --width <width> --height <height> \
  [--hdpi <hdpi>] [--vdpi <vdpi>] \
  [--file-prefix <prefix>] [--name-prefix <prefix>] \
  <input.ttf>
```

| Flag             | Description                                      | Default    |
|------------------|--------------------------------------------------|------------|
| `--width`        | Glyph width in 1/64 of a point (required)        | —          |
| `--height`       | Glyph height in 1/64 of a point (required)       | —          |
| `--hdpi`         | Horizontal device resolution in DPI               | 72         |
| `--vdpi`         | Vertical device resolution in DPI                 | 72         |
| `--file-prefix`  | Output file path prefix (produces `<prefix>.tab.c` and `<prefix>.tab.h`) | `char_map` |
| `--name-prefix`  | C symbol prefix for generated types and tables    | `char_map` |

### Example

```sh
monochrome-font-generator-c --width 1024 --height 1024 myfont.ttf
```

This produces `char_map.tab.h` and `char_map.tab.c`. Include the header in your
project and link the source to render text using the generated glyph table.

## Building

### With Nix (recommended)

```sh
nix build
```

### With CMake

Requires CMake >= 3.20 and FreeType 2.

```sh
cmake -B build
cmake --build build
```
