/*
 * RTC.c
 *
 *  Created on: Jan 21, 2026
 *      Author: wegi0
 */

//===================================================================================================================
#include "main.h"
#include <time.h>
#include "stdio.h"
#include "RTC.h"
#include "ADT_F.h"
#include "types_h.h"
#include "inttypes.h"
#include "stm32u5g9j_discovery_lcd.h"
#include "demos.h"
//===================================================================================================================
extern uint8_t idx[];
extern uint8_t idy[];
extern char tekst[];
extern RTC_HandleTypeDef hrtc;
extern uint16_t lcd_text_color ;
extern uint16_t lcd_background_color ;
extern TIM_HandleTypeDef htim6;
__IO  extern  int8_t pressed ;
extern uint32_t brightness;
extern void TIM6_IRQHandler(void);
extern uint32_t shifted ;





//---
RTC_TimeTypeDef my_time;
RTC_TimeTypeDef old_time;
RTC_DateTypeDef my_date;

RTC_TimeTypeDef currentTime;
RTC_DateTypeDef currentDate;
time_t timestamp;
struct tm currTime;
//---


int32_t century = 21;
int32_t SECONDS_TO_CALIBRATE = 0;
uint32_t HOURS_CALIBRATE_PERIOD = 1;
uint8_t  UPDATE_FLAG = 0;

uint8_t change_colors = 0;
uint8_t run_slideshow = 0;
uint8_t mode_night = 0;

//---
uint8_t max_day; // maximum day in month

//---


