/*
 * File:   main.c
 * Author: ian martins mendes e vinicius magnus
 *
 * Created on 24 April 2025, 15:26
 */


#include <xc.h>
#include <pic16f877a.h>
#include <stdio.h>
#include <stdbool.h>
#define _XTAL_FREQ 4000000



#pragma config FOSC = XT        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

#define T1H 0x0B
#define T1L 0xDC 

#include "Lib/lcd.h"

#define Motor_Active PORTCbits.RC0
#define Motor_Dir PORTCbits.RC1
#define Light_UP PORTCbits.RC3
#define Light_DWN PORTCbits.RC4
#define Cancel PORTCbits.RC2

// PortB é duas entradas 3 bit que devem ser processadas para separar

bool isFull();
bool isEmpty();
bool enqueue(int value);
bool dequeue();
bool top();

__bit moving = 0;
int queue[6];
int currentFloor = 0;
int pointer = 0;
int front = 0;
int rear = 0;
int count = 0;

char TargetFloor = 0;

int currentReading = 0;
char floor =0;
char floorbuff = 0;
char buff = 0;
char curfloor = 0;


    
void main(void) {
    CLRWDT();
    char Line1[20]; //vari?vel para o fun??o sprintf
    char Line2[10]; //vari?vel para o fun??o sprintf
    
    TRISB = 0xff; // IO port b
    TRISD = 0x00; // IO port D
    TRISC = 0b00000100;
    Lcd_Init();
    for(int i=0;i<=6;i++){
        queue[i]=0;
    }
    
    OPTION_REGbits.nRBPU    = 1; //portb PullUp
    OPTION_REGbits.INTEDG   = 1; //interrupt on port0 rising edge 0 -> 1
    OPTION_REGbits.T0CS     = 1; //TMR0 Clock Source Select bit 
    OPTION_REGbits.T0SE     = 1; // TMR0 Source Edge Select bit
    OPTION_REGbits.PSA      = 1; //Prescaler Assignment bit       1 = Prescaler is assigned to the WDT  0 = Prescaler is assigned to the Timer0 module  
    OPTION_REGbits.PS0      = 1; // Prescaler Rate Select bits
    OPTION_REGbits.PS1      = 1;
    OPTION_REGbits.PS2      = 1;
                    
    INTCONbits.GIE    = 1;  // global interrupt enable     
    INTCONbits.PEIE   = 1;  // Peripheral Interrupt Enable bit   
    INTCONbits.TMR0IE = 0;  // TMR0 Overflow Interrupt Enable bit     
    INTCONbits.INTE   = 1;  // RB0/INT External Interrupt Enable bit
    INTCONbits.RBIE   = 0;  // RB Port Change Interrupt Enable bit     
    INTCONbits.TMR0IF = 0;  // TMR0 Overflow Interrupt Flag bit    (must be cleared in software)   
    INTCONbits.INTF   = 0;  // RB0/INT External Interrupt Flag bit (must be cleared in software)
    INTCONbits.RBIF   = 0;  // RB Port Change Interrupt Flag bit   (must be cleared in software)
    
    PIE1bits.TMR1IE = 1; //Habilita int do timer 1
    PIE1bits.ADIE = 0;     // Enable ADC interrupt

    
    
    ADCON1bits.PCFG0 = 0;
    ADCON1bits.PCFG1 = 1;
    ADCON1bits.PCFG2 = 1;
    ADCON1bits.PCFG3 = 1;
    
    
    ADCON0bits.ADCS0 = 0;
    ADCON0bits.ADCS1 = 0;
    
    
    ADCON1bits.ADFM = 0;
    
    ADRESL = 0x00;
    ADRESH = 0X00;
    
    ADCON0bits.ADON = 1;
    
    

    
    
    
    // set up for TIMER
    T1CONbits.T1CKPS = 1;
    T1CONbits.T1CKPS0 = 1;
    T1CONbits.T1OSCEN = 0;
    T1CONbits.T1SYNC = 0;
    T1CONbits.TMR1CS = 0;
    PIR1bits.TMR1IF = 0; 
    //
    TMR1H = 0x17;        
    TMR1L = 0xB8;
    TMR1L = T1L; //carga do valor inicial no contador (65536-62500)
    TMR1H = T1H;
    PORTC=0x00;
    
    Light_UP = 1;
    Light_DWN = 1;
            
    
    T1CONbits.TMR1ON = 1;
    while(1){
        CLRWDT();
        
       
   //detecta o andar atual     
        floorbuff = PORTB;
        floor = floorbuff >> 5;
        //atualiza o display de 7 segmentos
       
        if (floorbuff & (1 << 4)) {
        curfloor = floor;
        
        }
        if (curfloor & (1 << 0)) {
            PORTCbits.RC5 = 1;
        } else {
            PORTCbits.RC5 = 0;
        }
        if (curfloor & (1 << 1)) {
            PORTCbits.RC6 = 1;
        } else {
            PORTCbits.RC6 = 0;
        }
        if (curfloor & (1 << 2)) {
            PORTCbits.RC7 = 1;
        } else {
            PORTCbits.RC7 = 0;
        }
   //
        CLRWDT();
        Lcd_Clear();
        sprintf(Line1, " %i A | fl: %i:%i ", currentReading,curfloor,TargetFloor); //Armazena em buffer o conte?do da vari?vel f formatado com duas casas;
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_String(Line1);
        
        if (Cancel == 1){
            TargetFloor = curfloor;
        }
        
        if(curfloor == TargetFloor){
               Motor_Active = 0;
               Light_UP = 1;
               Light_DWN = 1;
        }else{
            if((curfloor > TargetFloor)){
                    Motor_Active = 1;
                    Motor_Dir = 1;
                    Light_UP = 1;
                    Light_DWN = 0;
            }else{
                    Motor_Active = 1;
                    Motor_Dir = 0;
                    Light_UP = 0;
                    Light_DWN = 1;
            }
                
        }
        
        
        
        
        
   
           
        
        
    }
    
    return;
}

void __interrupt() TrataInt(void){
    
    
    if (PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0;     // Clear flag

        // Reload timer
        TMR1H = 0x15;
        TMR1L = 0xF8;

         if (ADCON0bits.GO_nDONE == 0) {
            ADCON0bits.GO_nDONE = 1;
            }
        
        
    }
    
    
    if(INTF == 1){
       
        INTCONbits.INTF = 0;
        buff = PORTB;
        buff = buff & 0x0E;
        if (TargetFloor == curfloor){
            TargetFloor = buff >> 1;
        }
        //enqueue(buff >> 1);
        
        
    }
    
    if (PIR1bits.ADIF) {
        PIR1bits.ADIF = 0;
        currentReading = ADRESH*2;
        if (currentReading> 50){
            Motor_Active = 0;
            
        }
        
    }
    
    CLRWDT();
}



//funcoes para a fila
/*
bool isFull() {
    return count == 6;
}


bool isEmpty() {
    return count == 0;
}


bool enqueue(int value) {
    if (isFull()) {
        return false; // Queue is full
    }
    if(!isEmpty()){
        for (int i = 0; i < count; i++) {
        if (queue[i] == value) {
            return false;
        }
    }
    }
    queue[rear] = value;
    rear = (rear + 1) % 6;
    count++;
    return true;
}

bool top() {
    if (isEmpty()) {
        return false; // Queue is empty
    }
    TargetFloor = queue[front];
    return true;
}

bool dequeue() {
    if (isEmpty()) {
        return false; // Queue is empty
    }
    TargetFloor = queue[front];
    front = (front + 1) % 6;
    count--;
    return true;
}

*/