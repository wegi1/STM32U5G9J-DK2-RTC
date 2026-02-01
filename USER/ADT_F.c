/*
 * ADT_F.c
 *
 *  Created on: Sep 23, 2025
 *      Author: BOLO
 */


//=====================================================================================
#include "main.h"
#include "stdio.h"


#include "stb_truetype.h"
#include "fonty.h"
#include "boldfonts.h"

#include "ADT_F.h"
#include "stm32_lcd.h"
//=====================================================================================

//================================================================================
extern TS_Init_t TsInit ;
//================================================================================

char tekst[30];
stbtt_fontinfo font;
uint16_t lcd_text_color = 0xffff;
uint16_t lcd_background_color = 0;
int efect_dx = 1;
uint8_t fade_effect = 0;
uint32_t shifted = 25;

static void lcd_text_bounds(stbtt_fontinfo *font, const char *text, float pixel_height, int *out_width, int *out_height);
//=====================================================================================




//=====================================================================================



extern uint8_t idx[];
extern uint8_t idy[];



//=====================================================================================
//===================================================================================


int render_biggestFont_portait(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height)
{
 	//lcd_Direction(ROTATE_0);
 	if (!stbtt_InitFont(&font, font_data, stbtt_GetFontOffsetForIndex(font_data, 0)))
 	{
 		return 0;  // nie uda³o sie zainicjowaæ fontów :(
 	}

 	float scale = stbtt_ScaleForPixelHeight(&font, pixel_height);

 	int ascent, descent, line_gap;
 	stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);
 	int baseline = (int) (ascent * scale);

 	// Ustalamy staly rozmiar glifu (monospace)
 	int adv_width, lsb;
 	stbtt_GetCodepointHMetrics(&font, 'M', &adv_width, &lsb);  // szerokoœæ najwiêkszego znaku
 	int mono_w = (int) (adv_width * scale);
 	int text_len = strlen(text);

 	int x0, y0, x1, y1;
 	stbtt_GetCodepointBitmapBox(&font, 'M', scale, scale, &x0, &y0, &x1, &y1);
 	int total_h = baseline + y1;


 	// Renderowanie znaków jeden po drugim
 	int pen_x = 0;
 	for (int i = 0; i < text_len; i++)
 	{
 		int cp = text[i];
 		int x0, y0, x1, y1;
 		stbtt_GetCodepointBitmapBox(&font, cp, scale, scale, &x0, &y0, &x1, &y1);

 		int gw = x1 - x0;
 		int gh = y1 - y0;

// 		u16 winx_start = (((240 + mono_w) >> 1)  - gw - ((240 - mono_w) >> 1)) ; // center monospace in LCD


// 		my_utoa(&idx[0], mono_w);
// 		my_utoa(&idy[0], gw);

 		unsigned char *g_bitmap = stbtt_GetCodepointBitmap(&font, scale, scale, cp, &gw, &gh, 0, 0);
 		uint32_t FRAME_ADDRESS = LTDC_Layer1->CFBAR;
 		uint32_t x_offset = (pen_x << 1);
 		uint32_t y_offset = pozy * 1600;
 		uint32_t GL_ADDR = FRAME_ADDRESS + x_offset + y_offset;


 		for (int row = 0; row < gh; row++)
 		{
 			uint16_t* pFBaddr = (uint16_t *) GL_ADDR;
 			for (int col = 0; col < gw; col++)
 			{
 				unsigned char val = g_bitmap[row * gw + col];

 				if (val > 0)
 				{
 					pFBaddr[col] =  lcd_text_color;
 				} else {
 					pFBaddr[col]  =  lcd_background_color;
 				}
 			}
 			GL_ADDR = GL_ADDR + 1600;
 		}
 		stbtt_FreeBitmap(g_bitmap, NULL);
 		pen_x += mono_w;
 	}
 	return total_h;
 }


//=====================================================================================

//TODO TTF DEMO HERE
//=====================================================================================

