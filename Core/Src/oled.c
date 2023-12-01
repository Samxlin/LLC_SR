#include "oled.h"
#include "gpio.h"
#include "i2c.h"
#include "signalGen.h"

#define XLevelL  	0x00
#define XLevelH  	0x10
#define XLevel    ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column 	128
#define Max_Row  	64
#define Brightness 	0xCF
#define X_WIDTH 	128
#define Y_WIDTH 	64

/************************************6*8的点阵************************************/
const unsigned char  F6x8[][6] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,// sp
  0x00, 0x00, 0x00, 0x2f, 0x00, 0x00,// !
  0x00, 0x00, 0x07, 0x00, 0x07, 0x00,// "
  0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14,// #
  0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12,// $
  0x00, 0x62, 0x64, 0x08, 0x13, 0x23,// %
  0x00, 0x36, 0x49, 0x55, 0x22, 0x50,// &
  0x00, 0x00, 0x05, 0x03, 0x00, 0x00,// '
  0x00, 0x00, 0x1c, 0x22, 0x41, 0x00,// (
  0x00, 0x00, 0x41, 0x22, 0x1c, 0x00,// )
  0x00, 0x14, 0x08, 0x3E, 0x08, 0x14,// *
  0x00, 0x08, 0x08, 0x3E, 0x08, 0x08,// +
  0x00, 0x00, 0x00, 0xA0, 0x60, 0x00,// ,
  0x00, 0x08, 0x08, 0x08, 0x08, 0x08,// -
  0x00, 0x00, 0x60, 0x60, 0x00, 0x00,// .
  0x00, 0x20, 0x10, 0x08, 0x04, 0x02,// /
  0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
  0x00, 0x00, 0x42, 0x7F, 0x40, 0x00,// 1
  0x00, 0x42, 0x61, 0x51, 0x49, 0x46,// 2
  0x00, 0x21, 0x41, 0x45, 0x4B, 0x31,// 3
  0x00, 0x18, 0x14, 0x12, 0x7F, 0x10,// 4
  0x00, 0x27, 0x45, 0x45, 0x45, 0x39,// 5
  0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
  0x00, 0x01, 0x71, 0x09, 0x05, 0x03,// 7
  0x00, 0x36, 0x49, 0x49, 0x49, 0x36,// 8
  0x00, 0x06, 0x49, 0x49, 0x29, 0x1E,// 9
  0x00, 0x00, 0x36, 0x36, 0x00, 0x00,// :
  0x00, 0x00, 0x56, 0x36, 0x00, 0x00,// ;
  0x00, 0x08, 0x14, 0x22, 0x41, 0x00,// <
  0x00, 0x14, 0x14, 0x14, 0x14, 0x14,// =
  0x00, 0x00, 0x41, 0x22, 0x14, 0x08,// >
  0x00, 0x02, 0x01, 0x51, 0x09, 0x06,// ?
  0x00, 0x32, 0x49, 0x59, 0x51, 0x3E,// @
  0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C,// A
  0x00, 0x7F, 0x49, 0x49, 0x49, 0x36,// B
  0x00, 0x3E, 0x41, 0x41, 0x41, 0x22,// C
  0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C,// D
  0x00, 0x7F, 0x49, 0x49, 0x49, 0x41,// E
  0x00, 0x7F, 0x09, 0x09, 0x09, 0x01,// F
  0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A,// G
  0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F,// H
  0x00, 0x00, 0x41, 0x7F, 0x41, 0x00,// I
  0x00, 0x20, 0x40, 0x41, 0x3F, 0x01,// J
  0x00, 0x7F, 0x08, 0x14, 0x22, 0x41,// K
  0x00, 0x7F, 0x40, 0x40, 0x40, 0x40,// L
  0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F,// M
  0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F,// N
  0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E,// O
  0x00, 0x7F, 0x09, 0x09, 0x09, 0x06,// P
  0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
  0x00, 0x7F, 0x09, 0x19, 0x29, 0x46,// R
  0x00, 0x46, 0x49, 0x49, 0x49, 0x31,// S
  0x00, 0x01, 0x01, 0x7F, 0x01, 0x01,// T
  0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F,// U
  0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F,// V
  0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F,// W
  0x00, 0x63, 0x14, 0x08, 0x14, 0x63,// X
  0x00, 0x07, 0x08, 0x70, 0x08, 0x07,// Y
  0x00, 0x61, 0x51, 0x49, 0x45, 0x43,// Z
  0x00, 0x00, 0x7F, 0x41, 0x41, 0x00,// [
  0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55,// 55
  0x00, 0x00, 0x41, 0x41, 0x7F, 0x00,// ]
  0x00, 0x04, 0x02, 0x01, 0x02, 0x04,// ^
  0x00, 0x40, 0x40, 0x40, 0x40, 0x40,// _
  0x00, 0x00, 0x01, 0x02, 0x04, 0x00,// '
  0x00, 0x20, 0x54, 0x54, 0x54, 0x78,// a
  0x00, 0x7F, 0x48, 0x44, 0x44, 0x38,// b
  0x00, 0x38, 0x44, 0x44, 0x44, 0x20,// c
  0x00, 0x38, 0x44, 0x44, 0x48, 0x7F,// d
  0x00, 0x38, 0x54, 0x54, 0x54, 0x18,// e
  0x00, 0x08, 0x7E, 0x09, 0x01, 0x02,// f
  0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C,// g
  0x00, 0x7F, 0x08, 0x04, 0x04, 0x78,// h
  0x00, 0x00, 0x44, 0x7D, 0x40, 0x00,// i
  0x00, 0x40, 0x80, 0x84, 0x7D, 0x00,// j
  0x00, 0x7F, 0x10, 0x28, 0x44, 0x00,// k
  0x00, 0x00, 0x41, 0x7F, 0x40, 0x00,// l
  0x00, 0x7C, 0x04, 0x18, 0x04, 0x78,// m
  0x00, 0x7C, 0x08, 0x04, 0x04, 0x78,// n
  0x00, 0x38, 0x44, 0x44, 0x44, 0x38,// o
  0x00, 0xFC, 0x24, 0x24, 0x24, 0x18,// p
  0x00, 0x18, 0x24, 0x24, 0x18, 0xFC,// q
  0x00, 0x7C, 0x08, 0x04, 0x04, 0x08,// r
  0x00, 0x48, 0x54, 0x54, 0x54, 0x20,// s
  0x00, 0x04, 0x3F, 0x44, 0x40, 0x20,// t
  0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C,// u
  0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C,// v
  0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C,// w
  0x00, 0x44, 0x28, 0x10, 0x28, 0x44,// x
  0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C,// y
  0x00, 0x44, 0x64, 0x54, 0x4C, 0x44,// z
  0x14, 0x14, 0x14, 0x14, 0x14, 0x14,// horiz lines
};


