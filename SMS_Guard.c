#include <avr/io.h>
#include <avr/interrupt.h>
#include "sim900.h"
#include "template.h"

#define LEDS_PORT PORTA
#define LEDS_DDR DDRA

#define BUTT_PORT PORTD
#define BUTT_DDR DDRD

#define POWER_ON_PIN 2
#define STATUS_PIN 1

#define BUTT_PIN 3

unsigned char status;

volatile unsigned char attentionFlag = FALSE;

int main( void )
{ 
 portsInit();
 sei();
 init();
 statusOk();

 for(int i=0; i<60; i++){
		delay(2000);
 }

 ints_init();

 for(;;){
   if (attentionFlag == TRUE){
   	   attentionFlag = FALSE;
   	   statusError();
	   sendMessage("+375447747024", "Attention! Someone crossed the area!");
   }
 }
}



void portsInit(){
	LEDS_DDR|=(1<<POWER_ON_PIN)|(1<<STATUS_PIN);
	LEDS_PORT&=~((1<<POWER_ON_PIN)|(1<<STATUS_PIN));
	SetBit(LEDS_PORT, POWER_ON_PIN);
}

void checkStatus(){
	 if (status==FALSE){
 		statusError();
 	}else{
 		statusOk();
 	}
}

void statusError(){
	for(int i=0; i<6; i++){
		SetBit(LEDS_PORT, STATUS_PIN);
		delay(400);
		ClrBit(LEDS_PORT, STATUS_PIN);
		delay(400);
	}
}

void statusOk(){
		SetBit(LEDS_PORT, STATUS_PIN);
		delay(3000);
		ClrBit(LEDS_PORT, STATUS_PIN);
}

void ints_init( void )
{
  MCUCR |= (1<<ISC01)|(1<<ISC00);
  SetBit(GICR, INT0);
}

ISR( INT0_vect )
{
 attentionFlag = TRUE;
 ClrBit(GICR, INT0);
}


