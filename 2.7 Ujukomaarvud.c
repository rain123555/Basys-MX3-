#include <xc.h>
#include <sys/attribs.h>
#include <stdint.h>
#include <math.h>        // For fabs function
#include <string.h>      // For strcpy function
#include <stdio.h>
#include "config.h"


#define N_HP 7  
#define N_BP 13          

#define LEVEL_1_THRESHOLD (0.25)
#define LEVEL_2_THRESHOLD (0.5)
#define LEVEL_3_THRESHOLD (0.75)

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

float applyFIRFilter(float* b, float* inputBuffer, int N) {
  float output = 0.000;
  for (int i = 0; i < N-1; i++) {
    output += inputBuffer[i] * b[i];
  }
  return output;
}

void bufferShift(float* inputBuffer, int N, float input) {
  for (int i = N-1; i > 0; i--) {
    inputBuffer[i] = inputBuffer[i - 1];
  }
  inputBuffer[0] = input;
}


void DisplayAmplitudeLevel1(float amplitude) {
   char lcdBuffer[32];
   char lcdBuffer2[32];
    sprintf(lcdBuffer, "Amp:%.4f", amplitude);
    LCD_WriteStringAtPos(lcdBuffer, 0, 0);
  
}


void DisplayAmplitudeLevel2(float amplitude) {
    char lcdBuffer1[16] = "        ";  // Clear the line
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

void DisplayAmplitudeLevel3(float amplitude) {
     char lcdBuffer[16] = "        ";   // Clear the line
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

void DisplayAmplitudeLevel4(float amplitude) {
    char lcdBuffer1[16] = "        ";   // Clear the line
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



void DisplayAmplitudeLevel5(float amplitude) {
     char lcdBuffer1[16] = "        ";  // Clear the line
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




int main(void) {
       TRISECLR = 1 << 9; 
    LATECLR = 1 << 9;
init(); // Initialize the system
  float maxAmplitudeHP = 0;
    float maxAmplitudeLP = 0;
    float maxAmplitudeBP1 = 0;
     float maxAmplitudeBP2 = 0;
  char lcdBuffer[16]; 
     float filtered_HP = 0;
    float filtered_LP = 0;
    float filtered_BP1 = 0;
    float filtered_BP2 = 0;
    // Set RB10 to low initially
   
    while (1) {
       
        int adcValue = ADC_AnalogRead()-310;  // ADC reading
        float voltage = adcValue*3.3/1024 ;
        filtered_HP = applyFIRFilter(b_HP, inputBuffer_BP2,N_HP );
        filtered_LP = applyFIRFilter(b_LP, inputBuffer_BP2, N_HP);
        filtered_BP1 = applyFIRFilter(b_BP1, inputBuffer_BP2, N_BP);
        filtered_BP2 = applyFIRFilter(b_BP2, inputBuffer_BP2, N_BP);
 
        bufferShift(inputBuffer_BP2, N_BP, voltage);
        
     if (filtered_HP > maxAmplitudeHP) {
            maxAmplitudeHP = filtered_HP;
        }
         if (filtered_LP > maxAmplitudeLP) {
            maxAmplitudeLP = filtered_LP;
        }
          if (filtered_BP1 > maxAmplitudeBP1) {
            maxAmplitudeBP1 = filtered_BP1;
        }
              if (filtered_BP2 > maxAmplitudeBP2) {
            maxAmplitudeBP2 = filtered_BP2;
        }
      
       if (updateDisplayFlag==1) {
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
   
        LATEINV = 1 << 9; // Toggle RB9
       
    }

    return 0; // This should never be reached
}
