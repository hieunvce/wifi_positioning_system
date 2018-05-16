#ifndef WPS_H_
#define WPS_H_

#include "math.h"

//======================================



void UARTSendByte(unsigned char byte);
void UARTSendString(char* TXData);

char UARTReadChar();
int Compare2String(char *string, const char *value, unsigned int n);

void ConvertRSSI2Number(char *rssi, int *r1, int *r2, int *r3);
float calculateDistance(int rssi);
void calculateLocation(float d1, float d2, float d3, int x1, int y1, int x2, int y2, int x3, int y3);

int EndOfReceiving(char *buffer);
void UARTSendInt(unsigned int n);
void UARTSendFloat(double x, unsigned char coma);
void StartTCPServer(char IP[], char port[]);
//void GetInfo(char *info, int &x1, int &y1, int &x2, int &y2, int &x3, int &y3);
#endif /* WPS_H_ */