void Big_TTF_Demo(void)
{
	uint32_t FRAME_ADDRESS = 0x200C0000  ;
	uint32_t * pCLSaddr = (uint32_t *) FRAME_ADDRESS;
	for(uint32_t i = 0; i < (800*480) ; i++) { pCLSaddr[i] = 0; } // clear 2 buffors

	for (uint8_t t=0;t<11;t++)
	{
		sprintf(tekst, "%01d", t%10);
//		render_biggestFont_portait(0, 0, tekst, digital_7_ttf, 380);
//		lcd_mono_text_boxed(0, 0, tekst, digital_7_ttf, 380);
//		lcd_mono_text_boxed(0, 0, "4321", digital_7_ttf, 230);
//		render_text_monospace(0, 0, "4321", digital_7_ttf, 230);
//		render_text_monospace(396, 0, "21", digital_7_ttf, 230);
//		render_text_monospace(0, 0, tekst, digital_7_ttf, 350);
//		lcd_mono_text_boxed(2, 0, tekst, digital_7_ttf, 230);
//		lcd_text_boxed(0, 0, tekst, digital_7_ttf, 380);
		render_rotate_text_monospace(0, 0, tekst, digital_7_ttf, 540);
//		HAL_Delay(200);

	}


	FRAME_ADDRESS = LTDC_Layer1->CFBAR ;
	if(FRAME_ADDRESS == 0x200C0000) {
		FRAME_ADDRESS = 0x2017B800;
	} else {
		FRAME_ADDRESS = 0x200C0000;
	}

	uint16_t * pFBaddr = (uint16_t *) FRAME_ADDRESS;

	for(uint32_t i = 0; i < (800*480) ; i++) { pFBaddr[i] = 0; }
	LTDC_Layer1->CFBAR  = FRAME_ADDRESS;
	LTDC->SRCR = LTDC_SRCR_VBR; // or LTDC_SRCR_IMR for immediatelly change
	while(((uint32_t) FRAME_ADDRESS) != LTDC_Layer1->CFBAR);

	if(FRAME_ADDRESS == 0x200C0000) { return; }

	FRAME_ADDRESS = 0x200C0000;
	pFBaddr = (uint16_t *) FRAME_ADDRESS;
	for(uint32_t i = 0; i < (800*480) ; i++) { pFBaddr[i] = 0; }
	LTDC_Layer1->CFBAR  = FRAME_ADDRESS ;
	LTDC->SRCR = LTDC_SRCR_IMR ; // immediatelly change

}
//=====================================================================================
//TODO ROTATE 90 deg render
int render_rotate_text_monospace(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height)
{
	uint32_t BUF_Y[800];
	uint16_t buforek[480];

	uint32_t FRAME_ADDRESS = LTDC_Layer1->CFBAR;

	if(FRAME_ADDRESS == 0x200C0000) {
		FRAME_ADDRESS = 0x2017B800;
	} else {
		FRAME_ADDRESS = 0x200C0000;
	}
	uint32_t init = FRAME_ADDRESS + 160;
	uint16_t* pFBaddr = 0;

	for(uint32_t tst = 0; tst < 800 ; tst++) {
		BUF_Y[tst] = init;
		init = init + 1600;
	}


	if (!stbtt_InitFont(&font, font_data, stbtt_GetFontOffsetForIndex(font_data, 0)))
	{
		return 0;
	}

	float scale = stbtt_ScaleForPixelHeight(&font, pixel_height);


	//  Ustalamy rozmiar glifu (monospace)
	int adv_width, lsb;
	stbtt_GetCodepointHMetrics(&font, 'M', &adv_width, &lsb);  //
	int mono_w = (int) (adv_width * scale);


	int x0, y0, x1, y1;
	int cp = text[0];

	stbtt_GetCodepointBitmapBox(&font, cp, scale, scale, &x0, &y0, &x1, &y1);

	int gw = x1 - x0;
	int gh = y1 - y0;
	unsigned char *g_bitmap = stbtt_GetCodepointBitmap(&font, scale, scale, cp, &gw, &gh, 0, 0);




	for (int row = 0; row < gh; row++)
	{
		for(uint32_t it = 0; it< (mono_w) ; it++) { buforek[it] = lcd_background_color  ; }
		for (int col = 0; col < gw; col++)
		{
			unsigned char val = g_bitmap[row * gw + col];
			int px = col + (mono_w - gw)  ;  // centrowanie w polu monospace
			if (val > 0) {	buforek[px] = lcd_text_color; }
		}
		for(uint32_t it = 0 ; it < mono_w ; it++) {

				pFBaddr = (uint16_t *) BUF_Y[520 - it];
				pFBaddr[row] = buforek[it];

		}

	}

	stbtt_FreeBitmap(g_bitmap, NULL);

	// switch screen buffor to actual drawing
	LTDC_Layer1->CFBAR  = FRAME_ADDRESS;
	LTDC->SRCR = LTDC_SRCR_VBR; // or LTDC_SRCR_IMR for immediatelly change
	while(((uint32_t) FRAME_ADDRESS) != LTDC_Layer1->CFBAR);
	return 0;
}
//=====================================================================================
//=====================================================================================
//=====================================================================================
//=====================================================================================
//=====================================================================================
int render_shiftedtext_monospace(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height)
{

	if (!stbtt_InitFont(&font, font_data, stbtt_GetFontOffsetForIndex(font_data, 0)))
	{
		return 0;
	}

	float scale = stbtt_ScaleForPixelHeight(&font, pixel_height);

	int ascent, descent, line_gap;
	stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);
	int baseline = (int) (ascent * scale);

	//  Ustalamy rozmiar glifu (monospace)
	int adv_width, lsb;
	stbtt_GetCodepointHMetrics(&font, 'M', &adv_width, &lsb);  //
	int mono_w = (int) (adv_width * scale);
	int text_len = strlen(text);

	int x0, y0, x1, y1;
	stbtt_GetCodepointBitmapBox(&font, 'M', scale, scale, &x0, &y0, &x1, &y1);
	int total_h = baseline + y1;

	//  Renderowanie znaeden po drugim
	int pen_x = pozx;
	for (int i = 0; i < text_len; i++)
	{
		int cp = text[i];
		int x0, y0, x1, y1;
		stbtt_GetCodepointBitmapBox(&font, cp, scale, scale, &x0, &y0, &x1, &y1);

		int gw = x1 - x0;
		int gh = y1 - y0;
		unsigned char *g_bitmap = stbtt_GetCodepointBitmap(&font, scale, scale, cp, &gw, &gh, 0, 0);

		uint16_t center_y = ((total_h - gh) >> 1) + pozy; // Y center in monospace fields
		uint16_t buforek[480];


 		uint32_t FRAME_ADDRESS = LTDC_Layer1->CFBAR;
 		uint32_t x_offset = (pen_x << 1);
 		uint32_t y_offset = center_y * 1600;
 		uint32_t GL_ADDR = FRAME_ADDRESS + x_offset + y_offset;


		for (int row = 0; row < gh; row++)
		{
			uint16_t* pFBaddr = (uint16_t *) GL_ADDR;
			for(uint16_t it = 0; it< (mono_w) ; it++) { buforek[it] = lcd_background_color  ; }
			for (int col = 0; col < gw; col++)
			{
				unsigned char val = g_bitmap[row * gw + col];
				int px = col + (mono_w - gw) -shifted ;  // centrowanie w polu monospace
				if (val > 0) {	buforek[px] = lcd_text_color; }
			}
			for(uint16_t it = 0 ; it < mono_w ; it++) { pFBaddr[it] = buforek[it];}
			GL_ADDR = GL_ADDR + 1600;
		}
		stbtt_FreeBitmap(g_bitmap, NULL);
		pen_x += mono_w;
	}

	return total_h;
}
//=====================================================================================