//===================================================================================================================
//	RTC_BKP_DR0 ROTATION TEST MODE - NOT USED
//
//	RTC_BKP_DR1 LIGHT VALUE 0 TO 100 brightness
//	RTC_BKP_DR2 COLOR FONTS ENABLED 0 = ENABLED  != 0 DISABLED
//	RTC_BKP_DR3 NIGHT MODE ENABLED 0 = ENABLED  != 0 DISABLED
//	RTC_BKP_DR4 HOURS of calibrate interval
//	RTC_BKP_DR5  SECONDS TO CALIBRATE
//	RTC_BKP_DR6 slideshow mode enabled/disabled
//	RTC_BKP_DR7 night light value
//	RTC_BKP_DR8 epoch LOW 32 bits
//	RTC_BKP_DR9 epoch high 32 bits
//	RTC_BKP_DR10 century
//===================================================================================================================
//===================================================================================================================
//===================================================================================================================
//===================================================================================================================
void print_RTC(void){

	check_night_mode();
	int hh = my_time.Hours, mm = my_time.Minutes, ss = my_time.Seconds;

	set_color_RTC();

	old_time.Seconds = my_time.Seconds;
	sprintf(tekst, "%02d", hh);
	shifted = 25;
	render_shiftedtext_monospace(0, 0, &tekst[0], digital_7_ttf, 230);
	sprintf(tekst, "%02d", mm);
	render_text_monospace(396, 0, &tekst[0], digital_7_ttf, 230);

	Seconds_print(ss);

	my_date.WeekDay = calc_dow((int) (((century - 1) * 100) + my_date.Year)  , (int) my_date.Month , (int) my_date.Date );

	if(ss == 0) { // update weekday field
		if(hh == 0) { // between 00:00:00 and 00:00:07
			if(mm < 8) {
				clear_week_field();
			} // END if(mm < 8)
		} // END if(hh == 0) { // between 00:00:00 and 00:00:07
	} // END if(ss == 0) { // update weekday field
	lcd_text_boxed(190, 300, getDayofweek(my_date.WeekDay), SourceCodePro_Bold_ttf, 140);


	hh = my_date.Year + ((century - 1) * 100);
	sprintf(tekst, "%04d-%02d-%02d", hh, my_date.Month, my_date.Date);
	lcd_mono_text_boxed(0, 380, &tekst[0], digital_7_ttf, 80);


}
//===================================================================================================================
void read_RTC(void){
	  __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);
	  //HAL_RTC_WaitForSynchro(&hrtc);
	  HAL_RTC_GetTime(&hrtc, &my_time, RTC_FORMAT_BIN);
	  HAL_RTC_GetDate(&hrtc, &my_date, RTC_FORMAT_BIN);
}
//===================================================================================================================
void colon_print(void){
	TIM6->CR1 = TIM6->CR1 | 1;
	HAL_TIM_Base_Start_IT(&htim6);

	BSP_LCD_FillRect(0, 385, 80, 30, 30, lcd_text_color);
	BSP_LCD_FillRect(0, 385, 181, 30, 30, lcd_text_color);
}
//===================================================================================================================
char* getDayofweek(uint8_t number) {
	if(number == 0) { number = 1;}
	if(number  > 7) { number = 7;}
 	char *weekday[] = { "MONDAY", "TUESDAY", "WEDNESDAY","THURSDAY","FRIDAY","SATURDAY" , "SUNDAY"};
	return weekday[number-1];
}
//===================================================================================================================
int calc_dow(int y, int m,int d) {
	int result;

	y-=m<3;
	result = (y+y/4-y/100+y/400+"-bed=pen+mad."[m]+d)%7;
	if(result == 0) {result = 7;}
	return result;
}
//===================================================================================================================
void clear_week_field(void){
	BSP_LCD_FillRect(0, 555, 294, 244, 84, lcd_background_color);
}
//===================================================================================================================
void stop_colon(void) {
    HAL_TIM_Base_Stop_IT(&htim6);
    TIM6->CR1 = TIM6->CR1 & 0xfffffffe;
	BSP_LCD_FillRect(0, 385, 80, 30, 30, lcd_background_color);
	BSP_LCD_FillRect(0, 385, 181, 30, 30, lcd_background_color);
}
//===================================================================================================================
void  test_print_RTC(void) {
	int32_t tmpr1;
	uint8_t tmpr2, tmpr3;
	struct tm * czas_lokal;
	//---
	read_RTC();
	tmpr2 = my_time.Minutes;

	int ss = my_time.Seconds;
	set_color_RTC(); // TODO: add set color

	if(my_time.Seconds != old_time.Seconds) {
		old_time.Seconds = my_time.Seconds;
		if(ss == 3) {
			if(UPDATE_FLAG == 0){ compare_EPOCH(); }
		}

		//TODO ADD TIME
		if(UPDATE_FLAG == 1) {
			if(my_time.Seconds == 4) {

				UPDATE_FLAG = 0; // clear update flag
				tmpr1 = SECONDS_TO_CALIBRATE; // get for compare and calculations seconds calibrate value

//------------------
				if(tmpr1 != 0) { // exist calibration seconds value != 0 ?

					read_timestamp();
					timestamp = timestamp + tmpr1;
					czas_lokal = gmtime(&timestamp);

					my_date.Year     =  czas_lokal->tm_year % 100 ;
					my_date.Date     =  czas_lokal->tm_mday ;
					my_date.Month    =  czas_lokal->tm_mon +1 ;
					my_date.WeekDay  =  czas_lokal->tm_mday ;

					my_time.Hours    =  czas_lokal->tm_hour ;
					my_time.Minutes  =  czas_lokal->tm_min ;
					my_time.Seconds  =  czas_lokal->tm_sec ;

					set_RTC_DATE();
					HAL_RTC_SetTime(&hrtc, &my_time, RTC_FORMAT_BIN); // store setup value
					ss = my_time.Seconds;
					print_RTC(); // immediatelly RTC print
//---
				}
			}
		}

		if(my_time.Seconds == 0) {
			print_RTC();
			return;
		} // end if(my_time.Seconds == 0)


		//TODO image

		if(ss == 5) {
			if(run_slideshow == 0) {
				if(tmpr2 %5 == 0) {
					(void) BSP_LCD_SetBrightness(0,  100);
					tmpr3 = tmpr2 / 5;
					TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
					readPicFromFlash(tmpr3);
					HAL_Delay(8000);
					for(int t = 100; t > -1; t--) {
						(void) BSP_LCD_SetBrightness(0, (uint32_t) t);
						HAL_Delay(50);
					}
					LCD_ClrScr(lcd_background_color);
					(void) BSP_LCD_SetBrightness(0,  33);
					HAL_Delay(50);
					print_RTC();
					return;
				}
			}
		}


		Seconds_print(ss); // print colon and seconds
	} // end if(my_time.Seconds != old_time.Seconds)
}
//---
void Seconds_print(int ss) {
	colon_print(); // print colon for half second blink
	sprintf(tekst, "%02d", ss); // prepare seconds
	render_text_monospace(0, 295, &tekst[0], digital_7_ttf, 65); // print prepared seconds
}
//===================================================================================================================
void  test_setup(void) {
	if(pressed == 0) { return;}  // no touch pressed so good bye

	wait_for_releasseTS();

	if(clear_ss() == true) { return; } // clear seconds test
	// TODO SETUP CLOCK
	if(set_hours() == true) { return; }
	if(set_minuts() == true) { return; }
	if(set_year() == true) { return; }
	if(set_month() == true) { return; }
	if(set_day() == true) { return; }
	if(setting_globals() == true) {
		LCD_ClrScr(0);
		print_RTC();
		wait_for_releasseTS(); // wait for releasse TS
		HAL_Delay(55);
		return;
	}

	wait_for_releasseTS();
}
//===================================================================================================================
void wait_for_releasseTS(void) {
	while(1) {
		pressed = 0;
		HAL_Delay(50);
		if(pressed == 0) { break;}
	}
}
//===================================================================================================================
bool clear_ss(void) {
	if(possition_x < 170) {
		if(possition_y > 290) {
			if(possition_y < 380) {
				read_RTC();
				my_time.Seconds = 0;
				HAL_RTC_SetTime(&hrtc, &my_time, RTC_FORMAT_BIN);
				print_RTC();
				wait_for_releasseTS(); // wait for releasse TS
				return true;
			}
		}
	}
	return false;
}
//===================================================================================================================
void LCD_ClrScr(uint16_t color) {
	uint16_t* pFBaddr = (uint16_t *) LTDC_Layer1->CFBAR;
	for(uint32_t i = 0; i < (800*480) ; i++) { pFBaddr[i] = color; }
}
//===================================================================================================================
bool set_hours(void) {
	uint8_t action, bckp, chk_hrs;
	read_RTC();
	chk_hrs = my_time.Hours;
  if((test_pressed_point(0, 390, 0, 290)) == true) {

		TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
		LCD_ClrScr(lcd_background_color);
		while(1){
			lcd_text_color = 0xffff;
			lcd_text_boxed(100, 0, "SET HOURS" , SourceCodePro_Bold_ttf, 140);
			put_number( my_time.Hours);
			action = test_action();
			if((action == 3)) { // DONE pressed
				bckp = my_time.Hours;
				read_RTC();
				my_time.Hours = bckp;
				if(chk_hrs != my_time.Hours) {
					HAL_RTC_SetTime(&hrtc, &my_time, RTC_FORMAT_BIN); // store setup value
				}
				LCD_ClrScr(lcd_background_color);
				print_RTC();
				wait_for_releasseTS();
				return true;
			} else {
				if(action == 2) { // PLUS pressed
					if(my_time.Hours == 23) { my_time.Hours = 0;} else { my_time.Hours++;}
				} else { // MINUS pressed
					if(my_time.Hours == 0) { my_time.Hours = 23;} else { my_time.Hours--;}
				}
			}

		}

	}
	return false;
}
//===================================================================================================================
bool test_pressed_point(uint16_t xmin, uint16_t xmax, uint16_t ymin, uint16_t ymax){
	if(possition_x > xmin) {
		if(possition_x < xmax){
			if(possition_y >= ymin){
				if(possition_y < ymax){
					return true;
				}
			}
		}
	}
	return false;
}
//===================================================================================================================
void put_number(uint8_t data){
	put_DATE_number();
	sprintf(tekst, "%02d", data);
	lcd_mono_text_boxed(200, 100, tekst, digital_7_ttf, 220);
}
//===================================================================================================================
uint8_t test_action(void) {
	while(1) {

		if(pressed != 0) {
			HAL_Delay(100);
			pressed = 0;
			// DONE test
			if((test_pressed_point(210, 590, 390, 479)) == true) { return 3;} // DONE
			if((test_pressed_point(600, 799, 100, 390)) == true) { return 2;} // PLUS
			if((test_pressed_point(0, 200, 100, 389)) == true) { return 1;}  // MINUS
		}
	}
}
//===================================================================================================================
bool set_minuts(void) {

	uint8_t action , bckp, chk_min;
	read_RTC();
	chk_min = my_time.Minutes;
	if((test_pressed_point(400, 799, 0, 290)) == true) {
		TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
		LCD_ClrScr(lcd_background_color);
		while(1){
			lcd_text_color = 0xffff;
			lcd_text_boxed(80, 0, "SET MINUTS", SourceCodePro_Bold_ttf, 140);
			put_number( my_time.Minutes);
			action = test_action();
			if((action == 3)) { // DONE pressed
				bckp = my_time.Minutes;
				read_RTC();
				my_time.Minutes = bckp;

				if(chk_min != my_time.Minutes) {

					my_time.Seconds ++;
					if(my_time.Seconds > 59) { my_time.Seconds = 0;}
					__HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);
					//HAL_RTC_WaitForSynchro(&hrtc);
					HAL_RTC_SetTime(&hrtc, &my_time, RTC_FORMAT_BIN); // store setup value
				}

				LCD_ClrScr(lcd_background_color);
				print_RTC();
				wait_for_releasseTS();
				return true;
			} else {
				if(action == 2) { // PLUS pressed
					if(my_time.Minutes == 59) { my_time.Minutes = 0;} else { my_time.Minutes++;}
				} else { // MINUS pressed
					if(my_time.Minutes == 0) { my_time.Minutes = 59;} else { my_time.Minutes--;}
				}
			}

		}

	}
	return false; // doesn't choose set hours
}
//===================================================================================================================
bool set_year(void) {
	uint8_t action;
	if((test_pressed_point(0, 280, 380, 479)) == true)  {
		TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
		LCD_ClrScr(lcd_background_color);
		while(1){
			lcd_text_color = 0xffff;
			lcd_text_boxed(120, 0, "SET YEAR", SourceCodePro_Bold_ttf, 140);
			sprintf(tekst, "%04d", (int) (my_date.Year + ((century - 1) * 100)));
			lcd_mono_text_boxed(250, 190, tekst, digital_7_ttf, 80);
			put_DATE_number();
			action = test_action();
			check_RTC_DATE();
			if((action == 3)) { // DONE pressed
				set_RTC_DATE();
				LCD_ClrScr(lcd_background_color);
				print_RTC();
				wait_for_releasseTS();
				return true;
			} else {
				if(action == 2) { // PLUS pressed
					if(my_date.Year == 99) { my_date.Year = 0;} else { my_date.Year++;}
				} else { // MINUS pressed
					if(my_date.Year == 0) { my_date.Year = 99;} else { my_date.Year--;}
				}
			}

		}

	}
	return false; // doesn't choose set year
}
//===================================================================================================================
void set_RTC_DATE(void){
	check_RTC_DATE();
	HAL_RTC_SetDate(&hrtc, &my_date, RTC_FORMAT_BIN);
}
void check_RTC_DATE(void){
	static const uint8_t month_maxdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31} ;
	uint8_t luty_add = 0;

	if(my_date.Year > 99) { my_date.Year = my_date.Year % 100;}
	if((my_date.Year %4) == 0) {luty_add =1;}
	if(my_date.Month == 0) {my_date.Month =1;}
	if(my_date.Month > 12) {my_date.Month =12;}
	max_day = month_maxdays[my_date.Month - 1];
	if(my_date.Month == 2) { max_day = max_day + luty_add;}
	if(my_date.Date == 0 ) {my_date.Date = 1;}
	if(my_date.Date > max_day) {my_date.Date = max_day;}
	my_date.WeekDay = calc_dow((int) (((century - 1) * 100) + my_date.Year)  , (int) my_date.Month , (int) my_date.Date );


}
//===================================================================================================================
void put_DATE_number(void){
    lcd_text_color = 0xffff;
	lcd_mono_text_boxed(565, 100, "+", digital_7_ttf, 230);
	shifted = 35;
	render_shiftedtext_monospace(0, 100, "-", digital_7_ttf, 230);
	lcd_text_boxed(300, 400, "DONE", SourceCodePro_Bold_ttf, 120);

	UTIL_LCD_DrawRect(200, 100, 400, 290, 0xffffff);  // number rect
	UTIL_LCD_DrawRect(0,   100, 190, 290, 0xffffff);  // MINUS rect
	UTIL_LCD_DrawRect(605, 100, 190, 290, 0xffffff);  // PLUS rect
	UTIL_LCD_DrawRect(200, 390, 400,  89, 0xffffff);  // DONE rect
}
//===================================================================================================================
bool set_month(void) {
	uint8_t action;
	if((test_pressed_point(290, 500, 380, 479)) == true)  {
		TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
		LCD_ClrScr(lcd_background_color);
		while(1){
			lcd_text_color = 0xffff;
			lcd_text_boxed(100, 0, "SET MONTH", SourceCodePro_Bold_ttf, 140);
			sprintf(tekst, "%02d", (my_date.Month));
			lcd_mono_text_boxed(320, 190, tekst, digital_7_ttf, 80);

			put_DATE_number();
			action = test_action();
			check_RTC_DATE();
			if((action == 3)) { // DONE pressed
				set_RTC_DATE();
				LCD_ClrScr(lcd_background_color);
				print_RTC();
				wait_for_releasseTS();
				return true;
			} else {
				if(action == 2) { // PLUS pressed
					if(my_date.Month == 12) { my_date.Month = 1;} else { my_date.Month++;}
				} else { // MINUS pressed
					if(my_date.Month == 1) { my_date.Month = 12;} else { my_date.Month--;}
				}
			}

		}

	}
	return false; // doesn't choose set year
}
//===================================================================================================================
bool set_day(void) {
	uint8_t action;
		if((test_pressed_point(520, 799, 380, 479)) == true)  {
		TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
		LCD_ClrScr(lcd_background_color);
		check_RTC_DATE(); // get max day in month
		while(1){
			lcd_text_color = 0xffff;
			lcd_text_boxed(160, 0, "SET DAY", SourceCodePro_Bold_ttf, 140);
			sprintf(tekst, "%02d", (my_date.Date));
			lcd_mono_text_boxed(320, 190, tekst, digital_7_ttf, 80);
			put_DATE_number();
			action = test_action();
			check_RTC_DATE();
			if((action == 3)) { // DONE pressed
				set_RTC_DATE();
				LCD_ClrScr(lcd_background_color);
				print_RTC();
				wait_for_releasseTS();
				return true;
			} else {
				if(action == 2) { // PLUS pressed
					if(my_date.Date == max_day) { my_date.Date = 1;} else { my_date.Date++;}
				} else { // MINUS pressed
					if(my_date.Date == 1) { my_date.Date = max_day;} else { my_date.Date--;}
				}
			}
		}
	}
	return false; // doesn't choose set year
}
//===================================================================================================================
void READ_CENTURY(void) {
	century = (int32_t) HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR10);
	if(century > 99) {
		century = 21;
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR10,  (uint32_t) century);
	}
	if(century < -99) {
		century = 21;
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR10,  (uint32_t) century);
	}
}
//===================================================================================================================
void compare_EPOCH() {
	uint64_t tmpr01;
	time_t pgm_timestamp;
	double elapsed;
	uint32_t hours, tmpr1, tmpr_hi, tmpr_lo;

	tmpr01 = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR9); // read high 32 bits
	tmpr01 = tmpr01 << 32;
	tmpr01 = tmpr01 |  HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR8); // add low 32 bits

	pgm_timestamp = (time_t) tmpr01;
	read_timestamp(); // read RTC to timestamp
	elapsed = difftime(timestamp, pgm_timestamp); // = TIMESTAMP - PGM_TIMESTAMP

	// ELAPSED = ACTUAL TIME - PGM_TIMESTAMP
	if(elapsed > 0) {
		UPDATE_FLAG = 1;
		hours = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4); // iterval hours calibrate event

		tmpr1 = hours * 3600;


		timestamp = pgm_timestamp;
		timestamp = timestamp + tmpr1;

		tmpr_hi = (uint32_t) (timestamp >> 32);
		tmpr_lo = (uint32_t) (timestamp & 0xFFFFFFFF);

		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR8,  tmpr_lo);  // LO timestamp
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR9, tmpr_hi);  // HI timestamp

	}
}

//=====================================================================================================================
void set_EPOCH(void) { // setting the EPOCH date to calibrate seconds date
	uint32_t hours, tmpr1, tmpr_hi, tmpr_lo;

	hours = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4); // iterval hours calibrate event

	tmpr1 = hours * 3600;

	read_timestamp(); // read RTC to timestamp

	timestamp = timestamp + tmpr1;

	tmpr_hi = (uint32_t) (timestamp >> 32);
	tmpr_lo = (uint32_t) (timestamp & 0xFFFFFFFF);

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR8,  tmpr_lo);  // LO timestamp
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR9, tmpr_hi);  // HI timestamp

	UPDATE_FLAG = 0;
}
//=====================================================================================================================
void read_timestamp(void) {
	HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BIN);

	currTime.tm_year = currentDate.Year + ((century -20) * 100);  // In fact: 2000 + 18 - 1900
	currTime.tm_mday = currentDate.Date;
	currTime.tm_mon  = currentDate.Month - 1;

	currTime.tm_hour = currentTime.Hours;
	currTime.tm_min  = currentTime.Minutes;
	currTime.tm_sec  = currentTime.Seconds;

	timestamp = mktime(&currTime);
}

//===================================================================================================================
void CHECK_CALIBRATE_TIME_VALUES(void){
	uint32_t hours ;
	int32_t  seconds;
	hours = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4); // iterval hours calibrate event
	seconds = (int32_t) HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5); // seconds to add calibrate

	if(hours == 0)      { hours   = 1;   }
	if(hours > 744)     { hours   = 744; }
	if(seconds > 99)    { seconds = 0;   } // seconds to calibrate
	if(seconds < (-99)) { seconds = 0;   } // seconds to calibrate

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint32_t) seconds);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, hours);
	HOURS_CALIBRATE_PERIOD = hours;
	SECONDS_TO_CALIBRATE   = seconds;
}
//===================================================================================================================
void set_color_RTC(void)
{
  lcd_text_color =  0xffff ;
  lcd_background_color = 0x0000;

  if ((change_colors & 0x01)!= 0) { return;}

  if(my_time.Minutes < 15) {
	  lcd_text_color = LCD_COLOR565_GREEN;
	  return;
  }
  if(my_time.Minutes < 30) {
	  lcd_text_color = LCD_COLOR565_RED;
	  return;
  }
  if(my_time.Minutes < 45) {
	  lcd_text_color = LCD_COLOR565_MAGENTA;
	  return;
  }
}
//===================================================================================================================
void SET_CENTURY(void) {

	uint8_t action ;
	read_RTC();
	READ_CENTURY();

	if((test_pressed_point(400, 800, 360, 480)) != true)  { return;}
	wait_for_releasseTS();
	HAL_Delay(100);

	TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
	LCD_ClrScr(lcd_background_color);
	lcd_text_boxed(40, 0, "SET CENTURY", SourceCodePro_Bold_ttf, 140);
	put_DATE_number();
	while(1){
		//===================================================================================================
		sprintf(tekst, "%d", (int) century);
		lcd_text_color = 0;
		lcd_mono_text_boxed(300, 190, "000", digital_7_ttf, 80);
		lcd_text_color = 0xffff;
		lcd_mono_text_boxed(300, 190, tekst, digital_7_ttf, 80);
		//===================================================================================================
		check_RTC_DATE();
		action = test_action();
		if((action == 3)) { // DONE pressed

			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR10,  (uint32_t) century);
			read_RTC();
			set_RTC_DATE();
			LCD_ClrScr(lcd_background_color);
			wait_for_releasseTS();
			HAL_Delay(100);
			clr_pressed();
			return ;
		} else {
			if(action == 2) { // PLUS pressed
				if(century == 99) { century = -99;} else { century++;}
			} else { // MINUS pressed
				if(century == -99) { century = 99;} else { century--;}
			}
		}
	}

}

//===================================================================================================================
void paint_test(void) {
	if((test_pressed_point(0, 400, 96, 192)) != true)  {
		return;
	}
	wait_for_releasseTS();
	HAL_Delay(111);
	Touchscreen_demo();
	wait_for_releasseTS();
	LCD_ClrScr(lcd_background_color);
	clr_pressed();
}
//===================================================================================================================
extern void TTF_test(void) {
	if((test_pressed_point(400, 800, 96, 192)) != true)  {
		return;
	}
	Big_TTF_Demo();
	wait_for_releasseTS();
	LCD_ClrScr(lcd_background_color);
	clr_pressed();
}
//===================================================================================================================
extern void V_BALL(void) {
	if((test_pressed_point(400, 800, 120, 240)) != true)  {
		return;
	}
	wait_for_releasseTS();
	HAL_Delay(111);
	extern void main_app(void);
	main_app();
	wait_for_releasseTS();
	HAL_Delay(111);
	LCD_ClrScr(lcd_background_color);
	clr_pressed();
}
//===================================================================================================================
void set_LIGHT(void) {

	if((test_pressed_point(0, 400, 192, 288)) != true)  {
		return;
	}

		TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
		LCD_ClrScr(lcd_background_color);
		lcd_text_boxed(120, 0, "SET LIGHT", SourceCodePro_Bold_ttf, 140);
		lcd_text_boxed(300, 400, "DONE", SourceCodePro_Bold_ttf, 120);
		UTIL_LCD_DrawRect(200, 390, 400,  89, 0xffffff);  // DONE rect
		UTIL_LCD_DrawRect(0, 185, 800,  115, 0xffffff);  // slider
		wait_for_releasseTS();
		brightness = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
		possition_x = brightness << 3;
		possition_y = 200; // set first time pressed values
		pressed = 1;

		while(1) {
			if(pressed != 0) {
				pressed = 0;
				if((test_pressed_point(210, 590, 390, 479)) == true) { // done pressed
					HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, brightness);
					wait_for_releasseTS();
					HAL_Delay(111);
					LCD_ClrScr(lcd_background_color);
					clr_pressed();
					return ;
				} // DONE
				if(test_pressed_point(0, 800, 185, 300) == true) {
					brightness = (possition_x >> 3) ; // x / 8
					if(brightness >= 100) { brightness = 99;}
					if(brightness < 2){ brightness = 2;}
					(void) BSP_LCD_SetBrightness(0, brightness);
					(void) BSP_LCD_GetBrightness(0,&brightness);

					uint32_t lendraw = (brightness << 3) ;


					BSP_LCD_FillRect(0, 4, 190, lendraw,  105, LCD_COLOR565_GREEN);
					BSP_LCD_FillRect(0, 4+lendraw, 190, 795-lendraw,  105, 0);
					sprintf(tekst, "%03d%%", (int) brightness);

					lcd_render_just_text(250, 190, tekst, digital_7_ttf, 80,0,0);

					pressed = 0;
					HAL_Delay(10);
				}
			}
		}

}
//===================================================================================================================
void night_mode(void) {

	uint8_t action ;
	uint32_t n_mode;



	if((test_pressed_point(0, 400, 288, 384)) != true)  {
		return;
	}
	TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
	LCD_ClrScr(lcd_background_color);

	lcd_text_boxed(80, 0, "NIGHT MODE", SourceCodePro_Bold_ttf, 140);
	put_DATE_number();
	n_mode = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);

	while(1){
//===================================================================================================

		n_mode = n_mode &0x01;

		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, n_mode);

		lcd_text_color = 0;
		lcd_mono_text_boxed(300, 190, "000", digital_7_ttf, 80);
		lcd_text_color = 0xffff;


		if(n_mode  != 0) {
			sprintf(tekst, "NO!");
			brightness = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
		} else {
			sprintf(tekst , "YES" );
			brightness = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR7);
		}

		if(brightness <2) {brightness = 2;}
		if(brightness > 100 ){brightness = 100;}
		(void) BSP_LCD_SetBrightness(0, brightness);

		lcd_mono_text_boxed(300, 190, tekst, digital_7_ttf, 80);
		if(n_mode  == 0) {
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR7, brightness);
		}
