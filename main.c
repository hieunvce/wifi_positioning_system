#include "wps.h"
#include "configMSP430.h"

int RSSI[3]={0,0,0};
int *p_rssi;
float DISTANCE[3]={0.0,0.0,0.0};
float *p_distance;
int COORDINATESOFAPS[6]={0,0,0,0,0,0};
int *p_coordinates;
float LOCATION[2]={0.0,0.0};
float *p_location;

//For RSSI getting
volatile int getRSSIFlag=0;
char RSSIString[9]={'\0'};
volatile unsigned int rssiStringIndex=0;

//For buffer getting
char buffer[7]={'\0'};
volatile unsigned int bufferIndex=0;

//For getting data from server
volatile int getDataFromServerFlag=0;
char data[50]={'\0'};
volatile unsigned int dataIndex=0;

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
	UARTSendString("AT+CWMODE=1");
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

	//Get data from server
	UARTSendString("AT+CWMODE=1");
	WaitingFor(OK);
	UARTSendString("AT+CWJAP=\"EmbeddedSystem\",\"12345678\"");
	WaitingFor(OK);
	UARTSendString("AT+CIPSTART=\"TCP\",\"192.168.1.78\",5000");
	WaitingFor(OK);
	getDataFromServerFlag=1;
	while (getDataFromServerFlag){}
	p_coordinates=COORDINATESOFAPS;
	p_coordinates=GetCoordinatesOfAPs(data);

	//Calculate location from data
	p_location=LOCATION;
	p_location=calculateLocation(DISTANCE,COORDINATESOFAPS);

	//Send location to server
	 UARTSendString("AT+CIPMODE=1");
	 WaitingFor(OK);
	 UARTSendString("AT+CIPSEND");
	 WaitingFor(SENDDATA);
	 SendLocationToServer(LOCATION);
	 UARTSendString("AT+CIPMODE=0"); //disable UART-wifi passthrough mode
	 WaitingFor(OK);
	 UARTSendString("AT+CIPCLOSE");  //close TCP server
	 WaitingFor(OK);
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
    //Get data from server
    if (getDataFromServerFlag==1)
    {
        if (character == '.'){
            getDataFromServerFlag=0;
        }
        data[dataIndex++]=character;
    }
}
