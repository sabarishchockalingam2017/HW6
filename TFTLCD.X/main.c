#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "ILI9163C.h"

// PIC32MX250F128B Configuration Bit Settings

// 'C' source line config statements
// DEVCFG0
#pragma config DEBUG = 0b11 // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_12 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 2017 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

void delay(void);

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    SPI1_init();
    LCD_init();
    __builtin_enable_interrupts();
    
    TRISBbits.TRISB4=1; //making B4 into PORT for usrbutton
    TRISBbits.TRISB8=1; //making B4 into PORT for usrbutton
    TRISBbits.TRISB7=0; //making B7 into LAT for output
    TRISAbits.TRISA4=0; //making A4 into LAT for LED output
    LATBbits.LATB7=1;
    LATAbits.LATA4=1;
    LCD_clearScreen(0x00FF);
    char buffer[50];
    sprintf(buffer,"Hello world %%d!");
    LCD_writeString(28,32,0xFC00,buffer);
    sprintf(buffer,"fps: ");
    LCD_writeString(28,60,0xFC00,buffer);

    unsigned char count=0;
    char cb[50];
    float fps;
    unsigned short ticks;
    
    
    while(1) {
//        LATBbits.LATB7=!PORTBbits.RB4;
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT()<4800000){
            ;
            while(!PORTBbits.RB4) {
                  ;   // Pin B4 is the USER switch, low (FALSE) if pressed.
             }
        }
        
        _CP0_SET_COUNT(0); 
        sprintf(buffer,"Hello world %%d!");
        LCD_writeString(28,32,0xFC00,buffer);
        sprintf(buffer,"fps: ");
        LCD_writeString(28,60,0xFC00,buffer);
        LCD_drawBar(20,42,0x0FF0,100,4,count);
        count++;
        sprintf(cb,"%d  ",count);
        LCD_writeString(62,49,0xFC00,cb);   
        fps=(float)24000000/_CP0_GET_COUNT();  
        
        sprintf(cb,"%5.2f",fps);
        LCD_writeString(58,60,0xFC00,cb);
        
        sprintf(buffer,"aaaaaaaaaaaaaaaaaaaaaaaaa");
        _CP0_SET_COUNT(0); 
        LCD_writeString(0,0,0xFC00,buffer);
        ticks=_CP0_GET_COUNT();
        sprintf(cb,"row ticks: %d",ticks);
        LCD_writeString(2,69,0xFC00,cb);
        if(count==100){
            count=0;
        }
        

        LATAINV=0b10000;
        
	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
		  // remember the core timer runs at half the sysclk
    }
    return 0;
}
