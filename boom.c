
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lcd.h"

#define F_CPU 1000000UL
#include <util/delay.h>

#define LCDPORT 		PORTC
#define LCDDDRPORT		DDRC
#define RELAYPORT		PORTD
#define RELAYDDRPORT 	DDRD
#define	BUTTONPORT		PORTA
#define BUTTONDDRPORT	DDRA
#define ENCODERPORT2	PORTA
#define ENCODERDDRPORT2	DDRA
#define ENCODERPORT		PORTF
#define ENCODERDDRPORT	DDRF

#define RELAYPIN1	0
#define RELAYPIN2	1

#define BUTTON_PLUS_PIN			2
#define BUTTON_MINUS_PIN		3
#define BUTTON_CORRECTION_PIN 	4


#define BUTTONPIN 	(PINA & ((1 << BUTTON_PLUS_PIN) | (1 << BUTTON_MINUS_PIN) | (1 << BUTTON_CORRECTION_PIN))) 

#define ENCODERPINMASK 0b11111111
#define ENCODERPIN2MASK 0b00000011


#define ENCODERPIN	(PINF & ENCODERPINMASK)
#define ENCODERPIN2 (PINA & ENCODERPIN2MASK)




//////////////////////////////////////////////////
//////////////// definitions
//////////////////////////////////////////////////

#define ST_SET_ANGLE 	1
#define ST_GET_ANGLE	2
#define ST_INIT			0

#define RELAY_ON		1
#define RELAY_OFF		0
#define RELAY_WAIT_TIME 1000

#define BUTTON_NONE				0
#define BUTTON_PLUS 			1
#define BUTTON_MINUS			2
#define BUTTON_CORRECTION		3

#define AIM_ANGLE_MAX	125.0
#define AIM_ANGLE_MIN	0
#define AIM_ANGLE_MECHANICAL_CORRECTION 15.0

#define AIM_ANGLE_DELTA 1.0
#define ZERRO_ANGLE_DELTA 5.5

#define DEFAULTE_AIM_ANGLE 45
#define DEFAULTE_ZERRO_ANGLE 0

#define DEFAULTE_ANGLE_CORRECTION 0
#define DEFAULTE_ANGLE_CORRECTION_STEP 0.5

#define LCD_FIRST_LINE 1
#define LCD_SECOND_LINE 2
#define LCD_TEXT_LEN 17

#define LOGIC_ZERRO_CORRECTION 0.0

#define DIMENTION 1024.0

//////////////////////////////////////////////////
/////////////// global vars and util functions
//////////////////////////////////////////////////

volatile float gl_aim_angle = DEFAULTE_AIM_ANGLE;
volatile float gl_aim_angle_delta = AIM_ANGLE_DELTA;
volatile float gl_aim_entered_angle = DEFAULTE_AIM_ANGLE;

volatile float gl_zerro_angle = DEFAULTE_ZERRO_ANGLE;
volatile float gl_zerro_angle_delta = ZERRO_ANGLE_DELTA;

volatile float gl_correction = DEFAULTE_ANGLE_CORRECTION;
volatile float gl_correction_step = DEFAULTE_ANGLE_CORRECTION_STEP;
volatile float gl_logic_zerro_angle_correction = LOGIC_ZERRO_CORRECTION;

volatile char gl_state = ST_INIT;

//////////////////////////////////////////////////

#define INVBIT(port, bit) port = port ^ (1<<bit);
#define UPBIT(port, bit) port = port | (1<<bit);
#define DOWNBIT(port, bit) port = port & (~(1<<bit));

//////////////////////////////////////////////////

char 	setRelay(char relay_state);
char 	getButton(char * button_code);
float 	getCurAngle();

void 	changeAimAngle(char button); 
void	changeCorrection(char button);
void	setCorrection();

void 	portInit();

void 	lcdInit();
void	lcdPrintHello();
void	lcdPrintCorrection(float val);
void 	lcdPrintFinalAngle(float val);
void 	lcdPrintCurrentlAngle(float val);

