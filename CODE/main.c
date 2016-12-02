/*
 * main.c
 *
 *  Created on: 19 сент. 2015 г.
 *      Author: gavrilov.iv
 */

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "bits_macros.h"
#include "i2c-soft.h"
#include "indicator_2f.h"
#include "rtc.h"
#include "sens.h"
#include "HTSensor.h"

#define INC_BUT_CODE				1
#define SET_BUT_CODE				2
#define DEC_BUT_CODE				3
#define AUTO_CHANGE_TIME			20
#define WAIT_SET_COUNT				6
#define RESET_WAIT_SET_COUNT		0

#define BLINK_LED_PIN	5
#define BLINK_LED_DDR	DDRB
#define BLINK_LED_PORT	PORTB

#define BUZ_PIN	5
#define BUZ_DDR	DDRC
#define BUZ_PORT	PORTC
#define BUZ_ON	SetBit(BUZ_PORT, BUZ_PIN);
#define BUZ_OFF	ClearBit(BUZ_PORT, BUZ_PIN);

#define OCR_BASE_5MS	0x1387
#define OCR_BASE_10MS	0x270F

enum ShowModeEnum
{
	StartShowMode,
	Time,
	Date,
	Temp,
	Hum,
	CasperShow,
	EndShowMode
} ShowModeEnum;

