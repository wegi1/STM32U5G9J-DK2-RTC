/*
 * demos.c
 *
 *  Created on: Feb 1, 2026
 *      Author: wegi0
 */
//===================================================================================================================
#include "demos.h"
#include "RTC.h"
//===================================================================================================================
void half_sec_delay(void);
void LCD_Put_Pixel(uint32_t x, uint32_t y, uint16_t Color);
void DelayMicro(uint32_t micros);


void demo_01(void);
void demo_02(void);
void demo_03(void);
void demo_04(void);
void demo_05(void);
void demo_06(void);
void demo_07(void);
void demo_08(void);
void demo_09(void);
void demo_10(void);
void demo_11(void);
void demo_12(void);
void demo_13(void);

uint32_t randomize(void);
uint16_t  getscanline(void);
//---
static void wait_t(void);
static void LCD_CLS_BLACK(void);
static uint32_t test_stop(void);
//===================================================================================================================
#define  LCD_ROW_NUM    480                // Y lines
#define  LCD_COL_NUM    800                // X pixels

#define sq_size 200
#define rect_size 100
#define t_wait 2
#define circle_size  230
//--------------------------------------------------------

//-------------------------
void DelayMicro( uint32_t micros)
{
    TIM7->CR1 = 0;
    TIM7->CNT = 0;
    TIM7->CR1 = 1;
    while (TIM7->CNT < micros) { __NOP();}
    TIM7->CR1 = 0;
}
//===================================================================================================================
extern	RNG_HandleTypeDef hrng;
extern TIM_HandleTypeDef htim7;
//---
uint32_t lines_addr[LCD_ROW_NUM];

//===================================================================================================================
//===================================================================================================================
void start_demos(void)
{
	demo_01();
	demo_02();
	demo_03();
	demo_04();
	demo_05();
	demo_06();
	demo_07();
	demo_08();
	demo_09();
	demo_10();
	demo_11();
	demo_12();
	demo_13();
}
//===================================================================================================================