char 	isInWork(); 											
void 	angleControl(); 
void	workWithUser();										
//////////////////////////////////////////////////
//////////////////////////////////////////////////
#define INVBIT(port, bit) port = port ^ (1<<bit);
#define UPBIT(port, bit) port = port | (1<<bit);
#define DOWNBIT(port, bit) port = port & (~(1<<bit));




int 	main()
{
	portInit();
	lcdInit();
	lcdPrintHello();

	gl_logic_zerro_angle_correction = getCurAngle();
	
	lcdPrintFinalAngle(gl_aim_entered_angle);
	lcdPrintCurrentlAngle(getCurAngle());

/*
	while (1){

	lcdPrintCurrentlAngle(getCurAngle());
	_delay_ms(500);
	INVBIT(PORTE, 5);

	}
*/


	while (1) 
	{
		if (isInWork())
		{
			angleControl();
		}
		else
		{
			workWithUser();
		}
		_delay_ms(10); // not so fast, baby
	}
	return 0;
}

/////////////////////////////////////////////////////
///////// all functions
/////////////////////////////////////////////////////

//---------------------------------------------------------------

void 	portInit()
{
	LCDPORT = 0b00000000;			LCDDDRPORT = 0b11110111;
	BUTTONPORT = 0b00000000;		BUTTONDDRPORT = BUTTONDDRPORT | 0b00000000;
	RELAYPORT = 0b00000000;			RELAYDDRPORT = RELAYDDRPORT | (1 << RELAYPIN1) | (1 << RELAYPIN2);
	ENCODERPORT = 0b00000000;		ENCODERDDRPORT = ENCODERDDRPORT | 0b00000000;
	
	PORTE = 0; DDRE = 0b00001100;
}

//--------------------------------------------------------------

void 	lcdInit()
{
	LCD_Init();
	LCDSendCommand(DISP_ON);
	LCDSendCommand(CLR_DISP);
}

//--------------------------------------------------------------

void	lcdPrintHello()
{
	char message1 [LCD_TEXT_LEN] = {'B','E','N','D','E','R',' ','1','.','0',' ',' ',' ',' ',' ',' ',0};
	char message2 [LCD_TEXT_LEN] = {'B','L','O','B','B','Y','@','Y','A','N','D','E','X','.','R','U',0};
	LCDSendCommand(CLR_DISP);
	LCDSendCommand(DD_RAM_ADDR);
	LCDSendTxt(message1);
	LCDSendCommand(DD_RAM_ADDR2);
	LCDSendTxt(message2);
	_delay_ms(3000);
	LCDSendCommand(CLR_DISP);
}

//----------------------------------------------------------------

void 	lcdPrintCorrection(float val)
{
	char message [LCD_TEXT_LEN] = {'C','o','r','r','e','c','t','.',':',' ',' ',' ',' ',' ',' ',' ',0};
	const char digit_pos = 9;
	LCDSendCommand(CLR_DISP);
	if (val >= 0)
		sprintf(&message[digit_pos], "%d.%d    ", (int)(val / 1), (int)(val * 10) % 10);
	else
		sprintf(&message[digit_pos], "-%d.%d    ", (int)((0 - val) / 1), (int)((0 - val) * 10) % 10);	
	LCDSendTxt(message);
}

//----------------------------------------------------------------

