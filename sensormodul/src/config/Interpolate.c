/*
 * interpolate.c
 *
 * Created: 11/20/2013 3:59:36 PM
 *  Author: hamni509
 */ 

#include "Interpolate.h"

float interpolate(uint8_t X1, uint8_t X2, uint8_t Y1, uint8_t Y2,uint8_t X)
{
	return ((X-X1)*(Y2-Y1)/(X2-X1))+Y1;
}