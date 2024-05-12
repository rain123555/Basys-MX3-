

#include <xc.h>
#include <sys/attribs.h>
#include <string.h>
#include "config.h"


//Global variables

#pragma config JTAGEN = OFF     
#pragma config FWDTEN = OFF      


/* ------------------------------------------------------------ */
/*                      Configuration Bits                      */
/* ------------------------------------------------------------ */


// Device Config Bits in  DEVCFG1:  
#pragma config FNOSC =  FRCPLL
#pragma config FSOSCEN =    OFF
#pragma config POSCMOD =    XT
#pragma config OSCIOFNC =   ON
#pragma config FPBDIV =     DIV_2

// Device Config Bits in  DEVCFG2:  
#pragma config FPLLIDIV =   DIV_2
#pragma config FPLLMUL =    MUL_20
#pragma config FPLLODIV =   DIV_1









void init()
{ 
    
    TRISECLR = 1 << 9; 
    LATECLR = 1 << 9;
    Timer5Setup();
    LCD_Init();
    ADC_Init();
    LED_Init();
}

void DelayAprox10Us( unsigned int  t100usDelay )
{
    int j;
    while ( 0 < t100usDelay )
    {
        t100usDelay--;
        j = 13;
        while ( 0 < j )
        {
            j--;
        }   // end while 
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
         
    }   // end while
}


// void DelayAprox1s(unsigned int sec)
// {
    // int i;
    // while(0 < sec)
    // {
        // sec--;
        // i=100;
        // while ( 0 < i)
        // {
            // DelayAprox10Us(i--);
        // }   // end while 
    // }
// }

void DelayForAproxSeconds(float n)
{
    DelayForAproxmSeconds((int)(n *1000));
}

void DelayForAproxmSeconds(int n)
{
    unsigned int i;
    i = n;
    while(0 < i)
    {
        i--;
        DelayAprox10Us(100);
    }
}



void LCD_Init()
{
    LCD_ConfigurePins();
    LCD_InitSequence(displaySetOptionDisplayOn);
}

void LCD_ConfigurePins()
{
    // set control pins as digital outputs.
    tris_LCD_DISP_RS = 0;
    tris_LCD_DISP_RW = 0;
    tris_LCD_DISP_EN = 0;
    
    // disable analog (set pins as digital))
    ansel_LCD_DISP_RS = 0;
    
    // default (IO) function for remapable pins
    rp_LCD_DISP_RS = 0;
    rp_LCD_DISP_RW = 0;
    rp_LCD_DISP_EN = 0;
    
    // make data pins digital (disable analog)
    ansel_LCD_DB2 = 0;
    ansel_LCD_DB4 = 0;
    ansel_LCD_DB5 = 0;
    ansel_LCD_DB6 = 0;
    ansel_LCD_DB7 = 0;
}

void LCD_WriteByte(unsigned char bData)
{
    DelayAprox10Us(5);  
    // Configure IO Port data pins as output.
   tris_LCD_DATA &= ~msk_LCD_DATA;
    DelayAprox10Us(5);  
    // clear RW
    lat_LCD_DISP_RW = 0;

    // access data as contiguous 8 bits, using pointer to the LSB byte of LATE register
    unsigned char *pLCDData = (unsigned char *)(0xBF886430);
    *pLCDData = bData;

    DelayAprox10Us(10);   

    // Set En
    lat_LCD_DISP_EN = 1;    

    DelayAprox10Us(5);
    // Clear En
    lat_LCD_DISP_EN = 0;

    DelayAprox10Us(5);
    // Set RW
    lat_LCD_DISP_RW = 1;
}

unsigned char LCD_ReadByte()
{
    unsigned char bData;
    // Configure IO Port data pins as input.
    tris_LCD_DATA |= msk_LCD_DATA;
    // Set RW
    lat_LCD_DISP_RW = 1;

    // set RW
    lat_LCD_DISP_RW = 1;    
    
    // Set En
    lat_LCD_DISP_EN = 1;

    DelayAprox10Us(50);   

    // Clear En
    lat_LCD_DISP_EN = 0;
    bData = (unsigned char)(prt_LCD_DATA & (unsigned int)msk_LCD_DATA);
    return bData;
}

