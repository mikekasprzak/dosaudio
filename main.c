//#include <stdio.h>
//
//#define STR2(x) #x
//#define STR(x) STR2(x)
//#define INCBIN(name, file) \
//    __asm__( \
//    	".section .rodata\n" \
//        ".global incbin_" STR(name) "_start\n" \
//        ".type incbin_" STR(name) "_start, @object\n" \
//        "incbin_" STR(name) "_start:\n" \
//        ".incbin \"" file "\"\n" \
//        \
//        ".global incbin_" STR(name) "_end\n" \
//        ".type incbin_" STR(name) "_end, @object\n" \
//        "incbin_" STR(name) "_end:\n" \
//    ); \
//    extern const void* incbin_ ## name ## _start; \
//    extern const void* incbin_ ## name ## _end;
//
//INCBIN(audio, "out.bin");

static void print( char *string ) {
    asm volatile(
    	"mov   $0x09, %%ah\n"
		"int   $0x21\n"
		: /* no output */
		: "d"(string)
		: "ah"
	);
}

unsigned char audio[] = {
#include "out.h"
//	0,32,0,37,127,37,127,49,127,49,127,37,127,37,127,49,127,49,127,37,127,37,127,49,127,49,127,37,127,37,127,49,127,49,127,
};

int main() {
	print("Hello, World!\n$");

	//unsigned short audioSize = (incbin_audio_end - incbin_audio_start)>>4;
	//unsigned short audioSize = _binary_out_bin_size;

	for ( short idx = 0; idx < sizeof(audio); ++idx ) {
		print(".$");
	}
	print("\n$");

//	char blah[80];
//	sprintf(blah, "Size: %i", incbin_audio_end - incbin_audio_start);

	return 0;
}