//===================================================================================================
		check_RTC_DATE();
		action = test_action();
		if((action == 3)) { // DONE pressed
			read_RTC();
			n_mode = n_mode &0x01;
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, n_mode);
			mode_night = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3); // read back data from backup register to variable

			wait_for_releasseTS(); // wait for releasse TS
			LCD_ClrScr(lcd_background_color);
			if(mode_night == 0) {night_light();}
			check_night_mode();
			clr_pressed();
			return ;
		} else {
			wait_for_releasseTS(); // wait for releasse TS
			if(action == 2) { // PLUS pressed
				n_mode++;
			} else { // MINUS pressed
				n_mode--;
			}
		}
	}
}

//===================================================================================================================
void set_PWM(void) {

	uint32_t light_val = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);

	if(light_val > 100) { light_val = 100;}
	if(light_val < 2)  { light_val = 2; }
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, light_val);
	brightness = light_val;
	(void) BSP_LCD_SetBrightness(0, light_val);
}
//---
void set_night_PWM(void) {

	uint32_t light_val = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR7);
	if(light_val > 100) { light_val = 100;}
	if(light_val < 2)  { light_val = 2; }
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR7, light_val);
	brightness = light_val;
	(void) BSP_LCD_SetBrightness(0, light_val);
}
//===================================================================================================================
void check_night_mode(void) {

#define nigh_value 20

	int hh;
	read_RTC();
	hh = my_time.Hours;

	if((mode_night &1) == 0) {
		if(hh < 6)  {
			set_night_PWM();
			return;
		}
		if(hh > (nigh_value-1)) {
			set_night_PWM();
			return;
		}

	}
	set_PWM(); // out of night mode turn on the light as day mode

}
//===================================================================================================================
void night_light(void){
	pressed = 1;
	TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
	LCD_ClrScr(lcd_background_color);
	lcd_text_boxed(120, 0, "SET NIGHT", SourceCodePro_Bold_ttf, 140);
	lcd_text_boxed(300, 400, "DONE", SourceCodePro_Bold_ttf, 120);
	UTIL_LCD_DrawRect(200, 390, 400,  89, 0xffffff);  // DONE rect
	UTIL_LCD_DrawRect(0, 185, 800,  115, 0xffffff);  // slider
	wait_for_releasseTS();
	possition_x = brightness << 3;
	possition_y = 200; // set first time pressed values
	pressed = 1;

	while(1) {
		if(pressed != 0) {
			pressed = 0;
			if((test_pressed_point(210, 590, 390, 479)) == true) { // done pressed
				HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR7, brightness);
				wait_for_releasseTS();
				HAL_Delay(111);
				LCD_ClrScr(lcd_background_color);
				return ;
			} // DONE
			if(test_pressed_point(0, 800, 185, 300) == true) {
				brightness = (possition_x >> 3) ; // x / 8
				if(brightness >= 100) { brightness = 99;}
				if(brightness < 2){ brightness = 2;}
				(void) BSP_LCD_SetBrightness(0, brightness);
				(void) BSP_LCD_GetBrightness(0,&brightness);

				uint32_t lendraw = (brightness << 3) ;


				BSP_LCD_FillRect(0, 4, 190, lendraw,  105, LCD_COLOR_RGB565_RED);
				BSP_LCD_FillRect(0, 4+lendraw, 190, 795-lendraw,  105, 0);
				sprintf(tekst, "%03d%%", (int) brightness);

				lcd_render_just_text(250, 190, tekst, digital_7_ttf, 80,0,0);

				pressed = 0;
				HAL_Delay(10);
			}
		}
	}
}
//===================================================================================================================
uint8_t calibr_action(void) {
	while(1) {

		HAL_Delay(200);
		// DONE test
		if((test_pressed_point(200, 600, 250, 330)) == true) { return 5;} // DONE

		if((test_pressed_point(1, 195, 100, 250)) == true) { return 1;} // minus sec
		if((test_pressed_point(1, 195, 330, 480)) == true) { return 3;} // minus hrs

		if((test_pressed_point(605, 800, 100, 250)) == true) { return 2;} // plus sec
		if((test_pressed_point(605, 800, 330, 480)) == true) { return 4;} // plus hrs

	}
}
//===================================================================================================================
void sec_calibrate_print(void) {
	lcd_text_color = lcd_background_color;
	lcd_mono_text_boxed(350, 120, "000", digital_7_ttf, 80); // clear field by spaces
	lcd_text_color = 0xFFFF;
	sprintf(tekst, "%d", ((int)SECONDS_TO_CALIBRATE));
	if(SECONDS_TO_CALIBRATE < 0) {
		lcd_mono_text_boxed(350, 120, tekst, digital_7_ttf, 80); // print seconds
	} else {
		lcd_mono_text_boxed(410, 120, tekst, digital_7_ttf, 80); // print seconds
	}
}
//===================================================================================================================
void hrs_calibrate_print(void) {
	lcd_text_color = lcd_background_color;
	lcd_mono_text_boxed(350, 350,"000", digital_7_ttf, 80); // clear field by spaces
	lcd_text_color = 0xFFFF;
	sprintf(tekst, "%d", ((int)HOURS_CALIBRATE_PERIOD));
	if(HOURS_CALIBRATE_PERIOD > 99) {
		lcd_mono_text_boxed(350, 350, tekst, digital_7_ttf, 80); // print hours
	} else {
		lcd_mono_text_boxed(410, 350, tekst, digital_7_ttf, 80); // print hours
	}
}
//===================================================================================================================
void TIM_CALIBRATE_SETUP(void) {
	uint8_t action ;
	if((test_pressed_point(400, 800, 288, 384)) != true)  {
		return;
	}
	TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
	//==============
	//= MENU PRINT =
	//==============
	LCD_ClrScr(lcd_background_color);
	lcd_text_color = 0xffff;
	lcd_mono_text_boxed(55, 0, "SET TIM CALIBR", SourceCodePro_Bold_ttf, 100);
	lcd_mono_text_boxed(200, 150, "sec", SourceCodePro_Bold_ttf, 80);
	lcd_mono_text_boxed(200, 380, "hrs", SourceCodePro_Bold_ttf, 80);
	lcd_mono_text_boxed(30, 100, "-", digital_7_ttf, 120);
	UTIL_LCD_DrawRect(0, 100, 195,  150, 0xffffff);  // MINUS SEC rect
	lcd_mono_text_boxed(30, 330, "-", digital_7_ttf, 120);
	UTIL_LCD_DrawRect(0, 330, 195,  150, 0xffffff);  // MINUS HRS rect
	lcd_mono_text_boxed(630, 100, "+", digital_7_ttf, 120);
	UTIL_LCD_DrawRect(605, 100, 195,  150, 0xffffff);  // PLUS SEC rect
	lcd_mono_text_boxed(630, 330, "+", digital_7_ttf, 120);
	UTIL_LCD_DrawRect(605, 330, 195,  150, 0xffffff);  // PLUS SEC rect
	lcd_text_boxed(320, 260, "DONE", SourceCodePro_Bold_ttf, 100);
	UTIL_LCD_DrawRect(200, 250, 400,  80, 0xffffff);  // DONE rect
	//============================================

	sec_calibrate_print();
	hrs_calibrate_print();
	wait_for_releasseTS();

	while(1){
		pressed = 0;
		possition_x = 0;
		possition_y = 0;

		action = calibr_action();
		HAL_Delay(50);

		if(action == 5) {
			wait_for_releasseTS(); // wait for releasse TS
			LCD_ClrScr(lcd_background_color);
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, HOURS_CALIBRATE_PERIOD);
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint32_t) SECONDS_TO_CALIBRATE);
			set_EPOCH();
			clr_pressed();
			return ;
		}
		if(action == 2) { // PLUS sec pressed
			if(SECONDS_TO_CALIBRATE < 99)  { SECONDS_TO_CALIBRATE++;} else { SECONDS_TO_CALIBRATE = -99;}
		}
		if (action == 1) { // MINUS sec pressed
			if(SECONDS_TO_CALIBRATE > -99)  { SECONDS_TO_CALIBRATE--;} else { SECONDS_TO_CALIBRATE = 99;}
		}
		if(action == 4) { // PLUS hrs pressed
			if(HOURS_CALIBRATE_PERIOD < 744)  { HOURS_CALIBRATE_PERIOD++;} else { HOURS_CALIBRATE_PERIOD = 1 ;}
		}
		if(action == 3) { // PLUS hrs pressed
			if(HOURS_CALIBRATE_PERIOD > 1)  { HOURS_CALIBRATE_PERIOD--;} else { HOURS_CALIBRATE_PERIOD = 744 ;}
		}

		if(action < 3) { // print seconds update
			sec_calibrate_print();

		} else {
			hrs_calibrate_print();
		}


	} // end while(1)
}
//===================================================================================================================
void COLOR_FONTS(void) {
	uint8_t action;
	uint32_t font_VAL = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
	if((test_pressed_point(400, 800, 192, 288)) != true)  {
		return;
	}

	TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
	LCD_ClrScr(lcd_background_color);
	lcd_text_color = 0xffff;
	lcd_text_boxed(40, 0, "COLOR FONTS", SourceCodePro_Bold_ttf, 140);
	put_DATE_number();

	while(1){
		font_VAL = font_VAL &0x01;
		lcd_text_color = 0;
		lcd_mono_text_boxed(300, 190, "000", digital_7_ttf, 80);
		lcd_text_color = 0xffff;

		if(font_VAL  != 0) { sprintf(tekst, "NO!"); } else { sprintf(tekst , "YES" ); }
		lcd_mono_text_boxed(300, 190, tekst, digital_7_ttf, 80);

		wait_for_releasseTS();
		action = test_action();
		if((action == 3)) { // DONE pressed
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, font_VAL);
			change_colors = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
			wait_for_releasseTS(); // wait for releasse TS
			LCD_ClrScr(lcd_background_color);
			HAL_Delay(100);
			clr_pressed();
			return ;
		} else {
			if(action == 2) { // PLUS pressed
				font_VAL++;
			} else { // MINUS pressed
				font_VAL--;
			}
		}

	}
}
//===================================================================================================================
void check_slideshow() {
	run_slideshow = (uint8_t) (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR6) & 0x01 );
}
//===================================================================================================================
extern void slideshow_disable(void) {
	uint8_t s_mode, action;
	if((test_pressed_point(400, 800, 240, 360)) != true)  { return;}

	lcd_text_color = 0xffff;
	TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
	LCD_ClrScr(lcd_background_color);
	check_slideshow(); // read slide setup
	s_mode = run_slideshow;
	lcd_text_boxed(80, 0, "SLIDE MODE", SourceCodePro_Bold_ttf, 140);

	put_DATE_number();
	wait_for_releasseTS();
	HAL_Delay(100);

	while(1){
		s_mode = s_mode &0x01;
		if(s_mode  != 0) { sprintf(tekst, "NO!"); } else { sprintf(tekst , "YES" ); }
		lcd_mono_text_boxed(300, 190, tekst, digital_7_ttf, 80);

		wait_for_releasseTS();
		action = test_action();
		if((action == 3)) { // DONE pressed
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR6, s_mode);
			check_slideshow();
			wait_for_releasseTS(); // wait for releasse TS
			LCD_ClrScr(lcd_background_color);
			HAL_Delay(100);
			clr_pressed();
			return ;
		} else {
			if(action == 2) { // PLUS pressed
				s_mode++;
			} else { // MINUS pressed
				s_mode--;
			}
		}
	}
}
//===================================================================================================================
bool go_to_back(void) {
	if((test_pressed_point(0, 400, 384, 480)) == true)  {
		wait_for_releasseTS(); // wait for releasse TS
		LCD_ClrScr(lcd_background_color);
		HAL_Delay(100);
		return true;
	}
	return false; // doesn't choose set year
}
//===================================================================================================================
void test_page(void) {
	if((test_pressed_point(400, 800, 384, 480)) == true)  {
		wait_for_releasseTS(); // wait for releasse TS
		LCD_ClrScr(lcd_background_color);

		next_page();

		wait_for_releasseTS(); // wait for releasse TS
		LCD_ClrScr(lcd_background_color);
		clr_pressed();
	}
}
//===================================================================================================================
bool setting_globals(void){
	if((test_pressed_point(170, 800, 290, 380)) != true)  {  // was pressed SETTING GLOBALS menu?
		return false;
	}
	LCD_ClrScr(lcd_background_color);

	while(1){
		(void) BSP_LCD_SetBrightness(0, 100);
		lcd_text_color = 0XFFFF;
		lcd_background_color = 0;
		TIM6_IRQHandler(); // disable timer6 and IRQ from TIM6
//---
		lcd_text_boxed(25, 10, "GLOBAL FUNCTIONS", SourceCodePro_Bold_ttf, 100);
//---
		lcd_text_boxed(90, 116, "PAINT", SourceCodePro_Bold_ttf, 100);
		lcd_text_boxed(470, 116, "BIGTTF", SourceCodePro_Bold_ttf, 100);
		lcd_text_boxed(90, 212, "LIGHT", SourceCodePro_Bold_ttf, 100);
		lcd_text_boxed(470, 212, "COLORS", SourceCodePro_Bold_ttf, 100);
		lcd_text_boxed(111, 308, "AUTO", SourceCodePro_Bold_ttf, 100);
		lcd_text_boxed(470, 308, "CALIBR", SourceCodePro_Bold_ttf, 100);
		lcd_text_boxed(110, 404, "EXIT", SourceCodePro_Bold_ttf, 100);
		lcd_text_boxed(510, 404, "NEXT", SourceCodePro_Bold_ttf, 100);
//---
		UTIL_LCD_DrawVLine(400, 96, 384, 0xffffff); // set partial menu draw

		UTIL_LCD_DrawRect(0, 96, 800,  384, 0xffffff);  // draw full frame of menu

		UTIL_LCD_DrawHLine(0, 192, 800, 0xffffff); // 1st 2 pos of menu
		UTIL_LCD_DrawHLine(0, 288, 800, 0xffffff); // 2nd 2 pos of menu
		UTIL_LCD_DrawHLine(0, 384, 800, 0xffffff); // 3rd 2 pos of menu

		wait_for_releasseTS();
		HAL_Delay(250);
		pressed = 0;
		while(pressed == 0) { asm volatile("NOP");}

		if( go_to_back() == true) { return true;}
		paint_test();
		TTF_test();
		set_LIGHT();
		COLOR_FONTS();
		night_mode();
		TIM_CALIBRATE_SETUP();
		test_page();
	}
}
//===================================================================================================================
void next_page(void) {
	LCD_ClrScr(lcd_background_color);

	while(1){
		(void) BSP_LCD_SetBrightness(0, 100);
		lcd_text_color = 0XFFFF;
		lcd_background_color = 0;
//---
		lcd_text_boxed(50, 25, "OTHER FUNCTIONS", SourceCodePro_Bold_ttf, 100);
//---
		lcd_text_boxed(90, 154, "DEMOS", SourceCodePro_Bold_ttf, 100);
		lcd_mono_text_boxed(450, 140, "V_BALL", SourceCodePro_Bold_ttf, 100);
		lcd_text_boxed(70, 274, "SLIDES", SourceCodePro_Bold_ttf, 100);
		lcd_text_boxed(510, 274, "PICS", SourceCodePro_Bold_ttf, 100);
		lcd_text_boxed(110, 394, "EXIT", SourceCodePro_Bold_ttf, 100);
		lcd_text_boxed(440, 394, "CENTURY", SourceCodePro_Bold_ttf, 100);

		UTIL_LCD_DrawVLine(400, 120, 360, 0xffffff); // set partial menu draw
		UTIL_LCD_DrawRect(0, 120, 800,  360, 0xffffff);  // draw full frame of menu
		UTIL_LCD_DrawHLine(0, 240, 800, 0xffffff); // 1st 2 pos of menu
		UTIL_LCD_DrawHLine(0, 360, 800, 0xffffff); // 2nd 2 pos of menu


		wait_for_releasseTS();
		HAL_Delay(250);
		pressed = 0;
		while(pressed == 0) { asm volatile("NOP");}

		if( go_to_back2() == true) { return ; }
		DEMOS_RUN();
		SLIDE_SHOW();
		V_BALL();
		SET_CENTURY();
		slideshow_disable();
		clr_pressed();

	} // end while(1)
}
//===================================================================================================================

