/*
 * configMSP430.c
 *
 *  Created on: May 17, 2018
 *      Author: Henry
 */

#include "configMSP430.h"

void Configure_Clock(void)
{
    if (CALBC1_8MHZ == 0xFF)//Neu calibration constant erased
    { while(1); }
    DCOCTL=0;
    BCSCTL1=CALBC1_8MHZ;
    DCOCTL=CALDCO_8MHZ;

    //BCSCTL2 |= SELM_0;
    BCSCTL2 |= SELM_3 + DIVM0;
    BCSCTL3 |= LFXT1S_2;
    // Chon nguon Clock CPU MCLK la DCO => Tan so hoat dong cua CPU la 1Mhz
    // Mac dinh MCLK duoc set la DCO roi nen co the bo qua
}

void Configure_IO(void)
{
    P1DIR |= BIT0+BIT6;
    P1OUT &= ~BIT0;
    P1OUT &= ~BIT6;
    P2OUT = 0;
}

void Configure_UART(void)
{
    P1SEL = BIT1+BIT2;//P1.1: RXD, P1.2: TXD
    P1SEL2 = BIT1+BIT2;//P1.1: RXD, P1.2: TXD

    UCA0CTL1 |= UCSWRST;//Bat RST len de bat dau cau hinh
    UCA0CTL0 = 0X00;
    /* Cau hinh:
     * Khong dung Parity BIT
     * Khung truyen/nhan 8 bit
     * 1 bit stop
     * Truyen bit thap truoc (LSB truoc)
     */

    UCA0CTL1 = UCSSEL_2; // Chon nguon Clock UART(SMCLK) la DCO 8MHz


    UCA0BR0 = 52; //8MHZ 9600
    UCA0BR1 = 00; //8MHZ 9600
    UCA0MCTL = UCBRF_1 | UCBRS_0 | UCOS16;//
    /* Theo bang 15-5 trong User guide:
     * 8Mhz 9600 co cac thong so nhu sau:
     * UCBRx=52, UCBRSx=0, UCBRFx=1, UCOS16=1
     * Ta chinh nhu sau:
     * (1) UCBRx gom 2 thanh ghi UCA0BR0 (byte thap) va UCA0BR1 (byte cao)
     * => UCBRx=52 => UCA0BR0 = 52 va UCA0BR1 = 00
     * (2) UCBRSx=0, UCBRFx=1, UCOS16=1 => UCA0MCTL = UCBRF_1 | UCBRS_0 | UCOS16
     */

    UCA0CTL1 &= ~UCSWRST; // Reset module de bat dau hoat dong
    IE2 |= UCA0RXIE; // Cho phep ngat nhan UART Rx
    __bis_SR_register(GIE);// Cho phep ngat toan cuc
}

void Configure_Timer(void)
{
    TACCTL0 = CCIE;
    TACTL=TASSEL_1 + MC_1 + ID_0; //SMCLK, Up, /1
    TACCR0 = 8000;
    __enable_interrupt();
    __bis_SR_register(GIE);
}

