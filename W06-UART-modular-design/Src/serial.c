#include "serial.h"

#include "stm32f303xc.h"



// We store the pointers to the GPIO and USART that are used
//  for a specific serial port. To add another serial port
//  you need to select the appropriate values.
struct _SerialPort {
	USART_TypeDef *UART;
	GPIO_TypeDef *GPIO;
	volatile uint32_t MaskAPB2ENR;	// mask to enable RCC APB2 bus registers
	volatile uint32_t MaskAPB1ENR;	// mask to enable RCC APB1 bus registers
	volatile uint32_t MaskAHBENR;	// mask to enable RCC AHB bus registers
	volatile uint32_t SerialPinModeValue;
	volatile uint32_t SerialPinSpeedValue;
	volatile uint32_t SerialPinAlternatePinValueLow;
	volatile uint32_t SerialPinAlternatePinValueHigh;
	void (*completion_function)(uint32_t);
	//volatile uint8_t incoming_buffer[BUFFER_SIZE];
	//volatile uint8_t buffer_count;
};



// instantiate the serial port parameters
//   note: the complexity is hidden in the c file
SerialPort USART1_PORT = {USART1,
		GPIOC,
		RCC_APB2ENR_USART1EN, // bit to enable for APB2 bus
		0x00,	// bit to enable for APB1 bus
		RCC_AHBENR_GPIOCEN, // bit to enable for AHB bus
		0xA00,
		0xF00,
		0x770000,  // for USART1 PC10 and 11, this is in the AFR low register
		0x00, // no change to the high alternate function register
		0x00, // default function pointer is NULL
		//0x00,	//incoming buffer is NULL by default
		//0x00	//buffer counter begins at 0
		};


// InitialiseSerial - Initialise the serial port
// Input: baudRate is from an enumerated set
void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint32_t)) {

	serial_port->completion_function = completion_function;

	// enable clock power, system configuration clock and GPIOC
	// common to all UARTs
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// enable the GPIO which is on the AHB bus
	RCC->AHBENR |= serial_port->MaskAHBENR;

	// set pin mode to alternate function for the specific GPIO pins
	serial_port->GPIO->MODER = serial_port->SerialPinModeValue;

	// enable high speed clock for specific GPIO pins
	serial_port->GPIO->OSPEEDR = serial_port->SerialPinSpeedValue;

	// set alternate function to enable USART to external pins
	serial_port->GPIO->AFR[0] |= serial_port->SerialPinAlternatePinValueLow;
	serial_port->GPIO->AFR[1] |= serial_port->SerialPinAlternatePinValueHigh;

	// enable the device based on the bits defined in the serial port definition
	RCC->APB1ENR |= serial_port->MaskAPB1ENR;
	RCC->APB2ENR |= serial_port->MaskAPB2ENR;

	// Get a pointer to the 16 bits of the BRR register that we want to change
	uint16_t *baud_rate_config = (uint16_t*)&serial_port->UART->BRR; // only 16 bits used!

	// Baud rate calculation from datasheet
	switch(baudRate){
	case BAUD_9600:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x341;  // 9600 at 8MHz				//needs to be finished
		break;
	case BAUD_19200:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x1A1;  // 19200 at 8MHz
		break;
	case BAUD_38400:
		// NEED TO FIX THIS !
		*baud_rate_config = 0xD0;  // 38400 at 8MHz
		break;
	case BAUD_57600:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x8B;  // 57600 at 8MHz
		break;
	case BAUD_115200:
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	}


	// enable serial port for tx and rx
	serial_port->UART->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}


void SerialOutputChar(uint8_t data, SerialPort *serial_port) {

	while((serial_port->UART->ISR & USART_ISR_TXE) == 0){
	}

	serial_port->UART->TDR = data;
}



void SerialOutputString(uint8_t *pt, SerialPort *serial_port) {

	uint32_t counter = 0;
	while(*pt != '0') {
		SerialOutputChar(*pt, serial_port);
		counter++;
		pt++;
	}

	serial_port->completion_function(counter);
}

void SerialIntputChar(uint8_t *data, SerialPort *serial_port) {

	while((serial_port->UART->ISR & USART_ISR_RXNE) == 0){
		//wait until there is data to be received
	}

	*data = serial_port->UART->RDR;
}

void SerialInputString(uint8_t *buffer, SerialPort *serial_port) {
    uint32_t counter = 0;
    while (1) {
        uint8_t received_char;
        SerialIntputChar(&received_char, serial_port); // Receive a character

        if (received_char == '\r') {
            // If carriage return is received, terminate the string
        	buffer[counter] = '0';
            break;
        }

        buffer[counter++] = received_char; // Store received character in buffer

        if (counter >= BUFFER_SIZE - 1) {
            // Prevent Buffer Overflow
            break;
        }
    }
}

/*void SerialInputString(uint8_t *pt, SerialPort *serial_port) {
    uint32_t counter = 0;
    while (1) {
        uint8_t received_char;
        SerialIntputChar(&received_char, serial_port); // Receive a character

        if (received_char == '\r') {
            // If carriage return is received, terminate the string
            break;
        }
        serial_port->incoming_buffer[counter++] = received_char;
        //buffer[counter++] = received_char; // Store received character in buffer
        buffer_count++;
        if (counter >= BUFFER_SIZE) {
            // Prevent Buffer Overflow
            break;
        }
    }
    uint8_t storage_buffer[BUFFER_SIZE];

    for(int i = 0; i <= buffer_count, i++){
    	storage_buffer[i]= incoming_buffer[i];
    }
}*/