void 	lcdPrintFinalAngle(float val)
{
	char message [LCD_TEXT_LEN] = {'S','E','T',':',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',0};
	const char digit_pos = 5;
	LCDSendCommand(DD_RAM_ADDR);
	if (val >= 0)
		sprintf(&message[digit_pos], "%d.%d    ", (int)(val / 1), (int)(val * 10) % 10);
	else
		sprintf(&message[digit_pos], "-%d.%d    ", (int)((0 - val) / 1), (int)((0 - val) * 10) % 10);	
	LCDSendTxt(message);
}

//----------------------------------------------------------------

void 	lcdPrintCurrentlAngle(float val)
{
	char message [LCD_TEXT_LEN] = {'C','U','R',':',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',0};
	const char digit_pos = 5;
	LCDSendCommand(DD_RAM_ADDR2);
	if (val >= 0)
		sprintf(&message[digit_pos], "%d.%d    ", (int)(val / 1), (int)(val * 10) % 10);
	else
		sprintf(&message[digit_pos], "-%d.%d    ", (int)((0 - val) / 1), (int)((0 - val) * 10) % 10);	
	LCDSendTxt(message);
}

//----------------------------------------------------------------

char 	isInWork()
{
	static float last_angle = DEFAULTE_ZERRO_ANGLE;
	float cuurent_angle = getCurAngle();

	if ( cuurent_angle != last_angle){
		lcdPrintCurrentlAngle (cuurent_angle);
		last_angle = cuurent_angle;	
	}

//	if ( cuurent_angle > (gl_zerro_angle + gl_zerro_angle_delta) && last_angle != cuurent_angle ){
//	if ( cuurent_angle > (gl_zerro_angle + gl_zerro_angle_delta)){
		// текущий угол больше предыдущего и больше чем нулева€ точка с учетом люфта
	if (cuurent_angle > (gl_zerro_angle + gl_zerro_angle_delta)) {
		last_angle = cuurent_angle;
		return 1;
	}
	return 0;	
}

//---------------------------------------------------------------

void workWithUser()
{
	char button = 0;

	if(getButton(& button))
	{
		switch (button)
		{
			case BUTTON_CORRECTION:
				setCorrection();
				break;			
			case BUTTON_PLUS:
			case BUTTON_MINUS:
				changeAimAngle(button);
				lcdPrintFinalAngle(gl_aim_entered_angle);
				break;
			default:
				break;
		}
	}
}

//---------------------------------------------------------------

void 	angleControl()
{
	float cuurent_angle = getCurAngle(); 

	const char print_tact_val = 20;
	char print_tact = print_tact_val;	


	// controll angle up
	while ( cuurent_angle <= (gl_aim_angle + gl_correction + AIM_ANGLE_MECHANICAL_CORRECTION) ) {
/*		if (!print_tact--){
			lcdPrintCurrentlAngle (cuurent_angle);
			print_tact = print_tact_val;
		}
*/		cuurent_angle = getCurAngle(); 
		_delay_ms(2);
	}
	
	setRelay(RELAY_ON);

	lcdPrintCurrentlAngle (cuurent_angle);

	_delay_ms(RELAY_WAIT_TIME);

	setRelay(RELAY_OFF);

	// больше не ждем достижени€ минимального угла

	// controll angle down
	while ( cuurent_angle > (gl_zerro_angle + gl_zerro_angle_delta)) {
		if (!print_tact--){
			lcdPrintCurrentlAngle (cuurent_angle);
			print_tact = print_tact_val;
		}
		cuurent_angle = getCurAngle(); 
		_delay_ms(5);
	}
	
	
}

//---------------------------------------------------------------

void 	changeAimAngle(char button)
{
	float step = gl_aim_angle_delta;


	if( button == BUTTON_MINUS)	{
		if (gl_aim_entered_angle <= AIM_ANGLE_MIN) // не позволим задать угол меньше мининмума
			return;
		gl_aim_entered_angle -= step;
		gl_aim_angle = gl_aim_entered_angle;
//		gl_aim_angle += gl_correction;
//		gl_aim_angle += AIM_ANGLE_MECHANICAL_CORRECTION;
		lcdPrintFinalAngle(gl_aim_entered_angle);
	}else{
		if (gl_aim_entered_angle >= AIM_ANGLE_MAX) // не позволим задать угол больше максимума
			return;
		gl_aim_entered_angle += step;
		gl_aim_angle = gl_aim_entered_angle;
//		gl_aim_angle += gl_correction;
//		gl_aim_angle += AIM_ANGLE_MECHANICAL_CORRECTION;
		lcdPrintFinalAngle(gl_aim_entered_angle);
	}
}

//---------------------------------------------------------------

void 	changeCorrection(char button)
{
	float step = gl_correction_step;

	if( button == BUTTON_MINUS)	{
		step = 0 - gl_correction_step;
	}

	gl_correction += step;
	lcdPrintCorrection(gl_correction);
}

//---------------------------------------------------------------

void 	setCorrection()
{
	lcdPrintCorrection(gl_correction);

	const long WAITEXIT = 500;
	long wait_exit = WAITEXIT;
	char sbutton = 0;
	while (wait_exit--)
	{

		char t = getButton(& sbutton);
		if(t)
		{
			switch (t)
			{
				case BUTTON_CORRECTION:
					lcdPrintFinalAngle(gl_aim_entered_angle);
					lcdPrintCurrentlAngle(getCurAngle());
					return;
					break;			
				case BUTTON_PLUS:
				case BUTTON_MINUS:
					changeCorrection(t);
					wait_exit = WAITEXIT;
					break;
				default:
					break;
			}
		}
		_delay_ms(10);
	}

	lcdPrintFinalAngle(gl_aim_entered_angle);
	lcdPrintCurrentlAngle(getCurAngle());
	return;
}

//----------------------------------------------------------------

char 	setRelay(char relay_state)
{
	if ( relay_state == RELAY_ON ){
		UPBIT(RELAYPORT, RELAYPIN1);
		UPBIT(RELAYPORT, RELAYPIN2);
		UPBIT(PORTE,5);
	}
	else{
		DOWNBIT(RELAYPORT, RELAYPIN1);
		DOWNBIT(RELAYPORT, RELAYPIN2);
		DOWNBIT(PORTE,5);
	}
	return 0;
}

//-----------------------------------------------------------------

float 	getCurAngle()
{

	long enc_val = 0;
	char * enc_val_p = &enc_val;
	*enc_val_p = (ENCODERPIN ^ 0xFF ) & ENCODERPINMASK;
	enc_val_p++;
	*enc_val_p = (ENCODERPIN2 ^ 0xFF) & ENCODERPIN2MASK;

	float angle = (360.0 / DIMENTION ) * enc_val;

	float countered_angle = angle + gl_logic_zerro_angle_correction;

	// сделаем дельту положительной
	if ( countered_angle < 0)
		countered_angle = 0 - countered_angle;

	// проверим не было ли перехода из 360 в 0
	if ( countered_angle > 180.0)
		countered_angle = 360 - countered_angle;
			
	return countered_angle;
}

//-----------------------------------------------------------------

char 	getButton(char * button_code)
{
	static char button_plus_was_released = 1;
	static char button_minus_was_released = 1;
	static char button_corr_was_released = 1;
	
	static char button_plus_pressed_cicles = 0;
	static char button_minus_pressed_cicles = 0;
	const char button_pressed_cicles_max = 35;
	
	char but_port_state = BUTTONPIN;

	INVBIT (but_port_state, BUTTON_PLUS_PIN);
	INVBIT (but_port_state, BUTTON_MINUS_PIN);
	INVBIT (but_port_state, BUTTON_CORRECTION_PIN);


	// check button plus
	if (but_port_state & (1 << BUTTON_PLUS_PIN)) {
		if (button_plus_was_released){
			button_plus_was_released = 0;
			* button_code = BUTTON_PLUS;		
			return * button_code;
		}else{
			if (button_plus_pressed_cicles++ > button_pressed_cicles_max){
				_delay_ms(100);
				* button_code = BUTTON_PLUS;		
				return * button_code;
			}
		}
	}else{
		button_plus_was_released = 1;
		button_plus_pressed_cicles = 0;
	}	

	// check button minus
	if (but_port_state & (1 << BUTTON_MINUS_PIN)) {
		if (button_minus_was_released){
			button_minus_was_released = 0;
			* button_code = BUTTON_MINUS;		
			return * button_code;		
		}else{
			if (button_minus_pressed_cicles++ > button_pressed_cicles_max){
				_delay_ms(100);
				* button_code = BUTTON_MINUS;		
				return * button_code;
			}
		}
	}else{
		button_minus_was_released = 1;
		button_minus_pressed_cicles = 0;
	}	
	
	// check button correction
	if (but_port_state & (1 << BUTTON_CORRECTION_PIN)) {
		if (button_corr_was_released){
			button_corr_was_released = 0;
			* button_code = BUTTON_CORRECTION;		
			return * button_code;
		}
	}else{
		button_corr_was_released = 1;
	}	
	
	* button_code = BUTTON_NONE;
	return * button_code;
}

//-----------------------------------------------------------------