//===================================================================================================================
uint32_t randomize(void){


	uint32_t data ;
	HAL_RNG_GenerateRandomNumber(&hrng, &data);
	return  data ;
}
//===================================================================================================================
uint16_t  getscanline(void)
{
	uint32_t data;
    data = LTDC->CPSR;
    data = data & 0xFFFF;

    return((uint16_t) data);
}
//===================================================================================================================
void half_sec_delay(void) {
	HAL_Delay(500);
}
//----------
static void wait_t(void)
{
    HAL_Delay(t_wait);
}
//===================================================================================================================
static void LCD_CLS_BLACK(void){ LCD_ClrScr(0) ;}
//===================================================================================================================
void demo_01(void) {
	LCD_CLS_BLACK();
	(void) BSP_LCD_SetBrightness(0, 100);

}
//===================================================================================================================
static uint32_t test_stop(void) {
	return (uint32_t) pressed;
}
//===================================================================================================================
void prepare_screen_line_address(void) {
#define FRAME_ADDRESS   ((uint32_t)0x200C0000)

	uint32_t data = FRAME_ADDRESS;
	for(uint32_t i = 0; i < LCD_ROW_NUM ; i++) {
		lines_addr[i] = data;
		data += 1600;
	}
}
//===================================================================================================================
void LCD_Put_Pixel(uint32_t x, uint32_t y, uint16_t Color) {
   uint16_t * PUT;
   PUT = (uint16_t*) (lines_addr[y] );
   PUT[x] = Color;
}
//===================================================================================================================
void demo_02(void)
{
    u32 i;
    LCD_CLS_BLACK();
    for(i=0;i<40000;i++)
    {
        LCD_Put_Pixel(randomize()%LCD_COL_NUM,
        		randomize()%LCD_ROW_NUM,
				randomize()%0xffff);
        DelayMicro(45);
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	LCD_ClrScr(0);
        	return;
        } // check the stop action
    }
    half_sec_delay();
}
//===================================================================================================================
void demo_03(void)
{
    u32 i;
    LCD_CLS_BLACK();

    for(i=0;i<1000;i++)
    {
    	UTIL_LCD_DrawLine(randomize()%LCD_COL_NUM,
        		randomize()%LCD_ROW_NUM,randomize()%LCD_COL_NUM,
				randomize()%LCD_ROW_NUM , randomize()%0xFFffff) ;
        HAL_Delay(1);
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
    }
    half_sec_delay();
}
//===================================================================================================================
void demo_04(void)
{
    u32 i;
    LCD_CLS_BLACK();

    for(i=0;i<1500;i++)
    {
		UTIL_LCD_DrawCircle(randomize()%720 + 40,
        		randomize()%400 + 40 ,
				randomize()%40+1,
				randomize()%0xFFffff);
        HAL_Delay(1);
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
    }
    half_sec_delay();
}
//===================================================================================================================
void demo_05(void)
{
    u16 i;
    u16 x0,y0,x1,y1;
    u32 color;
    LCD_CLS_BLACK();


    for(i=0;i<LCD_ROW_NUM;i++)
    {
    	UTIL_LCD_DrawHLine(0, i, 800, randomize()%0xffffff);
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
    }

    half_sec_delay();
    for(i=0;i<LCD_COL_NUM;i++)
    {
        UTIL_LCD_DrawVLine(i, 0, 480, randomize()%0xffffff);
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
    }

    half_sec_delay();

    x0 = 0;
    x1 = LCD_COL_NUM-1;
    y0 = 0;
    y1 = LCD_ROW_NUM-1;

    for(i=0; i<LCD_ROW_NUM/2 ;i++)
    {
        color = randomize()%0xFFffff;
        UTIL_LCD_DrawRect(x0, y0, (x1-x0), (y1-y0), color);
        x0++;
        x1--;
        y0++;
        y1--;
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
    }
    half_sec_delay();
}
//===================================================================================================================
void demo_06(void)
{
    u32 x0,y0,x1,y1,i;
    u32 Color, tmpr;
    LCD_CLS_BLACK();
    for(i=0;i<2000;i++)
    {
        Color = randomize()%0xFFffff;
        x0 = randomize()%LCD_COL_NUM;
        x1 = randomize()%LCD_COL_NUM;
        y0 = randomize()%LCD_ROW_NUM;
        y1 = randomize()%LCD_ROW_NUM;

        if(x1 < x0) {
        	tmpr = x0;
        	x0 = x1;
        	x1 = tmpr;
        }
        if(y1 < y0) {
        	tmpr = y0;
        	y0 = y1;
        	y1 = tmpr;
        }

        //UTIL_LCD_FillRect(Xpos, Ypos, Width, Height, Color)
        //LCD_DisARectangular(x0, y0, x1, y1, color);
        UTIL_LCD_FillRect(x0, y0, (x1-x0), (y1-y0), Color);

        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
    }
    half_sec_delay();

}
//===================================================================================================================
void demo_07(void)
{
    u32 Color = 0xff0000; //COLOR_565_RED;
    u32 x0,y0,x1,y1;
    u8 movetype = 0; //x++ y++
    u32 counter = 0;
    x0 = 0;
    x1 = x0+sq_size;
    y0 = 0;
    y1 = y0+sq_size;

    LCD_CLS_BLACK();
    UTIL_LCD_FillRect(x0, y0, (x1-x0), (y1-y0), Color);

    while(counter < 2000)
    {
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
        //-----------
        if(movetype == 0)
        {
            if(x0 < LCD_COL_NUM-sq_size)
            {
                if(y0 < LCD_ROW_NUM-sq_size )
                {
                    UTIL_LCD_DrawHLine(x0, y0, sq_size, 0); // gorna
                    UTIL_LCD_DrawVLine(x0, y0, sq_size, 0); // lewa

                    UTIL_LCD_DrawHLine(x0+1, y1, sq_size, Color); // dolna
                    UTIL_LCD_DrawVLine(x1, y0+1, sq_size, Color); // prawa


                    x0++;
                    x1++;
                    y0++;
                    y1++;

                    wait_t();
                }
                else
                {
                    movetype = 1; //x++ y--
                }
            }
            else
            {
                movetype = 3; //
            }

        }
        //--------------
        if(movetype == 1) //x++ y--
        {
            if(x0 < LCD_COL_NUM-sq_size)
            {
                if(y0 > 0 )
                {
                	UTIL_LCD_DrawHLine(x0, y1, sq_size, 0); // dolna
                	UTIL_LCD_DrawVLine(x0, y0, sq_size, 0); // lewa

                    UTIL_LCD_DrawHLine(x0+1, y0, sq_size, Color); // gorna
                    UTIL_LCD_DrawVLine(x1, y0-1, sq_size, Color); // prawa

                    x0++;
                    x1++;
                    y0--;
                    y1--;

                    wait_t();
                }
                else
                {
                    movetype = 0; //
                }
            }
            else
            {
                movetype = 2;
            }
        }
        //--------------
        if(movetype == 2)  // x-- y--
        {
            if(x0 > 0)
            {
                if(y0 > 0 )
                {
                	UTIL_LCD_DrawHLine(x0, y1, sq_size+1, 0); // dolna
                	UTIL_LCD_DrawVLine(x1, y0, sq_size, 0); // prawa

                    UTIL_LCD_DrawHLine(x0-1, y0, sq_size, Color); // gorna
                    UTIL_LCD_DrawVLine(x0, y0-1, sq_size, Color); // lewa

                    x0--;
                    x1--;
                    y0--;
                    y1--;

                    wait_t();
                }
                else
                {
                    movetype = 3; //x-- y++
                }
            }
            else
            {
                movetype = 1; //
            }
        }
        //--------------
        if(movetype == 3)  //x-- y++
        {
            if(x0 > 0)
            {
                if(y0 < LCD_ROW_NUM-sq_size )
                {
                    UTIL_LCD_DrawHLine(x0, y0, sq_size, 0); // gorna
                    UTIL_LCD_DrawVLine(x1, y0, sq_size, 0); // prawa

                	UTIL_LCD_DrawHLine(x0-1, y1, sq_size, Color); // dolna
                	UTIL_LCD_DrawVLine(x0, y0+1, sq_size, Color); // lewa

                    x0--;
                    x1--;
                    y0++;
                    y1++;

                    wait_t();
                }
                else
                {
                    movetype = 2;
                }
            }
            else
            {
                movetype = 0; // x-- y--
            }
        }
        //--------------



        counter ++;

    } // end while(counter < 2000)

}
//===================================================================================================================
void demo_08(void) {
	LCD_CLS_BLACK();
    int32_t x0,y0,x1,y1;

    x0 = 0;
    y0 = 0;
    x1 = 4;
    y1 = 479;

    while(y0 < 479)
    {
    	UTIL_LCD_DrawLine(x0, y0, x1, y1, 0x0000ff);

        y0 = y0+7;
        x1 = x1+12;
        HAL_Delay(10);
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
    }

    x0 = 799;
    y0 = 470;
    x1 = 798;
    y1 = 0;

    while(y0 > 0)
    {
    	UTIL_LCD_DrawLine(x0, y0, x1, y1, 0x0000ff);

        y0 = y0-7;
        x1 = x1-12;
        HAL_Delay(10);
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
    }
//-----------------------------
    x0 = 0;
    y0 = 479;
    x1 = 4;
    y1 = 0;

    while(y0 > 0)
    {
    	UTIL_LCD_DrawLine(x0, y0, x1, y1, 0xff0000);

        y0 = y0-7;
        x1 = x1+12;
        HAL_Delay(10);
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
    }

    x0 = 799;
    y0 = 0;
    x1 = 798;
    y1 = 479;

    while(y0 < 479)
    {
    	UTIL_LCD_DrawLine(x0, y0, x1, y1, 0xff0000);

        y0 = y0+7;
        x1 = x1-12;
        HAL_Delay(10);
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
    }

    HAL_Delay(1111);
}
//===================================================================================================================
void demo_09(void)
{
    u32 x0,y0,x1,y1,Color,i,tmpr;

    LCD_CLS_BLACK();

    for(i=0;i<100;i++)
    {
        Color = randomize()%0xffff;
        y0 = randomize()%LCD_ROW_NUM;
        y1 = randomize()%LCD_ROW_NUM;
        x0 = randomize()%LCD_COL_NUM;
        x1 = randomize()%LCD_COL_NUM;
        if(x1 < x0) {
        	tmpr = x0;
        	x0 = x1;
        	x1 = tmpr;
        }
        if(y1 < y0) {
        	tmpr = y0;
        	y0 = y1;
        	y1 = tmpr;
        }

        UTIL_LCD_DrawRect(x0, y0, (x1-x0), (y1-y0), Color);
        HAL_Delay(10);
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action

    }
    HAL_Delay(1000);
}
//===================================================================================================================
void demo_10(void) {
	    u32 i;
	    LCD_CLS_BLACK();

	    for(i=0;i<2500;i++)
	    {
	        UTIL_LCD_FillCircle(randomize()%600 +100, randomize()%280 +100, randomize()%100 +1, randomize()%0xffffff);
	        if(test_stop() != 0) {
	        	wait_for_releasseTS();
	        	half_sec_delay();
	        	return;
	        } // check the stop action
	    }

	    HAL_Delay(1000);
	}
