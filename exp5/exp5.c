#include <REG51F.H>

#define BASE 204  
#define RING_SIZE 16 // Length of circular buffers

unsigned char rx_ring[RING_SIZE]; // Reception circular buffer
unsigned char	tx_ring[RING_SIZE]; // Transmission circular buffer
unsigned char rx_head = 0, rx_tail = 0, tx_head = 0, tx_tail = 0, tx_busy = 0;

void serial_interrupt(void) interrupt 4 using 2 { // Interrupt while there's something to be transmitted or received
	if (TI == 1) {
		T1 = 0;
		if (tx_head != tx_tail) { // Is there something to be transmitted?
				SBUF = tx_ring[tx_head];
				tx_head = (tx_head + 1) % RING_SIZE;
		} else tx_busy = 0; // Not busy anymore (transmission done)
	}
	
	if (RI == 1) {
		RI = 0;
		if ((rx_tail + 1) % RING_SIZE != rx_head) { // Is there space in rx_ring to receive a new char?
			rx_ring[rx_tail] = SBUF;
			rx_tail = (rx_tail + 1) % RING_SIZE;
		}
	}
}

void start_timer() { // Set timer 1 to be used in 8-bit autoreload mode as baudrate for the serial interface
	TR1 = 0; // Stop timer
	TMOD = (TMOD & 0xF0) | 0x20; // 8-bit autoreload. TLx is automatically reloaded from THx
	PCON = (PCON & 0x0F) | 0x80; // SMOD = 1
 	TH1 = BASE; 
	TL1 = BASE;
	TR1 = 1; // Continue timer
}

void start_serial() { // Set serial interface to be used in mode 1
	ES = 0; // Stop serial interface
	SCON = (SCON & 0x0F) | 0x50; // Mode 1, Reception enabled
	ES = 1; // Enable serial interface
}

void sendChar(char c) { // Append and/or trigger a transmission of a byte through the circular buffer tx_ring
	if ((tx_tail + 1) % RING_SIZE != tx_head) { // Is there space in tx_ring to append a char? 
		tx_ring[tx_tail] = c;
		tx_tail = (tx_tail + 1) % RING_SIZE;
	}
	
	if (!tx_busy) { // Trigger a transmission if a transmission isn't happening
		tx_busy = 1;
		TI = 1;
	}
}

void sendString(char *s) { // Transmit a string
	unsigned char i;
	for (i = 0; s[i] != '\0'; i++) sendChar(s[i]);
}

char RxBufferVazio() { // Check whether the circular buffer rx_ring is empty
	if (rx_head == rx_tail) return 1;
	else return 0;
}

char receiveChar() { // Return a byte from the circular buffer rx_ring
	char c;
	
	if (!RxBufferVazio()) { // Is there something to be received?
		c = rx_ring[rx_head];
		rx_head = (rx_head + 1) % RING_SIZE;
	} return '\0'; // Else, end of string
	
	return c;
}

void receiveString(char *s) { // Receive a string from buffer and copy to s
	unsigned char i = 0;
	char c;
	
	do {
		c = receiveChar();
		s[i] = c;
		i++;
	} while (c != '\0');
}

void main() {
	start_timer();
	start_serial();
	EA = 1; // Enable all interrupt
	
	while(1);
}