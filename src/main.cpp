
static void print( const char *string ) {
    asm volatile ("mov   $0x09, %%ah\n"
                  "int   $0x21\n"
                  : /* no output */
                  : "d"(string)
                  : "ah");
}

unsigned char audio[] = {
#include "out.h"
};

static void SoundInit() {
	asm volatile(
		"mov $0b10110110, %%al\n"	// binary, square wave, 16bit (low then high), on counter #2
		"out %%al, $0x43\n"			// PIT mode (counters) setup
		:
	);
}

unsigned short freq = 4560;
static void SetFreq( unsigned short newFreq ) {
	freq = newFreq;
	asm volatile(
		//"mov $4560, %%ax\n"			// Frequency
		"out %%al, $0x42\n"			// write low to PIT counter 2's port
		"mov %%ah, %%al\n"			// move ah to low
		"out %%al, $0x42\n"			// write low (high) to same port
		: /*out*/
		: "ax"(newFreq)
	);
}

static void SoundOn() {
	asm volatile(
		"in $0x61, %%al\n"			// read PPI/Keyboard port B (because speaker bit is here)
		"or $0b00000011, %%al\n"	// mask-in the speaker and the "timer 2 gate" bits
		"out %%al, $0x61\n"			// re-write it to the PPI/Keyboard port B (i.e. TURN ON SOUND)
		:
	);
}

static void SoundOff() {
	asm volatile(
		"in $0x61, %%al\n"			// read the PPI/Keyboard port B again
		"and $0b11111100, %%al\n"	// mask-out the speaker and "timer 2 gate" bits
		"out %%al, $0x61\n"			// re-write it to PPI/Keyboard port B (i.e. TURN SOUND OFF)
		:
	);
}

// for some reason lower frequencies are higher... actually it's probably because pulse width
static void beep() {
	// Reference: http://muruganad.com/8086/8086-assembly-language-program-to-play-sound-using-pc-speaker.html
	asm volatile(
		"mov $0b10110110, %%al\n"	// binary, square wave, 16bit (low then high), on counter #2
		"out %%al, $0x43\n"			// PIT mode (counters) setup
		""
		"mov $4560, %%ax\n"			// Frequency
		"out %%al, $0x42\n"			// write low to PIT counter 2's port
		"mov %%ah, %%al\n"			// move ah to low
		"out %%al, $0x42\n"			// write low (high) to same port
		""
		"in $0x61, %%al\n"			// read PPI/Keyboard port B (because speaker bit is here)
		"or $0b00000011, %%al\n"	// mask-in the speaker and the "timer 2 gate" bits
		"out %%al, $0x61\n"			// re-write it to the PPI/Keyboard port B (i.e. TURN ON SOUND)
		""
		"mov $20, %%bx\n"
	".pause1:\n"
		"mov $65535, %%cx\n"
	".pause2:\n"
		"dec %%cx\n"
		"jne .pause2\n"
//		"dec %%bx\n"
//		"jne .pause1\n"
		""
		"in $0x61, %%al\n"			// read the PPI/Keyboard port B again
		"and $0b11111100, %%al\n"	// mask-out the speaker and "timer 2 gate" bits
		"out %%al, $0x61\n"			// re-write it to PPI/Keyboard port B (i.e. TURN SOUND OFF)
		: /* no output */
	);
}

int main() {
	print("Hello Worm!\n$");
	//beep();
	
	unsigned char* pos = audio + 4;
	
	unsigned short length = *(short*)(audio+2);
	
	SoundInit();
	SetFreq(freq);
	SoundOn();
	for ( unsigned short idx2 = 0; idx2 < length; ++idx2 ) {
		for ( unsigned short idx = 0; idx < 30000; ++idx ) {
			if ( (*pos > 0) && (*pos < 128) ) {
				SetFreq(100 * *pos);
			}
		}
		pos++;
	}
	SoundOff();

	return 0;
}