unsigned char LCD_ReadStatus()
{
    // Clear RS
    lat_LCD_DISP_RS = 0;
    
    unsigned char bStatus = LCD_ReadByte();
    return bStatus;
}

void LCD_WriteCommand(unsigned char bCmd)
{ 
    // Clear RS
    lat_LCD_DISP_RS = 0;

    // Write command byte
    LCD_WriteByte(bCmd);
}

void LCD_WriteDataByte(unsigned char bData)
{
    // Set RS 
    lat_LCD_DISP_RS = 1;

    // Write data byte
    LCD_WriteByte(bData);
}

void LCD_InitSequence(unsigned char bDisplaySetOptions)
{
    //  wait 40 ms

    DelayAprox10Us(40000);
    // Function Set
    LCD_WriteCommand(cmdLcdFcnInit);
    // Wait ~100 us
    DelayAprox10Us(10);
    // Function Set
    LCD_WriteCommand(cmdLcdFcnInit);
    // Wait ~100 us
    DelayAprox10Us(10); // Display Set
    LCD_DisplaySet(bDisplaySetOptions);
    // Wait ~100 us
    DelayAprox10Us(10);
    // Display Clear
    LCD_DisplayClear();
    // Wait 1.52 ms
    DelayAprox10Us(160);
    // Entry mode set
    LCD_WriteCommand(cmdLcdEntryMode);
        // Wait 1.52 ms
    DelayAprox10Us(160);
}

void LCD_DisplaySet(unsigned char bDisplaySetOptions)
{
    LCD_WriteCommand(cmdLcdCtlInit | bDisplaySetOptions);
}

void LCD_DisplayClear()
{
    LCD_WriteCommand(cmdLcdClear);
}

void LCD_ReturnHome()
{
    LCD_WriteCommand(cmdLcdRetHome);
}

void LCD_DisplayShift(unsigned char fRight)
{
    unsigned char bCmd = cmdLcdDisplayShift | (fRight ? mskShiftRL: 0);
    LCD_WriteCommand(bCmd);
}

void LCD_CursorShift(unsigned char fRight)
{
    unsigned char bCmd = cmdLcdCursorShift | (fRight ? mskShiftRL: 0);
    LCD_WriteCommand(bCmd);
}

void LCD_WriteStringAtPos(char *szLn, unsigned char idxLine, unsigned char idxPos)
{
    // crop string to 0x27 chars
    int len = strlen(szLn);
    if(len > 0x27)
    {
        szLn[0x27] = 0; // trim the string so it contains 40 characters 
        len = 0x27;
    }

    // Set write position
    unsigned char bAddrOffset = (idxLine == 0 ? 0: 0x40) + idxPos;
    LCD_SetWriteDdramPosition(bAddrOffset);

    unsigned char bIdx = 0;
    while(bIdx < len)
    {
        LCD_WriteDataByte(szLn[bIdx]);
        bIdx++;
    }
}

void LCD_SetCursorPosition(unsigned char idxLine, unsigned char idxPos){
    LCD_SetWriteDdramPosition((idxLine == 0 ? 0: 0x40) + idxPos);
}

void LCD_SetWriteCgramPosition(unsigned char bAdr)
{
    unsigned char bCmd = cmdLcdSetCgramPos | bAdr;
    LCD_WriteCommand(bCmd);
}

void LCD_WriteBytesAtPosCgram(unsigned char *pBytes, unsigned char len, unsigned char bAdr)
{
    // Set write position
    LCD_SetWriteCgramPosition(bAdr);

    // Write the string of bytes that define the character to CGRAM
    unsigned char idx = 0;
    while(idx < len)
    {
        LCD_WriteDataByte(pBytes[idx]);
        idx++;
    }
}

void ADC_Init() {
    AD1CON1 = 0; // Clear register
    AD1CON1bits.SSRC = 7; // Auto-convert
    AD1CON1bits.FORM = 0; // Integer output

    AD1CON2 = 0; // Default settings

    AD1CON3 = 0;
    AD1CON3bits.ADCS = 32; // Set Tad to 1 ?s
    AD1CON3bits.SAMC = 16; 

    ANSELBSET = 1 << 3; // Set AN3 (RB3) as analog
    TRISBSET = 1 << 3;  // Configure RB3 as input

    AD1CON1bits.ON = 1; // Turn ADC on
}

