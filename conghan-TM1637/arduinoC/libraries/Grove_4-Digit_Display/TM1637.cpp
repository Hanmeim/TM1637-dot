/*
 * TM1637.cpp
 * A library for the 4 digit display
 *
 * Copyright (c) 2012 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : Frankie.Chu
 * Create Time: 9 April,2012
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "TM1637.h"
#include <Arduino.h>

static int8_t TubeTab[] = {0x3f,0x06,0x5b,0x4f,
                           0x66,0x6d,0x7d,0x07,
                           0x7f,0x6f,0x77,0x7c,
                           0x39,0x5e,0x79,0x71,
						   //with dot
                           0xbf,0x86,0xdb,0xcf,//0~3
                           0xe6,0xed,0xfd,0x87,//4~7
                           0xff,0xef,0xf7,0xfc,//8~b
                           0xb9,0xde,0xf9,0xf1,0x40 //c~f,-
						   };//0~9,A,b,C,d,E,F
						   
TM1637::TM1637(uint8_t Clk, uint8_t Data)
{
  Clkpin = Clk;
  Datapin = Data;
  pinMode(Clkpin,OUTPUT);
  pinMode(Datapin,OUTPUT);
}

void TM1637::init(void)
{
  clearDisplay();
}

int TM1637::writeByte(int8_t wr_data)
{
  uint8_t i,count1;
  for(i=0;i<8;i++)        //sent 8bit data
  {
    digitalWrite(Clkpin,LOW);
    if(wr_data & 0x01)digitalWrite(Datapin,HIGH);//LSB first
    else digitalWrite(Datapin,LOW);
    wr_data >>= 1;
    digitalWrite(Clkpin,HIGH);

  }
  digitalWrite(Clkpin,LOW); //wait for the ACK
  digitalWrite(Datapin,HIGH);
  digitalWrite(Clkpin,HIGH);
  pinMode(Datapin,INPUT);

  bitDelay();
  uint8_t ack = digitalRead(Datapin);
  if (ack == 0) 
  {
     pinMode(Datapin,OUTPUT);
     digitalWrite(Datapin,LOW);
  }
  bitDelay();
  pinMode(Datapin,OUTPUT);
  bitDelay();
  
  return ack;
}
//send start signal to TM1637
void TM1637::start(void)
{
  digitalWrite(Clkpin,HIGH);//send start signal to TM1637
  digitalWrite(Datapin,HIGH);
  digitalWrite(Datapin,LOW);
  digitalWrite(Clkpin,LOW);
}
//End of transmission
void TM1637::stop(void)
{
  digitalWrite(Clkpin,LOW);
  digitalWrite(Datapin,LOW);
  digitalWrite(Clkpin,HIGH);
  digitalWrite(Datapin,HIGH);
}
void TM1637::display(float DispData)
{
	int8_t SegData[4];
	uint8_t i;
	int8_t flag = 0;
	if(DispData < 0)
	{
		DispData = - DispData;
		flag = 1;
		SegData[0] = 0x20;
	}
	DispData = DispData*100;
	
	int16_t tmp = (int)DispData;
	for(i = 0;i < 4 - flag;i ++)
  	{
    	SegData[3-i] = (int)tmp%10;
    	tmp =(int)tmp/10;
  	}
  	for(i = 0 + flag;i < 3;i ++)
  	{
  		if(SegData[i] != 0 || (tmp<100 && i >= 1)) 
			break;
    	SegData[i] = 0x7f;
  	}
  	SegData[3] += 16;
  	display(SegData);  
}
void TM1637::display(double DispData)
{
	display((float)DispData);
}
void TM1637::display(int16_t DispData)
{
	int8_t SegData[4];
	uint8_t i;
	int8_t flag = 0;
	if(DispData < 0)
	{
		DispData = - DispData;
		flag = 1;
		SegData[0] = 0x20;
	}
	for(i = 0;i < 4 - flag;i ++)
  	{
    	SegData[3-i] = (int)DispData%10;
    	DispData =(int)DispData/10;
  	}
  	for(i = 0 + flag;i < 3;i ++)
  	{
  		if(SegData[i] != 0) 
			break;
    	SegData[i] = 0x7f;
  	}
  	display(SegData);  
}

void TM1637::display(int32_t DispData)
{
	int16_t tmp = (int)DispData;
	display(tmp);
}
void TM1637::display(int64_t DispData)
{
	int16_t tmp = (int)DispData;
	display(tmp);
}
//display function.Write to full-screen.
void TM1637::display(int8_t DispData[])
{
  int8_t SegData[4];
  uint8_t i;
  for(i = 0;i < 4;i ++)
  {
    SegData[i] = DispData[i];
  }
  coding(SegData);
  start();          //start signal sent to TM1637 from MCU
  writeByte(ADDR_AUTO);//
  stop();           //
  start();          //
  writeByte(Cmd_SetAddr);//
  for(i=0;i < 4;i ++)
  {
    writeByte(SegData[i]);        //
  }
  stop();           //
  start();          //
  writeByte(Cmd_DispCtrl);//
  stop();           //
}
//******************************************
void TM1637::display(uint8_t BitAddr,int8_t DispData)
{
  int8_t SegData;
  SegData = coding(DispData);
  start();          //start signal sent to TM1637 from MCU
  writeByte(ADDR_FIXED);//
  stop();           //
  start();          //
  writeByte(BitAddr|0xc0);//
  writeByte(SegData);//
  stop();            //
  start();          //
  writeByte(Cmd_DispCtrl);//
  stop();           //
}

void TM1637::clearDisplay(void)
{
  display(0x00,0x7f);
  display(0x01,0x7f);
  display(0x02,0x7f);
  display(0x03,0x7f);
}
//To take effect the next time it displays.
void TM1637::set(uint8_t brightness,uint8_t SetData,uint8_t SetAddr)
{
  Cmd_SetData = SetData;
  Cmd_SetAddr = SetAddr;
  Cmd_DispCtrl = 0x88 + brightness;//Set the brightness and it takes effect the next time it displays.
}

//Whether to light the clock point ":".
//To take effect the next time it displays.
void TM1637::point(boolean PointFlag)
{
  _PointFlag = PointFlag;
}
void TM1637::coding(int8_t DispData[])
{
  uint8_t PointData;
  if(_PointFlag == POINT_ON)PointData = 0x80;
  else PointData = 0;
  for(uint8_t i = 0;i < 4;i ++)
  {
    if(DispData[i] == 0x7f)DispData[i] = 0x00;
    else DispData[i] = TubeTab[DispData[i]] + PointData;
  }
}
int8_t TM1637::coding(int8_t DispData)
{
  uint8_t PointData;
  if(_PointFlag == POINT_ON)PointData = 0x80;
  else PointData = 0;
  if(DispData == 0x7f) DispData = 0x00 + PointData;//The bit digital tube off
  else if(DispData == 0x20) DispData = 0x20;
  else DispData = TubeTab[DispData] + PointData;
  return DispData;
}
void TM1637::bitDelay(void)
{
	delayMicroseconds(50);
}
