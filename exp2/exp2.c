#include <REG51F.H>

sbit cl = P2^0;
sbit ch = P2^1;

int first_state = 0, second_state = 0, old_ch, old_cl;

int chDown();
int clDown();
int chUp();
int clUp();
int firstTimeout();
int secondTimeout();

void firstMachine();
void secondMachine();

void main() {
	old_ch = ch;
	old_cl = cl;
	while(1) {
		firstMachine();
		secondMachine();
	}
}

int chDown() {
	if (old_ch && !ch) {
		old_ch = ch;
		return 1;
	} else return 0;
}

int clDown() {
	if (old_cl && !cl) {
		old_cl = cl;
		return 1;
	} else return 0;
}

int chUp() {
	if (!old_ch && ch) {
		old_ch = ch;
		return 1;
	} else return 0;
}

int clUp() {
	if (!old_cl && cl) {
		old_cl = cl;
		return 1;
	} else return 0;
}

int firstTimeout() {
	return 0;
}

int secondTimeout() {
	return 0;
}

void firstMachine(){
	switch (first_state) {
		case 0:
			if (chUp()) {
				P1 = (P1 & 0x0F) | (P0 & 0xF0);
				first_state = 2;
			} else if (chDown()) {
				first_state = 1;
			}
			break;
		case 1:
			if (chUp()) {
				P1 = (P1 & 0x0F) | (P0 & 0xF0);
				first_state = 2;
			}
			break;	
		case 2:
			if (firstTimeout()) {
				P1 = P1 & 0x0F;
				first_state = 0;
			}
			break;
	}
}

void secondMachine() {
		switch (second_state) {
		case 0:
			if (clUp()) {
				P1 = (P1 & 0xF0) | (P0 & 0x0F);
				second_state = 2;
			} else if (clDown()) {
				second_state = 1;
			}
			break;
		case 1:
			if (clUp()) {
				P1 = (P1 & 0xF0) | (P0 & 0x0F);
				second_state = 2;
			}
			break;
		case 2:
			if (secondTimeout()) {
				P1 = P1 & 0xF0;
				first_state = 0;
			}
			break;
	}
}