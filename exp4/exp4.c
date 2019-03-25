#include <REG51F.H>

#define BASE 204

void start_timer();
void start_serial();

unsigned char buffer;

void main() {
	start_timer();
	start_serial();
	EA = 1;
	
	while(1);
}

void start_timer() {
	TR1 = 0;
	TMOD = (TMOD & 0xF0) | 0x20;
	PCON = (PCON & 0x0F) | 0x80;
 	TH1 = BASE;
	TL1 = BASE;
	TR1 = 1;
}

void start_serial() {
	ES = 0;
	SCON = (SCON & 0x0F) | 0x50;
	ES = 1;
}

void serial_interrupt(void) interrupt 4 using 2 {
	if (TI == 1)
		T1 = 0;
	
	if (RI == 1) {
		RI = 0;
		buffer = SBUF;
		SBUF = buffer + 1;
	}
}