//===================================================================================================================

//===================================================================================================================

//===================================================================================================================

//===================================================================================================================
void clr_pressed(void) {
	pressed = 0;
	possition_x = 1111;
	possition_y = 1111;
}
//===================================================================================================================
bool go_to_back2(void) {
	if((test_pressed_point(0, 400, 360, 480)) == true)  {
		wait_for_releasseTS(); // wait for releasse TS
		LCD_ClrScr(lcd_background_color);
		HAL_Delay(100);
		return true;
	}
	return false; // doesn't choose set year
}
//===================================================================================================================
void DEMOS_RUN(void){
	if((test_pressed_point(0, 400, 120, 240)) != true)  { return ; }

	wait_for_releasseTS();
	HAL_Delay(50);
	start_demos();

	clr_pressed();
}
//===================================================================================================================
void SLIDE_SHOW(void) {
	uint8_t i;

	if((test_pressed_point(0, 400, 240, 360)) != true)  { return ; }

	wait_for_releasseTS(); // wait for releasse TS


	LCD_ClrScr(lcd_background_color);
	(void) BSP_LCD_SetBrightness(0,  100);

	for( i=0; i<13; i++) {



		readPicFromFlash(i);
		pressed = 0;
		while(pressed == 0) { asm volatile ("NOP");}
		wait_for_releasseTS(); // wait for releasse TS
	}

	for(int t = 100; t > -1; t--) {
		(void) BSP_LCD_SetBrightness(0, (uint32_t) t);
		HAL_Delay(50);
	}
	wait_for_releasseTS(); // wait for releasse TS
	LCD_ClrScr(lcd_background_color);
	check_night_mode();
	clr_pressed();
}
//===================================================================================================================

