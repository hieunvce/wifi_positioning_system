#ifndef WPS_H_
#define WPS_H_

#include "math.h"

typedef enum
{
    OK = 1,
    ERROR = 0,
    SENDDATA = 2,
    ATUNKNOWN = -1
}  ATReturnStatus;

extern ATReturnStatus STT;
//======================================



void UARTSendByte(unsigned char byte);
void UARTSendString(char* TXData);

char UARTReadChar();
int Compare2String(char *string, char *value, unsigned int n);

int * ConvertRSSI2Number(char *rssiString);
float * calculateDistance(int rssi[]);
float * calculateLocation(float distance[], int coordinatesOfAPs[]);

int EndOfReceiving(char *buffer);
void WaitingFor(int stt);

int * GetCoordinatesOfAPs(char *dataString);

void UARTSendInt(int n);
void UARTSendFloat(double x, unsigned char coma);
void StartTCPServer(char IP[], char port[]);
void SendLocationToServer(float location[]);
//void GetInfo(char *info, int &x1, int &y1, int &x2, int &y2, int &x3, int &y3);
#endif /* WPS_H_ */
