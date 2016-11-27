//***************************************************************************
//
//  Author(s)...: Pashgan    http://ChipEnable.Ru   
//
//  Target(s)...: AVR
//
//  Compiler....: любой
//
//  Description.: Драйвер 7-ми сегментного 4-ех разрядного индикатора 
//
//  Data........: 17.12.13  Создан  
//
//***************************************************************************
#include "indicator_2f.h"

#define SEG_A 0   
#define SEG_B 1  
#define SEG_C 2  
#define SEG_D 3  
#define SEG_E 4  
#define SEG_F 5  
#define SEG_G 6  
#define SEG_H 7  

#define IND_NUM_0     ((1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(0<<SEG_G)|(0<<SEG_H))
#define IND_EMPTY     ((0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H))
#define IND_SIGN      ((0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H))

/*буфер 7-ми сегметного индикатора*/
volatile uint8_t ind_buf[IND_AMOUNT_NUM];

/*таблица перекодировки*/
const uint8_t number[] = 
{
  (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(0<<SEG_G)|(0<<SEG_H), //0
  (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H), //1
  (1<<SEG_A)|(1<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H), //2
  (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H), //3   
  (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H), //4
  (1<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H), //5 
  (1<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H), //6
  (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H), //7   
  (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H), //8
  (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H), //9     
};

#if (IND_SCAN_SEGMENT == 1)
const uint8_t mask[] = 
{
   (1<<SEG_A),  
   (1<<SEG_B),
   (1<<SEG_C),   
   (1<<SEG_D),  
   (1<<SEG_E),  
   (1<<SEG_F),
   (1<<SEG_G),   
   (1<<SEG_H)
};
#endif

FLASH_VAR(uint8_t sym[]) = 
{ /*символы*/
  '-', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H), 
  '=', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),  
  '*', (1<<SEG_A)|(1<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  '_', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  ' ', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  
  /*цифры*/
  '0', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  '1', (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H), 
  '2', (1<<SEG_A)|(1<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H), 
  '3', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),   
  '4', (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  '5', (1<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H), 
  '6', (1<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  '7', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),   
  '8', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  '9', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  /*буквы*/
  'A', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'U', (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  'S', (1<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'I', (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  't', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H), 
  'H', (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'E', (1<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'L', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  'r', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'O', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  'o', (0<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'P', (1<<SEG_A)|(1<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'C', (1<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  'c', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'n', (0<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'd', (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'h', (0<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
   0
};

#if (IND_USE_BLINK == 1)
uint8_t enable = 0;
uint8_t blinkMask = 0;
uint16_t blinkCount = 0;  
#endif 

/******************************************************************************/

void IND_Init(void)
{
  uint8_t i;
  
  /*настраиваем сегменты на выход*/ 
  PM_DirPort(IND_SEG_PORT, 0xff); 
  
  /*настраиваем разряды на выход*/
  PM_DirPort(IND_DIG_PORT, 0xff); 

  /*гасим сегменты*/
  PM_OffPort(IND_SEG_PORT); 

  /*гасим разряды*/
  PM_OffPort(IND_DIG_PORT); 
  
  /*очищаем внутренний буфер*/
  for(i = 0; i < IND_AMOUNT_NUM; i++) {
     ind_buf[i] = 0;
  }

#if (IND_USE_BLINK == 1)
  enable = 0;
  blinkMask = 0;
  blinkCount = IND_DUR_BLINK;  
#endif
}

/******************************************************************************/

INLINE static void IND_Conv(uint16_t value)
{
  uint8_t tmp;    
  uint8_t i;
  
  for(i = 0; i < IND_AMOUNT_NUM; i++){
    tmp = value % 10;
    if ((value > 0)||(i == 0)){ 
       ind_buf[(IND_AMOUNT_NUM - 1) - i] = number[tmp];
    }
    else{
       ind_buf[(IND_AMOUNT_NUM - 1) - i] = IND_EMPTY;  
    }
    value = value/10;
  }  
  
}

/******************************************************************************/

INLINE static void IND_ConvFormat(uint16_t value, uint8_t firstPos, uint8_t lastPos)
{
  uint8_t tmp;
  uint8_t i;
  
  if ((firstPos) && (firstPos <= IND_AMOUNT_NUM)){
     if ((lastPos) && (lastPos <= IND_AMOUNT_NUM)){
       
        for(i = lastPos; i >= firstPos; i--){
           tmp = value % 10;
           if ((i == lastPos)||(value > 0)){ 
              ind_buf[i - 1] = number[tmp];
           }
           else{
              ind_buf[i - 1] = IND_EMPTY;
           }      
           value = value/10;
        }
     }
  }
  
}

/******************************************************************************/

INLINE static void IND_Comma(uint8_t comma, uint8_t pos)
{
  uint8_t ind;
  uint8_t i; 
  
   ind = comma - 1;
   for (i = ind; i < pos; i++){
      if (!ind_buf[i]) {
         ind_buf[i] = number[0];
      }
   }
   
   ind_buf[ind] |= 1<<SEG_H;
}

/******************************************************************************/

/*вывод текста на индикатор. если символа в таблице нет,
то на индикатор ничего не выводится. */

void IND_OutSym(char *str, uint8_t pos)
{
  uint8_t i, j;
  char s;
  
  /*проверяем корректность заданной позиции*/
  if ((pos > IND_AMOUNT_NUM) || (!pos)){
     return;  
  }
  
  /*проходка по строке, пока она или индикатор не кончится*/
  for (j = pos-1; j < IND_AMOUNT_NUM; j++){
    
     s = *str++;
     if (!s){
        break;   
     }
     
     /*поиск символа по таблице*/
     for (i = 0; read_byte_flash(sym[i]) != 0; i += 2){ 
       if (s == read_byte_flash(sym[i])){
          ind_buf[j] = read_byte_flash(sym[i+1]);  
       }
     }
  }
}

/******************************************************************************/

void IND_OutUint(uint16_t value, uint8_t comma)
{

  IND_Conv(value);
  
  /*если нужно отобразить точку, пробегаемся по
  массиву и отображаем нули в нужных разрядах*/ 
  if (comma) {
     if (comma > IND_AMOUNT_NUM){
       return;
     }
     
     IND_Comma(comma, IND_AMOUNT_NUM);
  }
}
/******************************************************************************/

void IND_OutInt(int16_t value, uint8_t comma)
{
  uint8_t sign;
  
  sign = 0;  
  if (value < 0){
     value = ~value + 1; 
     sign = 1;
  }
      
  IND_Conv((uint16_t)value);
  
  /*если нужно отобразить точку, пробегаемся по
  массиву и отображаем нули в нужных разрядах*/ 
  if (comma) {
     if (comma > IND_AMOUNT_NUM){
       return;
     }
     
     IND_Comma(comma, IND_AMOUNT_NUM);
  }
  
  /*вывод отрицательного знака*/
  if (sign){
     ind_buf[0] = IND_SIGN;     
  }
  
}

/******************************************************************************/

void IND_OutUintFormat(uint16_t value, uint8_t comma, uint8_t firstPos, uint8_t lastPos)
{
  
  IND_ConvFormat(value, firstPos, lastPos);

  /*если нужно отобразить точку, пробегаемся по
  массиву и отображаем нули в нужных разрядах*/ 
  if (comma) {

     if ((comma < firstPos) || (comma > lastPos)){   
       return;    
     }
     
     IND_Comma(comma, lastPos);
  }
   
}
/******************************************************************************/

void IND_OutIntFormat(int16_t value, uint8_t comma, uint8_t firstPos, uint8_t lastPos)
{
  uint8_t sign;
  
  sign = 0;  
  if (value < 0){
     value = ~value + 1; 
     sign = 1;
  }
  
  IND_ConvFormat((uint16_t)value, firstPos, lastPos);

  /*если нужно отобразить точку, пробегаемся по
  массиву и отображаем нули в нужных разрядах*/ 
  if (comma) {

     if ((comma < firstPos) || (comma > lastPos)){   
       return;    
     }
     
     IND_Comma(comma, lastPos);
  }

  /*вывод отрицательного знака*/
  if (sign){
     ind_buf[firstPos - 1] = IND_SIGN;     
  }  
   
}

/*****************************************************************************/

void IND_BlinkMask(uint8_t value)
{
#if (IND_USE_BLINK == 1)
  enable = 0; 
  blinkMask = value;  
#endif
}

/*****************************************************************************/

void IND_Update(void)
{
  static uint8_t count = 0;   
  uint8_t seg, dig;
  
  dig = 0; 

  /*гасим все индикаторы*/
  PM_OffPort(IND_SEG_PORT); 
  PM_OffPort(IND_DIG_PORT); 
    
  /*____________реализация посегментной развертки___________*/ 
  
#if (IND_SCAN_SEGMENT == 1)
  uint8_t i; 
  
  seg = mask[count];

  for (i = 0; i < IND_AMOUNT_NUM; i++){
  #if (IND_USE_BLINK == 1)  
    if (!(enable & (1<<i))){  
       if (ind_buf[i] & seg){
          dig |= (1<<i);
       }
    }
  #elif (IND_USE_BLINK == 0)  
     if (ind_buf[i] & seg){
        dig |= (1<<i);
     }
  #endif
  }

  count++;
  count &= 0x07; //это магическое число для счетчика на 8
  
 /*____________реализаци поразрядной развертки____________*/  
  
#elif (IND_SCAN_SEGMENT == 0)
  seg = ind_buf[count];

  #if (IND_USE_BLINK == 1) 
  if (!(enable & (1<<count))){    
     dig = (1<<count);
   }
  #elif (IND_USE_BLINK == 0)
  dig = (1<<count);
  #endif
  
  count++;
  if (count >= IND_AMOUNT_NUM){
     count = 0;  
  } 
#endif

  /*поджигаем нужные индикаторы*/
  PM_SetLevBitsPort(IND_SEG_PORT, seg);
  PM_SetLevBitsPort(IND_DIG_PORT, dig);

#if (IND_USE_BLINK == 1)  
  if (blinkMask){
     blinkCount--;
     if (!blinkCount){
        blinkCount = IND_DUR_BLINK;
        enable ^= blinkMask;
     }
  }   
#endif
}
