
unsigned char audio[] = {
#include "out.h"
};

static void print( const char *string ) {
	asm volatile (
		"mov $0x09, %%ah\n"
		"int   $0x21\n"
		: /* no output */
		: "d"(string)
		: "ah"
	);
}

static unsigned char scanwait() {
	unsigned char ret;
	asm volatile (
		"mov $0x00, %%ah\n"
		"int $0x16\n"
		: /* no output */
		: "ah"(ret)			// scan code
	);
	return ret;
}

static unsigned char keywait() {
	unsigned char ret;
	asm volatile (
		"mov $0x00, %%ah\n"
		"int $0x16\n"
		: /* no output */
		: "al"(ret)			// ascii code
	);
	return ret;
}

/// NOTE: zf (zero flag) tells you if there's a key or not
/// NOTE: doesn't remove it from the queue
static unsigned char scanget() {
	unsigned char ret;
	asm volatile (
		"mov $0x01, %%ah\n"
		"int $0x16\n"
		: /* no output */
		: "ah"(ret)			// scan code
	);
	return ret;
}
/// NOTE: doesn't remove it from the queue
static unsigned char keyget() {
	unsigned char ret;
	asm volatile (
		"mov $0x01, %%ah\n"
		"int $0x16\n"
		: /* no output */
		: "al"(ret)			// ascii code
	);
	return ret;
}




static void SoundInit() {
	// Setup counter #2
	asm volatile(
		"mov $0b10110110, %%al\n"	// binary, square wave, 16bit (low then high), on counter #2
		"out %%al, $0x43\n"			// PIT mode (counters) setup
		:
	);
}

static void IFunc() {
	asm volatile(
		"mov $0b10110110, %%al\n"	// binary, square wave, 16bit (low then high), on counter #2
		"out %%al, $0x43\n"			// PIT mode (counters) setup
		:
	);
	print(".$");
}

static void SoundInterruptInit() {
	unsigned short newFreq = 10;
	
	void (*Func)() = &IFunc;
	void* Addr = (void*)(0x1C<<2);
	
	Addr = (void*)Func;

//	// Backup the interrupt pointer
//	asm volatile(
//		"mov %%ax, %%dx;"
//		: /*out*/
//		: "dx"(Addr), "ax"(Func)
//	);

	// Set the new pointer

	char fud[] = "..\n$";
	fud[0] = 'A' + sizeof(Func);
	print(fud);


	
	// Setup counter #0
	asm volatile(
		"mov $0b00110110, %%al\n"	// binary, square wave, 16bit (low then high), on counter #0
		"out %%al, $0x43\n"			// PIT mode (counters) setup
		: /*out*/
	);
	
	return;

	// Set frequency
	asm volatile(
		"out %%al, $0x40\n"			// write low to PIT counter 2's port
		"mov %%ah, %%al\n"			// move ah to low
		"out %%al, $0x40\n"			// write low (high) to same port
		: /*out*/
		: "ax"(newFreq)
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

#define PIT_FREQ 0x1234DD

unsigned short pitches[(12*10)+1] = {
	0,
	16, 17, 18, 19, 20, 21, 23, 24, 25, 27, 29, 30,
	32, 34, 36, 38, 41, 43, 46, 49, 51, 55, 58, 61,
	65, 69, 73, 77, 82, 87, 92, 98, 103, 110, 116, 123,
	130, 138, 146, 155, 164, 174, 185, 196, 207, 220, 233, 246,
	261, 277, 293, 311, 329, 349, 370, 392, 415, 440, 466, 493,
	523, 554, 587, 622, 659, 698, 740, 784, 830, 880, 932, 987,
	1047, 1109,	1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
	2093, 2217,	2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,
	4186, 4435,	4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902,

//	16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87,
//	32.70, 34.65, 36.71, 38.89, 41.20, 43.65, 46.25, 49.00, 51.91, 55.00, 58.27, 61.74,
//	65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.8, 110.0, 116.5, 123.5,
//	130.8, 138.6, 146.8, 155.6, 164.8, 174.6, 185.0, 196.0, 207.7, 220.0, 233.1, 246.9,
//	261.6, 277.2, 293.7, 311.1, 329.6, 349.2, 370.0, 392.0, 415.3, 440.0, 466.2, 493.9,
//	523.3, 554.4, 587.3, 622.3, 659.3, 698.5, 740.0, 784.0, 830.6, 880.0, 932.3, 987.8,
//	1047, 1109,	1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
//	2093, 2217,	2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,
//	4186, 4435,	4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902,
};

unsigned char* songData = audio;
unsigned char* songStart = songData + 4;
unsigned char* songPos = songStart;
unsigned short songLength = *(short*)(songData+2);
unsigned char* songEnd = songStart + songLength;

void song_Init() {
	SoundInit();
}

void song_Step() {
	// Update note
	if ( (*songPos > 0) && (*songPos < 127) ) {
		SoundOn();
		SetFreq(PIT_FREQ / pitches[*songPos-12]);
	}
	else if ( *songPos == 127 ) {
		SoundOff();
	}

	// Next note
	songPos++;
	if ( songPos >= songEnd )
		songPos = songStart;
}

void song_Exit() {
	// Stop playback
	SoundOff();
}


unsigned short MPUAddr = 0x330;
#define MPU_DATA			(MPUAddr+0)
#define MPU_COMMAND			(MPUAddr+1)
#define MPU_STATUS			(MPUAddr+1)
// The Command and Status ports are the same

// https://askubuntu.com/a/565566
unsigned char midiSysexPanic[] = {
	0xF0, 0x7E, 0x7F,
	0x09, 0x01, 0xF7,
};

void midi_Init( unsigned short newMPUAddr = 0x330 ) {
	MPUAddr = newMPUAddr;

	asm volatile(
		"mov $0x3F, %%al\n"		// 0x3f means 'enter UART mode'
		"out %%al, %%dx\n"		// Write to the MPU-401 COMMAND port
		: /* no output */
		: "dx"(MPU_COMMAND)
	);
}

void midi_Write( unsigned char newData ) {
	asm volatile(
		"out %%al, %%dx\n"		// Write to the MPU-401 DATA port
		: /* no output */
		: "dx"(MPU_DATA), "al"(newData)
	);
}

// how to open files? https://stackoverflow.com/questions/23810442/dos-assembly-program-reading-from-pipe

// int 0x21, 0x3d // Open file?
// int 0x21, 0x3f // Read file in to memory?
// int 0x21, 0x3e // Close file?

int main() {
	print("Hello Worm!\n$");

	char major, minor;
	unsigned char os;

	asm volatile(
		"mov $0x30, %%ah;"
		"int $0x21;"
		: "=al"(major), "=ah"(minor), "=bh"(os)
		:
		: "bl", "cx"
	);

	char ver[] = "X.XX X\n$";
	ver[0] = '0' + major;
	ver[2] = '0' + (minor / 10);
	ver[3] = '0' + (minor % 10);
	ver[5] = 'D' + os;

	print(ver);

	SoundInterruptInit();

	song_Init();

	for ( unsigned short idx2 = 0; idx2 < songLength; ++idx2 ) {
		// delay
		for ( unsigned short idx = 0; idx < 21000; ++idx ) {
			(void)(idx);
		}

		song_Step();
	}
	
	print("Press a key to exit\n$");
	keywait();
	song_Exit();


	midi_Init();

	print("Press a key to exit\n$");
	keywait();
	//midi_Exit();

	return 0;
}
