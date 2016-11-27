//***************************************************************************
//                 	  ������� ��������� ������
//                   		  v 1.0
//                  Copyright (c) ����� ����� aka Igoryosha
//			  Website : lobotryasy.net 
//***************************************************************************

#include "sens.h"

uint16_t RefVal[NUM_KEY], CurVal[NUM_KEY]; //������� ��� �������� ������� � ������� �������� ������� ������ ������� ��������� ������
char TouchKey [] = {1, 2, 3, 4, 5, 6, 7, 8}; //���� ��������� ������ (��������� ������������ �� 8 ������������ ������ (PORTX.0-PORTX.7))
char KeyPressed; //���������� ��� �������� ���� ������� ������

//===============================================================
//������������� ������ ����������������
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
//������� �������� ������� ������ ������� ��������� ������
//===============================================================
int TOUCH_Buttons(uint8_t channel)
{
    int timeCharge = 0; // ������� ������� ������ ������� ��������� ������
    
    TOUCH_DDR |= ( 1 << channel );
    TOUCH_PORT &=~ ( 1 << channel );
    while((TOUCH_PIN & ( 1 << channel )));
    
    TOUCH_DDR &=~ ( 1 << channel );
    while(!(TOUCH_PIN & ( 1 << channel )))  timeCharge++;
    return timeCharge;      
}    

//===============================================================
//������� �������� ������� ������ ������� ��������� ������ � ������������� ��������
//===============================================================
void TOUCH_Scan(uint16_t *array)	//NumKey - ���������� ������ (� ����������� ������ ��� ���������� ������� ������ ������� ������ � �.�.)
{
    char channel, count = 0, timeCount = 5;
   
    /* �������� ������������� ����� */
    for (channel = 0; channel < NUM_KEY; channel++) array[channel] = 0;
    asm("cli");
    do {
        for (channel = 0; channel < NUM_KEY; channel++) array[channel] += TOUCH_Buttons(channel + TOUCH_FIRST_PIN);
       } while (++count < timeCount); //��������� ����� �� ��������� ����� �� timeCount ������� 
    asm("sei");
    /* ������� �������������� ������� ������ */
    for ( channel = 0; channel < NUM_KEY; channel++) array[channel] /= timeCount;
}

#define PUSH_COUNT		10
#define RELEASE_COUNT	10

//===============================================================
// ������� ������ ��������� ������ � ����������� ������� ������
//===============================================================
uint8_t TOUCH_CompareKey()
{
    char channel;
    char tempKey = 0; //, MaxCount = 5;
    static char countUp=0, countDown=0;      
    
    TOUCH_Scan(CurVal); //
    
    for (channel = 0; channel < NUM_KEY; channel++)
    {   //���� ������ ������, �������� � ��� � ������
        if(CurVal[channel] > RefVal[channel]+1)
        {
            tempKey=TouchKey [channel];            
        }               
    }    
    
    //���� � ������ ���������� ��� ����� ������� ������
    if(tempKey)
    {
        countDown = 0;
        if(countUp < PUSH_COUNT) countUp++;
        if(countUp == PUSH_COUNT) //����������� �������� ������� ��� ������� ������ ��� ������ �� ������ ������������
        {                  
            countUp = PUSH_COUNT + 5;
            KeyPressed = tempKey;
            return 1;
        }
    }
    else 
    {   //����������� �������� ������� ��� ���������� ������ ��� ������ �� ������ ������������           
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
//        ������� �������� ���� ������� ��������� ������
//===============================================================
char TOUCH_GetKey()
{
    char key=KeyPressed;
    KeyPressed=0;
    return key;
}
