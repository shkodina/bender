#include <avr/io.h>
#include "keyboard.h"

#define F_CPU 1000000UL  // 16 MHz
#include <util/delay.h>

//char KEYCODE[16] = {'1','2','3','4','5','6','7','8','9','0','A','B','C','D','#','*'};
char KEYCODE[16] = {17,18,19,20,1,4,7,88,2,5,8,0,3,6,9,89};
//					 A  B  C  D 1 4 7  * 2 5 8 0 3 6 9  #


void InitializeKeyboard()
{
	//MPORT = SCANMASK;
	//MDDRPORT = INITMASK;
	PORTF = 0b00001111;
	DDRF  = 0b11110000;
}

char ReadFromKeyboard()
{
	char pv = 0;

//	MPORT = ~(1 << (3+4));
//	if ( (MPIN & 0b00001111) < 0b00001111 )
//		PORTB = PORTB ^ (1<<6);

	for (char i = 0; i < SCANLINE; i++){
		MPORT = ~(1 << (i+4));

		pv = MPIN & SCANMASK;

		if (pv){
			switch (pv) {
				case 0x0E:
					return KEYCODE[4*i]+1;
					break;
				case 0x0D:
					return KEYCODE[4*i+1]+1;
					break;
				case 0x0B:
					return KEYCODE[4*i+2]+1;
					break;
				case 0x07:
					return KEYCODE[4*i+3]+1;
					break;
				default:
					//return 0;
					break;
			}
		}
		//MPORT = 0xFF;
	}

	_delay_us(1);



	return 0; 
}

