#include <REG517A.H>

#define CLK 12000000
#define FREQ_TIMER 100
#define CORRECTION 6000
#define INITIAL_TH0 ((65536 - (CLK/(12*FREQ_TIMER)) + CORRECTION) >> 8)
#define INITIAL_TL0 ((65536 - (CLK/(12*FREQ_TIMER)) + CORRECTION) & 0xFF)

unsigned int counter = 0;

void start_timer() {
		TR0 = 0;
		TMOD = (TMOD & 0xF0) | 0x01;
		TL0 = INITIAL_TL0;
		TH0 = INITIAL_TH0;
		ET0 = 1;
		TR0 = 1;
}

void timer_interrupt(void) interrupt 1 using 2 {
	TR0 = 0;
	TL0 += INITIAL_TL0;
	TH0 += INITIAL_TH0 + (unsigned char) CY;
	TR0 = 1;
	
	counter++;
}

void pinSwitch(){
		if (counter == 100) {
			counter = 0;
			P4 = ~P4;
		}
}

void main() {
	start_timer();
	P4 = 0x00;
	EAL = 1;
	while(1) {
		pinSwitch();
	}
}