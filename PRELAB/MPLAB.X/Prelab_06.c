/* 
 * File:   Prelab_06.c
 * Author: pablo
 *
 * Created on 27 de marzo de 2023, 3:29 p.m.
 */

// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = ON          // Code Protection bit (Program memory code protection is enabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


/**********LIBRERIAS**********/

#include <xc.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pic16f887.h>

/**********DEFINIR CONSTANTES**********/

#define _tmr0_value 61
#define _XTAL_FREQ 8000000

/**********VARIABLES GLOBALES**********/

uint8_t var;

/**********PROTOTIPOS**********/
void setup(void);

/**********INTERRUPCIONES**********/
void __interrupt () isr(void)
{
    if(T0IF)
    {
        //tmr0
        PORTA ++;
        TMR0 = _tmr0_value;
        INTCONbits.T0IF = 0;
    }
    return;
}
/**********CÓDIGO PRINCIPAL**********/


/*
 * 
 */
void main(void) 
{
    setup();
    while(1)   //loop principal
    {
        PORTD ++;
        __delay_ms(10);
    }
}


/**********FUNCIONES**********/

void setup(void)
{
    //configuracion de entradas y salidas
    ANSEL = 0;
    ANSELH = 0;
    
    TRISA = 0;
    TRISD = 0;
    
    OSCCONbits.IRCF = 0b111;    //8MHz
    OSCCONbits.SCS = 1;         //prescaler al TMR0
    
    //configuración tmr0
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b111;
    TMR0 = _tmr0_value;
    
    //Configuración de las interrupciones
    INTCONbits.T0IF = 0;
    INTCONbits.T0IE = 1;
    INTCONbits.GIE = 1; //Usualmente la global se enciende de [ultimo]
    
    return;
}