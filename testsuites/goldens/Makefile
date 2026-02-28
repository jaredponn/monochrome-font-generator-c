TTF ?= ../font-fixtures/JetBrains_Mono/static/JetBrainsMono-Regular.ttf

.PHONY: all check-goldens update-goldens clean

render-to-pbm: main.c jet_brains_mono.tab.c jet_brains_mono.tab.h
	$(CC) -o $@ $^

# Choosing width/height values in pixels
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Let's say we have a display with dpi=169 dots/in.
# We want our width and height to be 13 pixels.
# Recall we have 
# > pixel_size = point_size * resolution / 72
# from <https://freetype.org/freetype2/docs/glyphs/glyphs-2.html>, so
# 
# ```
# 13 = point_size * 169 / 72
#   iff 13 = point_size * 169 / 72
#   iff point_size = 5.53846153846
# ```
# Finally, we recall that the `--width` and `--height` flags are in 1/64s of a
# point, so this means we must put
# ```
# width = height = 5.53846153846 * 64 =  354.461538461
# ```
# As a convenient one liner, the formula is given by
# ```
# width = height = 64 * pixel_size / (dpi / 72)
#                = 64 * pixel_size / (169 / 72)
#                       ^-- we later put this to 20
# ```
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
