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

#define IR_LED_PORT PD2
#define HIGH_US_38KHZ 7
#define LOW_US_38KHZ 20
#define HIGH_US_56KHZ 8
#define LOW_US_56KHZ 10

#define FREQUENCY_SELECT PD4
#define DEBUG_LED PD7

#define BUTTON1 PB0
#define BUTTON2 PB1
#define BUTTON3 PB2
#define BUTTON4 PB3
#define BUTTON5 PB4

#define B1_UP (PINB & (1<<BUTTON1))
#define B1_DOWN !B1_UP
#define B2_UP (PINB & (1<<BUTTON2))
#define B2_DOWN !B2_UP
#define B3_UP (PINB & (1<<BUTTON3))
#define B3_DOWN !B3_UP
#define B4_UP (PINB & (1<<BUTTON4))
#define B4_DOWN !B4_UP
#define B5_UP (PINB & (1<<BUTTON5))
#define B5_DOWN !B5_UP

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
	send_start_bit();
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
  PORTD |= (1<<DEBUG_LED);
  _delay_ms(300);
  PORTD &= ~(1<<DEBUG_LED);
  _delay_ms(300);
}

/*int main(void)
{
	DDRB &= ~(1<<0);
	DDRD |= (1<<7);
	PORTB |= (1<<0);
	PORTD |= (1<<7);//turn led on
	while(1)
	{
		if(PINB & (1<<0))
		{
			PORTD |= (1<<7);
		}
		else
		{
			PORTD &= ~(1<<7);
		}
	}
	return 0;
}*/
	


int main(void) {
	DDRD |= (1<<DEBUG_LED) | (1 << IR_LED_PORT);  //set the IR port pin to output
	PORTB |= (1<<BUTTON5) | (1<<BUTTON4) | (1<<BUTTON3) | (1<<BUTTON2) | (1<<BUTTON1);
	PORTD |= (1<<FREQUENCY_SELECT);
	DDRB |= (1<<5);

	if(!(PIND & (1<<FREQUENCY_SELECT))) {
		set_frequency_56();
		PORTB |=(1<<5);
		blink();
		blink();
	} else {
		set_frequency_38();
		blink();
	}

	//command scheme:
	//B1down: Left forwards
	//B2down: left backwards
	//b1up&b2up: left stop
	//B3down: right forwards
	//b4down: right bakwards
	//b3&b4up: right stop
	//b5down: 3 down(not yet implemented)
	//b5 up: 3 up (not yet implemented)


	char bstate=B1_DOWN;
	char bstate1=B2_DOWN;
	while(1)
	{
		if(bstate!=B1_DOWN)
		{
			bstate=B1_DOWN;
		if(B1_DOWN)
		{
			//send_start_bit();
			send_command(16);
		}
		}
		if(B2_DOWN)
		{
			send_command(17);
		}
		if(B1_UP && B2_UP)
		{
			send_command(18);
		}
		/*if(B3_DOWN)
		{
			send_command(104);
		}
		if(B4_DOWN)
		{
			send_command(105);
		}
		if(B3_UP && B4_UP)
		{
			send_command(106);
		}/**/
	}
	return 0;
}


 /* while(1) {
    CONTROL_PORT=CONTROL_PORT;
    //returns true for either arm moving button pressed
    if((CONTROL_PORT & (1<<BUTTON4)) == 0 || !(CONTROL_PORT & (1<<BUTTON5))) {
      blink();
      if(!(CONTROL_PORT & (1<<BUTTON5))) {
        //send command to move arm down
        send_start_bit();
        send_command(110);
      } else {
        //send command to move arm up
	send_start_bit();
	send_command(109);
      }
      break;
    } else if (!(CONTROL_PORT & ((1<<BUTTON1) | (1<<BUTTON2)))) {
      switch(~(CONTROL_PORT & (1<<BUTTON1) | (1<<BUTTON2))) {
        case (1<<BUTTON1):
	  if(!(CONTROL_PORT & (1<<BUTTON3))) {
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
	  if(!(CONTROL_PORT & (1<<BUTTON3))) {
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
	  if(!(CONTROL_PORT & (1<<BUTTON3))) {
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
}*/
