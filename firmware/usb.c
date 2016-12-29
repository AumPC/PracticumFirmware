#include <stddef.h>         /* for NULL */
#include <avr/io.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/pgmspace.h>   /* required by usbdrv.h */

#include "pt/pt.h"
#include "peri.h"
#include "timer.h"
#include "usbdrv.h"

#define RQ_GET_BUTTON     1
#define RQ_SET_FUNCTION   2

// นิยามมาโคร PT_DELAY ไว้เพื่อจำลองการหน่วงเวลาแบบมัลติทาสกิ้ง
#define PT_DELAY(pt, ms, ts) \
    ts = timer_millis(); \
    PT_WAIT_WHILE(pt, timer_millis()-ts < (ms));
 
struct pt pt_taskGetSwitch;
struct pt pt_taskSetLED;
struct pt pt_taskClear;
struct pt pt_taskRePress;
struct pt pt_taskF1;
struct pt pt_taskF2;
struct pt pt_taskF3;
struct pt pt_taskF4;
struct pt pt_taskF5;
struct pt pt_taskF6;
struct pt pt_taskF7;
struct pt pt_taskF8;
struct pt pt_taskF9;
struct pt pt_taskF10;
struct pt pt_taskF11;
struct pt pt_taskF12;
struct pt pt_taskF13;
struct pt pt_taskF14;
struct pt pt_taskF15;
struct pt pt_taskF16;

uint16_t tempFunct;
uint16_t function;
uint16_t swicth_last;
uint8_t  switch_press;

