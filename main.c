#include "wps.h"
#include "configMSP430.h"

int RSSI[3]={0,0,0};
int *p_rssi;
float DISTANCE[3]={0.0,0.0,0.0};
float *p_distance;
int coordinatesOfAPs[6]={0,0,0,0,0,0};

//For RSSI getting
volatile int getRSSIFlag=0;
char RSSIString[9]={'\0'};
volatile unsigned int rssiStringIndex=0;

//For buffer getting
char buffer[7]={'\0'};
volatile unsigned int bufferIndex=0;


void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	Configure_Clock();
	Configure_IO();
	Configure_UART();

	UARTSendString("ATE0");
	WaitingFor(OK);
	UARTSendString("AT+CWLAPOPT=0,4");
	WaitingFor(OK);
	UARTSendString("AT+CWMODE=3");
	WaitingFor(OK);
	UARTSendString("AT+CWLAP=\"EmbeddedSystem\"");
	getRSSIFlag=1;
	WaitingFor(OK);
	UARTSendString("AT+CWLAP=\"ES_02\"");
	getRSSIFlag=1;
	WaitingFor(OK);
	UARTSendString("AT+CWLAP=\"ES_03\"");
	getRSSIFlag=1;
	WaitingFor(OK);

	p_rssi=RSSI;
	p_rssi = ConvertRSSI2Number(RSSIString);
	p_distance=DISTANCE;
	p_distance=calculateDistance(RSSI);

	calculateLocation(d1,d2,d3,x1,y1,x2,y2,x3,y3);

	UARTSendString("AT+CWMODE=1");     // get rssi only //
	_delay_cycles(2000000);
	UARTSendString("AT+CWJAP=\"EmbeddedSystem\",\"12345678\"");
	_delay_cycles(10000000);
	UARTSendString("AT+CIPSTART=\"TCP\",\"192.168.1.78\",5000");
	getInfoFlag=1;//Bat flag de chuan bi nhan Info tu Server
	 _delay_cycles(5000000);



	 UARTSendString("AT+CIPMODE=1");
	 _delay_cycles(5000000);

	 UARTSendString("AT+CIPSEND");
	 _delay_cycles(1000000);
	 UARTSendString("3H,");
	 //----------------------------
	 //Gui thong so

	 //----------------------------
	 _delay_cycles(100000);
	 UARTSendString("+++");
	 _delay_cycles(2000000);
	 UARTSendString("AT+CIPMODE=0"); //disable UART-wifi passthrough mode
	 UARTSendString("AT+CIPCLOSE");  //close TCP server
	while(1){}
}

//INTERRUPT
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    char character = UARTReadChar();
    //Get RSSI String ----------------------------------------------------
    if (getRSSIFlag==1){
        P1OUT |= BIT6;
        if (character == '-')
            getRSSIFlag=2;
        if (getRSSIFlag==2)
        {
            if (character == ')'){
                getRSSIFlag=0;
                P1OUT |= ~BIT6;
            }
            RSSIString[rssiStringIndex++]=character;
        }
    }
    buffer[bufferIndex++]=character;
    if (character=='\n'){
        bufferIndex=0;
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
