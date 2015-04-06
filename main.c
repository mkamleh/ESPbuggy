/*
 * MAIN is here
 *
 */

#include "xc_config_settings.h"
#include "adc.h"
#include "delays.h"

//******************************************************************************
// Global Variables
//******************************************************************************

volatile unsigned char speed, left, str, right;
#define kp      70

//******************************************************************************
// Functions
//******************************************************************************

void init_PWM(void){
        OpenTimer2(TIMER_INT_OFF & T2_PS_1_1 & T2_POST_1_1 & T12_SOURCE_CCP);
	OpenPWM4(248);                          //frequency of 10k
	OpenPWM5(248);
}

void config_pins(void){

    TRISGbits.RG3 = 0;           //PWM4 output
    TRISGbits.RG4 = 0;           //PWM5 output
    TRISD = 0x00;
    LATD=0b00010000;             //'X'X'X'EN'DIR2'BP2'DIR1'BP1'

    TRISJ = 0xFF;               //front sensors bits 1..3

}

void motors_drive(void){
    speed = 650;
    if(left == 0 && str == 0 && right == 0){
        //speed = 1023;                   //STOP
        SetDCPWM4(1023);
        SetDCPWM5(1023);
    }
    else if(left == 1 && str == 1 && right == 1){
        //speed = 1023;                   //STOP
        SetDCPWM4(1023);
        SetDCPWM5(1023);
    }
    else if(left == 0 && str == 1 && right == 0){        //straight
        speed = 650;
        SetDCPWM4(speed);
        SetDCPWM5(speed);
    }
    else if(left == 1 && str == 1 && right == 0){        //abit left
        SetDCPWM4(speed - kp);
        SetDCPWM5(speed + kp);
    }
    else if(left == 0 && str == 1 && right == 1){        //abit right
        SetDCPWM4(speed + kp);
        SetDCPWM5(speed - kp);
    }
    else if(left == 0 && str == 0 && right == 1){        //right
        SetDCPWM4(speed + 2*kp);
        SetDCPWM5(speed - 2*kp);
    }
    else if(left == 1 && str == 0 && right == 0){        //left
        SetDCPWM4(speed - 2*kp);
        SetDCPWM5(speed + 2*kp);
    }
}

void front_sensors(void){
    right = (PORTJbits.RJ3 & 0x01);//r u sure this is right because it will be always zero and the same for str
    str = (PORTJbits.RJ2 & 0x01);
    left = (PORTJbits.RJ1 & 0x01);
}

void init_demonstration(void){

    TRISF = 0x00;            //LEDs outputs
    LATF = 0x00;             //clear LEDs
    TRISHbits.RH0 = 0;       //U1 enabled
    TRISHbits.RH1 = 0;       //U2 enabled
    LATHbits.LATH0 = 1;      //off
    LATHbits.LATH1 = 1;      //off
    TRISC = 0xFF;           //outputs for sw; RC2 for ECCP interrupt speed
    TRISH = 0xFF;           //outputs for sw

}

//******************************************************************************
// Main Program
//******************************************************************************

void main(void){

    config_pins();
    init_PWM();
    init_demonstration();
    //unsigned char sensors;

    while(1){
        front_sensors();
        motors_drive();
        LATFbits.LF1 = left;
        LATFbits.LF2 = str;
        LATFbits.LF3 = right;
    }

}
