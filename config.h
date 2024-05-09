


#ifndef CONFIG_H
#define	CONFIG_H
#include <stdbool.h>
#define PB_FRQ  40000000
#define SYS_FREQ 80000000L  // System frequency
#define TIMER_FREQ 1        // Timer frequency in Hz

#define macro_enable_interrupts() \
{  unsigned int val = 0;\
    asm volatile("mfc0 %0,$13":"=r"(val));  \
    val |= 0x00800000;  \
    asm volatile("mtc0 %0,$13" : "+r"(val)); \
    INTCONbits.MVEC = 1; \
__builtin_enable_interrupts(); }

#define macro_disable_interrupts __builtin_disable_interrupts()

//#define macro_enable_interrupts INTEnableSystemMultiVectoredInt()

#define TMR_TIME    1


#define LEDS_Led0SetValue(val) lat_LEDS_LED0 = val
#define LEDS_Led1SetValue(val) lat_LEDS_LED1 = val
#define LEDS_Led2SetValue(val) lat_LEDS_LED2 = val
#define LEDS_Led3SetValue(val) lat_LEDS_LED3 = val
#define LEDS_Led4SetValue(val) lat_LEDS_LED4 = val
#define LEDS_Led5SetValue(val) lat_LEDS_LED5 = val
#define LEDS_Led6SetValue(val) lat_LEDS_LED6 = val
#define LEDS_Led7SetValue(val) lat_LEDS_LED7 = val

#define  lat_LEDS_LED0  LATAbits.LATA0
#define  lat_LEDS_LED1  LATAbits.LATA1
#define  lat_LEDS_LED2  LATAbits.LATA2
#define  lat_LEDS_LED3  LATAbits.LATA3
#define  lat_LEDS_LED4  LATAbits.LATA4
#define  lat_LEDS_LED5  LATAbits.LATA5
#define  lat_LEDS_LED6  LATAbits.LATA6
#define  lat_LEDS_LED7  LATAbits.LATA7



void init();

extern volatile int updateDisplayFlag; 
extern volatile int updateDisplayFlag1; 
void Timer5Setup();



void DelayAprox10Us( unsigned int tusDelay );
void DelayForAproxmSeconds(int msec);
void DelayForAproxSeconds(float sec);


void LED_ToggleValue(unsigned char bNo);
void LED_ConfigurePins();
void LED_SetValue(unsigned char bNo, unsigned char bVal);
void LED_ToggleValue(unsigned char bNo);
void LED_SetGroupValue(unsigned char bVal);
void LED_ConfigurePins();  






// void DelayAprox1s(unsigned int sec);

#define cmdLcdFcnInit 	0x3C        // function set command, (8-bit interface, 2 lines, and 5x8 dots)
#define cmdLcdCtlInit 	0x0C        // display control set command
#define cmdLcdEntryMode 0x06        // Entry Mode Set
#define cmdLcdClear		0x01		// clear display command
#define cmdLcdRetHome	0x02		// return home command
#define cmdLcdDisplayShift 0x18		// shift display command
#define cmdLcdCursorShift  0x10		// shift cursor command
#define cmdLcdSetDdramPos	0x80	// set DDRAM position command
#define cmdLcdSetCgramPos	0x40	// set CGRAM position command

#define mskBStatus 0x80             // bit busy 
#define mskShiftRL 0x04             // bit for direction 
#define displaySetOptionDisplayOn	0x4 // Set Display On option
#define	displaySetOptionCursorOn 	0x2 // Set Cursor On option
#define	displaySetBlinkOn 			0x1 // Set Blink On option


#define posCgramChar0 0		// position in CGRAM for character 0
#define posCgramChar1 8		// position in CGRAM for character 1
#define posCgramChar2 16	// position in CGRAM for character 2
#define posCgramChar3 24	// position in CGRAM for character 3
#define posCgramChar4 32	// position in CGRAM for character 4
#define posCgramChar5 40	// position in CGRAM for character 5
#define posCgramChar6 48	// position in CGRAM for character 6
#define posCgramChar7 56	// position in CGRAM for character 7

#define LCD_SetWriteDdramPosition(bAddr) LCD_WriteCommand(cmdLcdSetDdramPos | bAddr);



void LCD_Init();
void LCD_InitSequence(unsigned char bDisplaySetOptions);
void LCD_WriteStringAtPos(char *szLn, unsigned char idxLine, unsigned char bAdr);
void LCD_DisplaySet(unsigned char bDisplaySetOptions);
void LCD_DisplayClear();
void LCD_DisplayShift(unsigned char fRight);
void LCD_CursorShift(unsigned char fRight);
void LCD_ReturnHome();
void LCD_WriteBytesAtPosCgram(unsigned char *pBytes, unsigned char len, unsigned char bAdr);
void LCD_SetCursorPosition(unsigned char idxLine, unsigned char idxPos);

// private
unsigned char LCD_ReadByte();
unsigned char LCD_ReadStatus();
void LCD_WriteByte(unsigned char bData);
void LCD_SetWriteCgramPosition(unsigned char bAdr);
void LCD_WriteCommand(unsigned char bCmd);
void LCD_WriteDataByte(unsigned char bData);
void LCD_ConfigurePins();

void ADC_Init();
unsigned int ADC_AnalogRead();




#define macro_disable_interrupts __builtin_disable_interrupts()
//#define macro_disable_interrupts INTDisableInterrupts()

#define SEGM0 0
#define SEGM1 1
#define SEGM2 2
#define SEGM3 3
#define TRUE 1
#define FALSE 0
#define DOT_ON 1
#define DOT_OFF 0

/*
#ifdef	__cplusplus
extern "C" {
#endif
*/
//LEDS
#define tris_LEDS_GRP   TRISA
#define  lat_LEDS_GRP   LATA
#define latinv_LEDS_GRP LATAINV
#define  BGRP   PORTA
#define  msk_LEDS_GRP   0xFF    
#define  lat_LEDS_GRP_ADDR   0xBF886030


// LCD

#define tris_LCD_DISP_RS    TRISBbits.TRISB15
#define lat_LCD_DISP_RS     LATBbits.LATB15
#define ansel_LCD_DISP_RS   ANSELBbits.ANSB15
#define rp_LCD_DISP_RS      RPB15R


#define tris_LCD_DISP_RW    TRISDbits.TRISD5
#define  lat_LCD_DISP_RW    LATDbits.LATD5
#define rp_LCD_DISP_RW      RPD5R

#define tris_LCD_DISP_EN    TRISDbits.TRISD4
#define  lat_LCD_DISP_EN    LATDbits.LATD4
#define rp_LCD_DISP_EN      RPD4R

#define tris_LCD_DATA       TRISE
#define lat_LCD_DATA        LATE
#define prt_LCD_DATA   		PORTE
#define msk_LCD_DATA        0xFF
#define ansel_LCD_DB2        ANSELEbits.ANSE2
#define ansel_LCD_DB4        ANSELEbits.ANSE4
#define ansel_LCD_DB5        ANSELEbits.ANSE5
#define ansel_LCD_DB6        ANSELEbits.ANSE6
#define ansel_LCD_DB7        ANSELEbits.ANSE7


// RGBLED


// Analog input AN2
#define tris_ADC_AN2    TRISBbits.TRISB2
#define ansel_ADC_AN2   ANSELBbits.ANSB2



#define AIN1_CHANNEL 3







#endif  /* CONFIG_H */