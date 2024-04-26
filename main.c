/* Universidad del Valle de Guatemala
 * Programacion de microcontroladores
 * Laboratorio 6.c
 *
 * 4/19/2024 
 * Josue Castro
 */  
#define F_CPU 16000000
#define entrada0 48
#define entrada1 49
#define entrada2 50

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void initUART9600(void);
void initADC(void);
void writeUART(char Caracter);
void writeUARTEXT(char * Texto);
void PROCESASCII(uint8_t);

uint8_t ASCII = 0;
volatile uint8_t entrada = 0;
volatile uint8_t buffertx = 0;
volatile uint8_t potValue = 0;
volatile uint8_t flag = 0;


int main(void)
{
	cli();
	DDRB = 0xFF;
	DDRD = 0xFF;
	initUART9600();
	initADC();
	sei();
	while (1)
	{
		if(flag == 1){
			
			entrada = 0;
			writeUART(10);
			writeUART(13);
			writeUARTEXT("Hola, selecciona una opcion");
			writeUART(10);
			writeUART(13);
			writeUARTEXT("1.Leer Potenciometro");
			writeUART(10);
			writeUART(13);
			writeUARTEXT("2.Enviar ASCII");
			writeUART(10);
			writeUART(13);
			while(!(entrada == entrada1 || entrada == entrada2));
			
			switch(entrada) {
				case entrada1: 
				
				ADCSRA |= (1 << ADSC);
				flag = 0;
				_delay_ms(30);
				break;
				
				case entrada2:
				
				writeUART(10);
				writeUART(30);
				writeUARTEXT("Ingrese ASCII");
				writeUART(10);
				writeUART(13);
				flag = 0;
				while(flag == 0);
				break;
			}
		}
		flag = 1;
	}//while
}//main

void initUART9600(void){

	DDRD &= ~(1<<DDD0);
	DDRD |= (1<<DDD1);
	UCSR0A = 0;
	//Configurar el UCSR0B, tx y rx
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0) | (1<<RXEN0) |(1<<TXEN0);
	UCSR0C = 0;
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
	UBRR0 = 103;

}

void initADC(void){

	// Lectura del ADC
	ADMUX = 0;
	ADMUX |= (1 << REFS0)| (1 << ADLAR);
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1)| (1 <<ADPS0);
	DIDR0 |= (1 << ADC0D); // Desactivar entrada digital para pin PC0
}
void writeUART(char Caracter) {
	while(!(UCSR0A & (1<<UDRE0))); //UCSR0A sea 1
	UDR0 = Caracter;
}

void writeUARTEXT(char * Texto) {
	uint8_t i;
	for(i=0; Texto[i]!='\0'; i++){
		while (!(UCSR0A & (1<<UDRE0)) );
		UDR0 = Texto[i];
	}
}

void PROCESASCII(uint8_t crudo) {
	uint8_t unidades;
	uint8_t decenas;
	uint8_t centenas;
	
	unidades = crudo % 10;
	crudo = (crudo - unidades)/10;
	decenas = crudo % 10;
	crudo = ( crudo - decenas)/10;
	centenas = crudo % 10;
	writeUART(entrada0 + centenas);
	writeUART(entrada0 + decenas);
	writeUART(entrada0 + unidades);
	writeUART(10);
	writeUART(30);
}


ISR(USART_RX_vect) {
	buffertx = UDR0;
	entrada = buffertx;
	
	if(flag == 1){
		if(entrada == entrada1){
			writeUART(10);
			writeUART(30);
			writeUARTEXT("Valor Potenciometro: ");
		}
		else if(entrada == entrada2) {
			writeUART(10);
			writeUART(13);
			writeUARTEXT("Valor ASCII: ");
			writeUART(10);
			writeUART(13);
		}
		else {
			writeUART(10);
			writeUART(13);
			writeUARTEXT("Por favor ingresa 1 o 2");
			writeUART(10);
			writeUART(13);
		}
	}
	if(flag == 0){
		
		PORTD = (buffertx & 0b00111111)* 0b00000100;
		PORTB = (buffertx >> 6) & 0b00000011;
		flag = 1;
	}
	
}
ISR(ADC_vect){
	ASCII = ADCH;
	PROCESASCII(ASCII);
	PORTD = (ASCII & 0b00111111)* 0b00000100;
	PORTB = (ASCII >> 6) & 0b00000011;
	
	ADCSRA |= (1 << ADIF);	//LIMPIA LA BANDERA
}