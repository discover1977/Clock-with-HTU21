//***************************************************************************
//
//  Author(s)...: Pashgan    http://ChipEnable.Ru   
//
//  Target(s)...: AVR
//
//  Compiler....: любой
//
//  Description.: Драйвер 7-ми сегментного индикатора 
//
//  Data........: 17.12.13  Создан  
//
//***************************************************************************
#ifndef INDICATOR_H
#define INDICATOR_H

#include "compilers_4.h"
#include "port_macros.h"

/********************** настройки ***************************/

/*количество цифр индикатора*/
#define IND_AMOUNT_NUM    4

/*какая используется развертка*/
#define IND_SCAN_SEGMENT  0

/*используется ли фунция моргания*/
#define IND_USE_BLINK     1

/*период моргания индикатора*/
#define IND_DUR_BLINK    200

/*куда подключены сегменты индикатора*/
#define IND_SEG_PORT   IND_SEG, E, _VIRT

/*  REAL  */
#define IND_SEG_0      B, 4, _HI	// A
#define IND_SEG_1      B, 3, _HI	// B
#define IND_SEG_2      B, 2, _HI	// C
#define IND_SEG_3      B, 0, _HI	// D
#define IND_SEG_4      B, 1, _HI	// E
#define IND_SEG_5      D, 7, _HI	// F
#define IND_SEG_6      D, 6, _HI	// G
#define IND_SEG_7      D, 5, _HI	// DP

/*куда подключены разряды индикатора*/
#define IND_DIG_PORT   IND_DIG, F, _VIRT

#define IND_DIG_0      B, 6, _HI
#define IND_DIG_1      B, 7, _HI
#define IND_DIG_2      C, 0, _HI
#define IND_DIG_3      C, 1, _HI
#define IND_DIG_4      C, 4, _NONE
#define IND_DIG_5      C, 5, _NONE
#define IND_DIG_6      C, 6, _NONE
#define IND_DIG_7      C, 7, _NONE


/******************** Пользовательские функции **********************************/

void IND_Init(void);
void IND_OutSym(char *str, uint8_t pos);
void IND_OutUint(uint16_t value, uint8_t comma);
void IND_OutInt(int16_t value, uint8_t comma);
void IND_OutUintFormat(uint16_t value, uint8_t comma, uint8_t firstPos, uint8_t lastPos);
void IND_OutIntFormat(int16_t value, uint8_t comma, uint8_t firstPos, uint8_t lastPos);
void IND_BlinkMask(uint8_t value);
void IND_Update(void);

#endif //INDICATOR_H
