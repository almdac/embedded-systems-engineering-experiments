#include <REG51F.H>

#define CLK 12000000
#define FREQ_TIMER 100
#define CORRECTION 6000
#define INITIAL_TH0 ((65536 - (CLK/(12*FREQ_TIMER)) + CORRECTION) >> 8)
#define INITIAL_TL0 ((65536 - (CLK/(12*FREQ_TIMER)) + CORRECTION) & 0xFF)

sbit cl = P2^0;
sbit ch = P2^1;

void firstMachine();
void secondMachine();

void start_timer();
void timer_interrupt(void);

unsigned int first_counter = 0, second_counter = 0;
unsigned char first_counter_enabled = 0, second_counter_enabled = 0;

void main() {
	start_timer();
	EA = 1;
	while(1) {
		firstMachine();
		secondMachine();
	}
}

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
	
	if (first_counter_enabled) first_counter++;
	if (second_counter_enabled) second_counter++;
}

void firstMachine(){
	static unsigned char state = 0;
	switch (state) {
		case 0:
			if (ch == 0) {
				state = 1;
			}
			break;
		case 1:
			if (ch == 1) {
				P1 = (P1 & 0x0F) | (P0 & 0xF0);
				first_counter = 0;
				first_counter_enabled = 1;
				state = 2;
			}
			break;	
		case 2:
			if (first_counter == 100) {
				P1 = P1 & 0x0F;
				state = 0;
			}
			break;
	}
}

void secondMachine() {
		static unsigned char state = 0;
		switch (state) {
		case 0:
			if (cl == 0) {
				state = 1;
			}
			break;
		case 1:
			if (cl == 1) {
				P1 = (P1 & 0xF0) | (P0 & 0x0F);
				second_counter = 0;
				second_counter_enabled = 1;
				state = 2;
			}
			break;
		case 2:
			if (second_counter == 100) {
				P1 = P1 & 0xF0;
				state = 0;
			}
			break;
	}
}