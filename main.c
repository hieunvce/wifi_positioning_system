#include "wps.h"
#include "configMSP430.h"

volatile int pass=0;

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
volatile int writeData=0;
char data[50]={'\0'};
volatile unsigned int dataIndex=0;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	Configure_Clock();
	Configure_IO();
	Configure_UART();
	Configure_Timer();

	int RSSI[3]={0,0,0};
	int DISTANCE[3]={ 0,0,0 };//Embedded, ES02, ES03
	int COORDINATESOFAPS[6]={ 0,0,0,0,0,0 };//Always keep it {0,0,0,0,0,0}
	float LOCATION[2]={0.0,0.0};

	UARTSendString("AT+RST\r\n");
	SendATCommand("ATE0");
	SendATCommand("AT+CWLAPOPT=0,4");
	SendATCommand("AT+CWMODE=1");
	SendATCommand("AT+CWJAP=\"Embedded System\",\"12345678\"");
//---------------------------------------------------------------------
	UARTSendString("AT+CIPSTART=\"TCP\",\"192.168.1.101\",9999\r\n");
	getDataFromServerFlag=1;
	while (getDataFromServerFlag){}
	UARTSendString(data);
	GetCoordinatesOfAPs(data,COORDINATESOFAPS);
	//600,650,900,240,0,0
//---------------------------------------------------------------------
	while(1){
	    do{
	UARTSendString("AT+CWLAP=\"Embedded System\"\r\n");
	getRSSIFlag=1;
	Delay(4);
	    }while (RSSIString[1]=='0');
	        do{
	UARTSendString("AT+CWLAP=\"ES_02\"\r\n");
	getRSSIFlag=1;
	Delay(4);
	        }while (RSSIString[4]=='0');
	        do{
	UARTSendString("AT+CWLAP=\"ES_03\"\r\n");
	getRSSIFlag=1;
	Delay(5);
	        }while (RSSIString[7]=='0');
	__delay_cycles(400000);
	ConvertRSSI2Number(RSSI,RSSIString);
	calculateDistance(RSSI,DISTANCE);

	//Calculate location from data
	calculateLocation(DISTANCE,COORDINATESOFAPS,LOCATION);

	//Send location to server
	 SendATCommand("AT+CIPMODE=1");
	 UARTSendString("AT+CIPSEND\r\n");
	 Delay(5);
	 SendLocationToServer(LOCATION);
	 SendLogToServer(RSSI,DISTANCE);
	 Delay(10);
	 UARTSendString("+++");
	 Delay(6);
	 SendATCommand("AT+CIPMODE=0"); //disable UART-wifi passthrough mode
	 UARTSendString("AT+CIPCLOSE");  //close TCP server
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


    //Get data from server
    if (getDataFromServerFlag==1)
    {
        if (character == '.'){
            getDataFromServerFlag=0;
        }
        if (character == '+')
            writeData=1;
        if (writeData==1){
        data[dataIndex++]=character;
        }
    }
}

	#pragma vector=TIMER0_A0_VECTOR
	__interrupt void Timer_A (void)
	{
		timeUp = 1;
		TACCTL0 = 0;
		TACCR0 = 8000;
	}