const unsigned char F8X16[]=	  
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// 0
	0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00,//! 1
	0x00,0x10,0x0C,0x06,0x10,0x0C,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//" 2
	0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,0x00,0x04,0x3F,0x04,0x04,0x3F,0x04,0x04,0x00,//# 3
	0x00,0x70,0x88,0xFC,0x08,0x30,0x00,0x00,0x00,0x18,0x20,0xFF,0x21,0x1E,0x00,0x00,//$ 4
	0xF0,0x08,0xF0,0x00,0xE0,0x18,0x00,0x00,0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E,0x00,//% 5
	0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10,//& 6
	0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//' 7
	0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00,//( 8
	0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00,//) 9
	0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00,//* 10
	0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x00,//+ 11
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00,//, 12
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,//- 13
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,//. 14
	0x00,0x00,0x00,0x00,0x80,0x60,0x18,0x04,0x00,0x60,0x18,0x06,0x01,0x00,0x00,0x00,/// 15
	0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00,//0 16
	0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//1 17
	0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00,//2 18
	0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00,//3 19
	0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00,//4 20
	0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00,//5 21
	0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00,//6 22
	0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,//7 23
	0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00,//8 24
	0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00,//9 25
	0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,//: 26
	0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00,//; 27
	0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00,//< 28
	0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,//= 29
	0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00,//> 30
	0x00,0x70,0x48,0x08,0x08,0x08,0xF0,0x00,0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00,//? 31
	0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x24,0x23,0x14,0x0B,0x00,//@ 32
	0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20,//A 33
	0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00,//B 34
	0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00,//C 35
	0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00,//D 36
	0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00,//E 37
	0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00,//F 38
	0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00,//G 39
	0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20,//H 40
	0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//I 41
	0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00,//J 42
	0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00,//K 43
	0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00,//L 44
	0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00,//M 45
	0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00,//N 46
	0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00,//O 47
	0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00,//P 48
	0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x18,0x24,0x24,0x38,0x50,0x4F,0x00,//Q 49
	0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20,//R 50
	0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00,//S 51
	0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//T 52
	0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//U 53
	0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00,//V 54
	0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00,//W 55
	0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20,//X 56
	0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//Y 57
	0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00,//Z 58
	0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00,//[ 59
	0x00,0x0C,0x30,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00,//\ 60
	0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00,//] 61
	0x00,0x00,0x04,0x02,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//^ 62
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,//_ 63
	0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//` 64
	0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20,//a 65
	0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00,//b 66
	0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00,//c 67
	0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20,//d 68
	0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00,//e 69
	0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//f 70
	0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00,//g 71
	0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//h 72
	0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//i 73
	0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,//j 74
	0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00,//k 75
	0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//l 76
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F,//m 77
	0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//n 78
	0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//o 79
	0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E,0x00,//p 80
	0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80,//q 81
	0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00,//r 82
	0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00,//s 83
	0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00,//t 84
	0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20,//u 85
	0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00,//v 86
	0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00,//w 87
	0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00,//x 88
	0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00,//y 89
	0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00,//z 90
	0x00,0x00,0x00,0x00,0x80,0x7C,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40,//{ 91
	0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,//| 92
	0x00,0x02,0x02,0x7C,0x80,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00,//} 93
	0x00,0x06,0x01,0x01,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//~ 94
};

