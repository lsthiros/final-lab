//Michael Ebert, et al
//
//Lab 4 part 2
//
//Controls a robot based on the input from a sony remote
//

#include <avr/io.h>
#include <avr/interrupt.h>

#define NUMBER_OF_MOTORS 3
#define NUMBER_OF_EVENTS NUMBER_OF_MOTORS*2
volatile char motorOn[NUMBER_OF_MOTORS];

volatile char currentEvent=0;
#define CURRENT_MOTOR currentEvent>>1//because there are 2 events per motor,
				//divide current event by 2 to get 
				//current motor
volatile int timeDelay[NUMBER_OF_EVENTS];

volatile uint32_t msPassed=0;

char receiver_pin=2;

void initialize_receiver(void)
{
	//DDRD &= ~(1<<4);//4=button port
	PORTD |= (1<<4);
	if(PIND & (1<<4))
	{
		receiver_pin=2;//button not pressed
	}
	else
	{
		receiver_pin=3;
		PORTB |= (1<<5);//set onborad LED
	}
	DDRD &= ~(1<<receiver_pin);
}

void initialize_motor_variables(void)
{
	////////initialization////////
	DDRB |= (1<<0);//motor 1 port to output
	DDRB |= (1<<1);//motor 2 port to output
	//initialize variables to 0
	char i=0;
	for(i=0;i<NUMBER_OF_MOTORS;i++)
	{
		motorOn[i]=0;
		timeDelay[i*2]=0;
		timeDelay[(i*2)+1]=0;
	}
	////////end initialization////////
}

void initialize_timer(void)
{
	TCCR1B = 2;//increment timer at 2 MHz (2000 cycles/ms)
	TIMSK1 |= (1<<OCIE1A);//enable output compare match
	sei();//enable global interrupts
}

void initialize_adc(void)
{
	ADCSRA |= (1<<ADPS2) |  (1<<ADPS1) |  (1<<ADPS0);  //set prescalar
	ADMUX |= (1<<REFS0);  //set reference voltage
	ADMUX |= (1<<ADLAR);  //left align data
	ADCSRA |= (1<<ADEN);  //enable ADC
}
//initialize timer 0 for lab 4
void lab4_initialize_timer0(void) {
  TCCR0B |= (1 << CS02);   //set timer 0 to increment at 62.5KHz
}

void toggleMotor(char motorNumber)
{
	if(PORTB & (1<<motorNumber))// pin motornumber is on
	{//so we turn it off
		PORTB &= ~(1<<motorNumber);
	}
	else//PORTB & 1<<motorNumber is false- the pin is not on
	{//so we turn it on
		PORTB |= (1<<motorNumber);
	}
}

void set_motor_speed(char motor, int speed)
{
	speed= -speed*10;//[-100,100]->[1000,-1000]
	speed+=3000;     //[1000,-1000]->[4000,2000]=2 to 1 ms
	if(motor==1)//hardcoded kludge so motor 1 runs backwards
	{
		speed= 6000-speed;//[4000,2000]->[2000,4000]
	}
	timeDelay[(motor*2)]=speed;//motor*2 = index of motor on delay
	timeDelay[(motor*2)+1]=(6000-speed);//index of motor off delay
}

char getFallingEdge(int usTimeout)
{
	char a=0;
	char b=0;
	char isFallingEdge=0;
	int tcntTimeout=usTimeout*625/10000;//625/10000 ticks/us
	TCNT0=0;
	while(isFallingEdge==0 && (TCNT0<tcntTimeout || tcntTimeout==0))
	{
		b=a;
		a=(PIND & (1<<receiver_pin));
		if(b!=0 && a==0)
		{
			isFallingEdge=1;
		}
	}
	return isFallingEdge;
}

char getStartBit(void)
{
	while(1)
	{
		getFallingEdge(0);//loop forever until falling edge found
		//start timer for 2.5 m
		if(getFallingEdge(2500)==0)
		{
			return 1;
		}

	}
}

char getDataBit(void)
{
	if(getFallingEdge(1200)==0)
	{
		return 2;
	}
	if(getFallingEdge(900))
	{
		return 2;
	}
	return !(PIND & (1<<receiver_pin));//read data bit and flip
}

char getDataBits(void)
{
	char dataByte=0;
	char i=0;
	char dataBitArray[7];
	for(i=0;i<7;i++)
	{
		dataBitArray[i]=getDataBit();
		if(dataBitArray[i]==2)
		{
			return (1<<7);//failed transmission
		}
		dataByte |= (dataBitArray[i]<<i);//stuff dataBit into array
	}
	return dataByte;
}

char getRemoteData()
{
	char isValidData=0;
	char dataByte=0;
	while(isValidData==0)
	{

		getStartBit();
		dataByte=getDataBits();
		if(dataByte>=(1<<7))//error! restart read
		{
			isValidData=0;
		}
		else
		{
			isValidData=1;
		}
	}
	return dataByte;
}




//Events always go in the same order - motor1 on, 1off, motor2on, 2off,etc.
ISR(TIMER1_COMPA_vect)
{
	motorOn[CURRENT_MOTOR]= !motorOn[CURRENT_MOTOR];
	toggleMotor(CURRENT_MOTOR);//toggle current motor
	OCR1A+=timeDelay[currentEvent];//set up timer for next event
	
	currentEvent++;//increment current event
	if(currentEvent>=NUMBER_OF_EVENTS)//We have reached the last
	       //event in the order
	{
		//time padding-         20 ms -    3 ms   * number of motors
		OCR1A += 22000;// ( (20000*2) - ((3000*2) * NUMBER_OF_MOTORS));
		currentEvent=0;//go back to the beginning
		msPassed+=20;
		if(msPassed>=120000)//2 minutes have passed
		{
			while(1){}
		}
	}
}

int main(void)
{
	DDRB |= (1<<5);
	initialize_receiver();
	initialize_motor_variables();
	initialize_adc();
	lab4_initialize_timer0();
	volatile char remoteData=0;
	initialize_timer();//this goes last so we don't have to worry about
	//being interrupted during initialization.
	set_motor_speed(1,0);
	set_motor_speed(0,0);
	while(1)
	{
		remoteData=getRemoteData();
		if(remoteData==16)//Left Forwards
		{
			set_motor_speed(0,100);
		}
		if(remoteData==17)//left backwards
		{		
			set_motor_speed(0,-100);
		}
		if(remoteData==18)//left stop
		{

			set_motor_speed(0,0);
		}	
		if(remoteData==104)//right forwards
		{
			set_motor_speed(1,100);
		}
		if(remoteData==105)//right backwards
		{
			set_motor_speed(1,-100);
		}	
		if(remoteData==106)//right stop
		{
			set_motor_speed(1,0);
		}
		if(remoteData==107)//both forwards
		{
			set_motor_speed(0,100);
			set_motor_speed(1,100);
		}
		if(remoteData==109)//arm up
		{
			set_motor_speed(2,100);
		}
		if(remoteData==110)//arm down
		{
			set_motor_speed(2,-100);
		}
	}
	return 0;
}