/* ----------------------------------------------------------------- */
/* ------------------------- USB interface ------------------------- */
/* ----------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uint8_t data[8])
{
    usbRequest_t *rq = (void *)data;

    /* declared as static so they stay valid when usbFunctionSetup returns */
    static uint8_t switch_state;  
    static uint16_t light_value;

    if (rq->bRequest == RQ_GET_BUTTON)
    {
        switch_state = switch_press;
		usbMsgPtr = &switch_state;
        return 1;
    }    
	else if (rq->bRequest == RQ_SET_FUNCTION)
    {        
		function = rq->wValue.word;
        return 0;
    }
    return 0;
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */
PT_THREAD(taskGetSwitch(struct pt* pt))
{
    static uint32_t ts;
    uint8_t round = 1;
    PT_BEGIN(pt);
	
    switch_press = 0;
    for (;;)
    {
        round = 1;
        reset_switchGround();
        PORTC &= ~(1<<SWG1);
        if(checkSwitch(round) != 0)
        {
          switch_press = checkSwitch(round);
        }
        PT_DELAY(pt,5,ts);

        round = 2;
        reset_switchGround();
        PORTC &= ~(1<<SWG2);
        if(checkSwitch(round) != 0)
        {
          switch_press = checkSwitch(round);
        }
        PT_DELAY(pt,5,ts);

		round = 3;
        reset_switchGround();
        PORTD &= ~(1<<SWG3);
        if(checkSwitch(round) != 0)
        {
          switch_press = checkSwitch(round);
        }
        PT_DELAY(pt,5,ts);

        round = 4;
        reset_switchGround();
        PORTD &= ~(1<<SWG4);
        if(checkSwitch(round) != 0)
        {
          switch_press = checkSwitch(round);
        }
        PT_DELAY(pt,5,ts);
		if(tempFunct != switch_press && switch_press != 0)
		{
			tempFunct = switch_press;
		}
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskRePress(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
    for (;;)
    {
		PT_YIELD_UNTIL(pt,function == 25);
		switch_press = 0;
		function = 0;		
		PT_DELAY(pt,1,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskClear(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
    for (;;)
    {
		PT_YIELD_UNTIL(pt,function == 20);
		switch_press = 0;	
		tempFunct = 0;	
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		set_led_value(row1,row2,row3,row4,col);
		function = 0;		
		PT_DELAY(pt,1000,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskSetLED(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	reset_led();
    for (;;)
    {
	//	PT_YIELD(pt);
		col = 1;
		reset_LedGround();
		PORTB &= ~(1<<LEDG1) ;
		set_led_value(row1,row2,row3,row4,col);
		PT_DELAY(pt,5,ts);

		col = 2;
		reset_LedGround();
		PORTB &= ~(1<<LEDG2) ;
		set_led_value(row1,row2,row3,row4,col);
		PT_DELAY(pt,5,ts);
	
		col = 3;	
		reset_LedGround();
		PORTD &= ~(1<<LEDG3) ;
		set_led_value(row1,row2,row3,row4,col);
		PT_DELAY(pt,5,ts);
	
		col = 4;
		reset_LedGround();
		PORTD &= ~(1<<LEDG4) ;
		set_led_value(row1,row2,row3,row4,col);
		PT_DELAY(pt,5,ts);
		
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF1(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 1);
		row1 = 0b1000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0100;
		row2 = 0b1000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0010;
		row2 = 0b0000;
		row3 = 0b1000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0001;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b1000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF2(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 2);
		row1 = 0b0100;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b1010;
		row2 = 0b0100;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0001;
		row2 = 0b0000;
		row3 = 0b0100;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0100;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF3(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 3);
		row1 = 0b0010;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0101;
		row2 = 0b0010;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b1000;
		row2 = 0b0000;
		row3 = 0b0010;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0010;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF4(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 4);
		row1 = 0b0001;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0010;
		row2 = 0b0001;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0100;
		row2 = 0b0000;
		row3 = 0b0001;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b1000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0001;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF5(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 5);
		row1 = 0b0000;
		row2 = 0b1000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b1000;
		row2 = 0b0100;
		row3 = 0b1000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0010;
		row3 = 0b0000;
		row4 = 0b1000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0001;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF6(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 6);
		row1 = 0b0000;
		row2 = 0b0100;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0100;
		row2 = 0b1010;
		row3 = 0b0100;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0001;
		row3 = 0b0000;
		row4 = 0b0100;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF7(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 7);
		row1 = 0b0000;
		row2 = 0b0010;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0010;
		row2 = 0b0101;
		row3 = 0b0010;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b1000;
		row3 = 0b0000;
		row4 = 0b0010;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF8(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 8);
		row1 = 0b0000;
		row2 = 0b0001;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0001;
		row2 = 0b0010;
		row3 = 0b0001;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0100;
		row3 = 0b0000;
		row4 = 0b0001;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b1000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF9(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 9);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b1000;
		row3 = 0b0100;
		row4 = 0b1000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b1000;
		row2 = 0b0000;
		row3 = 0b0010;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0001;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF10(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 10);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0100;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0100;
		row3 = 0b1010;
		row4 = 0b0100;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0100;
		row2 = 0b0000;
		row3 = 0b0001;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF11(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 11);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0010;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0010;
		row3 = 0b0101;
		row4 = 0b0010;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0010;
		row2 = 0b0000;
		row3 = 0b1000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF12(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 12);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0001;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0001;
		row3 = 0b0010;
		row4 = 0b0001;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0001;
		row2 = 0b0000;
		row3 = 0b0100;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b1000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
	}
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF13(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 13);
    	row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b1000;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b1000;
		row4 = 0b0100;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0000;
		row2 = 0b1000;
		row3 = 0b0000;
		row4 = 0b0010;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b1000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0001;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF14(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 14);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0100;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0100;
		row4 = 0b1010;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0000;
		row2 = 0b0100;
		row3 = 0b0000;
		row4 = 0b0001;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0100;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF15(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 15);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0010;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0010;
		row4 = 0b0101;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0000;
		row2 = 0b0010;
		row3 = 0b0000;
		row4 = 0b1000;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0010;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

PT_THREAD(taskF16(struct pt* pt))
{
    static uint32_t ts;
    PT_BEGIN(pt);
	uint16_t delay = 200;
    for (;;)
    {
		PT_YIELD_UNTIL(pt,tempFunct == 16);
		row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0001;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0001;
		row4 = 0b0010;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0000;
		row2 = 0b0001;
		row3 = 0b0000;
		row4 = 0b0100;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0001;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b1000;
		PT_DELAY(pt,delay,ts);
    	row1 = 0b0000;
		row2 = 0b0000;
		row3 = 0b0000;
		row4 = 0b0000;
		PT_DELAY(pt,delay,ts);
    }
    PT_END(pt);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

int main(void)
{
    init_peripheral();

    usbInit();

    /* enforce re-enumeration, do this while interrupts are disabled! */
    usbDeviceDisconnect();
    _delay_ms(300);
    usbDeviceConnect();

    timer_init();

    /* enable global interrupts */
    sei();

    PT_INIT(&pt_taskGetSwitch);
    PT_INIT(&pt_taskSetLED);
	PT_INIT(&pt_taskClear);
	PT_INIT(&pt_taskRePress);
	PT_INIT(&pt_taskF1);
	PT_INIT(&pt_taskF2);
	PT_INIT(&pt_taskF3);
	PT_INIT(&pt_taskF4);
	PT_INIT(&pt_taskF5);
	PT_INIT(&pt_taskF6);
	PT_INIT(&pt_taskF7);
	PT_INIT(&pt_taskF8);
	PT_INIT(&pt_taskF9);
	PT_INIT(&pt_taskF10);
	PT_INIT(&pt_taskF11);
	PT_INIT(&pt_taskF12);
	PT_INIT(&pt_taskF13);
	PT_INIT(&pt_taskF14);
	PT_INIT(&pt_taskF15);
	PT_INIT(&pt_taskF16);

    tempFunct = 0;
    /* main event loop */
    for(;;)
    {
        usbPoll();
        taskGetSwitch(&pt_taskGetSwitch);
        taskSetLED(&pt_taskSetLED);
        taskClear(&pt_taskClear);
		taskRePress(&pt_taskRePress);
        taskF1(&pt_taskF1);
        taskF2(&pt_taskF2);
        taskF3(&pt_taskF3);
		taskF4(&pt_taskF4);
        taskF5(&pt_taskF5);
        taskF6(&pt_taskF6);
        taskF7(&pt_taskF7);
		taskF8(&pt_taskF8);
        taskF9(&pt_taskF9);
        taskF10(&pt_taskF10);
        taskF11(&pt_taskF11);
		taskF12(&pt_taskF12);
        taskF13(&pt_taskF13);
        taskF14(&pt_taskF14);
		taskF15(&pt_taskF15);
        taskF16(&pt_taskF16);
    }

    return 0;
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */
