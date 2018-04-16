#ifndef WPS_H_
#define WPS_H_

#include "msp430g2553.h"
#include "math.h"

//======================================

void Configure_Clock(void);
void Configure_IO(void);
void Configure_UART(void);

void UARTSendByte(unsigned char byte);
void UARTSendString(char* TXData);

char UARTReadChar();
int Compare2String(char *string, const char *value, unsigned int n);

void ConvertRSSI2Number(char *rssi, int *r1, int *r2, int *r3);
float calculateDistance(int rssi);
void calculateLocation(float r1, float r2, float r3);

int EndOfReceiving(char *buffer);

#endif /* WPS_H_ */