int render_text_monospace(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height)
{

	if (!stbtt_InitFont(&font, font_data, stbtt_GetFontOffsetForIndex(font_data, 0)))
	{
		return 0;
	}

	float scale = stbtt_ScaleForPixelHeight(&font, pixel_height);

	int ascent, descent, line_gap;
	stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);
	int baseline = (int) (ascent * scale);

	//  Ustalamy rozmiar glifu (monospace)
	int adv_width, lsb;
	stbtt_GetCodepointHMetrics(&font, 'M', &adv_width, &lsb);  //
	int mono_w = (int) (adv_width * scale);
	int text_len = strlen(text);

	int x0, y0, x1, y1;
	stbtt_GetCodepointBitmapBox(&font, 'M', scale, scale, &x0, &y0, &x1, &y1);
	int total_h = baseline + y1;



	//  Renderowanie znaeden po drugim
	int pen_x = pozx;
	for (int i = 0; i < text_len; i++)
	{
		int cp = text[i];
		int x0, y0, x1, y1;
		stbtt_GetCodepointBitmapBox(&font, cp, scale, scale, &x0, &y0, &x1, &y1);

		int gw = x1 - x0;
		int gh = y1 - y0;
		unsigned char *g_bitmap = stbtt_GetCodepointBitmap(&font, scale, scale, cp, &gw, &gh, 0, 0);
		uint16_t center_y = ((total_h - gh) >> 1) + pozy; // Y center in monospace fields
		uint16_t buforek[480];


 		uint32_t FRAME_ADDRESS = LTDC_Layer1->CFBAR;
 		uint32_t x_offset = (pen_x << 1);
 		uint32_t y_offset = ( center_y)* 1600;
 		uint32_t GL_ADDR = FRAME_ADDRESS + x_offset + y_offset;

		for (int row = 0; row < gh; row++)
		{
			uint16_t* pFBaddr = (uint16_t *) GL_ADDR;
			for(uint16_t it = 0; it< (mono_w) ; it++) { buforek[it] = lcd_background_color  ; }
			for (int col = 0; col < gw; col++)
			{
				unsigned char val = g_bitmap[row * gw + col];
				int px = col + (mono_w - gw)  ;  // centrowanie w polu monospace
				if (val > 0) {	buforek[px] = lcd_text_color; }
			}
			for(uint16_t it = 0 ; it < mono_w ; it++) { pFBaddr[it] = buforek[it];}
			GL_ADDR = GL_ADDR + 1600;
		}
		stbtt_FreeBitmap(g_bitmap, NULL);
		pen_x += mono_w;
	}

	return total_h;
}
//=====================================================================================
int lcd_mono_text_boxed(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height)
{
	return pozy + render_text_monospace(pozx, pozy, text, font_data, pixel_height);
}
//=====================================================================================
static void lcd_text_bounds(stbtt_fontinfo *font, const char *text, float pixel_height, int *out_width, int *out_height)
{
	float scale = stbtt_ScaleForPixelHeight(font, pixel_height);

	int ascent, descent, line_gap;
	stbtt_GetFontVMetrics(font, &ascent, &descent, &line_gap);

	int baseline = (int) (ascent * scale);
	int min_x = 999999, min_y = 999999;
	int max_x = -999999, max_y = -999999;

	int x = 0;
	for (const char *p = text; *p; p++)
	{

		int glyph = stbtt_FindGlyphIndex(font, *p);

		int ax, lsb;
		stbtt_GetGlyphHMetrics(font, glyph, &ax, &lsb);

		int x0, y0, x1, y1;
		stbtt_GetGlyphBitmapBox(font, glyph, scale, scale, &x0, &y0, &x1, &y1);

		int gx0 = x + x0;
		int gy0 = baseline + y0;
		int gx1 = x + x1;
		int gy1 = baseline + y1;

		if (gx0 < min_x) min_x = gx0;
		if (gy0 < min_y) min_y = gy0;
		if (gx1 > max_x) max_x = gx1;
		if (gy1 > max_y) max_y = gy1;

		int kern = stbtt_GetGlyphKernAdvance(font, glyph, *(p + 1));
		x += (int) (ax * scale) + (int) (kern * scale);
	}

	*out_width = (max_x - min_x);
	*out_height = (max_y - min_y) + 3;
}
//===================================================================================
//===================================================================================

