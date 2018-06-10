
BIN_PREFIX			:=	../sourceryg++-2016.11/bin/ia16-elf

CC					:=	$(BIN_PREFIX)-gcc
CXX					:=	$(BIN_PREFIX)-g++
LD					:=	$(BIN_PREFIX)-ld

OUT					:=	fun.com

# -T ../sourceryg++-2016.11/ia16-elf/lib/dos-com.ld


$(OUT): $(wildcard *.c)
	$(CC) $^ out.o -Os -o $(OUT)


clean:
	rm -fr $(OUT)

dump:
	$(BIN_PREFIX)-objdump -d $(OUT)

.PHONY: clean dump
