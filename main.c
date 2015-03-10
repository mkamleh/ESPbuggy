/*
 * MAIN is here
 *
 * to add: motor pwm rotation and its control by adc to demonstrate speed conversion
 *
 */


#include "xc_config_settings.h"
#include "adc.h"
#include "delays.h"

#define FLEFTdetected       PORTEbits.RE0
#define LEFTdetected        PORTEbits.RE1
#define STRAIGHT1detected   PORTEbits.RE2
/*#define STRAIGHT2detected   PORTJbits.RJ3     */
#define RIGHTdetected       PORTEbits.RE3
#define FRIGHTdetected      PORTEbits.RE4

//RC2 & RG0 <= speed sensing

//for test purposes only, remove and uncomment previous for tech demo
//#define FLEFTdetected       PORTHbits.RH7
//#define LEFTdetected        PORTHbits.RH6
//#define STRAIGHT1detected   PORTHbits.RH5
//#define STRAIGHT2detected   PORTHbits.RH4
//#define RIGHTdetected       PORTCbits.RC5
//#define FRIGHTdetected      PORTCbits.RC4

//******************************************************************************
// Global Variables
//******************************************************************************

enum turn {FLEFT, LEFT, STRAIGHT, fullSTRAIGHT, RIGHT, FRIGHT, BREAK, STOP, ERROR};
volatile enum turn buggy_turn = STRAIGHT;

volatile char TIMER1, TIMER3;
unsigned char pattern [] = {0x84, 0xf5, 0x4c, 0x64, 0x35, 0x26, 0x06, 0xf4, 
                        0x04, 0x34, 0x14, 0x04, 0x8E, 0x84, 0x0E, 0x1E, 0xFF};
unsigned char speed_K;
unsigned char pwm;

//******************************************************************************
// Function Prototypes
//******************************************************************************

void init_demonstration(void);
unsigned char convert_dispos(enum turn turned);
unsigned char convert_speed(unsigned char speed);
void configure_ADC(void);
unsigned int get_ADC_value(void);
void dispos(void);
//unsigned char calc_speed_right(void);
//unsigned char calc_speed_left(void);
void sensors_init(void);
void motors_init(void);
void motors_drive(void);


//******************************************************************************
// Functions for tests
//******************************************************************************

//void test_init(void){
//    TRISBbits.RB0 = 1;          //PB2
//    TRISH = 0xFF;
//    TRISC = 0xFF;
//}

void display_timer(unsigned int dtimer){
    LATF = dtimer;
    
}

void display_mode(void){
    if(PORTCbits.RC3==1){                       //switch bit1
        dispos();                               //show sensors
        LATF = convert_dispos(buggy_turn);
        return ;
    }
    else if(PORTCbits.RC4==1){          //switch bit2
        LATF = TIMER1;                  //show timer1
        return ;
    }
    else if(PORTCbits.RC5==1){          //switch bit3
        LATF = TIMER3;                  //show timer3
        return ;
    }
    else if(PORTHbits.RH4==1){          //switch bit4, change motor speed

    }


}

//******************************************************************************
// Main Program
//******************************************************************************

void main(void){
    ADCON1= 0x0F;
    motors_init();
    sensors_init();
    init_demonstration();
    //test_init();

    while(1){

        display_mode();


    }
}


//******************************************************************************
// Functios
//******************************************************************************

void init_demonstration(void){

    TRISF = 0x00;            //LEDs outputs
    LATF = 0x00;             //clear LEDs
    TRISHbits.RH0 = 0;       //U1 enabled
    TRISHbits.RH1 = 0;       //U2 enabled
    LATHbits.LATH0 = 1;      //off
    LATHbits.LATH1 = 1;      //off
    TRISC = 0xFF;           //outputs for sw
    TRISH = 0xFF;           //outputs for sw

}

unsigned char convert_dispos(enum turn turned){
    if (turned==FLEFT)
        return 0b11000000;
    else if (turned==LEFT)
        return 0b01100000;
    else if (turned==STRAIGHT)
        return 0b00011000;
    else if (turned==fullSTRAIGHT)
        return 0b00111100;
    else if (turned==RIGHT)
        return 0b00000110;
    else if (turned==FRIGHT)
        return 0b00000011;
    
        return 0;
}

