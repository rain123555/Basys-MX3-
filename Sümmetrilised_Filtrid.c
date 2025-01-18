#include <xc.h>
#include <sys/attribs.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "config.h"

#define N_HP 7  // Number of coefficients in the high-pass filter
#define N_BP 13
#define LEVEL_1_THRESHOLD 256// Adjusted for fixed-point
#define LEVEL_2_THRESHOLD 512
#define LEVEL_3_THRESHOLD 768

const int fractionalBits = 10;
const int scaleFactor = 1 << fractionalBits;  // 2^10 = 1024

float b_LP[N_HP] = {
0.013496923634082027,
    0.078450868623101772,
    0.240862474239630725,
    0.334379467006370668,
    0.240862474239630836,
    0.078450868623101772,
    0.013496923634082027,
};


float b_HP[N_HP] = {
 -0.007989489486156473,
    0.046438907638284489,
    -0.142578283594033794,
    0.208257730883811720,
    -0.142578283594033822,
    0.046438907638284489,
    -0.007989489486156473,
};



float b_BP1[N_BP] = {
0.000068846646483278,
    0.000692805227012897,
    0.000766957006578657,
    -0.023022647175066756,
    -0.112305811974730135,
    0.007865141015345082,
    0.251869418508747500,
    0.007865141015345307,
    -0.112305811974731121,
    -0.023022647175066833,
    0.000766957006578617,
    0.000692805227012890,
    0.000068846646483297,
};


float b_BP2[N_BP] = {
 0.000057788131750764,
    -0.000189444906772452,
    -0.001761240259910298,
    0.023097939925800299,
    -0.041233912169902534,
    -0.116671537824880267,
    0.273400814207824805,
    -0.116671537824881349,
    -0.041233912169903061,
    0.023097939925800420,
    -0.001761240259910355,
    -0.000189444906772427,
    0.000057788131750786,
};

int inputBuffer_BP2[N_BP] = {0};

int applyFIRFilter1(int* b, int* inputBuffer, int N) {
    LATEINV = 1 << 9; 
    long output = 0;
    for (int i = 0; i < 3; i++) {  
        output += (long)((inputBuffer[i]+inputBuffer[6-i]) * b[i]);
    }
    if(N & 1){
      int middle_index = 3;
      output += (long)(inputBuffer[middle_index] * b[middle_index]);
    }
    
    return output >> fractionalBits; 
}
int applyFIRFilter2(int* b, int* inputBuffer, int N) {
    LATEINV = 1 << 9; 
    long output = 0;
    for (int i = 0; i < 6; i++) {  
        output += (long)((inputBuffer[i]+inputBuffer[12-i]) * b[i]);
    }
    if(N & 1){
      int middle_index = 6;
      output += (long)(inputBuffer[middle_index] * b[middle_index]);
    }
    
    return output >> fractionalBits; 
}


void bufferShift(int* inputBuffer, int N, int input) {
    LATEINV = 1 << 9; 
    for (int i = N-1; i > 0; i--) {
        inputBuffer[i] = inputBuffer[i - 1];
    }
    inputBuffer[0] = input;
}

void DisplayAmplitudeLevel(int amplitude) {
    char lcdBuffer[32];
     float displayValue = (int)amplitude;  // Convert back for display
    sprintf(lcdBuffer, "Amp: %.4f V", displayValue);
    LCD_WriteStringAtPos(lcdBuffer, 1, 0);
}

void DisplayAmplitudeLevel2(int amplitude) {
    LATEINV = 1 << 9; 
    char lcdBuffer1[16] = "                ";  // Clear the line
    LCD_WriteStringAtPos(lcdBuffer1, 0, 0);
    if (amplitude > LEVEL_3_THRESHOLD) {
        strcpy(lcdBuffer1, "LP:||||");
    } else if (amplitude > LEVEL_2_THRESHOLD) {
        strcpy(lcdBuffer1, "LP:|||");
    } else if (amplitude > LEVEL_1_THRESHOLD) {
        strcpy(lcdBuffer1, "LP:||");
    } else {
        strcpy(lcdBuffer1, "LP:|");
    }
    LCD_WriteStringAtPos(lcdBuffer1, 0, 0);
}

void DisplayAmplitudeLevel3(int amplitude) {
    LATEINV = 1 << 9; 
    char lcdBuffer[16] = "                ";  // Clear the line
    LCD_WriteStringAtPos(lcdBuffer, 0, 8);
    if (amplitude > LEVEL_3_THRESHOLD) {
        strcpy(lcdBuffer, "HP:||||");
    } else if (amplitude > LEVEL_2_THRESHOLD) {
        strcpy(lcdBuffer, "HP:|||");
    } else if (amplitude > LEVEL_1_THRESHOLD) {
        strcpy(lcdBuffer, "HP:||");
    } else {
        strcpy(lcdBuffer, "HP:|");
    }
    LCD_WriteStringAtPos(lcdBuffer, 0, 8);
}

