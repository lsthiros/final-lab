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

#define FREQUENCY_SELECT PD2
#define DEBUG_LED PD7

#define BUTTON1 PB1
#define BUTTON2 PB2
#define BUTTON3 PB3
#define BUTTON4 PB4
#define BUTTON5 PB5

#define CONTROL_PORT PORTB

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
  DDRD |= (1 << IR_LED_PORT) | (1 << FREQUENCY_SELECT);  //set the IR port pin to output

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
    command=CONTROL_PORT;
    //returns true for either arm moving button pressed
    if(!(command & ((1<<BUTTON5)|(1<<BUTTON4)))) {
      if(!(command & (1<<BUTTON5))) {
        //send command to move arm down
        send_start_bit();
        send_command(110);
      } else {
        //send command to move arm up
	send_start_bit();
	send_command(109);
      }
      break;
    } else if (!(command & ((1<<BUTTON1) | (1<<BUTTON2)))) {
      switch(~(command & (1<<BUTTON1) | (1<<BUTTON2))) {
        case (1<<BUTTON1):
	  if(!(command & (1<<BUTTON3))) {
	    //move left motor in reverse
	    send_start_bit();
	    send_command(103);
	  } else {
	    //move left motor forwards
	    send_start_bit();
	    send_command(106);
	  };
	  break;
	case (1<<BUTTON2):
	  if(!(command & (1<<BUTTON3))) {
	    //move right motor in reverse
	    send_start_bit();
	    send_command(102);
	  } else {
	    //move right motor forwards
	    send_start_bit();
	    send_command(105);
	  }
	  break;
	case (1<<BUTTON1) | (1<<BUTTON2): 
	  if(!(command & (1<<BUTTON3))) {
	    //move both in reverse
	    send_start_bit();
	    send_command(107);
	  } else {
	    //move both backwards
	    send_start_bit();
	    send_command(104);
	  }
      }
    }
  }

  return 0;
}
