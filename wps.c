
#include "wps.h"
#include "msp430g2553.h"

ATReturnStatus STT;
extern char buffer[7];
extern int RSSI[3];
extern float DISTANCE[3];
extern int COORDINATESOFAPS[6];
extern float LOCATION[2];
extern int timeUp;
//===========FUNCTIONS=====================================================

void UARTSendByte(unsigned char byte)
{
    while (!(IFG2 & UCA0TXIFG)); // Doi den khi bo dem truyen thong san sang

    UCA0TXBUF = byte; // Gan du lieu de tu dong truyen di
}

void UARTSendString(char* string)
{
    unsigned int i=0;
    while(string[i])
    {
        while((UCA0STAT & UCBUSY));
        UCA0TXBUF = string[i++];
    }
}

void SendATCommand(char *command)
{
    while (CheckATReturn() != 1){
    UARTSendString(command);
    UARTSendByte('\r');
    UARTSendByte('\n');
    Delay(5);
    }
}

void Delay(unsigned int milisecond)
{
    TACCTL0 = CCIE;
    while (milisecond>0){
        while (!timeUp){}
        milisecond--;
        timeUp=0;
        TACCTL0 = CCIE;
    }
    P1OUT ^= BIT0;
}

char UARTReadChar()
{
    while(!(IFG2 & UCA0TXIFG));
    return UCA0RXBUF;
}

int Compare2String(char *string,const char *value, unsigned int n)
{
    unsigned int m=0;
    for (m=0;m<n;m++)
        if(string[m] != value[m])
            return 0;
    string[0]='\0';
    return 1;
}


void ConvertRSSI2Number(char *rssiString)
{
    RSSI[0]=(10*(rssiString[1]-'0')+(rssiString[2]-'0'));
    RSSI[1]=(10*(rssiString[4]-'0')+(rssiString[5]-'0'));
    RSSI[2]=(10*(rssiString[7]-'0')+(rssiString[8]-'0'));

}

void calculateDistance(int rssi[])
{
    DISTANCE[0] = (powf(10.0,(-40+rssi[0])/20.0));
    DISTANCE[1] = (powf(10.0,(-40+rssi[1])/20.0));
    DISTANCE[2] = (powf(10.0,(-40+rssi[2])/20.0));
}

void calculateLocation(float distance[], int coordinatesOfAPs[])
{

    float d1=distance[0];
    float d2=distance[1];
    float d3=distance[2];

    int x1=coordinatesOfAPs[0];
    int y1=coordinatesOfAPs[1];
    int x2=coordinatesOfAPs[2];
    int y2=coordinatesOfAPs[3];
    int x3=coordinatesOfAPs[4];
    int y3=coordinatesOfAPs[5];

    float y=((x1-x3)*(d1*d1-d2*d2-x1*x1+x2*x2-y1*y1+y2*y2)-(x1-x2)*(d1*d1-d3*d3-x1*x1+x3*x3-y1*y1+y3*y3))/(-2*(y1-y2)*(x1-x3)+2*(y1-y3)*(x1-x2));//y
    float x=(d1*d1-d2*d2-x1*x1+x2*x2-y1*y1+y2*y2+2*y*y2-2*y*y2)/(-2*x1+2*x2);//x

    LOCATION[0]=x;
    LOCATION[1]=y;
}

int CheckATReturn()
{
    int returnValue=0;
    if ((Compare2String(buffer,"OK\r\n",4)))
    {
        returnValue=1;
    }
    else if ((Compare2String(buffer,"ERROR\r\n",7)))
    {
        returnValue=-1;
    }

    unsigned int clearBufferIndex=0;
    for (clearBufferIndex=0;clearBufferIndex<7;clearBufferIndex++)
        buffer[clearBufferIndex]='\0';
    return returnValue;
}

void UARTSendInt(int n)
{
    unsigned char buffer[7];
    int i,j;

    if (n==0){
        UARTSendByte('0');
        return;
    }
    if (n<0){
        UARTSendByte('-');
        n=~n+1;
    }
    for (i=6;i>=0 && n>0;i--){
        buffer[i]=(n%10)+'0';
        n/=10;
    }
    for (j=i+1;j<=6;j++){
        UARTSendByte(buffer[j]);
    }
}

void UARTSendFloat(double x, unsigned char coma)
    {
        unsigned long temp;
        if (coma > 4)
            coma = 4; // de trong 1 ki tu o dau cho hien thi dau
        if (x < 0)
        {
            UARTSendByte('-'); //In so am
            x *= -1;
        }
        temp = (unsigned long)x; // Chuyan thanh so nguyen.
        UARTSendInt(temp);
        x = ((float)x - temp);
        if (coma != 0)
            UARTSendByte('.'); // In ra dau "."
        while (coma > 0)
        {
            x *= 10;
            coma--;
        }
        temp = (unsigned long)(x + 0.5); //Lam tron
        UARTSendInt(temp);
    }

void SendLocationToServer(float location[])
{
    UARTSendString("Team 3H,(");
    UARTSendFloat(loc   ation[0],2);//Hoi Phu vu coma nay
    UARTSendString(",");
    UARTSendFloat(location[1],2);
    UARTSendString(")\r\n");
}

//+IDP,23:0,121,231,232,221,112,823,231,268.
void GetCoordinatesOfAPs(char *dataString) {
    volatile unsigned int i = 0;
    volatile unsigned int coordinatesIndex = 0;
    while (dataString[i] != ':')
    {
        i++;
    }
    i++;
    while (dataString[i] != '.' && coordinatesIndex<6)
    {
        if (dataString[i] != ',')
        {
            int temp = COORDINATESOFAPS[coordinatesIndex];
            temp = temp * 10 + (dataString[i] - '0');
            COORDINATESOFAPS[coordinatesIndex] = temp;
            i++;
        }
        else
        {
            i++;
            coordinatesIndex++;
        }
    }
}
