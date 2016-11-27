//***************************************************************************
//                 	  Драйвер сенсорных кнопок
//                   		  v 1.0
//                  Copyright (c) Кизим Игорь aka Igoryosha
//			  Website : lobotryasy.net 
//***************************************************************************

#include "sens.h"

uint16_t RefVal[NUM_KEY], CurVal[NUM_KEY]; //Массивы для хранения опорных и текущих значений времени заряда ёмкости сенсорных кнопок
char TouchKey [] = {1, 2, 3, 4, 5, 6, 7, 8}; //Коды сенсорных кнопок (программа поддерживает до 8 подключённых кнопок (PORTX.0-PORTX.7))
char KeyPressed; //Переменная для хранения кода нажатой кнопки

//===============================================================
//Инициализация входов микроконтроллера
//===============================================================
void TOUCH_Init (void)
{
    char x;
    for( x = TOUCH_FIRST_PIN; x < ( NUM_KEY + TOUCH_FIRST_PIN ); x++ )
    {
        TOUCH_DDR &=~ ( 1 << x );
        TOUCH_PORT &=~ ( 1 << x );
    }    	
}

//===============================================================
//Функция подсчёта времени заряда ёмкости сенсорной кнопки
//===============================================================
int TOUCH_Buttons(uint8_t channel)
{
    int timeCharge = 0; // Счётчик времени заряда ёмкости сенсорной кнопки
    
    TOUCH_DDR |= ( 1 << channel );
    TOUCH_PORT &=~ ( 1 << channel );
    while((TOUCH_PIN & ( 1 << channel )));
    
    TOUCH_DDR &=~ ( 1 << channel );
    while(!(TOUCH_PIN & ( 1 << channel )))  timeCharge++;
    return timeCharge;      
}    

//===============================================================
//Функция подсчёта времени заряда ёмкости сенсорных кнопок в относительных единицах
//===============================================================
void TOUCH_Scan(uint16_t *array)	//NumKey - количество кнопок (и размерность буфера для сохранения времени заряда ёмкости кнопок в о.е.)
{
    char channel, count = 0, timeCount = 5;
   
    /* Очистить накопительный буфер */
    for (channel = 0; channel < NUM_KEY; channel++) array[channel] = 0;
    asm("cli");
    do {
        for (channel = 0; channel < NUM_KEY; channel++) array[channel] += TOUCH_Buttons(channel + TOUCH_FIRST_PIN);
       } while (++count < timeCount); //Находимся здесь до окончания цикла из timeCount замеров 
    asm("sei");
    /* Среднее арифметическое времени заряда */
    for ( channel = 0; channel < NUM_KEY; channel++) array[channel] /= timeCount;
}

#define PUSH_COUNT		10
#define RELEASE_COUNT	10

//===============================================================
// Функция опроса сенсорных кнопок и определение нажатой кнопки
//===============================================================
uint8_t TOUCH_CompareKey()
{
    char channel;
    char tempKey = 0; //, MaxCount = 5;
    static char countUp=0, countDown=0;      
    
    TOUCH_Scan(CurVal); //
    
    for (channel = 0; channel < NUM_KEY; channel++)
    {   //Если кнопка нажата, сохраняю её код в буфере
        if(CurVal[channel] > RefVal[channel]+1)
        {
            tempKey=TouchKey [channel];            
        }               
    }    
    
    //Если в буфере содержится код любой нажатой кнопки
    if(tempKey)
    {
        countDown = 0;
        if(countUp < PUSH_COUNT) countUp++;
        if(countUp == PUSH_COUNT) //Обеспечиваю выдержку времени при нажатии кнопки для защиты от ложных срабатываний
        {                  
            countUp = PUSH_COUNT + 5;
            KeyPressed = tempKey;
            return 1;
        }
    }
    else 
    {   //Обеспечиваю выдержку времени при отпускании кнопки для защиты от ложных срабатываний           
        if(countDown < RELEASE_COUNT) countDown++;
        if(countDown == RELEASE_COUNT)
        {                  
            countUp = 0;
            countDown = RELEASE_COUNT;
        }
        return 0;
    }   
    return 0;
}

//===============================================================
//        Функция возврата кода нажатой сенсорной кнопки
//===============================================================
char TOUCH_GetKey()
{
    char key=KeyPressed;
    KeyPressed=0;
    return key;
}
