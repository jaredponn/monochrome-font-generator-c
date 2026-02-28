# DESCRIPTION
# ~~~~~~~~~~~
# The target `render-to-pbm` compiles main.c against the generated font tables
# into an executable that renders text strings to PBM images.
#
# The PHONY target `check-goldens` renders each test fixture in `./fixtures/`
# and diffs the output against the expected images in `./goldens/`.
#
# The PHONY target `update-goldens` renders each test fixture in `./fixtures/`
# and overwrites the expected images in `./goldens/`.

TTF ?= ../font-fixtures/JetBrains_Mono/static/JetBrainsMono-Regular.ttf

.PHONY: all check-goldens update-goldens clean

render-to-pbm: main.c jet_brains_mono.tab.c jet_brains_mono.tab.h
	$(CC) -o $@ $^

# Choosing --width/--height values for a target pixel size
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# From <https://freetype.org/freetype2/docs/glyphs/glyphs-2.html>:
#
#   pixel_size = point_size * resolution / 72
#
# The --width and --height flags are in 1/64ths of a point, so the formula
# to convert a desired pixel size to the flag value is:
#
#   height = width = 64 * pixel_size * 72 / dpi
#
# For a 20-pixel glyph height at 169 DPI:
#
#   height = width = 64 * 20 * 72 / 169 = 544.97...
#
jet_brains_mono.tab.c jet_brains_mono.tab.h:
	monochrome-font-generator-c \
		--width 0 --height 545 \
		--hdpi 0 --vdpi 169 \
		--lsb-leftmost \
		--advance-x \
		--file-prefix jet_brains_mono --name-prefix jet_brains_mono \
		$(TTF)

clean:
	rm -f render-to-pbm jet_brains_mono.tab.c jet_brains_mono.tab.h

check-goldens: render-to-pbm
	mkdir -p actuals
	for TEST_FIXTURE in ./fixtures/*; do \
		TEST_FIXTURE_NAME="$$(basename "$$TEST_FIXTURE" .txt)"; \
		./render-to-pbm "$$(cat "$$TEST_FIXTURE")" "actuals/$$TEST_FIXTURE_NAME.pbm"; \
		diff "actuals/$$TEST_FIXTURE_NAME.pbm" "goldens/$$TEST_FIXTURE_NAME.pbm" \
			|| { \
					echo "Expected goldens/$$TEST_FIXTURE_NAME.pbm:"; \
					cat "goldens/$$TEST_FIXTURE_NAME.pbm"; \
					echo "to match actuals/$$TEST_FIXTURE_NAME.pbm:"; \
					cat "actuals/$$TEST_FIXTURE_NAME.pbm"; \
					exit 1; }; \
	done

update-goldens: render-to-pbm
	mkdir -p goldens
	for TEST_FIXTURE in ./fixtures/*; do \
		TEST_FIXTURE_NAME="$$(basename "$$TEST_FIXTURE" .txt)"; \
		./render-to-pbm "$$(cat "$$TEST_FIXTURE")" "goldens/$$TEST_FIXTURE_NAME.pbm"; \
	done
