#ifndef MYMATRIXKEYBOARD
#define MYMATRIXKEYBOARD

#define MPORT PORTF
#define MDDRPORT DDRF
#define MPIN PINF
#define SCANMASK 0b00001111;
#define INITMASK 0b11110000;
#define SCANLINE 4


void InitializeKeyboard();

char ReadFromKeyboard();


#endif // MYMATRIXKEYBOARD