// Глобальные переменные и флаги
volatile uint8_t AlarmEn = 0;
volatile uint8_t ButtonCode = 0;
volatile uint16_t RefVal[NUM_KEY];
volatile uint8_t ShowMode;
volatile uint16_t WaitSetCount = 0;
volatile uint8_t IncButtonCnt = 0;
volatile uint8_t DecButtonCnt = 0;
volatile uint8_t AutoShowModeCnt;
volatile uint8_t AutoShowModeEnable = 1;
volatile uint8_t ReadRTC = 0;
volatile uint8_t SetDateTimeEnabled = 0;
volatile uint8_t DayInMons[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
volatile uint16_t BeepCycles = 0;
volatile uint8_t SaveEEPROM = 0;

volatile struct DateTime
{
	uint8_t Year;
	uint8_t Mons;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
} DateTime;

volatile struct Alarm
{
	uint8_t Hour;
	uint8_t Minute;
	uint8_t CasperFlag;
} Alarm;

void beep(uint16_t cycles)
{
	TCNT1 = 0;
	OCR1A = OCR_BASE_5MS;
	BeepCycles = cycles;
	TCCR1B = (0 << CS12) | (1 << CS11) | (0 << CS10);
	BUZ_ON;
}

void show_alarm()
{
	char AlarmStr[4];
	sprintf(AlarmStr, "%02d%02d", Alarm.Hour, Alarm.Minute);
	IND_OutSym(AlarmStr, 1);
}

void show_time()
{
	char TimeStr[4];
	sprintf(TimeStr, "%02d%02d", DateTime.Hour, DateTime.Minute);
	IND_OutSym(TimeStr, 1);
}

void show_date()
{
	IND_OutUintFormat(DateTime.Day, 2, 1, 2);
	IND_OutUintFormat(DateTime.Mons, 0, 3, 4);
}

void set_alarm()
{
	uint8_t SetState = 1;
	SetDateTimeEnabled = 1;

	IND_BlinkMask(0x03);

	while (SetState == 1)
	{
		ButtonCode = TOUCH_GetKey();
		if ( ButtonCode == 1 ) {
			if ( ++Alarm.Hour == 24 ) Alarm.Hour = 0;
		}
		if ( ButtonCode == 3 ) {
			if ( --Alarm.Hour == 255 ) Alarm.Hour = 23;
		}
		if ( ButtonCode == 2 ) {
			SetState = 2;
		}
		show_alarm();
	}

	IND_BlinkMask(0x0C);

	while (SetState == 2)
	{
		ButtonCode = TOUCH_GetKey();
		if ( ButtonCode == 1 ) {
			if ( ++Alarm.Minute == 60 ) Alarm.Minute = 0;
		}
		if ( ButtonCode == 3 ) {
			if ( --Alarm.Minute == 255 ) Alarm.Minute = 59;
		}
		if ( ButtonCode == 2 ) {
			SetState = 0;
		}
		show_alarm();
	}
	SaveEEPROM = 1;
	SetDateTimeEnabled = 0;
	IND_BlinkMask(0x00);
	AutoShowModeEnable = 0;
	ShowMode = Time;
}

void set_time()
{
	uint8_t SetState = 1;
	SetDateTimeEnabled = 1;

	IND_BlinkMask(0x03);

	while (SetState == 1)
	{
		ButtonCode = TOUCH_GetKey();
		if ( ButtonCode == 1 ) {
			if ( ++DateTime.Hour == 24 ) DateTime.Hour = 0;
		}
		if ( ButtonCode == 3 ) {
			if ( --DateTime.Hour == 255 ) DateTime.Hour = 23;
		}
		if ( ButtonCode == 2 ) {
			SetState = 2;
		}
		show_time();
	}

	IND_BlinkMask(0x0C);

	while (SetState == 2)
	{
		ButtonCode = TOUCH_GetKey();
		if ( ButtonCode == 1 ) {
			if ( ++DateTime.Minute == 60 ) DateTime.Minute = 0;
		}
		if ( ButtonCode == 3 ) {
			if ( --DateTime.Minute == 255 ) DateTime.Minute = 59;
		}
		if ( ButtonCode == 2 ) {
			SetState = 0;
		}
		show_time();
	}
	rtc_set_time(DateTime.Hour, DateTime.Minute, 0);
	SetDateTimeEnabled = 0;
	IND_BlinkMask(0x00);
	AutoShowModeEnable = 0;
	ShowMode = Time;
}

void set_date()
{
	uint8_t SetState = 1;
	uint16_t TmpYear;
	char YearStr[4];
	SetDateTimeEnabled = 1;

	IND_BlinkMask(0x0F);

	while (SetState == 1)
	{
		ButtonCode = TOUCH_GetKey();

		if ( ButtonCode == 1 ) {
			if ( ++DateTime.Year == 100 ) DateTime.Year = 0;
		}
		if ( ButtonCode == 3 ) {
			if ( --DateTime.Year == 255 ) DateTime.Year = 99;
		}
		if ( ButtonCode == 2 ) {
			SetState = 2;
		}

		sprintf( YearStr, "20%02d", DateTime.Year );
		IND_OutSym( YearStr, 1);
	}

	TmpYear = 2000 + DateTime.Year;
	DayInMons[2] =  28 + ( ( ( ( TmpYear % 4 == 0 ) && ( TmpYear % 100 != 0 ) ) || ( TmpYear % 400 ) == 0 ) ? 1 : 0 );

	IND_BlinkMask(0x0C);

	while (SetState == 2)
	{
		ButtonCode = TOUCH_GetKey();
		if ( ButtonCode == 1 ) {
			if ( ++DateTime.Mons > 12 ) DateTime.Mons = 1;
		}
		if ( ButtonCode == 3 ) {
			if ( --DateTime.Mons < 1 ) DateTime.Mons = 12;
		}
		if ( ButtonCode == 2 ) {
			SetState = 3;
		}
		show_date();
	}

	IND_BlinkMask(0x03);

	while (SetState == 3)
	{
		ButtonCode = TOUCH_GetKey();
		if ( ButtonCode == 1 ) {
			if ( ++DateTime.Day > DayInMons[DateTime.Mons] ) DateTime.Day = 1;
		}
		if ( ButtonCode == 3 ) {
			if ( --DateTime.Day < 1 ) DateTime.Day = DayInMons[DateTime.Mons];
		}
		if ( ButtonCode == 2 ) {
			SetState = 0;
		}
		show_date(0x0F);
	}

	rtc_set_date(DateTime.Day, DateTime.Mons, DateTime.Year);
	SetDateTimeEnabled = 0;
	IND_BlinkMask(0x00);
	AutoShowModeEnable = 1;
	ShowMode = Date;
}

void show_message( char* string)
{
	char OutputString[4];
	for (int i = 0; i < strlen(string); i++) {
		strncpy(OutputString, string + i, 4);

		IND_OutSym(OutputString, 1);
		_delay_ms(250);
	}
}

ISR(TIMER0_OVF_vect)
{
	IND_Update();
	//TOUCH_CompareKey();
	if(TOUCH_CompareKey()) beep(1);
}

ISR(TIMER1_COMPA_vect)
{
	TCNT1 = 0;
	if (--BeepCycles == 0) {
		TCCR1B = (0 << CS12) | (0 << CS11) | (0 << CS10);
		BUZ_OFF;
	}
}

ISR(INT0_vect)
{
	if ( SetDateTimeEnabled == 0 ) {
		ReadRTC = 1;
		if ( ShowMode == Time ) InvBit( BLINK_LED_PORT, BLINK_LED_PIN );
		else ClearBit( BLINK_LED_PORT, BLINK_LED_PIN );
	}

	if (AlarmEn == 1) {
		beep(20);
	}
}

int main()
{
	int16_t Humidity;
	int16_t Temperature;
	char Text[40];
	eeprom_read_block( (uint8_t*)&Alarm, 0, sizeof( Alarm ) );

	if(Alarm.Hour == 0xFF) {
		Alarm.Hour = 0;
		Alarm.Minute = 0;
		DateTime.Hour = 0;
		DateTime.Minute = 0;
		rtc_set_time(DateTime.Hour, DateTime.Minute, 0);
	}

	// Timer 0 init
	TCCR0B = (0 << CS02) | (1 << CS01) | (1 << CS00);
	TCNT0 = 0x00;
	TIMSK0 = (1 << TOIE0);

	// Timer 1 init
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = 0;
	TIMSK1 = (1 << OCIE1A);

	// External interrupt init
	EIMSK = (1 << INT0);
	EICRA = ( 0 << ISC01) | (1 << ISC00);

	SetBit( BLINK_LED_DDR, BLINK_LED_PIN );
	SetBit( BUZ_DDR, BUZ_PIN );

	IND_Init();
	i2c_Init();
	TOUCH_Init();

#ifdef DS1307
	rtc_init( 1 << CTRL_SQWE );	// DS1307 init data
#endif
#ifdef DS323x
	rtc_init( ( 1 << CTRL_CONV ) | ( 0 << CTRL_EOSC ) | ( 1 << CTRL_BBSQW ) | ( 0 << CTRL_INTCN ) );	// DS323x init data
#endif
	HTSensorReset();

	_delay_ms(500);
	TOUCH_Scan((uint16_t*)RefVal);
	_delay_ms(500);

	sei();

	beep(1);
	show_message("    HELLO  CASPEr ");

	ShowMode = Time;

	while(1)
	{
		ButtonCode = TOUCH_GetKey();

		if ( SaveEEPROM ) {
			cli();
			eeprom_update_block( (uint8_t*)&Alarm, 0, sizeof( Alarm ) );
			sei();
			SaveEEPROM = 0;
		}

		if ( ButtonCode == INC_BUT_CODE ) {
			if(++ShowMode == EndShowMode) ShowMode = (StartShowMode + 1);
			AlarmEn = 0;
			AutoShowModeEnable = 0;
			if (WaitSetCount < WAIT_SET_COUNT) {
				IncButtonCnt++;
			}
		}

		if ( ButtonCode == DEC_BUT_CODE ) {
			if(--ShowMode == StartShowMode) ShowMode = Hum;
			AlarmEn = 0;
			AutoShowModeEnable = 0;
			if (WaitSetCount < WAIT_SET_COUNT) {
				DecButtonCnt++;
			}
		}

		if ( ButtonCode == SET_BUT_CODE ) {
			IncButtonCnt = 0;
			DecButtonCnt = 0;
			AlarmEn = 0;
			AutoShowModeEnable = 1;
			WaitSetCount = RESET_WAIT_SET_COUNT;
		}

		// Установка времени
		// Нажать один раз кнопку "Set" и 3 раза кнопку "+"
		if ( (IncButtonCnt == 3) && (DecButtonCnt == 0) ) {
			IncButtonCnt = 0;
			DecButtonCnt = 0;
			set_time();
		}
		// Установка даты
		// Нажать один раз кнопку "Set" и 3 раза кнопку "-"
		if ( (IncButtonCnt == 0) && (DecButtonCnt == 3) ) {
			IncButtonCnt = 0;
			DecButtonCnt = 0;
			set_date();
		}
		// Установка будильника
		// Нажать один раз кнопку "Set", один раз кнопку "-" и 3 раза кнопку "+"
		if ( (IncButtonCnt == 3) && (DecButtonCnt == 1) ) {
			IncButtonCnt = 0;
			DecButtonCnt = 0;
			set_alarm();
		}
		// Включение/отключение отображения приветственной надписи.
		// Нажать один раз кнопку "Set", один раз кнопку "+" и 3 раза кнопку "-"
		if ( (IncButtonCnt == 1) && (DecButtonCnt == 3) ) {
			IncButtonCnt = 0;
			DecButtonCnt = 0;
			if (Alarm.CasperFlag == 0) {
				Alarm.CasperFlag = 1;
			}
			else {
				Alarm.CasperFlag = 0;
			}
			sprintf(Text, "    CASPEr %s ", (Alarm.CasperFlag == 0)?("OFF"):("On"));
			SaveEEPROM = 1;
			show_message(Text);
		}

		switch(ShowMode)
		{
			case Time: {
				show_time();
			}; break;
			case Date: {
				ClearBit( BLINK_LED_PORT, BLINK_LED_PIN );
				show_date();
			}; break;
			case Temp: {
				ClearBit( BLINK_LED_PORT, BLINK_LED_PIN );
				IND_OutSym(" ", 1);
				IND_OutSym((Temperature >= 0)?(" "):("-"), 1);
				IND_OutUintFormat(Temperature, 0, 2, 3);
				IND_OutSym("*", 4);
			}; break;
			case Hum: {
				ClearBit( BLINK_LED_PORT, BLINK_LED_PIN );
				IND_OutSym("H", 1);
				IND_OutUintFormat(Humidity, 0, 2, 4);
			}; break;
			case CasperShow: {
				if (Alarm.CasperFlag == 1) {
					ClearBit( BLINK_LED_PORT, BLINK_LED_PIN );
					show_message("    HELLO  CASPEr ");
				}
				ShowMode = Time;
			}; break;
			default: {
				show_time();
			}; break;
		}

		if ( ReadRTC == 1 )
		{
			if ( AutoShowModeEnable == 1 ) {
				if ( ++AutoShowModeCnt == AUTO_CHANGE_TIME ) {
					if (++ShowMode == EndShowMode) ShowMode = Time;
					AutoShowModeCnt = 0;
				}
			}
			if ( WaitSetCount < WAIT_SET_COUNT ) WaitSetCount++;

			rtc_get_time( (uint8_t*)&DateTime.Hour, (uint8_t*)&DateTime.Minute, (uint8_t*)&DateTime.Second);
			rtc_get_date( (uint8_t*)&DateTime.Day, (uint8_t*)&DateTime.Mons, (uint8_t*)&DateTime.Year);

			// События будильника активны, если Часы и Минуты будильника отличны от нуля
			if ( (Alarm.Hour != 0) && (Alarm.Minute != 0) ) {
				// Включение сигнала будильника
				if ( (DateTime.Hour == Alarm.Hour) &&
					 (DateTime.Minute == Alarm.Minute) &&
					 (DateTime.Second == 0)) AlarmEn = 1;
				// Выключение сигнала будильника
				if ( (DateTime.Hour == Alarm.Hour) &&
					 (DateTime.Minute == (Alarm.Minute + 1)) &&
					 (DateTime.Second == 0)) AlarmEn = 0;
			}
			Temperature = (int16_t)HTSensorReadTemp();
			Humidity = (int16_t)HTSensorReadCompensatedHumidity();
			ReadRTC = 0;
		}
	};
};
