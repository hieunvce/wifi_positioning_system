#include "wps.h"

volatile int flag=0;
char buffer[7]={'\0'};
char data[9]={'\0'};
int getInfoFlag=0;
int temp=0;
int infoArray[4];
int infoIndex=0;

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

extern enum ATReturnStatus;

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

	UARTSendstring("AT+CWMODE=1\r\n");
	_delay_cycles(2000000);
	UARTSendString("AT+CWLAP=\"EmbeddedSystem\"\r\n");
	while (!EndOfReceiving(buffer)) {}

	UARTSendString("AT+CWLAP=\"ES_02\"\r\n");
	while (!EndOfReceiving(buffer)) {}

	UARTSendString("AT+CWLAP=\"ES_03\"\r\n");
	while (!EndOfReceiving(buffer)) {}

	ConvertRSSI2Number(data,&rssi1,&rssi2,&rssi3);
	d1=calculateDistance(rssi1);
	d2=calculateDistance(rssi2);
	d3=calculateDistance(rssi3);

	calculateLocation(d1,d2,d3,x1,y1,x2,y2,x3,y3);

	UARTSendString("AT+CWMODE=1\r\n");     // get rssi only //
	_delay_cycles(2000000);
	UARTSendString("AT+CWJAP=\"EmbeddedSystem\",\"12345678\"\r\n");
	_delay_cycles(10000000);
	UARTSendString("AT+CIPSTART=\"TCP\",\"192.168.1.78\",5000\r\n");
	getInfoFlag=1;//Bat flag de chuan bi nhan Info tu Server
	 _delay_cycles(5000000);



	 UARTSendString("AT+CIPMODE=1\r\n");
	 _delay_cycles(5000000);

	 UARTSendString("AT+CIPSEND\r\n");
	 _delay_cycles(1000000);
	 UARTSendString("3H,");
	 //----------------------------
	 //Gui thong so

	 //----------------------------
	 _delay_cycles(100000);
	 UARTSendString("+++");
	 _delay_cycles(2000000);
	 UARTSendString("AT+CIPMODE=0\r\n"); //disable UART-wifi passthrough mode
	 UARTSendString("AT+CIPCLOSE\r\n");  //close TCP server
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
    //+IDP,23:0,121,24231,23232,1221,21312,3123,12231,23.
    if (getInfoFlag==1)
    {
       if (character!=','){
           int tempo=character-'0';
           temp=temp*10+tempo;
       } else if (character==','){
           infoArray[infoIndex]=temp;
           temp=0;
           infoIndex++;
           if (infoIndex>3)
           {
               infoIndex=0;
               getInfoFlag=0;
           }
       }
    }
}