//===================================================================================================================
void demo_11(void)
{
    u32 color = 0xff0000;
    u32 x0,y0;
    u8 movetype = 0; //x++ y++

    x0 = circle_size;
    y0 = LCD_ROW_NUM >> 1;


    LCD_CLS_BLACK();

    UTIL_LCD_FillCircle(x0, y0, circle_size, color);

    //-----------
    while(movetype == 0)
    {
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
        //---
        if(x0 < LCD_COL_NUM-circle_size)
        {
        	UTIL_LCD_DrawCircle(x0, y0, circle_size+2, 0);
        	UTIL_LCD_DrawCircle(x0, y0, circle_size+1, 0);
            x0++;
            UTIL_LCD_DrawCircle(x0, y0, circle_size, color);
            UTIL_LCD_DrawCircle(x0, y0, circle_size+1, color);
            UTIL_LCD_DrawCircle(x0, y0, circle_size+2, color);
        }
        else
        {
            movetype = 1; //
        }
    }
    //--------------
    while(movetype == 1)
    {
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
        //---
        if(x0 > circle_size+2)
        {
        	UTIL_LCD_DrawCircle(x0, y0, circle_size+2, 0);
        	UTIL_LCD_DrawCircle(x0, y0, circle_size+1, 0);
            x0--;
            UTIL_LCD_DrawCircle(x0, y0, circle_size, color);
            UTIL_LCD_DrawCircle(x0, y0, circle_size+1, color);
            UTIL_LCD_DrawCircle(x0, y0, circle_size+2, color);
        }
        else
        {
            movetype = 0; // x-- y--
        }
    }


    half_sec_delay();
}
//===================================================================================================================
void demo_12(void)
{
    u32 color = 0xff; // blue
    u32 x0,y0;
    u8 movetype = 0; //x++ y++
    u32 counter = 0;

    x0 = circle_size/2;
    y0 = circle_size/2;


    LCD_CLS_BLACK();

    UTIL_LCD_FillCircle(x0, y0, (circle_size >> 1), color);



    while(counter < 2000)
    {
        if(test_stop() != 0) {
        	wait_for_releasseTS();
        	half_sec_delay();
        	return;
        } // check the stop action
        //-----------
        if(movetype == 0)
        {
            if(x0 < LCD_COL_NUM-circle_size/2)
            {
                if(y0 < LCD_ROW_NUM-circle_size/2 )
                {
                	UTIL_LCD_DrawCircle(x0, y0, circle_size/2+2, 0);
                	UTIL_LCD_DrawCircle(x0, y0, circle_size/2+1, 0);
                	UTIL_LCD_DrawCircle(x0, y0, circle_size/2+3, 0);
                    x0++;
                    y0++;
                    UTIL_LCD_FillCircle(x0, y0, circle_size/2, color);

                    wait_t();
                }
                else
                {
                    movetype = 1; //x++ y--
                }
            }
            else
            {
                movetype = 3; //
            }
        }
        //--------------
        if(movetype == 1)
        {
            if(x0 < LCD_COL_NUM-circle_size/2)
            {
                if(y0 > circle_size/2 )
                {
                	UTIL_LCD_DrawCircle(x0, y0, circle_size/2+2, 0);
                	UTIL_LCD_DrawCircle(x0, y0, circle_size/2+1, 0);
                	UTIL_LCD_DrawCircle(x0, y0, circle_size/2+3, 0);
                    x0++;
                    y0--;
                    UTIL_LCD_FillCircle(x0, y0, circle_size/2, color);

                    wait_t();
                }
                else
                {
                    movetype = 0; //
                }
            }
            else
            {
                movetype = 2; // x-- y--
            }
        }
        //--------------
        if(movetype == 2)
        {
            if(x0 > circle_size/2)
            {
                if(y0 > circle_size/2 )
                {
                	UTIL_LCD_DrawCircle(x0, y0, circle_size/2+2, 0);
                	UTIL_LCD_DrawCircle(x0, y0, circle_size/2+1, 0);
                	UTIL_LCD_DrawCircle(x0, y0, circle_size/2+3, 0);

                    x0--;
                    y0--;
                    UTIL_LCD_FillCircle(x0, y0, circle_size/2, color);
                    wait_t();
                }
                else
                {
                    movetype = 3; //x-- y++
                }
            }
            else
            {
                movetype = 1; //
            }
        }
        //--------------
        if(movetype == 3)
        {
            if(x0 > circle_size/2)
            {
                if(y0 < LCD_ROW_NUM-circle_size/2 )
                {
                	UTIL_LCD_DrawCircle(x0, y0, circle_size/2+2, 0);
                	UTIL_LCD_DrawCircle(x0, y0, circle_size/2+1, 0);
                	UTIL_LCD_DrawCircle(x0, y0, circle_size/2+3, 0);
                    x0--;
                    y0++;
                    UTIL_LCD_FillCircle(x0, y0, circle_size/2, color);
                    wait_t();
                }
                else
                {
                    movetype = 2;
                }
            }
            else
            {
                movetype = 0; // x-- y--
            }
        }
        //--------------

        counter ++;

    }// while(counter < 2000)
    HAL_Delay(1000);
}
//===================================================================================================================
void demo_13(void)
{
    LCD_ClrScr(0x001f);
    HAL_Delay(1000);
    LCD_CLS_BLACK();
}
//===================================================================================================================

//===================================================================================================================
//===================================================================================================================
//===================================================================================================================