unsigned char F16x16[] =
{
	0x10,0x94,0x53,0x32,0x1E,0x32,0x52,0x10,0x00,0x7E,0x42,0x42,0x42,0x7E,0x00,0x00,
	0x00,0x00,0x00,0xFF,0x49,0x49,0x49,0x49,0x49,0x49,0x49,0xFF,0x00,0x00,0x00,0x00,/*"智”,0*/

	0x08,0xCC,0x4A,0x49,0x48,0x4A,0xCC,0x18,0x00,0x7F,0x88,0x88,0x84,0x82,0xE0,0x00,
	0x00,0xFF,0x12,0x12,0x52,0x92,0x7F,0x00,0x00,0x7E,0x88,0x88,0x84,0x82,0xE0,0x00,/*"能",1*/

	0x40,0x20,0xF0,0x28,0x24,0x27,0x24,0xE4,0x24,0x34,0x2C,0x20,0xE0,0x00,0x00,0x00,
	0x40,0x40,0x4F,0x49,0x49,0x49,0x49,0x4F,0x49,0x49,0x49,0x49,0x4F,0x40,0x40,0x00,/*"鱼",2*/

	0x50,0x48,0x47,0x44,0xFC,0x44,0x44,0x44,0x00,0x04,0x04,0xFC,0x04,0x04,0x04,0x00,
	0x00,0x3E,0x20,0x20,0x3F,0x10,0x10,0x7E,0x20,0x20,0x20,0x3F,0x20,0x20,0x20,0x00,/*"缸",3*/

	0x04,0x24,0x44,0x84,0x64,0x9C,0x40,0x30,0x0F,0xC8,0x08,0x08,0x28,0x18,0x00,0x00,
	0x10,0x08,0x06,0x01,0x82,0x4C,0x20,0x18,0x06,0x01,0x06,0x18,0x20,0x40,0x80,0x00,/*"欢",4*/

0x40,0x40,0x42,0xCC,0x00,0x00,0xFC,0x04,0x02,0x00,0xFC,0x04,0x04,0xFC,0x00,0x00,
0x00,0x40,0x20,0x1F,0x20,0x40,0x4F,0x44,0x42,0x40,0x7F,0x42,0x44,0x43,0x40,0x00,//迎5

0x40,0x40,0x42,0xCC,0x00,0x80,0x88,0x88,0xFF,0x88,0x88,0xFF,0x88,0x88,0x80,0x00,
0x00,0x40,0x20,0x1F,0x20,0x40,0x50,0x4C,0x43,0x40,0x40,0x5F,0x40,0x40,0x40,0x00,//进6

0x00,0x00,0x00,0x00,0x00,0x01,0xE2,0x1C,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x80,0x40,0x20,0x10,0x0C,0x03,0x00,0x00,0x00,0x03,0x0C,0x30,0x40,0x80,0x80,0x00,//入7

0x00,0x02,0x02,0xC2,0x02,0x02,0x02,0xFE,0x82,0x82,0x82,0x82,0x82,0x02,0x00,0x00,
0x40,0x40,0x40,0x7F,0x40,0x40,0x40,0x7F,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,//正8

0x08,0x08,0x88,0xC8,0x38,0x0C,0x0B,0x08,0x08,0xE8,0x08,0x08,0x08,0x08,0x08,0x00,
0x02,0x01,0x00,0xFF,0x40,0x41,0x41,0x41,0x41,0x7F,0x41,0x41,0x41,0x41,0x40,0x00,//在9

0x02,0xFE,0x92,0x92,0xFE,0x02,0x00,0x10,0x11,0x16,0xF0,0x14,0x13,0x10,0x00,0x00,
0x10,0x1F,0x08,0x08,0xFF,0x04,0x81,0x41,0x31,0x0D,0x03,0x0D,0x31,0x41,0x81,0x00,//联10

0x00,0xFE,0x02,0x22,0x42,0x82,0x72,0x02,0x22,0x42,0x82,0x72,0x02,0xFE,0x00,0x00,
0x00,0xFF,0x10,0x08,0x06,0x01,0x0E,0x10,0x08,0x06,0x01,0x4E,0x80,0x7F,0x00,0x00,//网11

0x10,0x60,0x02,0x8C,0x00,0x00,0xFE,0x92,0x92,0x92,0x92,0x92,0xFE,0x00,0x00,0x00,
0x04,0x04,0x7E,0x01,0x40,0x7E,0x42,0x42,0x7E,0x42,0x7E,0x42,0x42,0x7E,0x40,0x00,//温12

0x00,0x00,0xFC,0x24,0x24,0x24,0xFC,0x25,0x26,0x24,0xFC,0x24,0x24,0x24,0x04,0x00,
0x40,0x30,0x8F,0x80,0x84,0x4C,0x55,0x25,0x25,0x25,0x55,0x4C,0x80,0x80,0x80,0x00,//度13

0x40,0x40,0x42,0xCC,0x00,0x08,0x48,0x88,0x08,0x08,0x08,0xFF,0x08,0x08,0x08,0x00,
0x00,0x40,0x20,0x1F,0x20,0x40,0x40,0x41,0x40,0x48,0x50,0x4F,0x40,0x40,0x40,0x00,/*"过",14*/

0x10,0x60,0x02,0x8C,0x00,0xF8,0x48,0x48,0x48,0xFF,0x2A,0x2A,0x0A,0xCA,0x18,0x00,
0x04,0x04,0x7E,0x81,0x60,0x1F,0x80,0x70,0x00,0x78,0x83,0x8D,0xC1,0x09,0x70,0x00,/*"虑",15*/

0x08,0x24,0x23,0x6A,0xAA,0x2A,0xAA,0x6A,0x2A,0x2A,0x2A,0xEA,0x02,0x02,0x00,0x00,
0x10,0x11,0x15,0x15,0x15,0xFF,0x15,0x15,0x15,0x11,0x10,0x0F,0x30,0x40,0xF8,0x00,/*"氧",16*/

0x42,0x42,0x22,0x12,0xFA,0x96,0x92,0x92,0x92,0x92,0x92,0xF2,0x02,0x02,0x02,0x00,
0x40,0x44,0x24,0x14,0x0C,0x44,0x80,0x7F,0x04,0x08,0x10,0x28,0x24,0x42,0x40,0x00,/*"泵",17*/

0x80,0x82,0x82,0x82,0xFE,0x82,0x82,0x82,0x82,0x82,0xFE,0x82,0x82,0x82,0x80,0x00,
0x00,0x80,0x40,0x30,0x0F,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,/*"开",18*/


0x00,0x00,0x10,0x11,0x16,0x10,0x10,0xF0,0x10,0x10,0x14,0x13,0x10,0x00,0x00,0x00,
0x81,0x81,0x41,0x41,0x21,0x11,0x0D,0x03,0x0D,0x11,0x21,0x41,0x41,0x81,0x81,0x00,/*"关",18*/
};

const unsigned char NC_Logo[1024] =
{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,
0xE0,0xE0,0xF0,0xF0,0xF0,0xF8,0x78,0x78,0x3C,0x3C,0x3C,0x3C,0x1C,0x1C,0x1C,0x1C,
0x1C,0x1C,0x1C,0x1C,0x1C,0x18,0x18,0x18,0x30,0x30,0x30,0x60,0x60,0x40,0xC0,0x80,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xF8,0xFC,0xFE,0xFE,0x3F,0x1F,0x0F,0x07,
0x07,0x03,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x19,0x3C,0x7E,0x7E,0x3C,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x80,0xE0,0xFC,0xFE,0xFF,0xFF,0x3F,0x0F,0x03,0x01,0x00,0x00,0x00,0x01,0x03,
0x07,0x1F,0x3F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x3F,0x7F,0x7F,
0x7E,0xFE,0xFC,0xFC,0xF8,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xF8,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0x1F,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xF0,0xFF,0xFF,0xFF,0xFF,0x1F,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0xE0,0xF8,0xFE,0xFF,0xFF,0xFF,0xFF,0x7F,0x1F,0x0F,0x7E,0xFC,
0xF0,0x80,0x01,0x07,0x0F,0x1F,0x00,0x00,0x80,0xE0,0xF8,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0x3F,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x1F,0xFF,0xFF,0xFF,0xFF,0xC0,0xFF,0xF8,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xE0,0xF8,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0x1F,0x07,0x01,0x00,0x00,0x00,0x00,0x03,
0x1F,0xFF,0xF8,0xC0,0x00,0xC0,0xF0,0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x1F,
0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xFF,0x3F,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x0F,0x3F,0xFF,0xFF,0xFC,0xEF,0xBF,0xFE,0xF8,0xF0,0xC0,0x90,0x34,0xF7,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,
0x7C,0x7C,0x7D,0x7F,0x7F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF3,0xF1,0xF0,0xF0,
0xE0,0xE0,0xE0,0xC0,0xE0,0xF0,0xFC,0x7F,0x1F,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x30,0x10,0x18,0x08,0x0C,
0x0E,0x06,0x07,0x07,0x07,0x03,0x07,0x0F,0x1F,0x3E,0x7D,0xF7,0xEF,0xDF,0xBF,0x7E,
0xFC,0xF8,0xF0,0xF0,0xE0,0xC0,0xC0,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0xC0,0xE0,0xE0,0xE0,0x70,0x70,0x78,0xB9,0xBD,
0xDF,0xDF,0x6F,0x37,0x17,0x0B,0x03,0x03,0x07,0x07,0x06,0x04,0x0C,0x08,0x18,0x10,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x03,0x07,
0x06,0x0F,0x0D,0x1B,0x1B,0x17,0x37,0x3F,0x2F,0x2F,0x3F,0x1F,0x1F,0x1E,0x1E,0x1E,
0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x0E,0x0E,0x0F,0x07,0x07,0x03,0x03,0x01,
0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};





void WriteCmd(unsigned char I2C_Command)//写命令
{
	HAL_I2C_Mem_Write(&hi2c1,OLED0561_ADD,COM,I2C_MEMADD_SIZE_8BIT,&I2C_Command,1,100);	
}

void WriteDat(unsigned char I2C_Data)//写数据 
{
	HAL_I2C_Mem_Write(&hi2c1,OLED0561_ADD,DAT,I2C_MEMADD_SIZE_8BIT,&I2C_Data,1,100);	
}


void OLED_Init(void)
{
	HAL_Delay(200); //这里的延时很重要

	WriteCmd(0xAE); //display off
	WriteCmd(0x20);	//Set Memory Addressing Mode	
	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	//Set COM Output Scan Direction
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //--set contrast control register
	WriteCmd(0xff); //亮度调节 0x00~0xff
	WriteCmd(0xa1); //--set segment re-map 0 to 127
	WriteCmd(0xa6); //--set normal display
	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); //--set divide ratio
	WriteCmd(0xd9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); //--set vcomh
	WriteCmd(0x20); //0x20,0.77xVcc
	WriteCmd(0x8d); //--set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); //--turn on oled panel
	
	HAL_Delay(100);
}

void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)//全屏填充
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
		{
			WriteDat(fill_Data);
		}
	}
}


void OLED_CLS(void)//清屏
{
	OLED_Fill(0x00);
	HAL_Delay(100);
}

void OLED_ON(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X14);  //开启电荷泵
	WriteCmd(0XAF);  //OLED唤醒
}

void OLED_OFF(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X10);  //关闭电荷泵
	WriteCmd(0XAE);  //OLED休眠
}

// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
				WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
				WriteDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
				WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}


// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在.h中的索引
// Description    : 显示ASCII_8x16.h中的汉字,16*16点阵
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}



// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

	if(y1%8==0)
	y = y1/8;
	else
	y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
		for(x=x0;x<x1;x++)
		{
			WriteDat(BMP[j++]);
		}
	}
}

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//得到偏移后的值			
	if(x>128-1){x=0;y=y+2;}
	if(Char_Size ==16)
	{
		OLED_SetPos(x,y);	
		for(i=0;i<8;i++)
		WriteDat(F8X16[c*16+i]);
		OLED_SetPos(x,y+1);
		for(i=0;i<8;i++)
		WriteDat(F8X16[c*16+i+8]);
	}
	else 
	{	
		OLED_SetPos(x,y);
		for(i=0;i<6;i++)
		WriteDat(F6x8[c][i]);
	}
}

u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	


//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}
			else 
				enshow=1; 
		}
		OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 

//以字符串的形式显示数字
//x,y :起点坐标	
//temp:数字
void OLEDShowData(u8 x,u8 y,u8 temp)
{
	switch(temp)
	{
		case  0: OLED_ShowStr(x,y,"0",1);
		break;
		case  1: OLED_ShowStr(x,y,"1",1);
		break;
		case  2: OLED_ShowStr(x,y,"2",1);
		break;
		case  3: OLED_ShowStr(x,y,"3",1);
		break;
		case  4: OLED_ShowStr(x,y,"4",1);
		break;
		case  5: OLED_ShowStr(x,y,"5",1);
		break;
		case  6: OLED_ShowStr(x,y,"6",1);
		break;
		case  7: OLED_ShowStr(x,y,"7",1);
		break;
		case  8: OLED_ShowStr(x,y,"8",1);
		break;
		case  9: OLED_ShowStr(x,y,"9",1);
		break;		
	}
}


void Draw_Logo1(void)
{
  unsigned int ii=0;
  unsigned char x,y;  
  for(y=0;y<8;y++)
  {
    OLED_SetPos(0,y);
    for(x=0;x<128;x++)
    {
      WriteDat(NC_Logo[ii++]);
      HAL_Delay(1);
    }
  }
  HAL_Delay(500);
  OLED_CLS();
}


