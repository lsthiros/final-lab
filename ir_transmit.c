/**
 * IR transmitter code
 * Louie Thiros
 * Using input from four buttons, sends commands to a
 * matching robot. Sends commands at one of two frequencies.
 * Commands are sent as a single byte between 101-110.
 */
#include<avr/io.h> 
#include<avr/interrupt.h>
#include<util/delay.h>

#define IR_LED_PORT PD4
#define HIGH_US_38KHZ 7
#define LOW_US_38KHZ 20
#define HIGH_US_56KHZ 8
#define LOW_US_56KHZ 10

//Global variables for switching carrier frequencies
//Variables are global for timing correctness
unsigned char loop_count;
char logic_one_delay, logic_zero_delay;
char frequency;

void set_frequency_38(void) {
  frequency = 38;
  loop_count = 89;  //number of periods in 2.4ms
  logic_one_delay = 45;
  logic_zero_delay = 22;
}

void set_frequency_56(void) {
  frequency = 56;
  loop_count = 141;  //number of periods in 2.4ms
  logic_one_delay = 70;
  logic_zero_delay = 35;
}

void send_start_bit() {
  unsigned char i;

  for (i=0;i<loop_count;i++) {
    PORTD |= (1 << IR_LED_PORT);  //turn on the IR LED
    if (frequency == 38) {
      _delay_us(HIGH_US_38KHZ);
    } else {
      _delay_us(HIGH_US_56KHZ);
    }

    PORTD &= ~(1 << IR_LED_PORT);  //turn off the IR LED
    if (frequency == 38) {
      _delay_us(LOW_US_38KHZ);
    } else {
      _delay_us(LOW_US_56KHZ);
    }
  }

  _delay_us(600);
}

void send_command(unsigned char command) {
  char i,j,delay;

  for (i=0;i<8;i++) {  //run 8 times
    if ((command & (1 << i)) != 0) {
      delay = logic_one_delay;
    } else {
      delay = logic_zero_delay;
    }

    for (j=0;j<delay;j++) {
      PORTD |= (1 << IR_LED_PORT); //turn on the IR LED
      if (frequency == 38) {
        _delay_us(HIGH_US_38KHZ);
      } else {
        _delay_us(HIGH_US_56KHZ);
      }

      PORTD &= ~(1 << IR_LED_PORT);  //turn off the IR LED
      if (frequency == 38) {
        _delay_us(LOW_US_38KHZ);
      } else {
        _delay_us(LOW_US_56KHZ);
      }
    }

    _delay_us(600);
  }
}

void blink() {
  PORTB |= (1<<DEBUG_LED);
  _delay_ms(300);
  PORTB &= ~(1<<DEBUG_LED);
}

int main(void) {
  DDRD |= (1 << IR_LED_PORT);  //set the IR port pin to output

  if(!(PIND & (1<<FREQUENCY_SELECT))) {
    set_frequency_56;
    blink();
    blink();
  } else {
    set_frequency_38();
    blink();
  }
  char command;
  while(1) {
    command=PINB;
    //returns true for either arm moving button pressed
    if(!(command & ((1<<PB5)|(1<<PB4)))) {
      if(!(command & (1<<PB5)) {
        //send command to move arm down
      } else {
        //send command to move arm up
      }
      break;
    }
    send_start_bit();
    send_command(18); //send code for Volume up 
    _delay_ms(100);   //send no more than 1 command every 100ms
  }

  return 0;
}
