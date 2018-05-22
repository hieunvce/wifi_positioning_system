#include "wps.h"
#include "configMSP430.h"

int RSSI[3]={0,0,0};
int *p_rssi;
float DISTANCE[3]={0.0,0.0,0.0};
float *p_distance;
int COORDINATESOFAPS[6]={0};
int *p_coordinates;
float LOCATION[2]={0.0,0.0};
float *p_location;

//For Timer
unsigned int timeUp=0;

//For RSSI getting
volatile int getRSSIFlag=0;
char RSSIString[9]={'-','0','0','-','0','0','-','0','0'};
volatile unsigned int rssiStringIndex=0;
volatile unsigned int count=0;

//For buffer getting
char buffer[8]={'\0'};
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
	Configure_Timer();



	SendATCommand("ATE0");
	SendATCommand("AT+CWLAPOPT=0,4");
	SendATCommand("AT+CWMODE=1");
	while(1){
	UARTSendString("AT+CWLAP=\"Mang Day KTX\"\r\n");
	getRSSIFlag=1;
	Delay(8);
	UARTSendString("AT+CWLAP=\"windows10\"\r\n");
	getRSSIFlag=1;
	Delay(8);
	UARTSendString("AT+CWLAP=\"MangXaHoi\"\r\n");
	getRSSIFlag=1;
	Delay(8);

	ConvertRSSI2Number(RSSIString);
	calculateDistance(RSSI);


	//Get data from server
	SendATCommand("AT+CWJAP=\"windows10\",\"235723579\"");
	SendATCommand("AT+CIPSTART=\"TCP\",\"192.168.137.1\",5000");
	getDataFromServerFlag=1;
	while (getDataFromServerFlag){}
	UARTSendString(data);
	GetCoordinatesOfAPs(data);



	//Calculate location from data
	calculateLocation(DISTANCE,COORDINATESOFAPS);

	//Send location to server
	 SendATCommand("AT+CIPMODE=1");
	 UARTSendString("AT+CIPSEND\r\n");
	 Delay(5);
	 SendLocationToServer(LOCATION);
	 Delay(10);
	 UARTSendString("+++\r\n");
	 Delay(6);
	 SendATCommand("AT+CIPMODE=0"); //disable UART-wifi passthrough mode
	 SendATCommand("AT+CIPCLOSE");  //close TCP server
	 Delay(5);
	}

}

//INTERRUPT
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    char character = UARTReadChar();
    //Get RSSI String ----------------------------------------------------
    if (getRSSIFlag==1 || getRSSIFlag==2){
        if (rssiStringIndex>8){
            rssiStringIndex=0;
            unsigned int z=0;
            for (z=0;z<9;z++){
                RSSIString[z]='0';
            }
            RSSIString[0]='-';
            RSSIString[3]='-';
            RSSIString[6]='-';
        }
        P1OUT |= BIT6;
        if (character == '-')
            getRSSIFlag=2;
        if (getRSSIFlag==2)
        {
            count++;
            RSSIString[rssiStringIndex]=character;
            rssiStringIndex++;
            if (count==3)
            {
                count=0;
                getRSSIFlag=0;
            }
        }
    }
    buffer[bufferIndex]=character;
    bufferIndex++;
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

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    timeUp = 1;
    TACCTL0 = 0;
    TACCR0 = 8000;
}
