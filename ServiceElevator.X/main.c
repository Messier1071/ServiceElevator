/*
 * File:   main.c
 * Author: ianma
 *
 * Created on 24 April 2025, 15:26
 */


#include <xc.h>
#include <pic16f877a.h>
#include <stdio.h>
#define _XTAL_FREQ 4000000

#pragma config FOSC = XT        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit

void main(void) {
    OPTION_REGbits.nRBPU    = 0; //portb PullUp
    OPTION_REGbits.INTEDG   = 1; //interrupt on port0 rising edge 0 -> 1
    OPTION_REGbits.T0CS     = 1; //TMR0 Clock Source Select bit 
    OPTION_REGbits.T0SE     = 1; // TMR0 Source Edge Select bit
    OPTION_REGbits.PSA      = 1; //Prescaler Assignment bit       1 = Prescaler is assigned to the WDT  0 = Prescaler is assigned to the Timer0 module  
    OPTION_REGbits.PS0      = 0; // Prescaler Rate Select bits
    OPTION_REGbits.PS1      = 0;
    OPTION_REGbits.PS2      = 0;
                    
    INTCONbits.GIE    = 1;  // global interrupt enable     
    INTCONbits.PEIE   = 0;  // Peripheral Interrupt Enable bit   
    INTCONbits.TMR0IE = 0;  // TMR0 Overflow Interrupt Enable bit     
    INTCONbits.INTE   = 0;  // RB0/INT External Interrupt Enable bit
    INTCONbits.RBIE   = 0;  // RB Port Change Interrupt Enable bit     
    INTCONbits.TMR0IF = 0;  // TMR0 Overflow Interrupt Flag bit    (must be cleared in software)   
    INTCONbits.INTF   = 0;  // RB0/INT External Interrupt Flag bit (must be cleared in software)
    INTCONbits.RBIF   = 0;  // RB Port Change Interrupt Flag bit   (must be cleared in software)
    
    TRISB = 0xff; // IO port b
    TRISD = 0x00; // IO port D
    
    
    while(1){
       
        
                        
    }
    
    return;
}
