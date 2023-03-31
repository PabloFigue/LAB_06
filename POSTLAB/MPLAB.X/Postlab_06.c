/* 
 * File:   Postlab_06.c
 * Author: pablo
 *
 * Created on 31 de marzo de 2023, 12:19 a.m.
 */

#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF          // Code Protection bit (Program memory code protection is enabled)
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

int adc1;
int adc2;
int disp0, disp1, disp2;


// TABLA DE CONVERSION -> VALORES PARA LOS DISPLAYS (Unicamente usaremos los primeros 9)
uint8_t TABLA[16] = {0b00111111,    //0
                     0b00000110,    //1
                     0b01011011,    //2
                     0b01001111,    //3
                     0b01100110,    //4
                     0b01101101,    //5
                     0b01111101,    //6
                     0b00000111,    //7
                     0b01111111,    //8
                     0b01100111,    //9
                     0b01110111,    //A
                     0b01111100,    //B
                     0b00111001,    //C
                     0b01011110,    //D
                     0b01111001,    //E
                     0b01110001};   //F 


/**********PROTOTIPOS**********/

void setup(void);
void calculo(int adc1);

/**********INTERRUPCIONES**********/
void __interrupt () isr(void)
{
    if(T0IF)    //tmr0
    { 
        PORTB ++;
        TMR0 = _tmr0_value;
        INTCONbits.T0IF = 0;
    }
    if (ADIF)   //Interrupcion del ADC
    { 
        if (ADCON0bits.CHS == 0b0000) {         // Canal 0
            adc1 = ADRESH;
            calculo(adc1);
            ADCON0bits.CHS = 0b0001;            // cambia a canal 1
        } else if (ADCON0bits.CHS == 0b0001){   // Canal 1
            PORTD = ADRESH;
            ADCON0bits.CHS = 0b0000;            // Canal 0
        }
        PIR1bits.ADIF = 0;
    }
    return;
}

/**********CÓDIGO PRINCIPAL**********/


void main(void) 
{
    setup();
    while(1)   //loop principal
    { 
        ADCON0bits.GO = 1;      //Iniciar a convertir
        while (ADIF == 0); 
        __delay_ms(10);
        
    }
}

/**********FUNCIONES**********/

    
void calculo(int adc1)
{
    int voltaje = (int)(adc1*((float)(5/255)*(100))); //Conversión del ADC a voltaje (Se multiplica por 100 para mostrar 3 displays en una representación de 0.01V)
    disp2 = (uint8_t)(voltaje/100);    //Calcular las Unidades de Voltio, que corresponde al display más significativo por lo que se divide entre 100 y se redondea.
    int residuo = voltaje%100;         //El residuo es el restante de la division entre 2 numeros gracias al módulo (%)                          
    disp1 = residuo/10;                //El display 1 corresponde al primer Decimal, por lo que se divide entre 10 (decenas del voltaje)
    disp0 = residuo%10;                //El display directamente es el restante de la división de las decenas de voltaje entre 10.
    return;    
}

void setup(void)
{
    //configuracion de entradas y salidas
    ANSELH = 0;
    
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;   
    
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
        

    OSCCONbits.IRCF = 0b111;    //8MHz
    OSCCONbits.SCS = 1;         //prescaler al TMR0
    
    //configuración tmr0
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b111;
    TMR0 = _tmr0_value;
    
    //Configuración de las interrupciones
    INTCONbits.T0IF = 0; // Limpiar la bandera de interrupcion del TMR0
    INTCONbits.T0IE = 1; // Habilitacion de la interrupcion del TMR0
    PIR1bits.ADIF = 0;   // limpiar la bandera de interrupcion del ADC
    PIE1bits.ADIE = 1;   // habilitar interrupciones de ADC
    INTCONbits.GIE = 1;  // Usualmente la global se enciende de [ultimo]
   // INTCONbits.PEIE = 1; // habilitar interrupciones perifericas
    
    //Configuracion ADC
    ANSEL = 0b00000011;
    TRISA = 0b00000011;
    ADCON0bits.ADCS = 0b10;     //Conversion Clock
    ADCON0bits.CHS = 0b0000;    //Canal 0
    __delay_ms(1);
    ADCON1bits.ADFM = 0;    //justificado a la izquierda
    ADCON1bits.VCFG0 = 0;   //Vdd
    ADCON1bits.VCFG1 = 0;   //Vss
    ADCON0bits.ADON = 1;    //ADC enable
    ADIF =  0;
    
   
    return;
}
