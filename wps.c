
#include "wps.h"
#include "msp430g2553.h"

double x = 0.0;
double y = 0.0;

enum ATReturnStatus
{
    Ok = 0,
    Error = -1,
    Senddata = -2,
    UnknowATReturn = -3
};
//===========FUNCTIONS=====================================================

void UARTSendByte(unsigned char byte)
{
    while (!(IFG2 & UCA0TXIFG)); // Doi den khi bo dem truyen thong san sang

    UCA0TXBUF = byte; // Gan du lieu de tu dong truyen di
}

void UARTSendString(char* TXData)
{
    unsigned int i=0;
    while(TXData[i])
    {
        while((UCA0STAT & UCBUSY));
        UCA0TXBUF = TXData[i++];
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
    unsigned int m=n-1;
    for (;m>=0;m--)
        if(string[m] != value[m])
            return 0;
    string[0]='\0';
    return 1;
}


void ConvertRSSI2Number(char *rssi, int *r1, int *r2, int *r3)
{
    *r1=(10*(rssi[1]-'0')+(rssi[2]-'0'));
    *r2=(10*(rssi[4]-'0')+(rssi[5]-'0'));
    *r3=(10*(rssi[7]-'0')+(rssi[8]-'0'));
}

float calculateDistance(int rssi)
{
    return (powf(10.0,(-40+rssi)/20.0));
}

void calculateLocation(float d1, float d2, float d3, int x1, int y1, int x2, int y2, int x3, int y3)
{
    y=((x1-x3)*(d1*d1-d2*d2-x1*x1+x2*x2-y1*y1+y2*y2)-(x1-x2)*(d1*d1-d3*d3-x1*x1+x3*x3-y1*y1+y3*y3))/(-2*(y1-y2)*(x1-x3)+2*(y1-y3)*(x1-x2));
    x=(d1*d1-d2*d2-x1*x1+x2*x2-y1*y1+y2*y2+2*y*y2-2*y*y2)/(-2*x1+2*x2);
}

int EndOfReceiving(char *buffer)
{
    if (Compare2String(buffer,"OK\r\n",4)){
        P1OUT ^= BIT0;
        return Ok;
    }
    else if (Compare2String(buffer,"ERROR\r\n",7))
        return Error;
    else if (Compare2String(buffer,">\r\n",3))
        return Senddata;
    else
        return UnknowATReturn;
}

void UARTSendInt(unsigned int n)
{
    unsigned char buffer[16];
    unsigned char i,j;

    if (n==0){
        UARTSendByte('0');
        return;
    }
    for (i=15;i>0 && n>0;i--){
        buffer[i]=(n%10)+'0';
        n/=10;
    }
    for (j=i+1;j<=15;j++){
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
//void GetInfo(char *info, int &x1, int &y1, int &x2, int &y2, int &x3, int &y3)
//{

//}
