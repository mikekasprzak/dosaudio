BIN_PREFIX			:=		~/mgc/embedded/codebench/bin/ia16-elf
#BIN_PREFIX			:=	../sourceryg++-2016.11/bin/ia16-elf


BIN2C				:=	tools/bin2c/bin2c.exe
NOISELOAD			:=	tools/noiseload/noiseload.exe

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

$(NOISELOAD):
	$(MAKE) -C $(dir $(NOISELOAD))

$(BIN2C):
	$(MAKE) -C $(dir $(BIN2C_DIR))

dump:
	$(BIN_PREFIX)-objdump -d $(OUT)

.PHONY: clean dump run
