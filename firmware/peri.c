#include <avr/io.h>
#include "peri.h"

void init_peripheral()
{
/* LED --> OUTPUT*/
	DDRB  |= ((1<<LED1) | (1<<LED2) | (1<<LED3) | (1<<LED4));
	PORTB &= ~((1<<LED1) | (1<<LED2) | (1<<LED3) | (1<<LED4));
/* LED GROUND --> OUTPUT*/
	DDRB  |= ((1<<LEDG1) | (1<<LEDG2));
	PORTB |= ((1<<LEDG1) | (1<<LEDG2));
	DDRD  |= ((1<<LEDG3) | (1<<LEDG4));
	PORTD |= ((1<<LEDG3) | (1<<LEDG4));

/* SWITCH --> INPUT*/
	DDRC  &= ~((1<<SW1) | (1<<SW2) | (1<<SW3) | (1<<SW4));
	PORTC |= ((1<<SW1) | (1<<SW2) | (1<<SW3) | (1<<SW4));
/* SWITCH GROUND --> OUTPUT*/
	DDRC  |= ((1<<SWG1) | (1<<SWG2));
	PORTC |= ((1<<SWG1) | (1<<SWG2));
	DDRD  |= ((1<<SWG3) | (1<<SWG4));
	PORTD |= ((1<<SWG3) | (1<<SWG4));
}


/* SET LED FUNCTION */
void reset_led()
{	
	row1 = 0b0000 ;
	row2 = 0b0000 ;
	row3 = 0b0000 ;
	row4 = 0b0000 ;
}

void set_led_value(uint8_t row1, uint8_t row2, uint8_t row3, uint8_t row4, uint8_t col)
{
	set_led(LED1,(row1 &(1<<(4-col))));
	set_led(LED2,(row2 &(1<<(4-col))));
	set_led(LED3,(row3 &(1<<(4-col))));
	set_led(LED4,(row4 &(1<<(4-col))));
}

void set_led(uint8_t pin,uint8_t state)
{
	if(state != 0) {
		PORTB |= (1<<pin);
	} else {
   		PORTB &= ~(1<<pin);
	}
}

void reset_LedGround()
{
	PORTB |= ((1<<LEDG1) | (1<<LEDG2));
	PORTD |= ((1<<LEDG3) | (1<<LEDG4));
}


/* SET SWITCH FUNCTION */
void reset_switchGround()
{
    PORTC |= (1<<SWG1) | (1<<SWG2) ;
    PORTD |= (1<<SWG3) | (1<<SWG4) ;
}

uint8_t checkSwitch(uint8_t round)
{
    uint8_t read1 = IS_SW_PRESSED_1();
    uint8_t read2 = IS_SW_PRESSED_2();
    uint8_t read3 = IS_SW_PRESSED_3();
    uint8_t read4 = IS_SW_PRESSED_4();
    if(read1)
    {
      switch (round) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 4;
      }
    }
    if(read2)
    {
      switch (round) {
        case 1: return 5;
        case 2: return 6;
        case 3: return 7;
        case 4: return 8;
      }
    }
    if(read3)
    {
      switch (round) {
        case 1: return 9;
        case 2: return 10;
        case 3: return 11;
        case 4: return 12;
      }
    }
    if(read4)
    {
      switch (round) {
        case 1: return 13;
        case 2: return 14;
        case 3: return 15;
        case 4: return 16;
      }
    }
    return 0;
}
