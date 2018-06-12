TARGET									:=	dosaudio.com

SRC										:=	src
OUT										:=	.output

INCLUDE_DIRS							:=	$(SRC)

BIN2C									:=	tools/bin2c/bin2c.exe
NOISELOAD								:=	tools/noiseload/noiseload.exe

BIN_PREFIX								:=	~/mgc/embedded/codebench/bin/ia16-elf

CC										:=	$(BIN_PREFIX)-gcc
CXX										:=	$(BIN_PREFIX)-g++
LD										:=	$(BIN_PREFIX)-ld

C_FLAGS									:=
CPP_FLAGS								:=
COMMON_FLAGS							:=	$(addprefix -I ,$(INCLUDE_DIRS))
LD_FLAGS								:=	-Os

# Functions (must use '=', and not ':=') #
REMOVE_UNDERSCORE						=	$(foreach v,$(1),$(if $(findstring /_,$(v)),,$(v)))
FIND_FILE								=	$(call REMOVE_UNDERSCORE,$(shell find $(1) -name '$(2)'))
# NOTE: My standard build tree rule is to ignore any file/folder prefixed with an underscore #

.C_FILES								:=	$(call FIND_FILE,$(SRC)/,*.c)
.CPP_FILES								:=	$(call FIND_FILE,$(SRC)/,*.cpp)
.XRNS_FILES								:=	$(call FIND_FILE,$(SRC)/,*.xrns)

OUT_FILES								+=	$(subst $(SRC)/,$(OUT)/,$(.CPP_FILES:.cpp=.cpp.o))
OUT_FILES								+=	$(subst $(SRC)/,$(OUT)/,$(.C_FILES:.c=.c.o))

GEN_FILES								+=	$(subst $(SRC)/,$(SRC)/gen/,$(.XRNS_FILES:.xrns=.xrns.h))

OUT_FOLDERS								:=	$(sort $(dir $(OUT_FILES) $(GEN_FILES)))


$(TARGET): $(OUT_FOLDERS) $(OUT_FILES) $(GEN_FILES)
	$(CC) $(OUT_FILES) $(LD_FLAGS) -o $@


$(OUT)/%.c.o: $(SRC)/%.c
	$(CC) -c $< $(C_FLAGS) $(COMMON_FLAGS) -o $@

$(OUT)/%.cpp.o: $(SRC)/%.cpp
	$(CXX) -c $< $(CPP_FLAGS) $(COMMON_FLAGS) -o $@

$(SRC)/gen/%.xrns.bin: $(SRC)/%.xrns
	$(NOISELOAD) $< $@

$(SRC)/gen/%.xrns.h: $(SRC)/gen/%.xrns.bin
	$(BIN2C) $< >$@


$(OUT_FOLDERS):
	mkdir -p $@

run: $(TARGET)
	dosbox $(TARGET)

clean:
	rm -fr $(OUT) $(TARGET) $(GEN_FILES)

dump:
	$(BIN_PREFIX)-objdump -d $(TARGET)


$(NOISELOAD):
	$(MAKE) -C $(dir $(NOISELOAD))

$(BIN2C):
	$(MAKE) -C $(dir $(BIN2C_DIR))


info:
	@echo "$(OUT_FILES)"

.PHONY: clean dump run

#.INTERMEDIATE: $(GEN_FILES:.xrns.h=.xrns.bin)
