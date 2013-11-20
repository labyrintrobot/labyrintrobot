/*
 * sensorTabel.h
 *
 * Created: 2013-11-18 18:46:34
 *  Author: Hampus Nilsson
 */ 


#ifndef SENSORTABEL_H_
#define SENSORTABEL_H_
#include <asf.h>
#include "Interpolate.h"

enum sensor_t{S12,S21,S22,S11,S23,S24,S13};

uint8_t sensorTabel(uint8_t data,enum sensor_t sensor);
uint8_t sensor11(uint8_t data);
uint8_t sensor12(uint8_t data);
uint8_t sensor13(uint8_t data);
uint8_t sensor21(uint8_t data);
uint8_t sensor22(uint8_t data);
uint8_t sensor23(uint8_t data);
uint8_t sensor24(uint8_t data);

#endif /* SENSORTABEL_H_ */