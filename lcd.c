

#define LCDPORT PORTC
#define PORTDELAY  _delay_us(3);
#define TEXTDELAY  _delay_us(3);


#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>
#include "lcd.h"

//#define	__AVR_ATMEGA128__	1

//unsigned char data, Line = 0;
//char Text[16], Ch;
//unsigned int Bl = 1, LCD_State = 0, i, j;

/*****************************L C D**************************/

void E_Pulse()
{
	LCDPORT = LCDPORT | 0b00000100;	//set E to high
	_delay_ms(1) ;				//delay ~110ms
	LCDPORT = LCDPORT & 0b11111011;	//set E to low
}

void LCD_Init()
{
	//LCD initialization
	//step by step (from Gosho) - from DATASHEET

	LCDPORT = LCDPORT & 0b11111110;
	
	_delay_ms(200);


	LCDPORT = 0b00110000;						//set D4, D5 port to 1
	PORTDELAY
	E_Pulse();								//high->low to E port (pulse)
	PORTDELAY 

	LCDPORT = 0b00110000;						//set D4, D5 port to 1
	PORTDELAY
	E_Pulse();								//high->low to E port (pulse)
	PORTDELAY

	LCDPORT = 0b00110000;						//set D4, D5 port to 1
	PORTDELAY
	E_Pulse();								//high->low to E port (pulse)
	PORTDELAY

	LCDPORT = 0b00100000;						//set D4 to 0, D5 port to 1
	PORTDELAY
	E_Pulse();								//high->low to E port (pulse)
}

void LCDSendCommand(unsigned char a)
{
	unsigned char data = 0b00001111 | a;					//get high 4 bits
	LCDPORT = (LCDPORT | 0b11110000) & data;	//set D4-D7
	PORTDELAY
	LCDPORT = LCDPORT & 0b11111110;				//set RS port to 0
	PORTDELAY
	E_Pulse(); 
	
	_delay_us(500);                             //pulse to set D4-D7 bits

	data = a<<4;							//get low 4 bits
	LCDPORT = (LCDPORT & 0b00001111) | data;	//set D4-D7
	PORTDELAY
	LCDPORT = LCDPORT & 0b11111110;				//set RS port to 0 -> display set to command mode
	PORTDELAY
	E_Pulse();                              //pulse to set d4-d7 bits

	_delay_us(500);

}

void LCDSendChar(unsigned char a)
{
	unsigned char data = 0b00001111 | a;					//get high 4 bits
	LCDPORT = (LCDPORT | 0b11110000) & data;	//set D4-D7
	PORTDELAY
	LCDPORT = LCDPORT | 0b00000001;				//set RS port to 1
	PORTDELAY
	E_Pulse();                              //pulse to set D4-D7 bits

	_delay_us(500);

	data = a<<4;							//get low 4 bits
	LCDPORT = (LCDPORT & 0b00001111) | data;	//clear D4-D7
	PORTDELAY
	LCDPORT = LCDPORT | 0b00000001;				//set RS port to 1 -> display set to command mode
	PORTDELAY
	E_Pulse();                              //pulse to set d4-d7 bits

	_delay_us(500);
}

void LCDSendTxt(char* a)
{

	//_delay_ms(1);

	int Temp;
	for(Temp=0; Temp < strlen(a); Temp++)
  {
    LCDSendChar(a[Temp]);
	TEXTDELAY
  }
}

void LCDSendUnsafeCounteredTxt(char* a, char count)
{

	//_delay_ms(1);

	int Temp;
	for(Temp=0; Temp < count; Temp++)
  {
    LCDSendChar(a[Temp]);
	TEXTDELAY
  }
}


void LCDSendInt(long a)
{
	int C[20];
	unsigned char Temp=0, NumLen = 0;
	if (a < 0)
	{
		LCDSendChar('-');
		a = -a;
	}
	do
	{	
		Temp++;
		C[Temp] = a % 10;
		a = a/10;
	}
	while (a);
	NumLen = Temp;
	for (Temp = NumLen; Temp>0; Temp--) LCDSendChar(C[Temp] + 48);
}

void LCDSendInt_Old(int a)
{
  int h = 0;
  int l = 0;

  l = a%10;
  h = a/10;

  LCDSendChar(h+48);
  LCDSendChar(l+48);
}

void SmartUp(void)
{
	int Temp;
	for(Temp=0; Temp<1; Temp++) LCDSendCommand(CUR_UP);
}

void SmartDown(void)
{
	int Temp;
	for(Temp=0; Temp<40; Temp++) LCDSendCommand(CUR_DOWN);
}

void Light(short a)
{
  if(a == 1)
  {
	LCDPORT = LCDPORT | 0b00100000;
	DDRC = LCDPORT | 0b00100000;

    //IO0SET_bit.P0_25 = 1;
    //IO0DIR_bit.P0_25 = 1;
  }
  if(a == 0)
  {
    LCDPORT = LCDPORT & 0b11011111;
    DDRC = DDRC & 0b11011111;

    //IO0SET_bit.P0_25 = 0;
    //IO0DIR_bit.P0_25 = 0;
  }

}

/*****************************L C D**************************/