void DisplayAmplitudeLevel4(int amplitude) {
    LATEINV = 1 << 9; 
    char lcdBuffer1[16] = "                ";  // Clear the line
    LCD_WriteStringAtPos(lcdBuffer1, 1, 0);
    if (amplitude > LEVEL_3_THRESHOLD) {
        strcpy(lcdBuffer1, "BP1:||||");
    } else if (amplitude > LEVEL_2_THRESHOLD) {
        strcpy(lcdBuffer1, "BP:|||");
    } else if (amplitude > LEVEL_1_THRESHOLD) {
        strcpy(lcdBuffer1, "BP1:||");
    } else {
        strcpy(lcdBuffer1, "BP1:|");
    }
    LCD_WriteStringAtPos(lcdBuffer1, 1, 0);
}



void DisplayAmplitudeLevel5(int amplitude) {
    LATEINV = 1 << 9; 
    char lcdBuffer1[16] = "                ";  // Clear the line
    LCD_WriteStringAtPos(lcdBuffer1, 1, 8);
    if (amplitude > LEVEL_3_THRESHOLD) {
        strcpy(lcdBuffer1, "BP2:||||");
    } else if (amplitude > LEVEL_2_THRESHOLD) {
        strcpy(lcdBuffer1, "BP2:|||");
    } else if (amplitude > LEVEL_1_THRESHOLD) {
        strcpy(lcdBuffer1, "BP2:||");
    } else {
        strcpy(lcdBuffer1, "BP2:|");
    }
    LCD_WriteStringAtPos(lcdBuffer1, 1, 8);
}


int b_HP_Fix[N_HP + 1] = {0};
int b_LP_Fix[N_HP + 1] = {0};
int b_BP1_Fix[N_BP + 1] = {0};
int b_BP2_Fix[N_BP + 1] = {0};

int main(void) {
    init();
    int maxAmplitudeHP = 0;
    int maxAmplitudeLP = 0;
    int maxAmplitudeBP1 = 0;
    int maxAmplitudeBP2 = 0;
    int filtered_HP = 0;
    int filtered_LP = 0;
    int filtered_BP1 = 0;
    int filtered_BP2 = 0;
    
    for(int i = 0; i <= N_HP-1; i++){
           b_LP_Fix[i] = round(b_LP[i]*scaleFactor);
           b_HP_Fix[i] = round(b_HP[i]*scaleFactor);
    
     };
        for(int i = 0; i <= N_BP-1; i++){
           b_BP1_Fix[i] = round(b_BP1[i]*scaleFactor);
           b_BP2_Fix[i] = round(b_BP2[i]*scaleFactor);

    
     };
    while (1) {
          LATEINV = 1 << 9; 
       
        int adcValue = ADC_AnalogRead()-310; 

        int voltage = (adcValue * 3380) >> fractionalBits;  // see 3380 on adcreferens vaartus korda scalfactoriga ja ümardatud
       bufferShift(inputBuffer_BP2, N_BP, voltage);
       filtered_HP = applyFIRFilter1(b_HP_Fix, inputBuffer_BP2, N_HP);
       filtered_LP = applyFIRFilter1(b_LP_Fix, inputBuffer_BP2, N_HP);
       filtered_BP1 = applyFIRFilter2(b_BP1_Fix, inputBuffer_BP2, N_BP);
       filtered_BP2 = applyFIRFilter2(b_BP2_Fix, inputBuffer_BP2, N_BP);
        
       
   
      
        
       if (filtered_HP > maxAmplitudeHP) {
           maxAmplitudeHP = filtered_HP;
            LATEINV = 1 << 9;
       }
       
         if (filtered_LP > maxAmplitudeLP) {
            maxAmplitudeLP = filtered_LP;
            LATEINV = 1 << 9;
        }
          if (filtered_BP1 > maxAmplitudeBP1) {
            maxAmplitudeBP1 = filtered_BP1;
             LATEINV = 1 << 9;
        }
              if (filtered_BP2 > maxAmplitudeBP2) {
            maxAmplitudeBP2 = filtered_BP2;
             LATEINV = 1 << 9;
        }


        if (updateDisplayFlag == 1) {


            DisplayAmplitudeLevel2(maxAmplitudeLP);
        
            DisplayAmplitudeLevel3(maxAmplitudeHP);
    
            DisplayAmplitudeLevel4(maxAmplitudeBP1);
    
            DisplayAmplitudeLevel5(maxAmplitudeBP2);

        
            updateDisplayFlag = 0;
            maxAmplitudeLP = 0;
            maxAmplitudeHP = 0;
            maxAmplitudeBP1 = 0;
            maxAmplitudeBP2 = 0;
        }

        LATEINV = 1 << 9; 
         LATBINV = 1 << 5; 
         
    }

    return 0;
}
