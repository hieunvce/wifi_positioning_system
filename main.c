#include "wps.h"

volatile int flag=0;
char buffer[7]={'\0'};
char data[9]={'\0'};

int rssi1=0;
int rssi2=0;
int rssi3=0;

float d1=0.0;
float d2=0.0;
float d3=0.0;

volatile unsigned int i=0;
volatile unsigned int j=0;
volatile unsigned int k=0;
volatile unsigned int id=1;
volatile unsigned int on=0;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	Configure_Clock();
	Configure_IO();
	Configure_UART();

	UARTSendString("ATE0\r\n");
	while (!EndOfReceiving(buffer)) {}

	UARTSendString("AT+CWLAPOPT=0,4\r\n");
	while (!EndOfReceiving(buffer)) {}

	UARTSendString("AT+CWLAP=\"AI-THINKER_ACA782\"\r\n");

	while (!EndOfReceiving(buffer)) {}

	UARTSendString("AT+CWLAP=\"WATERFOUNTAINS\"\r\n");
	while (!EndOfReceiving(buffer)) {}

	UARTSendString("AT+CWLAP=\"ESP_ACA5B3\"\r\n");
	while (!EndOfReceiving(buffer)) {}

	ConvertRSSI2Number(data,&rssi1,&rssi2,&rssi3);
	d1=calculateDistance(rssi1);
	d2=calculateDistance(rssi2);
	d3=calculateDistance(rssi3);

	calculateLocation(d1,d2,d3);

	while(1){}
}

//INTERRUPT
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    char character = UARTReadChar();
    if (character == '-')
        on=1;
    else if (character == ')')
        on=0;
    if (on){
        data[j++]=character;
    }
    buffer[k++]=character;

    if (character=='\n'){
        k=0;
        P1OUT ^= BIT6;
        flag=1;
    }
}