unsigned char convert_speed(unsigned char speed){
    return speed*speed_K;
}

void configure_ADC(void)
{
    TRISAbits.RA0 = 1;
    OpenADC(ADC_FOSC_16 & ADC_RIGHT_JUST & ADC_2_TAD,
            ADC_CH0 & ADC_INT_OFF & ADC_VREFPLUS_VDD & ADC_VREFMINUS_VSS,
            14);

}

unsigned int get_ADC_value(void)
{
	ConvertADC();
	while (BusyADC())
		;
	return ReadADC();
}


void dispos(void){
    
    if(STRAIGHT1detected==0){

//        if(STRAIGHT2detected==0){
//            buggy_turn = fullSTRAIGHT;
//        }
//        else
            buggy_turn = STRAIGHT;
            if(STRAIGHT1detected==1){       //detecting line break
                if(LEFTdetected==1 && RIGHTdetected==1){
                    buggy_turn = BREAK;
                    delay();                //how long should it be??
                    buggy_turn = STOP;
                }
            }
    }

    else if(LEFTdetected==0) {
        buggy_turn = LEFT;
    }
    
    else if(FLEFTdetected==0){
        buggy_turn = FLEFT;
    }

    else if(RIGHTdetected==0) {
        buggy_turn = RIGHT;
    }

    else if(FRIGHTdetected==0){
        buggy_turn = FRIGHT;
    }


}

//unsigned char calc_speed_right(void){
//    return (60/(N*TIMER3));
//}
//
//unsigned char calc_speed_left(void){
//    return (60/(N*TIMER1));
//}
void motors_init(void){
//	LATG=0X03;				//ENABLE,unipolar,BI
// 	LATE=0X05;				//DIRECTION1 AND 2

    TRISGbits.TRISG3 = 0;           //PWM1 output
    TRISGbits.TRISG4 = 0;           //PWM2 output
    //which values are we writing for directions??
    TRISJ = 0x00;
    LATJ = 0x
	OpenPWM4(248);                          //frequency of 10k
	OpenPWM5(248);
	OpenTimer4( TIMER_INT_OFF & T2_PS_1_1 );
}

void motors_drive(){
	SetDCPWM4();
	SetDCPWM5();
}

void sensors_init(void){
    CCP1CON = 0b0101;     //CCP4 Capture mode for speed, rising edge
    CCP3CON = 0b0101;



    OpenTimer1( TIMER_INT_OFF &
                T1_8BIT_RW &
                T1_SOURCE_INT &
                T1_PS_1_1 &
                T1_OSC1EN_OFF &
                T1_SYNC_EXT_OFF &
                T12_CCP12_T34_CCP345
                );

    OpenTimer3( TIMER_INT_OFF &
                T1_8BIT_RW &
                T1_SOURCE_INT &
                T1_PS_1_1 &
                T1_OSC1EN_OFF &
                T1_SYNC_EXT_OFF &
                T12_CCP12_T34_CCP345
                );


//ports as inputs
    TRISCbits.TRISC2 = 1;
    TRISGbits.TRISG0 = 1;
    TRISB=0xFF;                 //inputs for front sensors

    INTCONbits.PEIE = 1;	// Enable peripheral interrupt
    INTCONbits.GIE = 1;		// Enable global interrupt

    PIE1bits.CCP1IE = 1;        //RC2/ECCP1
    PIE3bits.CCP3IE = 1;        //RG0/ECCP3
}

void interrupt isr(void){
    static char TIMER1read = 0;
    static char TIMER3read = 0;
    if(PIR1bits.CCP1IF){
        PIR1bits.CCP1IF = 0;
        if(TIMER1read==0){
            WriteTimer1(0);
            TIMER1read = 1;
        }
        else{
            TIMER1 = ReadTimer1();
            TIMER1read = 0;
        }

    }
    if(PIR3bits.CCP3IF){
        PIR3bits.CCP3IF=0;
        if(TIMER3read==0){
            WriteTimer3(0);
            TIMER3read = 1;
        }
        else{
            TIMER3 = ReadTimer1();
            TIMER3read = 0;
        }
    }
}
