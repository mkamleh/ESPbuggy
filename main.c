/*
 * MAIN is here
 *
 * to add: motor pwm rotation and its control by adc to demonstrate speed conversion
 *
 */


#include "xc_config_settings.h"
#include "adc.h"


//#define FLEFTdetected       PORTJbits.RJ0
//#define LEFTdetected        PORTJbits.RJ1
//#define STRAIGHT1detected   PORTJbits.RJ2
//#define STRAIGHT2detected   PORTJbits.RJ3
//#define RIGHTdetected       PORTJbits.RJ4
//#define FRIGHTdetected      PORTJbits.RJ5

//for test purposes only, remove and uncomment previous for tech demo
#define FLEFTdetected       PORTHbits.RH7
#define LEFTdetected        PORTHbits.RH6
#define STRAIGHT1detected   PORTHbits.RH5
#define STRAIGHT2detected   PORTHbits.RH4
#define RIGHTdetected       PORTCbits.RC5
#define FRIGHTdetected      PORTCbits.RC4

//******************************************************************************
// Global Variables
//******************************************************************************

enum turn {FLEFT, LEFT, STRAIGHT, fullSTRAIGHT, RIGHT, FRIGHT, ERROR};
volatile enum turn buggy_turn = STRAIGHT;

volatile char TIMER1, TIMER3;
unsigned char pattern [] = {0x84, 0xf5, 0x4c, 0x64, 0x35, 0x26, 0x06, 0xf4, 
                        0x04, 0x34, 0x14, 0x04, 0x8E, 0x84, 0x0E, 0x1E, 0xFF};
unsigned char speed_K;

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

//******************************************************************************
// Functions for tests
//******************************************************************************

void test_init(void){
    TRISBbits.RB0 = 1;
    TRISH = 0xFF;
    TRISC = 0xFF;
}

//******************************************************************************
// Main Program
//******************************************************************************

void main(void){
    ADCON1= 0x0F;
    sensors_init();
    init_demonstration();
    test_init();

    while(1){

        dispos();
        LATF = convert_dispos(buggy_turn);

    }
}


//******************************************************************************
// Functios
//******************************************************************************

void init_demonstration(void){

    TRISF = 0x00;              //LEDs outputs
    LATF = 0x00;               //clear LEDs
    TRISHbits.RH0 = 0;      //U1 enabled
    TRISHbits.RH1 = 0;      //U2 enabled
    LATHbits.LATH0 = 1;     //off
    LATHbits.LATH1 = 1;     //off
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
        if(STRAIGHT2detected==0){
            buggy_turn = fullSTRAIGHT;
        }
        else
            buggy_turn = STRAIGHT;
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


void sensors_init(void){
    CCP4CON = 0b0101;     //CCP4 Capture mode for speed, rising edge
    CCP5CON = 0b0101;



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
    TRISGbits.TRISG0 = 1;
    TRISGbits.TRISG1 = 1;
    TRISJ=0xFF;                 //inputs for front sensors

    INTCONbits.PEIE = 1;	// Enable peripheral interrupt
    INTCONbits.GIE = 1;		// Enable global interrupt

    PIE3bits.CCP5IE = 1;        //RG4
    PIE3bits.CCP4IE = 1;        //RG3
}

void interrupt isr(void){
    static char TIMER1read = 0;
    static char TIMER3read = 0;
    if(PIR2bits.CCP2IF){
        PIR2bits.CCP2IF = 0;
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