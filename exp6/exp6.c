#include <REG517A.H>
#include <string.h>

#define BASE 204  
#define RING_SIZE 16 // Length of circular buffers

unsigned char rx_ring[RING_SIZE]; // Reception circular buffer
unsigned char	tx_ring[RING_SIZE]; // Transmission circular buffer
unsigned char rx_head = 0, rx_tail = 0, tx_head = 0, tx_tail = 0, tx_busy = 0;

void serial_interrupt(void) interrupt 4 using 2 { // Interrupt while there's something to be transmitted or received
	if (TI0 == 1) {
		TI0 = 0;
		if (tx_head != tx_tail) { // Is there something to be transmitted?
				S0BUF = tx_ring[tx_head];
				tx_head = (tx_head + 1) % RING_SIZE;
		} else tx_busy = 0; // Not busy anymore (transmission done)
	}
	
	if (RI0 == 1) {
		RI0 = 0;
		if ((rx_tail + 1) % RING_SIZE != rx_head) { // Is there space in rx_ring to receive a new char?
			rx_ring[rx_tail] = S0BUF;
			rx_tail = (rx_tail + 1) % RING_SIZE;
		}
	}
}

void start_serial() { // Set serial interface 0 to be used in mode 1 and enable baud rate generator (9600 baud)
	ES0 = 0; // Stop serial interface 0
	S0CON = (S0CON & 0x0F) | 0x50; // Mode 1, Reception enabled
	BD = 1; // Baud rate enable (take from a dedicated prescaler)
	PCON = (PCON | 0x80); //SMOD = 1 (baud rate of serial interface 0 in modes 1, 2, 3 is doubled)
	ES0 = 1; // Enable serial interface 0
}

unsigned char sendChar(char c) { // Append and/or trigger a transmission of a byte through the circular buffer tx_ring
	unsigned char status = 0;
	
	if ((tx_tail + 1) % RING_SIZE != tx_head) { // Is there space in tx_ring to append a char? 
		tx_ring[tx_tail] = c;
		tx_tail = (tx_tail + 1) % RING_SIZE;
		status = 1;
	}
	
	if (!tx_busy) { // Trigger a transmission if a transmission isn't happening
		tx_busy = 1;
		TI0 = 1;
	}
	
	return status;
}

void sendString(char *s) { // Transmit a string
	unsigned char i = 0;
	
	while (s[i] != '\0') {
		if (sendChar(s[i])) 
			i++;
	}
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
	start_serial();
	EAL = 1; // Enable all interrupt
	
	while(1) {
		sendString("Hello World\n");
	}
}