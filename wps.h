#ifndef WPS_H_
#define WPS_H_

#include "math.h"

extern ATReturnStatus STT;
//======================================



void UARTSendByte(unsigned char byte);
void UARTSendString(char* TXData);
void SendATCommand(char *command);
void Delay(unsigned int milisecond);

char UARTReadChar();
int Compare2String(char *string, const char *value, unsigned int n);

void ConvertRSSI2Number(int rssi[]);
void calculateDistance(int rssi[], int distance[]);
void calculateLocation(int distance[], int coordinatesOfAPs[], float location[]);

int CheckATReturn();

void GetCoordinatesOfAPs(char *dataString, int coordinatesOfAPs[] );

void UARTSendInt(int n);
void UARTSendFloat(double x, unsigned char coma);
void StartTCPServer(char IP[], char port[]);
void SendLocationToServer(float location[]);
#endif /* WPS_H_ */
