#include <REG51F.H>

#define BASE 204
#define RING_SIZE 16

void start_timer();
void start_serial();

void sendChar(char c);
void sendString(char *s);
char RxBufferVazio();
char receiveChar();
void receiveString(char *s);

unsigned char rx_ring[RING_SIZE], tx_ring[RING_SIZE];
unsigned char rx_head = 0, rx_tail = 0, tx_head = 0, tx_tail = 0, tx_busy = 0;


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
	if (TI == 1) {
		T1 = 0;
		if (!(tx_head == tx_tail)) {
				SBUF = tx_ring[tx_head];
				tx_head = (tx_head + 1) % RING_SIZE;
		} else tx_busy = 0;
	}
	
	if (RI == 1) {
		RI = 0;
		if ((rx_tail + 1) % RING_SIZE != rx_head) {
			rx_tail = (rx_tail + 1) % RING_SIZE;
			rx_ring[rx_tail] = SBUF;
		}
	}
}

void sendChar(char c) {
	if ((tx_tail + 1) % RING_SIZE != tx_head) {
		tx_tail = (tx_tail + 1) % RING_SIZE;
		tx_ring[tx_tail] = c;
	}
	
	if (!tx_busy) {
		tx_busy = 1;
		TI = 1;
	}
}

void sendString(char *s) {
	for (unsigned char i = 0; s[i] != '\0'; i++) sendChar(s[i]);
}

char RxBufferVazio() {
	if (rx_head == rx_tail) return 1;
	else return 0;
}

char receiveChar() {
	char c;
	
	if (!RxBufferVazio()) {
		c = rx_ring[rx_head];
		rx_head = (rx_head + 1) % RING_SIZE;
	} return '\0';
	
	return c;
}

void receiveString(char *s) {
	unsigned char i = 0;
	
	do {
		c = receiveChar();
		s[i] = c;
		i++;
	} while (c != '\0');
}