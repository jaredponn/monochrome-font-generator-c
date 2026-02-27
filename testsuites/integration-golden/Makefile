FONT_TTF ?= ../fixtures/JetBrains_Mono/static/JetBrainsMono-Regular.ttf
OUTPUT_PBM ?= output.pbm
TEXT ?= Hello, World!

.PHONY: all clean

all: $(OUTPUT_PBM)

$(OUTPUT_PBM): render
	./render "$(TEXT)" "$@"

render: main.c char_map.tab.c char_map.tab.h
	$(CC) -o $@ main.c char_map.tab.c

char_map.tab.c char_map.tab.h:
	monochrome-font-generator-c \
		--width 1024 --height 1024 \
		--file-prefix char_map --name-prefix char_map \
		$(FONT_TTF)

clean:
	rm -f render char_map.tab.c char_map.tab.h $(OUTPUT_PBM)
