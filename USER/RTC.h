/*
 * RTC.h
 *
 *  Created on: Jan 21, 2026
 *      Author: wegi0
 */

#ifndef RTC_H_
#define RTC_H_

extern void print_RTC(void);
extern void read_RTC(void);
extern void colon_print(void);
extern char* getDayofweek(uint8_t number);
extern int calc_dow(int y, int m,int d);
extern void clear_week_field(void);
extern void stop_colon(void);

extern 	void  test_print_RTC(void);
extern 	void  test_setup(void);
extern void wait_for_releasseTS(void);
extern bool clear_ss(void) ;
extern void LCD_ClrScr(uint16_t color) ;
extern bool set_hours(void) ;
bool test_pressed_point(uint16_t xmin, uint16_t xmax, uint16_t ymin, uint16_t ymax);
extern void put_number(uint8_t data);
extern uint8_t test_action(void) ;
extern bool set_minuts(void);
extern bool set_year(void) ;
extern void set_RTC_DATE(void);
extern void check_RTC_DATE(void);
extern void put_DATE_number(void);
extern bool set_month(void);
extern bool set_day(void);
extern void Seconds_print(int ss) ;
extern bool setting_globals(void);
extern void CHECK_CALIBRATE_TIME_VALUES(void);
extern void compare_EPOCH();
extern void set_EPOCH(void);
extern void read_timestamp(void);
extern void READ_CENTURY(void);
extern void SET_CENTURY(void);
extern void paint_test(void);
extern void TTF_test(void);
extern void V_BALL(void);
extern void set_LIGHT(void);
extern void night_mode(void);
extern void set_PWM(void);
extern void set_night_PWM(void);
extern void night_light(void);
extern void check_night_mode(void);
extern void TIM_CALIBRATE_SETUP(void);
extern uint8_t calibr_action(void);
extern void sec_calibrate_print(void);
extern void hrs_calibrate_print(void);
extern void COLOR_FONTS(void);
extern void set_color_RTC(void);
extern void slideshow_disable(void);
extern void check_slideshow();
extern void test_page(void);
extern bool go_to_back(void);
extern bool go_to_back2(void);
extern void clr_pressed(void);
//---
extern void next_page(void);
//---
extern void DEMOS_RUN(void);
extern void SLIDE_SHOW(void);
//---
#endif /* RTC_H_ */