void OLED_SHOW()
{
	unsigned int freqDis[4]={0,0,0,0};
	unsigned int sita1Dis[4]={0,0,0,0};
	unsigned int sita2Dis[4]={0,0,0,0};
	unsigned int phiDis[4]={0,0,0,0};
	unsigned int dt1Dis[4]={0,0,0,0};
	unsigned int dt2Dis[4]={0,0,0,0};	
	unsigned int dt3Dis[4]={0,0,0,0};
//	unsigned int dt4Dis[4]={0,0,0,0};
	
	freqDis[0] = (u8)(freq/1000);
	freqDis[1] = (u8)((freq-(uint8_t)freqDis[0]*1000)/100);
	freqDis[2] = (u8)((freq-(uint16_t)freqDis[0]*1000-(uint16_t)freqDis[1]*100)/10);
	freqDis[3] = (u8)(freq-(uint16_t)freqDis[0]*1000-(uint16_t)freqDis[1]*100-(uint16_t)freqDis[2]*10);	
	
	sita1Dis[0] = (u8)(sita1Deg/1000);
	sita1Dis[1] = (u8)((sita1Deg-(uint8_t)sita1Dis[0]*1000)/100);
	sita1Dis[2] = (u8)((sita1Deg-(uint16_t)sita1Dis[0]*1000-(uint16_t)sita1Dis[1]*100)/10);
	sita1Dis[3] = (u8)(sita1Deg-(uint16_t)sita1Dis[0]*1000-(uint16_t)sita1Dis[1]*100-(uint16_t)sita1Dis[2]*10);	
	
	sita2Dis[0] = (u8)(sita2Deg/1000);
	sita2Dis[1] = (u8)((sita2Deg-(uint8_t)sita2Dis[0]*1000)/100);
	sita2Dis[2] = (u8)((sita2Deg-(uint16_t)sita2Dis[0]*1000-(uint16_t)sita2Dis[1]*100)/10);
	sita2Dis[3] = (u8)(sita2Deg-(uint16_t)sita2Dis[0]*1000-(uint16_t)sita2Dis[1]*100-(uint16_t)sita2Dis[2]*10);	
	
	phiDis[0] = (u8)(phiDeg/1000);
	phiDis[1] = (u8)((phiDeg-(uint8_t)phiDis[0]*1000)/100);
	phiDis[2] = (u8)((phiDeg-(uint16_t)phiDis[0]*1000-(uint16_t)phiDis[1]*100)/10);
	phiDis[3] = (u8)(phiDeg-(uint16_t)phiDis[0]*1000-(uint16_t)phiDis[1]*100-(uint16_t)phiDis[2]*10);	
	
//	dt1Dis[0] = (u8)(dt1/1000);
//	dt1Dis[1] = (u8)((dt1-(uint8_t)dt1Dis[0]*1000)/100);
//	dt1Dis[2] = (u8)((dt1-(uint16_t)dt1Dis[0]*1000-(uint16_t)dt1Dis[1]*100)/10);
//	dt1Dis[3] = (u8)(dt1-(uint16_t)dt1Dis[0]*1000-(uint16_t)dt1Dis[1]*100-(uint16_t)dt1Dis[2]*10);	
//	
//	dt2Dis[0] = (u8)(dt2/1000);
//	dt2Dis[1] = (u8)((dt2-(uint8_t)dt2Dis[0]*1000)/100);
//	dt2Dis[2] = (u8)((dt2-(uint16_t)dt2Dis[0]*1000-(uint16_t)dt2Dis[1]*100)/10);
//	dt2Dis[3] = (u8)(dt2-(uint16_t)dt2Dis[0]*1000-(uint16_t)dt2Dis[1]*100-(uint16_t)dt2Dis[2]*10);	
//	
//	dt3Dis[0] = (u8)(dt3/1000);
//	dt3Dis[1] = (u8)((dt3-(uint8_t)dt3Dis[0]*1000)/100);
//	dt3Dis[2] = (u8)((dt3-(uint16_t)dt3Dis[0]*1000-(uint16_t)dt3Dis[1]*100)/10);
//	dt3Dis[3] = (u8)(dt3-(uint16_t)dt3Dis[0]*1000-(uint16_t)dt3Dis[1]*100-(uint16_t)dt3Dis[2]*10);	
//	
//	dt4Dis[0] = (u8)(dt4/1000);
//	dt4Dis[1] = (u8)((dt4-(uint8_t)dt4Dis[0]*1000)/100);
//	dt4Dis[2] = (u8)((dt4-(uint16_t)dt4Dis[0]*1000-(uint16_t)dt4Dis[1]*100)/10);
//	dt4Dis[3] = (u8)(dt4-(uint16_t)dt4Dis[0]*1000-(uint16_t)dt4Dis[1]*100-(uint16_t)dt2Dis[2]*10);	
	
	//hall
	dt1Dis[0] = (u8)(ADC_ConvertedValue[0]/1000);
	dt1Dis[1] = (u8)((ADC_ConvertedValue[0]-(uint8_t)dt1Dis[0]*1000)/100);
	dt1Dis[2] = (u8)((ADC_ConvertedValue[0]-(uint16_t)dt1Dis[0]*1000-(uint16_t)dt1Dis[1]*100)/10);
	dt1Dis[3] = (u8)(ADC_ConvertedValue[0]-(uint16_t)dt1Dis[0]*1000-(uint16_t)dt1Dis[1]*100-(uint16_t)dt1Dis[2]*10);	
	
	//shunt
	dt2Dis[0] = (u8)(ADC_ConvertedValue[1]/1000);
	dt2Dis[1] = (u8)((ADC_ConvertedValue[1]-(uint8_t)dt2Dis[0]*1000)/100);
	dt2Dis[2] = (u8)((ADC_ConvertedValue[1]-(uint16_t)dt2Dis[0]*1000-(uint16_t)dt2Dis[1]*100)/10);
	dt2Dis[3] = (u8)(ADC_ConvertedValue[1]-(uint16_t)dt2Dis[0]*1000-(uint16_t)dt2Dis[1]*100-(uint16_t)dt2Dis[2]*10);	
	
	//电压 6.2k/(49.9k*2+6.2k)*4096/3.3=72.6
	dt3Dis[0] = (u8)(ADC_ConvertedValue[2]/1000);
	dt3Dis[1] = (u8)((ADC_ConvertedValue[2]-(uint8_t)dt3Dis[0]*1000)/100);
	dt3Dis[2] = (u8)((ADC_ConvertedValue[2]-(uint16_t)dt3Dis[0]*1000-(uint16_t)dt3Dis[1]*100)/10);
	dt3Dis[3] = (u8)(ADC_ConvertedValue[2]-(uint16_t)dt3Dis[0]*1000-(uint16_t)dt3Dis[1]*100-(uint16_t)dt3Dis[2]*10);	
	
	OLED_ShowStr(25,0,"MODE  :  DAB ",1);
	OLED_ShowStr(10,2,"Freq  :  ",1);
	OLED_ShowStr(100,2,"kHz",1);
	OLED_ShowStr(10,3,"sita1 : ",1);
	OLED_ShowStr(10,4,"sita2 : ",1);
	OLED_ShowStr(10,5,"phi   : ",1);
	
	OLEDShowData(65,2,freqDis[0]);
	OLEDShowData(72,2,freqDis[1]);
	OLEDShowData(79,2,freqDis[2]);
	OLED_ShowStr(86,2,".",1);
	OLEDShowData(93,2,freqDis[3]);
	
	OLEDShowData(80,3,sita1Dis[0]);
	OLEDShowData(87,3,sita1Dis[1]);
	OLEDShowData(94,3,sita1Dis[2]);
	OLED_ShowStr(101,3,".",1);
	OLEDShowData(108,3,sita1Dis[3]);
	
	OLEDShowData(80,4,sita2Dis[0]);
	OLEDShowData(87,4,sita2Dis[1]);
	OLEDShowData(94,4,sita2Dis[2]);
	OLED_ShowStr(101,4,".",1);
	OLEDShowData(108,4,sita2Dis[3]);
	
	OLEDShowData(80,5,phiDis[0]);
	OLEDShowData(87,5,phiDis[1]);
	OLEDShowData(94,5,phiDis[2]);
	OLED_ShowStr(101,5,".",1);
	OLEDShowData(108,5,phiDis[3]);
	
	OLEDShowData(10,6,dt1Dis[0]);
	OLEDShowData(17,6,dt1Dis[1]);
	OLEDShowData(24,6,dt1Dis[2]);
	OLEDShowData(31,6,dt1Dis[3]);
	
	OLEDShowData(80,6,dt2Dis[0]);
	OLEDShowData(87,6,dt2Dis[1]);
	OLEDShowData(94,6,dt2Dis[2]);
	OLEDShowData(101,6,dt2Dis[3]);
	
	OLEDShowData(10,7,dt3Dis[0]);
	OLEDShowData(17,7,dt3Dis[1]);
	OLEDShowData(24,7,dt3Dis[2]);
	OLEDShowData(31,7,dt3Dis[3]);
//	
//	OLEDShowData(80,7,dt4Dis[0]);
//	OLEDShowData(87,7,dt4Dis[1]);
//	OLEDShowData(94,7,dt4Dis[2]);
//	OLEDShowData(101,7,dt4Dis[3]);
}
