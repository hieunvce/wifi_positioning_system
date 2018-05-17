
#include "wps.h"
#include "msp430g2553.h"

ATReturnStatus STT;
extern char buffer[7];
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
    UARTSendByte('\r');
    UARTSendByte('\n');
    UARTSendByte('\0');
}

char UARTReadChar()
{
    while(!(IFG2 & UCA0TXIFG));
    return UCA0RXBUF;
}

int Compare2String(char *string, char *value, unsigned int n)
{
    unsigned int m=0;
    for (m=0;m<n;m++)
        if(string[m] != value[m])
            return 0;
    string[0]='\0';
    return 1;
}


int * ConvertRSSI2Number(char *rssiString)
{
    static int rssi[3];
    rssi[0]=((rssiString[1]-'0')<<1+(rssiString[1]-'0')<<3+(rssiString[2]-'0'));
    rssi[1]=((rssiString[4]-'0')<<1+(rssiString[4]-'0')<<3+(rssiString[5]-'0'));
    rssi[2]=((rssiString[7]-'0')<<1+(rssiString[7]-'0')<<3+(rssiString[8]-'0'));
    return rssi;
}

float * calculateDistance(int rssi[])
{
    float distance[3]={0.0,0.0,0.0};
    distance[0] = (powf(10.0,(-40+rssi[0])/20.0));
    distance[1] = (powf(10.0,(-40+rssi[1])/20.0));
    distance[2] = (powf(10.0,(-40+rssi[2])/20.0));
    return distance;
}

float * calculateLocation(float distance[], int coordinatesOfAPs[])
{
    static float location[2];

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

    location[0]=x;
    location[1]=y;

    return location;
}

int EndOfReceiving(char *buffer)
{
    if (Compare2String(buffer,"OK\r\n",4)){
        P1OUT ^= BIT0;
        return OK;
    }
    else if (Compare2String(buffer,"ERROR\r\n",7))
        return ERROR;
    else if (Compare2String(buffer,">\r\n",3))
        return SENDDATA;
    else
        return ATUNKNOWN;
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

void WaitingFor(ATReturnStatus stt)
{
    while (EndOfReceiving(buffer)!=stt) {}
}
//void GetInfo(char *info, int &x1, int &y1, int &x2, int &y2, int &x3, int &y3)
//{

//}
