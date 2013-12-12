//================================================================
//================================================================
// Gyro.c
// Dai D Trinh
// Version 1.1
// 2013-11-23
//================================================================
//================================================================
#ifndef _GYRO_H_
#define _GYRO_H_


unsigned char SPI_EXCH (unsigned char output);
unsigned int SPI_Init(void);
unsigned int SPI_GetRate(void);
unsigned int SPI_GetTemp(void);
int SPI_GetNullvalue(void);

#endif


