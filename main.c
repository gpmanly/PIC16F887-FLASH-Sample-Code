/* 
 * File:   main.c
 * Author: A19356
 *
 * Created on July 19, 2017, 2:22 PM
 */
/*============================================================================
 Hardware:
 
 =============================================================================*/



/*==============================================================================
 ===== HEADERS =================================================================
 =============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

/*
 * 
 */

/*==============================================================================
 ===== DEFINES =================================================================
 =============================================================================*/
#define _XTAL_FOSC      4000000
#define laguna          0x1E
#define Female          0x00

/*==============================================================================
 ===== CONFIGURATIONS ==========================================================
 =============================================================================*/
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

/*==============================================================================
 ===== GLOBAL VARIABLES ========================================================
 =============================================================================*/
typedef struct {    //construct a 14bit wide word
    unsigned HIGHBYTE:  6;
    unsigned LOWBYTE:   8;
    
} word;

typedef struct {    //construct a 14bit wide address
    unsigned MSB:   6;
    unsigned LSB:   8; 
} destination_address;

/*==============================================================================
 ===== FUNCTION PROTOTYPES =====================================================
 =============================================================================*/
void initSystem(void);
void initOSC(void);
void readFLASH(destination_address adr, word *PMBYTE);

void writeFLASHWr8(destination_address addr, word *PMBYTE){
    //load the start address
    //program must be written 8words sequential
    //ensure to start at a lower boundary defined by an address EEADR<2:0>=000
    EEADR = addr.LSB;               //LSB of the dest address to EEADR
    EEADRH = addr.MSB;              //MSB of the dest address to EEADRH
    
    for(int i = 0; i < 8; i++){         //fill all the buffers
        EEDATA = PMBYTE[i].LOWBYTE;       //LSB of the program data to EEDATA
        EEDATH = PMBYTE[i].HIGHBYTE;      //MSB of the program data to EEDATH

        EECON1bits.EEPGD = 1;           //Point to program memory
        EECON1bits.WREN = 1;            //Enable write
        if(INTCONbits.GIE){             //save the bit of the GIE to CARRY
            STATUSbits.CARRY = 1;
        }
        GIE = 0;
        EECON2 = 0x55;                  //Required Sequence
        EECON2 = 0xAA;
        EECON1bits.WR = 1;              //begin writing
        _delay(2);                      //delay 2 instruction cycle
        EECON1bits.WREN = 0;            //Disable write
        if(STATUSbits.CARRY){           //restore interrupt
            GIE = 1;
        }
        EEADR++;                        //go to next address
    }
}

/*==============================================================================
 ===== INTERRUPT SERVICE ROUTINE ===============================================
 =============================================================================*/



/*==============================================================================
 ===== MAIN ====================================================================
 =============================================================================*/
int main(int argc, char** argv) 
{
    initSystem();
    word GP;
    word Chix[8] = {{Female,'A'},
                    {Female,'B'},
                    {Female,'C'},
                    {Female,'D'},
                    {Female,'E'},
                    {Female,'F'},
                    {Female,'G'},
                    {Female,'H'}};
    char Van[8];
    
    //ONE BLOCK OF FLASH
    destination_address sta_cruz =  {laguna, 0x80};
    destination_address sta_rosa =  {laguna, 0x81};
    destination_address binan =     {laguna, 0x82};
    destination_address san_pedro = {laguna, 0x83};
    destination_address calamba =   {laguna, 0x84};
    destination_address los_banos = {laguna, 0x85};
    destination_address cabuyao =   {laguna, 0x86};
    destination_address san_pablo = {laguna, 0x87};
     
    writeFLASHWr8(sta_cruz, &Chix);
    
    readFLASH(sta_cruz, &GP);
    Van[0] = GP.LOWBYTE;     
    
    readFLASH(sta_rosa, &GP);
    Van[1] = GP.LOWBYTE;
    
    readFLASH(binan, &GP);
    Van[2] = GP.LOWBYTE;
    
    readFLASH(san_pedro, &GP);
    Van[3] = GP.LOWBYTE;
    
    readFLASH(calamba, &GP);
    Van[4] = GP.LOWBYTE;
    
    readFLASH(los_banos, &GP);
    Van[5] = GP.LOWBYTE;
    
    readFLASH(cabuyao, &GP);
    Van[6] = GP.LOWBYTE;
    
    readFLASH(san_pablo, &GP);
    Van[7] = GP.LOWBYTE;
    
    while(1);                   //infinite loop
    return (EXIT_SUCCESS);
}



/*==============================================================================
 ===== FUNCTIONS ===============================================================
 =============================================================================*/
void initSystem(void){
    initOSC();
}

void initOSC(){
    OSCCONbits.IRCF = 0b110;    //4Mhz
    OSCCONbits.OSTS = 0;
    OSCCONbits.HTS = 1;
    OSCCONbits.LTS = 1;
    OSCCONbits.SCS = 1;
}

void readFLASH(destination_address addr, word *PMBYTE){
    EEADR = addr.LSB;                //address LSB to EEADR
    EEADRH = addr.MSB;               //address MSB to EEADRH
    EEPGD = 1;                      //Point to Program Memory
    RD = 1;                         //Start Reading
    _delay(2);                      //delay 2 instruction cycle
    PMBYTE->LOWBYTE = EEDAT;             //Store LSB OPCODE 
    PMBYTE->HIGHBYTE = EEDATH;           //Store MSB OPCODE
}