/**
 * @brief procedura rysuj¹ca
 * @param pozx
 * @param pozy
 * @param text
 * @param font_data
 * @param pixel_height
 * @param fast
 * @param boxed
 */
int lcd_render_text(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height, uint8_t fast, uint8_t boxed)
{

	int text_width, text_height;

	if (!stbtt_InitFont(&font, font_data, stbtt_GetFontOffsetForIndex(font_data, 0)))
	{
		return 0;  //
	}

	lcd_text_bounds(&font, text, pixel_height, &text_width, &text_height);
	float scale = stbtt_ScaleForPixelHeight(&font, pixel_height);

	int ascent, descent, line_gap;
	stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);

//	int baseline = (int) (ascent * scale);
	int x = pozx;

	for (const char *p = text; *p; p++)
	{
		int glyph = stbtt_FindGlyphIndex(&font, *p);

		int ax, lsb;
		stbtt_GetGlyphHMetrics(&font, glyph, &ax, &lsb);

		int x0, y0, x1, y1;
		stbtt_GetGlyphBitmapBox(&font, glyph, scale, scale, &x0, &y0, &x1, &y1);

		int width = x1 - x0;
		int height = y1 - y0;

		unsigned char *bitmap = stbtt_GetCodepointBitmap(&font, scale, scale, *p, &width, &height, &x0, &y0);


		uint32_t FRAME_ADDRESS = LTDC_Layer1->CFBAR;
		uint32_t x_offset = (x << 1);
		uint32_t y_offset = pozy * 1600;
		uint32_t GL_ADDR = FRAME_ADDRESS + x_offset + y_offset;

		for (int row = 0; row < height; row++)
		{
			uint16_t* pFBaddr = (uint16_t *) GL_ADDR;
			for (int col = 0; col < width; col++)
			{
				uint8_t value = bitmap[row * width + col];
				if (value > 0)
				{
					pFBaddr[col] =  lcd_text_color;
				}
				else
				{
					pFBaddr[col] =  lcd_background_color;
				}
			}
			GL_ADDR = GL_ADDR + 1600;
		}

		stbtt_FreeBitmap(bitmap, NULL);

		int kern = stbtt_GetGlyphKernAdvance(&font, glyph, *(p + 1));
		x += (int) (ax * scale) + (int) (kern * scale);
	}

	return text_height;
}

