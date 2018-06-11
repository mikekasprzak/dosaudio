BIN_PREFIX			:=		~/mgc/embedded/codebench/bin/ia16-elf
#BIN_PREFIX			:=	../sourceryg++-2016.11/bin/ia16-elf

CC					:=	$(BIN_PREFIX)-gcc
CXX					:=	$(BIN_PREFIX)-g++
LD					:=	$(BIN_PREFIX)-ld

OUT					:=	dosaudio.com

# -T ../sourceryg++-2016.11/ia16-elf/lib/dos-com.ld


$(OUT): $(wildcard src/*.cpp)
	$(CC) $^ -Os -o $(OUT)

run: $(OUT)
	dosbox $(OUT)

clean:
	rm -fr $(OUT)

dump:
	$(BIN_PREFIX)-objdump -d $(OUT)

.PHONY: clean dump run
