#include <REG51F.H>

sbit c1 = P2^0;
sbit c2 = P2^1;

void firstMachine() {
	if (c1 == 1) {
		P1 = (P1 & 0xF0) | (P0 & 0x0F);
	} else if (c1 == 0) {
		P1 = P1 & 0xF0;
	}
}

void secondMachine() {
	if (c2 == 1) {
		P1 = (P1 & 0x0F) | (P0 & 0xF0);
	} else if (c2 == 0) {
		P1 = P1 & 0x0F;
	}
}

void main() {
	while (1) {
		firstMachine();
		secondMachine();
	}
}