//=====================================================================================
int lcd_text_boxed(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height)
{
	return pozy + lcd_render_text(pozx, pozy, text, font_data, pixel_height, 0, 0);
}

//=====================================================================================

//=====================================================================================
/**
 * @brief Kolor tekstu w formacie rgb565.
 * @param kolor
 */
void lcd_set_text_color(uint16_t kolor)
{
	lcd_text_color = kolor;
}
//===================================================================================

/**
 * @brief Kolor t³a tekstu w formacie rgb565.
 * @param kolor
 */
void lcd_set_background_color(uint16_t kolor)
{
	lcd_background_color = kolor;
}
//=====================================================================================

/**
 * @brief procedura rysuj¹ca
 * @param pozx
 * @param pozy
 * @param text
 * @param font_data
 * @param pixel_height
 * @param fast
 * @param boxed
 */
int lcd_render_just_text(int pozx, int pozy, const char *text, const unsigned char *font_data, float pixel_height, uint8_t fast, uint8_t boxed)
{

	int text_width, text_height;

	if (!stbtt_InitFont(&font, font_data, stbtt_GetFontOffsetForIndex(font_data, 0)))
	{
		return 0;  //
	}

	lcd_text_bounds(&font, text, pixel_height, &text_width, &text_height);
	float scale = stbtt_ScaleForPixelHeight(&font, pixel_height);

	int ascent, descent, line_gap;
	stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);

//	int baseline = (int) (ascent * scale);
	int x = pozx;

	for (const char *p = text; *p; p++)
	{
		int glyph = stbtt_FindGlyphIndex(&font, *p);

		int ax, lsb;
		stbtt_GetGlyphHMetrics(&font, glyph, &ax, &lsb);

		int x0, y0, x1, y1;
		stbtt_GetGlyphBitmapBox(&font, glyph, scale, scale, &x0, &y0, &x1, &y1);

		int width = x1 - x0;
		int height = y1 - y0;

		unsigned char *bitmap = stbtt_GetCodepointBitmap(&font, scale, scale, *p, &width, &height, &x0, &y0);


		uint32_t FRAME_ADDRESS = LTDC_Layer1->CFBAR;
		uint32_t x_offset = (x << 1);
		uint32_t y_offset = pozy * 1600;
		uint32_t GL_ADDR = FRAME_ADDRESS + x_offset + y_offset;

		for (int row = 0; row < height; row++)
		{
			uint16_t* pFBaddr = (uint16_t *) GL_ADDR;
			for (int col = 0; col < width; col++)
			{
				uint8_t value = bitmap[row * width + col];
				if (value > 0)
				{
					pFBaddr[col] =  lcd_text_color;
				}
			}
			GL_ADDR = GL_ADDR + 1600;
		}

		stbtt_FreeBitmap(bitmap, NULL);

		int kern = stbtt_GetGlyphKernAdvance(&font, glyph, *(p + 1));
		x += (int) (ax * scale) + (int) (kern * scale);
	}

	return text_height;
}

//=====================================================================================

//=====================================================================================================================


//=====================================================================================


//=====================================================================================
//=====================================================================================
//=====================================================================================
//=====================================================================================
//=====================================================================================
//=====================================================================================
//=====================================================================================
//=====================================================================================
//=====================================================================================
//=====================================================================================
//=====================================================================================
