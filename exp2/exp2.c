#include <REG51F.H>

sbit cl = P2^0;
sbit ch = P2^1;

void firstMachine();
void secondMachine();

void main() {
	while(1) {
		firstMachine();
		secondMachine();
	}
}


void firstMachine(){
	static unsigned char state = 0;
  static unsigned int counter;
	switch (state) {
		case 0:
			if (ch == 0) {
				state = 1;
			}
			break;
		case 1:
			if (ch == 1) {
				P1 = (P1 & 0x0F) | (P0 & 0xF0);
				counter = 0;
				state = 2;
			}
			break;	
		case 2:
			if (counter++ == 25000) {
				P1 = P1 & 0x0F;
				state = 0;
			}
			break;
	}
}

void secondMachine() {
		static unsigned char state = 0;
    static unsigned int counter;
		switch (state) {
		case 0:
			if (cl == 0) {
				state = 1;
			}
			break;
		case 1:
			if (cl == 1) {
				P1 = (P1 & 0xF0) | (P0 & 0x0F);
				counter = 0;
				state = 2;
			}
			break;
		case 2:
			if (counter++ == 25000) {
				P1 = P1 & 0xF0;
				state = 0;
			}
			break;
	}
}