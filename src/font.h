#include "font6x8.h"
#include "font12x16.h"

int font_sizex;
int font_sizey;
int fonts_WIDTH;
int fonts_HEIGHT;
const u16 *fontData;

void setfont(int sizex,int sizey, int WIDTH, int HEIGHT,const u16* Data)
{
	font_sizex = sizex;
	font_sizey = sizey;
	fonts_WIDTH = WIDTH;
	fonts_HEIGHT = HEIGHT;
	fontData = Data;	
}