unsigned int ADC_AnalogRead()
{
    unsigned int adc_val = 0;
    unsigned char analogPIN = 3;  // Assuming AN3 is the analog pin to read

    // Configure the input channel (ensure that other settings for ADC are already configured properly)
    AD1CHSbits.CH0SA = analogPIN;  // Select AN3 for ADC input

    AD1CON1bits.SAMP = 1;          // Start sampling
    while (AD1CON1bits.SAMP);      // Optional: wait for sampling to start (not usually necessary)
    while (!AD1CON1bits.DONE);     // Wait for conversion to complete

    adc_val = ADC1BUF0;            // Read the ADC result
    AD1CON1bits.DONE = 0;          // Clear the DONE bit (in some setups this might be needed)

    return adc_val;                // Return the raw ADC value
}


volatile int updateDisplayFlag = 0;  
volatile int updateDisplayFlag1 = 0;  

void __ISR(_TIMER_5_VECTOR, ipl2auto) Timer5ISR(void)  {
    
    static int counter = 0;
    static int counter1 = 0;
    counter++;
    counter1++;
    if (counter >= 400) {  // Adjust this number to change the blink rate
        counter = 0;
         updateDisplayFlag = 1;  
         // Toggle LED connected to a specific port bit
    }
    if (counter1 >= 1600) {  // Adjust this number to change the blink rate
        counter1 = 0;
        updateDisplayFlag1 = 1;  
         // Toggle LED connected to a specific port bit
    }
     
     IFS0bits.T5IF = 0;    //
    
}

void Timer5Setup()
{
        //clock speed?1MHz in main loop atm tested with port toggeling, 1MHz/256*0.2=781.25
        PR5 = 15625;                        //             set period register, generates one interrupt every 1 ms
        TMR5 = 0;                            //             initialize count to 0
        T5CONbits.TCKPS = 3;                //            1:256 prescale value
        T5CONbits.TGATE = 0;                //             not gated input (the default)
        T5CONbits.TCS = 0;                  //             PCBLK input (the default)                         
        IPC5bits.T5IP = 2;                  //             priority  step 4: priority
        IPC5bits.T5IS = 0;                  //             subpriority
        IFS0bits.T5IF = 0;                  //             clear interrupt flag
        IEC0bits.T5IE = 1;                  //             enable interrupt
        T5CONbits.ON = 1;                   //             turn on Timer5
        macro_enable_interrupts();          //    enable interrupts at CPU

}


void LED_Init()
{
    LED_ConfigurePins();
    LED_SetGroupValue(0);    
}


void LED_ConfigurePins()
{
    // Configure LEDs as digital outputs.
    tris_LEDS_GRP &= ~msk_LEDS_GRP;
}

void LED_SetValue(unsigned char bNo, unsigned char bVal)
{
    bVal = bVal ? 1: 0;
    if(bNo == (bNo & 0x07))
    {
        switch (bNo)
        {
            case 0:
                LEDS_Led0SetValue(bVal);
                break;
            case 1:
                LEDS_Led1SetValue(bVal);
                break;
            case 2:
                LEDS_Led2SetValue(bVal);
                break;
            case 3:
                LEDS_Led3SetValue(bVal);
                break;
            case 4:
                LEDS_Led4SetValue(bVal);
                break;
            case 5:
                LEDS_Led5SetValue(bVal);
                break;
            case 6:
                LEDS_Led6SetValue(bVal);
                break;
            case 7:
                LEDS_Led7SetValue(bVal);
                break;
        }
    }
}



void LED_ToggleValue(unsigned char bNo)
{
    if(bNo == (bNo & 0x07))
    {
        latinv_LEDS_GRP = (1<<bNo);
    }
}


void LED_SetGroupValue(unsigned char bVal)
{
    unsigned char *pLedData = (unsigned char *)lat_LEDS_GRP_ADDR;
    *pLedData = bVal;    
}




