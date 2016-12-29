uint8_t row1;
uint8_t row2;
uint8_t row3;
uint8_t row4;
uint8_t col;


/* SW 	= SWITCH 	--> ROW
   SWG	= SWITCH GROUND	--> COL */
#define SW1    PC0
#define SW2    PC1
#define SW3    PC2
#define SW4    PC3
#define SWG1   PC4
#define SWG2   PC5
#define SWG3   PD5
#define SWG4   PD6


/* LED  = LED		--> ROW
   LEDG = LED GROUND  	--> COL */
#define LED1   PB0
#define LED2   PB1
#define LED3   PB2
#define LED4   PB3
#define LEDG1  PB4
#define LEDG2  PB5
#define LEDG3  PD0
#define LEDG4  PD1


#define IS_SW_PRESSED_1() ((PINC & (1<<SW1))==0)
#define IS_SW_PRESSED_2() ((PINC & (1<<SW2))==0)
#define IS_SW_PRESSED_3() ((PINC & (1<<SW3))==0)
#define IS_SW_PRESSED_4() ((PINC & (1<<SW4))==0)


/* SET BOARD */
void init_peripheral();


/* SET SWITCH */
void reset_switchGround();
uint8_t checkSwitch(uint8_t round);


/* SET LED */
void reset_LedGround();
void set_led_value(uint8_t row1, uint8_t row2, uint8_t row3, uint8_t row4, uint8_t col);
void set_led(uint8_t pin, uint8_t state);
void reset_led();

