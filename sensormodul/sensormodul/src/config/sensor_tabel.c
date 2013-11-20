/*
 * sensor_tabel.c
 *
 * Created: 2013-11-18 18:49:45
 *  Author: Hampus Nilsson
 */ 

#include "sensor_tabel.h"



uint8_t sensorTabel(uint8_t data,enum sensor_t sensor)
{
	float fData;
	switch(sensor)
	{
		case S11:
		fData = sensor11(data);
		break;
		case S12:
		fData = sensor12(data);
		break;
		case S21:
		fData = sensor21(data);
		break;
		case S22:
		fData = sensor22(data);
		break;
		case S23:
		fData = sensor23(data);
		break;
		case S24:
		fData = sensor24(data);
		break;
		default:
		fData = 0;
		break;
	}
	return (uint8_t)fData;
}

uint8_t sensor11(uint8_t data)
{
	return 0;
}
uint8_t sensor12(uint8_t data)
{
	float fData;
	uint8_t x1,x2,y1,y2;
	if(data > 221)
		return 255;
	else if(data > 154)
	{
		x1 = 221;
		x2 = 154;
		y1 = 5;
		y2 = 8;
	}
	else if(data > 103)
	{
		x1 = 154;
		x2 = 103;
		y1 = 8;
		y2 = 12;
	}
	else if(data > 79)
	{
		x1 = 103;
		x2 = 79;
		y1 = 12;
		y2 = 16;
	}
	else if(data > 63)
	{
		x1 = 79;
		x2 = 63;
		y1 = 16;
		y2 = 20;
	}
	else if(data > 51)
	{
		x1 = 63;
		x2 = 51;
		y1 = 20;
		y2 = 24;
	}
	else if(data > 45)
	{
		x1 = 51;
		x2 = 45;
		y1 = 24;
		y2 = 28;
	}
	else if(data > 39)
	{
		x1 = 45;
		x2 = 39;
		y1 = 28;
		y2 = 32;
	}
	else
		return 0;
	fData = interpolate(y1,y2,x1,x2,data);
	return (int)fData;
}
uint8_t sensor13(uint8_t data)
{
	return 0;
}
uint8_t sensor21(uint8_t data)
{
	float fData;
	uint8_t x1,x2,y1,y2;
	if(data > 219)
		return 255;
	else if(data > 192)
	{
		x1=25;
		x2=30;
		y1=219;
		y2=192;
	}
	else if(data > 148)
	{
		x1=30;
		x2=40;
		y1=192;
		y2=148;
	}
	else if(data > 120)
	{
		x1=40;
		x2=50;
		y1=148;
		y2=120;
	}
	else if(data > 100)
	{
		x1=50;
		x2=60;
		y1=120;
		y2=100;
	}
	else if(data > 84)
	{
		x1=60;
		x2=70;
		y1=100;
		y2=84;
	}
	else if(data > 76)
	{
		x1=70;
		x2=80;
		y1=84;
		y2=76;
	}
	else if(data > 68)
	{
		x1=80;
		x2=90;
		y1=76;
		y2=68;
	}
	else if(data > 60)
	{
		x1=90;
		x2=100;
		y1=68;
		y2=60;
	}
	else if(data > 56)
	{
		x1=100;
		x2=110;
		y1=60;
		y2=56;
	}
	else if(data > 50)
	{
		x1=110;
		x2=120;
		y1=56;
		y2=50;
	}
	else if(data > 48)
	{
		x1=120;
		x2=130;
		y1=50;
		y2=48;
	}
	else if(data > 44)
	{
		x1=130;
		x2=140;
		y1=48;
		y2=44;
	}
	else if(data > 40)
	{
		x1=140;
		x2=150;
		y1=44;
		y2=40;
	}
	else
		return 0;
	fData = interpolate(y1,y2,x1,x2,data);	
	return fData;
}
uint8_t sensor22(uint8_t data)
{
	float fData;
	uint8_t x1,x2,y1,y2;
	if(data > 222)
		return 255;
	else if(data > 195 )
	{
		x1 = 25;
		x2 = 30;
		y1 = 222;
		y2 = 195;
	}
	else if(data > 149)
	{
		x1 = 30;
		x2 = 40;
		y1 = 195;
		y2 = 149;
	}
	else if(data > 120)
	{
		x1 = 40;
		x2 = 50;
		y1 = 149;
		y2 = 120;
	}
	else if(data > 99)
	{
		x1 = 50;
		x2 = 60;
		y1 = 120;
		y2 = 99;
	}
	else if(data > 85)
	{
		x1 = 60;
		x2 = 70;
		y1 = 99;
		y2 = 85;
	}
	else if(data > 76)
	{
		x1 = 70;
		x2 = 80;
		y1 = 85;
		y2 = 76;
	}
	else if(data > 67)
	{
		x1 = 80;
		x2 = 90;
		y1 = 76;
		y2 = 67;
	}
	else if(data > 61)
	{
		x1 = 90;
		x2 = 100;
		y1 = 67;
		y2 = 61;
	}
	else if(data > 54)
	{
		x1 = 100;
		x2 = 110;
		y1 = 61;
		y2 = 54;
	}
	else if(data > 50)
	{
		x1 = 110;
		x2 = 120;
		y1 = 54;
		y2 = 50;
	}
	else if(data > 48)
	{
		x1 = 120;
		x2 = 130;
		y1 = 50;
		y2 = 48;
	}
	else if(data > 44)
	{
		x1 = 130;
		x2 = 140;
		y1 = 48;
		y2 = 44;
	}
	else if(data > 39)
	{
		x1 = 140;
		x2 = 150;
		y1 = 44;
		y2 = 39;
	}
	else
		return 0;
	fData = interpolate(y1,y2,x1,x2,data);
	return fData;

}
uint8_t sensor23(uint8_t data)
{
	float fData;
	uint8_t x1,x2,y1,y2;
	if(data > 190)
		return 255;
	else if (data >163)
	{
		x1 = 25;
		x2 = 30;
		y1 = 190;
		y2 = 163;
	}
	else if (data > 151)
	{
		x1 = 30;
		x2 = 40;
		y1 = 163;
		y2 = 151;
	}
	else if (data > 123)
	{
		x1 = 40;
		x2 = 50;
		y1 = 151;
		y2 = 123;
	}
	else if (data > 101)
	{
		x1 = 50;
		x2 = 60;
		y1 = 123;
		y2 = 101;
	}
	else if (data > 88)
	{
		x1 = 60;
		x2 = 70;
		y1 = 101;
		y2 = 88;
	}
	else if (data > 78)
	{
		x1 = 70;
		x2 = 80;
		y1 = 88;
		y2 = 78;
	}
	else if (data > 70)
	{
		x1 = 80;
		x2 = 90;
		y1 = 78;
		y2 = 70;
	}
	else if (data > 64)
	{
		x1 = 90;
		x2 = 100;
		y1 = 70;
		y2 = 64;
	}
	else if (data > 58)
	{
		x1 = 100;
		x2 = 110;
		y1 = 64;
		y2 = 58;
	}
	else if (data > 52)
	{
		x1 = 110;
		x2 = 120;
		y1 = 58;
		y2 = 52;
	}
	else if (data > 48)
	{
		x1 = 120;
		x2 = 130;
		y1 = 52;
		y2 = 48;
	}
	else if (data > 44)
	{
		x1 = 130;
		x2 = 140;
		y1 = 48;
		y2 = 44;
	}
	else if (data > 39)
	{
		x1 = 140;
		x2 = 150;
		y1 = 44;
		y2 = 39;
	}
	else
		return 0;
	fData = interpolate(y1,y2,x1,x2,data);
	return fData;
}
uint8_t sensor24(uint8_t data)
{
	float fData;
	uint8_t x1,x2,y1,y2;
	if(data > 219)
		return 255;
	else if(data > 193)
	{
		x1=25;
		x2=30;
		y1=219;
		y2=193;
	}
	else if(data > 148)
	{
		x1=30;
		x2=40;
		y1=192;
		y2=148;
	}
	else if(data > 120)
	{
		x1=40;
		x2=50;
		y1=148;
		y2=120;
	}
	else if(data > 98)
	{
		x1=50;
		x2=60;
		y1=120;
		y2=98;
	}
	else if(data > 84)
	{
		x1=60;
		x2=70;
		y1=100;
		y2=84;
	}
	else if(data > 75)
	{
		x1=70;
		x2=80;
		y1=84;
		y2=75;
	}
	else if(data > 68)
	{
		x1=80;
		x2=90;
		y1=76;
		y2=68;
	}
	else if(data > 60)
	{
		x1=90;
		x2=100;
		y1=68;
		y2=60;
	}
	else if(data > 54)
	{
		x1=100;
		x2=110;
		y1=60;
		y2=54;
	}
	else if(data > 48)
	{
		x1=110;
		x2=120;
		y1=54;
		y2=48;
	}
	else if(data > 44)
	{
		x1=120;
		x2=130;
		y1=48;
		y2=44;
	}
	else if(data > 43)
	{
		x1=130;
		x2=140;
		y1=44;
		y2=43;
	}
	else if(data > 40)
	{
		x1=140;
		x2=150;
		y1=43;
		y2=40;
	}
	else
		return 0;
	fData = interpolate(y1,y2,x1,x2,data);
	return fData;
}
