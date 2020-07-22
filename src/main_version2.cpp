#include <stdlib.h>
#include "gba.h"	//GBA register definitions
#include "Game_pal2.h"  //Game palette
#include "capcom_sprites.h"  //sprite image data 
#include "SNK_sprites.h"  //sprite image data 
#include "back_offs.h"  //background image data 
#include "other_gfx.h"
#include "font.h"
#include "card_font_sprites.h"
#include "intro_screen.h"
#include "Capcom_chars_hp.h"
#include "Capcom_chars_sp.h"
#include "SNK_chars_hp.h"
#include "SNK_chars_sp.h"
#include "SNK_backups.h"
#include "Capcom_backups.h"
#include "capcom_names.h"
#include "SNK_names.h"
#include "num_names.h"
#include "hpsp_names.h"

#include "combat_area_gfx.h"
#include "player_faces.h"
#include "explode_gfx.h"
#include "magic_gfx.h"
#include "snk_abilities.h"
#include "snk_abilities_desc.h"
#include "capcom_abilities.h"
#include "capcom_abilities_desc.h"

#include "battle_engine_defs.h"
#include "SNK_AI_VALUES.h"
#include "CAPCOM_AI_VALUES.h"
#include "hgreen_gfx.h"
//#include "sfx.h"
//#include "gbfs.h"
/*
const GBFS_FILE *samples;

s32 sound_length;
s32 bgm_length;

void isr(void)
{
	//disable/store interrupts
	REG_IME = 0x00;
	int INT_FLAGS = REG_IF;
	if((REG_IF & INT_VBLANK) == INT_VBLANK)
	{
		if (!sound_length--)
		{
				//stop playback: disable the timer and DMA
				REG_TM0CNT = 0;
				REG_DMA1CNT = 0;
		}
		
		if (!bgm_length--)
		{
				//stop playback: disable the timer and DMA
				REG_TM1CNT = 0;
				REG_DMA2CNT = 0;
		}
	}
	
	REG_IF = INT_FLAGS;
	REG_IME = 0x01;
}

void init_sfx_system(void)
{

    REG_SOUNDCNT_X = SND_ENABLED;
    REG_SOUNDCNT_L = 0;
    REG_SOUNDCNT_H |= SND_OUTPUT_RATIO_100 | 
                     DSA_OUTPUT_RATIO_100 | 
                     DSA_OUTPUT_TO_BOTH |   
                     DSA_TIMER0 |           
                     DSA_FIFO_RESET;        
}

void play_sfx(const SAMPLE *pSample,int FREQUENCY)
{
    sound_length = (s32)(((pSample->length)/(16777216/FREQUENCY))*15.57);
    
    REG_TM0CNT = 0;
    REG_DMA1CNT = 0;
    REG_SOUNDCNT_H |= DSA_FIFO_RESET;   
    
    int TIMER_INTERVAL = 65536-(int)(16777216/FREQUENCY);
    REG_TM0D   = TIMER_INTERVAL;
    REG_TM0CNT = TIMER_ENABLED;
    REG_DMA1SAD = (u32)(pSample->pData);
    REG_DMA1DAD = (u32)REG_FIFO_A;
    REG_DMA1CNT = ENABLE_DMA | WORD_DMA | DMA_REPEAT | DEST_REG_SAME | DMA_SPECIAL;
}

void init_bgm_system(void)
{
    REG_SOUNDCNT_X = SND_ENABLED;
    REG_SOUNDCNT_L = 0;
    REG_SOUNDCNT_H |= SND_OUTPUT_RATIO_100 | 
                     DSB_OUTPUT_RATIO_100 | 
                     DSB_OUTPUT_TO_BOTH |   
                     DSB_TIMER1 |           
                     DSB_FIFO_RESET;        
}

void play_bgm(const SAMPLE *pSample,int FREQUENCY)
{
    bgm_length = (s32)(((pSample->length)/(16777216/FREQUENCY))*15.57);
    
    REG_TM1CNT = 0;
    REG_DMA2CNT = 0;
    REG_SOUNDCNT_H |= DSB_FIFO_RESET; 
    
    int TIMER_INTERVAL = 65536-(int)(16777216/FREQUENCY);
    REG_TM1D   = TIMER_INTERVAL;
    REG_TM1CNT = TIMER_ENABLED;
    REG_DMA2SAD = (u32)(pSample->pData);
    REG_DMA2DAD = (u32)REG_FIFO_B;
    REG_DMA2CNT = ENABLE_DMA | WORD_DMA | DMA_REPEAT | DEST_REG_SAME | DMA_SPECIAL;
}
*/

//Fast Copy via DMA
void DMAFastCopy(void *source, void *dest, unsigned int count)
{
		REG_DMA3SAD = (unsigned int)source;
		REG_DMA3DAD = (unsigned int)dest;
		REG_DMA3CNT = count|ENABLE_DMA|HALF_WORD_DMA;
}

void itoa_lpad(char *buf, int len, unsigned int n, int lpad_chr)
{
  buf[len] = 0;
  if(n <= 0) n = 0;
 
  do 
  {
    buf[--len] = (n % 10) + '0';
    n = n / 10;
    if (n<=0) n= 0;
    if (len <=0) len =0;
  } while(n && len);

  while(len)
  {
  	buf[--len] = lpad_chr;
	if (len <=0) len =0;
  }
}

//wait for the screen to stop drawing
void WaitForVsync()
{
	if (*ScanlineCounter>160)
	{
		while(*ScanlineCounter>160) {}
	}
	
	while(*ScanlineCounter<160) {}
}

void Flip(void) // flips between the back/front buffer
{
    if(REG_DISPCNT & BACKBUFFER)    //back buffer is the current buffer so we need to switch it to the font buffer
    { 
		REG_DISPCNT &= ~BACKBUFFER; //flip active buffer to front buffer by clearing back buffer bit
		VideoBuffer = BackBuffer;   //now we point our drawing buffer to the back buffer
    }
    else       //front buffer is active so switch it to backbuffer
    { 
		REG_DISPCNT |= BACKBUFFER; //flip active buffer to back buffer by setting back buffer bit
		VideoBuffer = FrontBuffer; //now we point our drawing buffer to the front buffer
    }
}

void cls(u8 c)
{
	
	for (int y=0;y<SCREEN_HEIGHT;y++)
	{	for (int x=0;x<SCREEN_WIDTH/2;x++)
		{
			VideoBuffer[y*SCREEN_WIDTH/2+x] = (c<<8)+c;
		}
	}
}

void putBG(const u16* sprite)
{
	DMAFastCopy((void *)sprite,(void*)VideoBuffer,19200);
}

void PlotPixelBG(int x,int y, unsigned short int c,unsigned short int bg)	
{ 
	unsigned short int temp;
	unsigned short int bg2=bg<<8;
	unsigned short int c2=VideoBuffer[y*SCREEN_WIDTH/2+x];
	u16* PixLoc = (u16*)(VideoBuffer+y*SCREEN_WIDTH/2+x);

	if (x<0||x>=SCREEN_WIDTH/2||y<0||y>=SCREEN_HEIGHT) return;
	if ((c&0x00ff)==bg)
	{
		if ((c&0xFF00)==bg2) return;
		temp = (c2&0x00FF)|(c&0xff00); 
		PixLoc[0] = temp;
	}
	else
	{
		if ((c&0xff00)==bg2)
		{
			temp = (c2&0xFF00)|(c&0x00ff);
			PixLoc[0] = temp;
		}
		else
		{
			PixLoc[0] = c; 
		}
	}
	return;
}

void putsprite(int x,int y, int sizex, int sizey, const u16* sprite)
{
	for (int sy=0;sy<sizey;sy++)
	{
		int sx = sizex>>1;
		DMAFastCopy((void *)(sprite+sy*sx),(void*)(VideoBuffer+(y+sy)*SCREEN_WIDTH/2+x),sx);
	}
}

void putspriteBG(int x,int y, int sizex, int sizey, unsigned short int bg, const u16* sprite)
{
	for (int sy=0;sy<sizey;sy++)
	{
		for (int sx=0;sx<sizex>>1;sx++)
		{
			PlotPixelBG(sx+x,sy+y,sprite[sy*(sizex>>1)+sx],bg);
		}
	}
}

void PutFontBG(int x,int y,int font_num,unsigned short int c,unsigned short int bg)
{
	for (int sy=0;sy<font_sizey;sy++)
	{
		for (int sx=0;sx<(font_sizex>>1);sx++)
		{
			int putfont = fontData[(font_num*(font_sizex>>1))+sy*(fonts_WIDTH>>1)+sx];
			
			if (c==0)
			{	
				PlotPixelBG(x+sx,y+sy,putfont,0x00);
			}
			else
			{
				unsigned short int font_to_put = 0;
				if (putfont&0xff00) font_to_put=c<<8;
				if (putfont&0x00ff) font_to_put+=c;
				PlotPixelBG(x+sx,y+sy,font_to_put,0x00);
			}
		}
	}
}

void PutString(int x, int y,char *buff, unsigned short c)
{
	int start_x = x;
	int fx = font_sizex/2;
	int valid_char = 1;
		
	while (*buff!='$'&&valid_char)
	{
		valid_char = 0;
		
		int font_num=0;
		
		if (*buff>='A'&&*buff<='Z')
		{
			font_num = *buff - 'A';
			valid_char = 1;
		}

		if (*buff>='a'&&*buff<='z')
		{
			font_num = *buff - 'a' + 26;
			valid_char = 1;
		}

		if (*buff>='0'&&*buff<='9')
		{
			font_num = (*buff - '0') + 52;
			valid_char = 1;
		}
	
		if ((*buff!=' ')&&(*buff!='/')) {PutFontBG(start_x,y,font_num,c,0x00); start_x+=fx; }
		if ((*buff==' ')) {start_x += fx; valid_char = 1;}
		if ((*buff=='/')) {y+=font_sizey; start_x = x; valid_char = 1;}
		buff++;
	}
}

//create an OAM variable and make it point to the address of OAM
u16* OAM = (u16*)0x7000000;

typedef struct
{
	u16 attrib_0;
	u16 attrib_1;
	u16 attrib_2;
	u16 attrib_3;
} OAMEntry, *psprite;

//create the array of sprites (128 is the maximum)
OAMEntry sprites[128];

//Copy our sprite array to OAM
void CopyOAM()
{
	DMAFastCopy((void *)sprites,(void *)OAM,512);
}

void WaitFlipCopy(void)
{
	WaitForVsync();
	Flip();
	CopyOAM();
}

//Set sprites to off screen
void InitializeSprites()
{
	u16 loop;
	for(loop = 0; loop < 128; loop++)
	{
		sprites[loop].attrib_0 = 160;  //y to > 159
		sprites[loop].attrib_1 = 240;  //x to > 239
	}
}

void setup_char_sprite(int x, int y, int char_num)
{
	sprites[1+char_num*14].attrib_0 = 0x0 | (0x2000) | y;	//setup sprite info, 256 colour, shape and y-coord
        sprites[1+char_num*14].attrib_1 = 0xC000 | x;             //size 64x64 and x-coord
        sprites[1+char_num*14].attrib_2 = 32+char_num*216+512;       //pointer to tile where sprite starts

	for (int loop=0;loop<12;loop++)
	{
		sprites[1+char_num*14+loop+1].attrib_0 = 0x0 | (0x2000) | (y+loop*8);		//setup sprite info, shape and y-coord	
		sprites[1+char_num*14+loop+1].attrib_1 = 0x0 | (64+x);				//size 8x8 and x-coord
		sprites[1+char_num*14+loop+1].attrib_2 = 32+char_num*216 + 128 + loop*2 + 512;	//pointer to tile where sprite starts
	}
	
	sprites[1+char_num*14+13].attrib_0 = 0x4000|(0x2000)|(y+64);	 //setup sprite info, shape and y-coord
	sprites[1+char_num*14+13].attrib_1 = 0xC000|(x);			 //size 64x32 and x-coord
	sprites[1+char_num*14+13].attrib_2 = 32+char_num*216+152+512; //pointer to tile where sprite starts
}

void setup_char_sprite_data(int char_num, void *sprite_data)
{
	if (char_num == 0) DMAFastCopy(sprite_data,(void*)(OAMData+8192+512),3456);
	if (char_num == 1) DMAFastCopy(sprite_data,(void*)(OAMData+8192+512+3456),3456);
}

int coord_calc(int x, int y)
{
	return ((y*8+x)*32);
}

int dig_num(int dig)
{
	return (dig * 32);	
}

const int NO_STATS = 255;
const int STATS_SQUARE = 0;
const int STATS_TRIANGLE = 1;
const int STATS_CIRCLE = 2;
const int STATS_UNITED_ATTACK = 3;

int ability_stats(char *char_ability)
{
	int return_flag = NO_STATS;
	
	if (char_ability[0] == 'C') return_flag = STATS_CIRCLE;
	if (char_ability[0] == 'S') return_flag = STATS_SQUARE;
	if (char_ability[0] == 'T') return_flag = STATS_TRIANGLE;
	if (char_ability[0] == 'N') return_flag = NO_STATS;

	return return_flag;
}

void put_char_stats(int char_num, int hp, int sp, int ability)
{
	u16* base_offs = OAMData + 8192 + 512 + 2048 + 384;
	if (char_num == 1) base_offs += (1024 + 2048 + 384);

	DMAFastCopy((void *)(blank_digData),(void *)(base_offs+coord_calc(7,1)),32);
	DMAFastCopy((void *)(blank_digData),(void *)(base_offs+coord_calc(6,1)),32);
	DMAFastCopy((void *)(blank_digData),(void *)(base_offs+coord_calc(5,1)),32);
	DMAFastCopy((void *)(blank_digData),(void *)(base_offs+coord_calc(4,1)),32);
	DMAFastCopy((void *)(blank_dig2Data),(void *)(base_offs+coord_calc(7,2)),32);
	DMAFastCopy((void *)(blank_dig2Data),(void *)(base_offs+coord_calc(6,2)),32);
	DMAFastCopy((void *)(blank_dig2Data),(void *)(base_offs+coord_calc(5,2)),32);
	DMAFastCopy((void *)(blank_dig2Data),(void *)(base_offs+coord_calc(4,2)),32);

	if (ability!=NO_STATS)
	{
		int dig_ability = 11;
		if (ability == STATS_CIRCLE) 	dig_ability = 12;
		if (ability == STATS_TRIANGLE) 	dig_ability = 13;
		if (ability == STATS_UNITED_ATTACK) dig_ability = 10;
		DMAFastCopy((void *)(card_fonts_aboveData+dig_num(dig_ability)),(void *)(base_offs+coord_calc(1,1)),32);
		DMAFastCopy((void *)(card_fonts_bottomData+dig_num(dig_ability)),(void *)(base_offs+coord_calc(1,2)),32);
	}
	else
	{
		DMAFastCopy((void *)(blank_digData),(void *)(base_offs+coord_calc(1,1)),32);
		DMAFastCopy((void *)(blank_dig2Data),(void *)(base_offs+coord_calc(1,2)),32);
	}
	
	if (sp != NO_STATS)
	{
		DMAFastCopy((void *)(card_fonts_baseData+dig_num(14)),(void *)(base_offs+coord_calc(2,3)),32);
		DMAFastCopy((void *)(card_fonts_belowData+dig_num(14)),(void *)(base_offs+coord_calc(2,2)),32);
		DMAFastCopy((void *)(card_fonts_baseData+dig_num(15)),(void *)(base_offs+coord_calc(3,3)),32);
		DMAFastCopy((void *)(card_fonts_belowData+dig_num(15)),(void *)(base_offs+coord_calc(3,2)),32);
		
		int sp_dig1 = sp/10;
		int sp_dig2 = sp - sp_dig1;
		
		if (sp_dig1)
		{
			DMAFastCopy((void *)(card_fonts_baseData+dig_num(sp_dig1)),(void *)(base_offs+coord_calc(6,3)),32);
			DMAFastCopy((void *)(card_fonts_belowData+dig_num(sp_dig1)),(void *)(base_offs+coord_calc(6,2)),32);
		}
		else
		{
			if (hp!=0) DMAFastCopy((void *)(card_fonts_bottomData+dig_num(sp_dig1)),(void *)(base_offs+coord_calc(6,2)),32);
		}
		
		DMAFastCopy((void *)(card_fonts_baseData+dig_num(sp_dig2)),(void *)(base_offs+coord_calc(7,3)),32);
		DMAFastCopy((void *)(card_fonts_belowData+dig_num(sp_dig2)),(void *)(base_offs+coord_calc(7,2)),32);
	}
	else
	{
		if (hp!=0) DMAFastCopy((void *)(card_fonts_bottomData+dig_num(0)),(void *)(base_offs + coord_calc(6,2)),32);
		DMAFastCopy((void *)(card_fonts_bottomData+dig_num(0)),(void *)(base_offs+coord_calc(7,2)),32);
		DMAFastCopy((void *)(card_fonts_bottomData+dig_num(16)),(void *)(base_offs+coord_calc(2,2)),32);
		DMAFastCopy((void *)(card_fonts_bottomData+dig_num(17)),(void *)(base_offs+coord_calc(3,2)),32);
	}

	DMAFastCopy((void *)(card_fonts_aboveData+dig_num(16)),(void *)(base_offs+coord_calc(2,1)),32);
	DMAFastCopy((void *)(card_fonts_aboveData+dig_num(17)),(void *)(base_offs+coord_calc(3,1)),32);
	
	if (hp != 0)
	{
		int base_hp = hp/100;
		int dig_1 = base_hp/10;
		int dig_2 = base_hp - (dig_1*10);

		if (dig_1)
		{
			DMAFastCopy((void *)(card_fonts_aboveData+dig_num(dig_1)),(void *)(base_offs+coord_calc(4,1)),32);
			DMAFastCopy((void *)(card_fonts_bottomData+dig_num(dig_1)),(void *)(base_offs+coord_calc(4,2)),32);
		}
		
		DMAFastCopy((void *)(card_fonts_aboveData+dig_num(dig_2)),(void *)(base_offs+coord_calc(5,1)),32);
		DMAFastCopy((void *)(card_fonts_bottomData+dig_num(dig_2)),(void *)(base_offs+coord_calc(5,2)),32);
		DMAFastCopy((void *)(card_fonts_aboveData+dig_num(0)),(void *)(base_offs+coord_calc(6,1)),32);
		DMAFastCopy((void *)(card_fonts_aboveData+dig_num(0)),(void *)(base_offs+coord_calc(7,1)),32);
	}
	else
	{
		DMAFastCopy((void *)(card_fonts_aboveData+dig_num(0)),(void *)(base_offs+coord_calc(7,1)),32);
		if (sp==NO_STATS) DMAFastCopy((void *)(blank_dig2Data),(void *)(base_offs+coord_calc(6,2)),32);
	}
}

void setup_face_sprite(int x, int y, int char_num)
{
	sprites[1+14+char_num].attrib_0 = 0x0|(0x2000)| y;	//setup sprite info, 256 colour, shape and y-coord
        sprites[1+14+char_num].attrib_1 = 0xC000| x;             //size 64x64 and x-coord
        sprites[1+14+char_num].attrib_2 = 32+216+char_num*128+512;   //pointer to tile where sprite starts
}

void setup_face_sprite_data(int char_num, void *sprite_data)
{
	if (char_num == 0) DMAFastCopy(sprite_data,(void*)(OAMData+8192+3456+512),2048);
	if (char_num == 1) DMAFastCopy(sprite_data,(void*)(OAMData+8192+3456+512+2048),2048);
}

void setup_explode_sprite(int x, int y)
{
	sprites[0].attrib_0 = 0x0 |(0x2000) | y;	//setup sprite info, 256 colour, shape and y-coord
        sprites[0].attrib_1 = 0x8000 | x;      		//size 32x32 and x-coord
        sprites[0].attrib_2 = 512;   			//pointer to tile where sprite starts
}

void setup_explode_sprite_data(void *sprite_data)
{
	DMAFastCopy(sprite_data,(void*)(OAMData+8192),512);
}

void upload_effects(void* effects[], int count, int delay_max)
{
	for (int i=0; i<count;i++)
	{
		setup_explode_sprite_data((void *)effects[i]);
		int do_nothing = 0;
		for (int delay=0;delay<delay_max;delay++) do_nothing = 1-do_nothing;
	}
}

void do_magic_effects(int x, int y, int count, int delay_max, void *magic_fx[])
{
	setup_explode_sprite(x,y);
	WaitForVsync();
	CopyOAM();
	upload_effects(magic_fx,count,delay_max);
}

void explode_xy(int x, int y)
{
	do_magic_effects(x,y,22,5000,(void **)explode_gfx);
}

void explode_card(int player, int card_num)
{
	explode_xy(104+card_num*40+8,4+(1-player)*50+9);
}

void explode(int player)
{
	if (player == 0) explode_xy(54,38); else explode_xy(158,38);

}

void defrost_fx(int x, int y)
{
	do_magic_effects(x,y,5,25000,(void **)magic_gfx);
}

void defrost(int player, int card_pos)
{
	defrost_fx(104+card_pos*40+8,4+(1-player)*50+9);
}

void firezap_fx(int x, int y)
{
	do_magic_effects(x,y,7,25000,(void **)firezapper_gfx);
}

void firezap(int player, int card_pos)
{
	firezap_fx(104+card_pos*40+8,4+(1-player)*50+9);
}

void bloodsplat_fx(int x, int y)
{
	do_magic_effects(x,y,11,15000,(void **)bloodsplat_gfx);
}

void bloodsplat(int player, int card_pos)
{
	bloodsplat_fx(104+card_pos*40+8,4+(1-player)*50+9);
}

void lightningv_fx(int x, int y)
{
	do_magic_effects(x,y,7,15000,(void **)lightning_gfx);
}

void lightningv(int player, int card_pos)
{
	lightningv_fx(104+card_pos*40+8,4+(1-player)*50+9);
}

void twinkle_fx(int x, int y)
{
	do_magic_effects(x,y,9,15000,(void **)twinkle_gfx);
}

void twinkle(int player, int card_pos)
{
	twinkle_fx(104+card_pos*40+8,4+(1-player)*50+9);
}

void FadeFace(int char_num)
{
	u16* char_data_offset_a = OAMData+8192+512+3456;
	if (char_num == 1) char_data_offset_a+=2048;

	for (int i=1;i<8;i+=2)
	{
		for (int j=0;j<32;j++) *(char_data_offset_a+i*32+j) = 0;
	}
		
	int row = 1;
	int odd = 0;
	
	while (row<9)
	{
		if (row<8)
		{
			for (int i=0;i<8;i++)
			{
				for (int j=0;j<32;j++) *(char_data_offset_a+((row-1)*8+i)*32+j) = 0;
			}
			
			for (int i=odd;i<8;i+=2)
			{
				for (int j=0;j<32;j++) *(char_data_offset_a+(row*8+i)*32+j) = 0;
			}
		}	
		else
		{
			for (int i=0;i<8;i++)
			{
				for (int j=0;j<32;j++) *(char_data_offset_a+((row-1)*8+i)*32+j)=0;
			}
		}
		
		row++;
		odd = 1-odd;
		int do_nothing = 0;
		for (int delay = 0; delay<10000; delay++) do_nothing = 1 - do_nothing;
	}
}

void FadeOut(int char_num)
{
	u16* char_data_offset_a = OAMData+8192+512;
	if (char_num == 1) char_data_offset_a+=3456;
	u16* char_data_offset_b = char_data_offset_a + 2048;
	u16* char_data_offset_c = char_data_offset_b + 384;

	for (int i=1;i<8;i+=2)
	{
		for (int j=0;j<32;j++) *(char_data_offset_a+i*32+j) = 0;
	}
		
	int row = 1;
	int odd = 0;
	
	while (row<12)
	{
		if (row<8)
		{
			for (int i=0;i<8;i++)
			{
				for (int j=0;j<32;j++)
				{
					*(char_data_offset_a+((row-1)*8+i)*32+j) = 0;
					*(char_data_offset_b+(row-1)*32+j) = 0;
				}
			}
			
			for (int i=odd;i<8;i+=2)
			{
				for (int j=0;j<32;j++)
				{
					*(char_data_offset_a+(row*8+i)*32+j) = 0;
					if (!odd) *(char_data_offset_b+row*32+j) = 0;
				}
			}
			
		}	
		else
		{
			if (row == 8)
			{
				for (int i=0;i<8;i++)
				{
					for (int j=0;j<32;j++)
					{
						*(char_data_offset_a+((row-1)*8+i)*32+j)=0;
						*(char_data_offset_b+(row-1)*32+j) = 0;
					}
				}
				
				for (int i=odd;i<8;i+=2)
				{
					for (int j=0;j<32;j++)
					{
						*(char_data_offset_c+((row-8)*8+i)*32+j) = 0;
						if (!odd) *(char_data_offset_b+row*32+j) = 0;
					}
				}
			}
			else
			{
				for (int i=0;i<8;i++)
				{
					for (int j=0;j<32;j++)
					{
						*(char_data_offset_c+((row-9)*8+i)*32+j) = 0;
						*(char_data_offset_b+(row-1)*32+j) = 0;
					}
				}
				
				for (int i=odd;i<8;i+=2)
				{
					for (int j=0;j<32;j++)
					{
						*(char_data_offset_c+((row-8)*8+i)*32+j) = 0;
						if (!odd) *(char_data_offset_b+row*32+j) = 0;
					}
				}
			}
		}
		
		row++;
		odd = 1-odd;

		int do_nothing = 0;
		for (int delay = 0; delay<10000; delay++) do_nothing = 1 - do_nothing;
	}
	
	for (int i=0;i<8;i++)
	{
		for (int j=0;j<32;j++)
		{
			*(char_data_offset_c+((row-9)*8+i)*32+j) = 0;
			*(char_data_offset_b+(row-1)*32+j) = 0;
		}
	}
}

void draw_left_combat_stats(int hp)
{
	char hp_points [] = {"HP$"};
	PutString(19,122,hp_points,0x9D);
	if (hp<60)
	{
		PutString(26,122,(char *)hpsp[hp],0x6B);
		PutString(29,122,(char *)hpsp[0],0x6B);
		PutString(32,122,(char *)hpsp[0],0x6B);
	}
	else
	{
		char max_hpsp  [] = {"MAX$"};
		PutString(26,122,max_hpsp,0x6B);
	}
}

void draw_right_combat_stats(int hp)
{
	char hp_points [] = {"HP$"};
	PutString(71,122,hp_points,0x9D);
	if (hp<60)
	{
		PutString(78,122,(char *)hpsp[hp],0x6B);
		PutString(81,122,(char *)hpsp[0],0x6B);
		PutString(84,122,(char *)hpsp[0],0x6B);
	}
	else
	{
		char max_hpsp  [] = {"MAX$"};
		PutString(78,122,max_hpsp,0x6B);
	}
}

const int START_ATTACK	= 0;
const int RESOLVE_ATTACK = 1;
int card_1hp = 0;
int card_2hp = 0;
int card_bhp = 0;
int card_1_type = 0;
int card_2_type = 0;
int card_b_type = 0;
int card_1_num = 0;
int card_2_num = 0;
int card_b_num = 0;
int opp_hp = 0;

int blocker_hp = 0;
void animated_card_battle(int player,int mode, int attacker_1, int attacker_2, int blocker)
{
	int opponent = 1 - player;
	int attackers[2];
	attackers[0] = attacker_1;
	attackers[1] = attacker_2;

	int intro_count = 0;
	int left_stats = 0;
	int right_stats = 0;
		
	if (mode == START_ATTACK)
	{

		int cx1 = 0;
		int cx2 = 0;
		int cbx	= 0;
		
		if (!attacker_1) return;
		
		InitializeSprites();
		putBG(hgreen[0]);
		WaitFlipCopy();
		
		if (player == 0) setup_char_sprite(0,4,0); else setup_char_sprite(168,4,0); 
		if (card_1_type == SNK_CARD)
		{
			setup_char_sprite_data(0,(void*)SNK_char_offs[card_1_num]);	//copies sprite image data to OAM Data Mem
		}
		
		if (card_1_type == CAPCOM_CARD)
		{
			setup_char_sprite_data(0,(void*)Capcom_char_offs[card_1_num]);	//copies sprite image data to OAM Data Mem
		}

		put_char_stats(0,card_1hp,NO_STATS,NO_STATS);
		
		if (attackers[1])
		{
			if (player == 0) setup_char_sprite(0,4,1); else setup_char_sprite(168,4,1); 
			if (card_2_type == SNK_CARD)
			{
				setup_char_sprite_data(1,(void*)SNK_char_offs[card_2_num]);	//copies sprite image data to OAM Data Mem
			}

			if (card_2_type == CAPCOM_CARD)
			{
				setup_char_sprite_data(1,(void*)Capcom_char_offs[card_2_num]);	//copies sprite image data to OAM Data Mem
			}
			
			put_char_stats(1,card_2hp,NO_STATS,NO_STATS);
			setup_char_sprite(240,4,0);
			
			cx2 = 0;
			cx1 = 0;
							
			if (player!=0) cx2 = 168;
			if (player == 0)
			{
				while (cx2<32)
				{
					cx2++;
					WaitForVsync();
					setup_char_sprite(cx2,4,1);
					CopyOAM();
				}
					
				WaitForVsync();
				setup_char_sprite(32,4,1);
				CopyOAM();
			}
			else
			{
				while (cx2>136)
				{
					cx2--;
					WaitForVsync();
					setup_char_sprite(cx2,4,1);
					CopyOAM();
				}
					
				WaitForVsync();
				setup_char_sprite(136,4,1);
				CopyOAM();
			}

			if (player!=0) cx1 = 168;
			if (player == 0)
			{
				while (cx1<32)
				{
					cx1++;
					WaitForVsync();
					setup_char_sprite(cx1,4,0);
					CopyOAM();
				}
					
				WaitForVsync();
				setup_char_sprite(32,4,0);
				CopyOAM();
			}
			else
			{
				while (cx1>136)
				{
					cx1--;
					WaitForVsync();
					setup_char_sprite(cx1,4,0);
					CopyOAM();
				}
					
				WaitForVsync();
				setup_char_sprite(136,4,0);
				CopyOAM();
			}
			
			InitializeSprites();
			
			if (player == 0) setup_char_sprite(32,4,0); else setup_char_sprite(136,4,0); 
			if (card_1_type == SNK_CARD)
			{
				setup_char_sprite_data(0,(void*)SNK_char_offs[card_1_num]);	//copies sprite image data to OAM Data Mem
			}
			
			if (card_1_type == CAPCOM_CARD)
			{
				setup_char_sprite_data(0,(void*)Capcom_char_offs[card_1_num]);	//copies sprite image data to OAM Data Mem
			}

			put_char_stats(0,card_1hp+card_2hp,NO_STATS,STATS_UNITED_ATTACK);
			
			if (blocker==255||!blocker)
			{
				if (opponent == 1) setup_face_sprite(176,22,0); else setup_face_sprite(0,22,0);
				setup_face_sprite_data(0,(void*)player_faces[opponent]);
				
				cbx = 176;
				
				if (opponent != 1) cbx = 0;
				if (opponent == 1)
				{
					while (cbx>142)
					{
						cbx--;
						WaitForVsync();
						setup_face_sprite(cbx,22,0);
						CopyOAM();
					}
					
					WaitForVsync();
					setup_face_sprite(142,22,0);
					CopyOAM();
				}
				else
				{
					while (cbx<38)
					{
						cbx++;
						WaitForVsync();
						setup_face_sprite(cbx,22,0);
						CopyOAM();
					}

					WaitForVsync();
					setup_face_sprite(38,22,0);
					CopyOAM();
				}	
			}
			else
			{
				if (opponent == 0) setup_char_sprite(0,4,1); else setup_char_sprite(168,4,1); 
				
				if (card_b_type == SNK_CARD)
				{
					setup_char_sprite_data(1,(void*)SNK_char_offs[card_b_num]);	//copies sprite image data to OAM Data Mem
				}

				if (card_b_type == CAPCOM_CARD)
				{
					setup_char_sprite_data(1,(void*)Capcom_char_offs[card_b_num]);	//copies sprite image data to OAM Data Mem
				}

				put_char_stats(1,card_bhp,NO_STATS,NO_STATS);

				cx1 = 32;
				cbx = 168;
				
				if (player!=0) cx1 = 136;
				if (opponent!=1) cbx = 0;
				
				if (player == 0)
				{
					while (cbx>136)
					{
						cbx--;
						WaitForVsync();
						setup_char_sprite(cx1,4,0);
						setup_char_sprite(cbx,4,1);
						CopyOAM();
					}
					
					WaitForVsync();
					setup_char_sprite(cx1,4,0);
					setup_char_sprite(136,4,1);
					CopyOAM();
				}
				else
				{
					while (cbx<32)
					{
						cbx++;
						WaitForVsync();
						setup_char_sprite(cx1,4,0);
						setup_char_sprite(cbx,4,1);
						CopyOAM();
					}
					
					WaitForVsync();
					setup_char_sprite(cx1,4,0);
					setup_char_sprite(32,4,1);
					CopyOAM();
				}
			}
		}
		else
		{
			if (blocker==255||!blocker)
			{
				if (opponent == 1) setup_face_sprite(176,22,0); else setup_face_sprite(0,22,0);
				setup_face_sprite_data(0,(void*)player_faces[opponent]);
				
				cbx = 176;
				cx1 = 0;
				
				if (player!=0) cx1 = 168;
				if (opponent != 1) cbx = 0;
				
				if (opponent == 1)
				{
					while (cbx>142)
					{
						cbx--;
						cx1++;
						WaitForVsync();
						setup_face_sprite(cbx,22,0);
						setup_char_sprite(cx1,4,0);
						CopyOAM();
					}
					
					WaitForVsync();
					setup_face_sprite(142,22,0);
					setup_char_sprite(32,4,0);
					CopyOAM();
				}
				else
				{
					while (cbx<38)
					{
						cbx++;
						cx1--;
						WaitForVsync();
						setup_face_sprite(cbx,22,0);
						setup_char_sprite(cx1,4,0);
						CopyOAM();
					}

					WaitForVsync();
					setup_face_sprite(38,22,0);
					setup_char_sprite(136,4,0);
					CopyOAM();
				}	
			}
			else
			{
				if (opponent == 0) setup_char_sprite(0,4,1); else setup_char_sprite(168,4,1); 
				if (card_b_type == SNK_CARD)
				{
					setup_char_sprite_data(1,(void*)SNK_char_offs[card_b_num]);	//copies sprite image data to OAM Data Mem
				}

				if (card_b_type == CAPCOM_CARD)
				{
					setup_char_sprite_data(1,(void*)Capcom_char_offs[card_b_num]);	//copies sprite image data to OAM Data Mem
				}

				put_char_stats(1,card_bhp,NO_STATS,NO_STATS);

				cx1 = 0;
				cbx = 168;
				
				if (player!=0) cx1 = 168;
				if (opponent!=1) cbx = 0;
				
				if (player == 0)
				{
					while (cx1<32&&cbx>136)
					{
						cbx--;
						cx1++;
						WaitForVsync();
						setup_char_sprite(cx1,4,0);
						setup_char_sprite(cbx,4,1);
						CopyOAM();
					}
					
					WaitForVsync();
					setup_char_sprite(32,4,0);
					setup_char_sprite(136,4,1);
					CopyOAM();
				}
				else
				{
					while (cx1>136&&cbx<32)
					{
						cbx++;
						cx1--;
						WaitForVsync();
						setup_char_sprite(cx1,4,0);
						setup_char_sprite(cbx,4,1);
						CopyOAM();
					}
					
					WaitForVsync();
					setup_char_sprite(136,4,0);
					setup_char_sprite(32,4,1);
					CopyOAM();
				}
			}
		}

		putBG(hgreen[intro_count]);

		if (player == 0)
		{
			int hp = card_1hp;
			if (attackers[1]) hp += card_2hp;
			if (attackers[1]) put_char_stats(0,hp,NO_STATS,STATS_UNITED_ATTACK); else put_char_stats(0,hp,NO_STATS,NO_STATS);
			hp = hp/100;
			left_stats = hp;

			hp = card_bhp/100;
			right_stats = hp;
			if (blocker==255||!blocker)
			{
				putsprite(62,108,96,45,bottom_leftData);
				draw_right_combat_stats(hp);
			}
			else
			{
				put_char_stats(1,card_bhp,NO_STATS,NO_STATS);
			}
		}
		else
		{
			int hp = card_1hp;
			if (attackers[1]) hp += card_2hp;
			if (attackers[1]) put_char_stats(0,hp,NO_STATS,STATS_UNITED_ATTACK); else put_char_stats(0,hp,NO_STATS,NO_STATS);
			hp = hp/100;
			right_stats = hp;
			
			hp = card_bhp/100;
			left_stats = hp;
			if (blocker==255||!blocker)
			{
				putsprite(10,108,96,45,bottom_leftData);
				draw_left_combat_stats(hp);
			}
			else
			{
				put_char_stats(1,card_bhp,NO_STATS,NO_STATS);
			}
		}
		
		WaitForVsync();
		Flip();
	}

	int delay_max = 10000;
	int do_nothing = 0;
	int key_pressed= 0;
	
	while (!key_pressed)
	{
		putBG(hgreen[intro_count]);

		if (player == 0)
		{
			if (blocker==255||!blocker)
			{
				putsprite(62,108,96,45,bottom_leftData);
				draw_right_combat_stats(right_stats);
			}
		}
		else
		{
			if (blocker==255||!blocker)
			{
				putsprite(10,108,96,45,bottom_leftData);
				draw_left_combat_stats(left_stats);
			}
		}
		
		WaitForVsync();
		Flip();
		if (intro_count == 13) intro_count = 0; else intro_count++;
		for (int i=0; i<delay_max; i++) { do_nothing = 1 - do_nothing; }
		if (!(*KEYS&KEY_A)||!(*KEYS&KEY_B)||!(*KEYS&KEY_L)||!(*KEYS&KEY_R)||!(*KEYS&KEY_START)||!(*KEYS&KEY_SELECT))
		{
			key_pressed = 1;
			mode = RESOLVE_ATTACK;
		}
	}

	if (mode == RESOLVE_ATTACK)
	{
		putBG(hgreen[intro_count]);

		if (blocker==255||!blocker)
		{
			explode(opponent);
			int oppnt_hp = 0;
						
			if (player == 0)
			{
				int hp = card_1hp;		
				if (attackers[1]) hp += card_2hp;
				if (attackers[1]) put_char_stats(0,hp,NO_STATS,STATS_UNITED_ATTACK); else put_char_stats(0,hp,NO_STATS,NO_STATS);
				hp = hp/100;
			
				hp = card_bhp - card_1hp;
				if (attackers[1]) hp -= card_2hp;
				if (hp<0) hp = 0;
				oppnt_hp = hp;
				hp = hp/100;
				putsprite(62,108,96,45,bottom_leftData);
				draw_right_combat_stats(hp);
			}
			else
			{
				int hp = card_1hp;		
				if (attackers[1]) hp += card_2hp;
				if (attackers[1]) put_char_stats(0,hp,NO_STATS,STATS_UNITED_ATTACK); else put_char_stats(0,hp,NO_STATS,NO_STATS);
				hp = hp/100;
			
				hp = card_bhp - card_1hp;
				if (attackers[1]) hp -= card_2hp;
				if (hp<0) hp = 0;
				oppnt_hp = hp;
				hp = hp/100;
				putsprite(10,108,96,45,bottom_leftData);
				draw_left_combat_stats(hp);
			}

			WaitForVsync();
			Flip();
			if (!oppnt_hp) FadeFace(0);
			for (int i=0; i<delay_max*3; i++) { do_nothing = 1 - do_nothing; }
		}
		else
		{
			if (player == 0)
			{
				int hp = card_1hp;		
				if (attackers[1]) hp += card_2hp;
				hp = hp - card_bhp;
				if (hp<0) hp = 0;
				if (attackers[1]) put_char_stats(0,hp,NO_STATS,STATS_UNITED_ATTACK); else put_char_stats(0,hp,NO_STATS,NO_STATS);
				hp = card_bhp - card_1hp;
				if (attackers[1]) hp -= card_2hp;
				if (hp<0) hp = 0;
				put_char_stats(1,hp,NO_STATS,NO_STATS);
			}
			else
			{
				int hp = card_1hp;		
				if (attackers[1]) hp += card_2hp;
				hp = hp - card_bhp;
				if (hp<0) hp = 0;
				if (attackers[1]) put_char_stats(0,hp,NO_STATS,STATS_UNITED_ATTACK); else put_char_stats(0,hp,NO_STATS,NO_STATS);
			
				hp = card_bhp - card_1hp;
				if (attackers[1]) hp -= card_2hp;
				if (hp<0) hp = 0;
				put_char_stats(1,hp,NO_STATS,NO_STATS);
			}
		
			WaitForVsync();
			Flip();

			if (attackers[1])
			{
				if ((card_1hp+card_2hp>card_bhp)&&!united_invincible(opponent,blocker-1))
				{
					int oppnt_hp = 0;
					explode(opponent);
					FadeOut(1);
							
					putBG(hgreen[intro_count]);

					// Additional Damage
					InitializeSprites();
					if (player == 0) setup_char_sprite(32,4,0); else setup_char_sprite(136,4,0); 
					if (card_1_type == SNK_CARD)
					{
						setup_char_sprite_data(0,(void*)SNK_char_offs[card_1_num]);	//copies sprite image data to OAM Data Mem
					}

					if (card_1_type == CAPCOM_CARD)
					{
						setup_char_sprite_data(0,(void*)Capcom_char_offs[card_1_num]);	//copies sprite image data to OAM Data Mem
					}

					if (opponent == 1) setup_face_sprite(142,22,0); else setup_face_sprite(38,22,0);
					setup_face_sprite_data(0,(void*)player_faces[opponent]);

					if (player == 0)
					{
						int hp = 0;
						hp = card_1hp+card_2hp-card_bhp;		
						put_char_stats(0,hp,NO_STATS,STATS_UNITED_ATTACK);
						//put_char_stats(1,hp,NO_STATS,NO_STATS);
						hp = opp_hp/100;
						putsprite(62,108,96,45,bottom_leftData);
						draw_right_combat_stats(hp);
						
						WaitFlipCopy();
						
						key_pressed = 0;
						while (!key_pressed)
						{
							if (!(*KEYS&KEY_A))
							{
								while (!(*KEYS&KEY_A));
								key_pressed = 1;
							}	
						}

						explode(opponent);					
						putBG(hgreen[intro_count]);

						hp = card_1hp+card_2hp-card_bhp;		
						put_char_stats(0,hp,NO_STATS,STATS_UNITED_ATTACK);
			
						hp = opp_hp-(card_1hp+card_2hp-card_bhp);
						if (hp<0) hp = 0;
						oppnt_hp = hp;
						hp = hp/100;
						putsprite(62,108,96,45,bottom_leftData);
						draw_right_combat_stats(hp);

						WaitFlipCopy();
						
						if (!oppnt_hp) FadeFace(0);
						for (int i=0; i<delay_max*3; i++) { do_nothing = 1 - do_nothing; }
						for (int i=0; i<delay_max*3; i++) { do_nothing = 1 - do_nothing; }
					}
					else
					{
						int hp = card_1hp+card_2hp;		
						hp = hp - card_bhp;
						put_char_stats(0,hp,NO_STATS,STATS_UNITED_ATTACK);

						hp = opp_hp/100;
						putsprite(10,108,96,45,bottom_leftData);
						draw_left_combat_stats(hp);

						WaitFlipCopy();

						key_pressed = 0;
						while (!key_pressed)
						{
							if (!(*KEYS&KEY_A))
							{
								while (!(*KEYS&KEY_A));
								key_pressed = 1;
							}	
						}

						explode(opponent);					
						putBG(hgreen[intro_count]);

						hp = (card_1hp+card_2hp-card_bhp);
						put_char_stats(0,hp,NO_STATS,STATS_UNITED_ATTACK);
			
						hp = opp_hp - (card_1hp+card_2hp-card_bhp);
						if (hp<0) hp = 0;
						oppnt_hp = hp;
						hp = hp/100;
						putsprite(10,108,96,45,bottom_leftData);
						draw_left_combat_stats(hp);

						WaitFlipCopy();

						if (!oppnt_hp) FadeFace(0);
						for (int i=0; i<delay_max*3; i++) { do_nothing = 1 - do_nothing; }
						for (int i=0; i<delay_max*3; i++) { do_nothing = 1 - do_nothing; }
					}
				}
				else
				{
					int outcome_hp = (card_1hp+card_2hp)-card_bhp;
					if (outcome_hp<=0) outcome_hp = 0;
					put_char_stats(0,outcome_hp,NO_STATS,STATS_UNITED_ATTACK);
					
					if (!outcome_hp)
					{
						explode(player);					
						FadeOut(0);
					}
					
					if (card_bhp <= card_1hp+card_2hp)
					{
						put_char_stats(1,0,NO_STATS,NO_STATS);
						explode(opponent);
						FadeOut(1);
					}
					else
					{
						put_char_stats(1,card_bhp-(card_1hp+card_2hp),NO_STATS,NO_STATS);
					}
				}
			}	
			else
			{
				if (card_1hp>card_bhp)
				{
					explode(opponent);
					FadeOut(1);
				}
				else
				{
					explode(player);
					FadeOut(0);
					
					if (card_bhp == card_1hp)
					{
						explode(opponent);
						FadeOut(1);
					}
				}
			}
		}
	}
}

void print_right_pane_deck(int player, int card)
{
	putsprite(54,4,120,100,right_paneData);
	
	if (card_types[player][card] == SNK_CARD)
	{
		char *card_ability;

		char card_set1[] = {"SNK$"};
		int card_num = combat_decks[player][card];
		PutString(62,26,(char *)num_names[card_num],0x6B);
		PutString(62,18,card_set1,0x9D);
		
		int hp = SNK_chars_hp[card_num]/100;
		int sp = SNK_chars_sp[card_num];

		card_ability = (char *)SNK_ABILITIES[card_num+1];
		int stats_ability = ability_stats(card_ability);
		setup_char_sprite_data(0,(void*)SNK_char_offs[combat_decks[player][card]]);	//copies sprite image data to OAM Data Mem
		put_char_stats(0,hp*100,sp,stats_ability);
		setup_char_sprite(152,4,0);
	}

	if (card_types[player][card] == CAPCOM_CARD)
	{
		char *card_ability;
		
		char card_set2[] = {"CAPCOM$"};
		int card_num = combat_decks[player][card];

		PutString(62,26,(char *)num_names[card_num],0x6B);
		PutString(57,18,card_set2,0x9D);
					
		int hp = Capcom_chars_hp[card_num]/100;
		int sp = Capcom_chars_sp[card_num];

		card_ability = (char *)CAPCOM_ABILITIES[card_num+1];
		int stats_ability = ability_stats(card_ability);
			
		setup_char_sprite_data(0,(void*)Capcom_char_offs[combat_decks[player][card]]);	//copies sprite image data to OAM Data Mem
		put_char_stats(0,hp*100,sp,stats_ability);
		setup_char_sprite(152,4,0);
	}
}

void print_right_pane_discard_pile(int player, int card)
{
	putsprite(54,4,120,100,right_paneData);
	
	if (discard_pile_card_type[player][card] == SNK_CARD)
	{
		char *card_ability;

		char card_set1[] = {"SNK$"};
		int card_num = discard_pile_cards[player][card];
		PutString(62,26,(char *)num_names[card_num],0x6B);
		PutString(62,18,card_set1,0x9D);
		
		int hp = SNK_chars_hp[card_num]/100;
		int sp = SNK_chars_sp[card_num];

		card_ability = (char *)SNK_ABILITIES[card_num+1];
		int stats_ability = ability_stats(card_ability);

		setup_char_sprite_data(0,(void*)SNK_char_offs[card_num]);	//copies sprite image data to OAM Data Mem
		put_char_stats(0,hp*100,sp,stats_ability);
		setup_char_sprite(152,4,0);
	}

	if (discard_pile_card_type[player][card] == CAPCOM_CARD)
	{
		char *card_ability;
		
		char card_set2[] = {"CAPCOM$"};
		int card_num = discard_pile_cards[player][card];

		PutString(62,26,(char *)num_names[card_num],0x6B);
		PutString(57,18,card_set2,0x9D);
					
		int hp = Capcom_chars_hp[card_num]/100;
		int sp = Capcom_chars_sp[card_num];

		card_ability = (char *)CAPCOM_ABILITIES[card_num+1];
		int stats_ability = ability_stats(card_ability);
			
		setup_char_sprite_data(0,(void*)Capcom_char_offs[card_num]);	//copies sprite image data to OAM Data Mem
		put_char_stats(0,hp*100,sp,stats_ability);
		setup_char_sprite(152,4,0);
	}
}

void print_right_pane(int player, int card)
{
	putsprite(54,4,120,100,right_paneData);
	
	if (hand_card_type[player][card] == SNK_CARD)
	{
		char *card_ability;
		
		char card_set1[] = {"SNK$"};
		int card_num = hand_cards[player][card];
		PutString(62,26,(char *)num_names[card_num],0x6B);
		PutString(62,18,card_set1,0x9D);
		
		int hp = SNK_chars_hp[card_num]/100;
		int sp = SNK_chars_sp[card_num];
		
		card_ability = (char *)SNK_ABILITIES[card_num+1];
		int stats_ability = ability_stats(card_ability);

		setup_char_sprite_data(0,(void*)SNK_char_offs[card_num]);	//copies sprite image data to OAM Data Mem
		put_char_stats(0,hp*100,sp,stats_ability);
		setup_char_sprite(152,4,0);
	}

	if (hand_card_type[player][card] == CAPCOM_CARD)
	{
		char *card_ability;

		char card_set2[] = {"CAPCOM$"};
		int card_num = hand_cards[player][card];

		PutString(62,26,(char *)num_names[card_num],0x6B);
		PutString(57,18,card_set2,0x9D);
					
		int hp = Capcom_chars_hp[card_num]/100;
		int sp = Capcom_chars_sp[card_num];

		card_ability = (char *)CAPCOM_ABILITIES[card_num+1];
		int stats_ability = ability_stats(card_ability);
					
		setup_char_sprite_data(0,(void*)Capcom_char_offs[card_num]);	//copies sprite image data to OAM Data Mem
		put_char_stats(0,hp*100,sp,stats_ability);
		setup_char_sprite(152,4,0);
	}
}

const int PRINT_CARD_IN_HAND  = 0;
const int PRINT_CARD_IN_PLAY  = 1;

void print_area_chars(int player, int pos, int mode)
{
	putsprite(5,4,96,100,left_paneData);
	putsprite(5,108,96,45,bottom_leftData);
	
	int stats_ability = NO_STATS;
	char *card_ability;
	
	int type = 0;
		
	if (mode == PRINT_CARD_IN_PLAY)
	{
		type = combat_area_card_type[player][pos];
	}
	else
	{
		type = hand_card_type[player][pos];
	}
	
	if (type == SNK_CARD)
	{
		int hp;

		if (mode == PRINT_CARD_IN_PLAY)
		{
			card_ability = (char *)SNK_ABILITIES[combat_area[player][pos]+1];
			stats_ability = ability_stats(card_ability);
			hp = get_hp(player,pos)/100;
			setup_char_sprite_data(0,(void*)SNK_char_offs[combat_area[player][pos]]);	//copies sprite image data to OAM Data Mem
			put_char_stats(0,hp*100,NO_STATS,stats_ability);
		}
		else
		{
			card_ability = (char *)SNK_ABILITIES[hand_cards[player][pos]+1];
			stats_ability = ability_stats(card_ability);
			hp = SNK_chars_hp[hand_cards[player][pos]]/100;
			setup_char_sprite_data(0,(void*)SNK_char_offs[hand_cards[player][pos]]);	//copies sprite image data to OAM Data Mem
			put_char_stats(0,hp*100,NO_STATS,stats_ability);
		}
		
		setup_char_sprite(22,4,0);
	}

	if (type == CAPCOM_CARD)
	{
		int hp;
		
		if (mode == PRINT_CARD_IN_PLAY)
		{
			card_ability = (char *)CAPCOM_ABILITIES[combat_area[player][pos]+1];
			stats_ability = ability_stats(card_ability);
			hp = get_hp(player,pos)/100;
			setup_char_sprite_data(0,(void*)Capcom_char_offs[combat_area[player][pos]]);	//copies sprite image data to OAM Data Mem
			put_char_stats(0,hp*100,NO_STATS,stats_ability);
		}
		else
		{
			card_ability = (char *)CAPCOM_ABILITIES[hand_cards[player][pos]+1];
			stats_ability = ability_stats(card_ability);
			hp = Capcom_chars_hp[hand_cards[player][pos]]/100;
			setup_char_sprite_data(0,(void*)Capcom_char_offs[hand_cards[player][pos]]);	//copies sprite image data to OAM Data Mem
			put_char_stats(0,hp*100,NO_STATS,stats_ability);
		}
		
		setup_char_sprite(22,4,0);
	}
}

void print_pile_char(int player, int pos, int pile_type)
{
	putsprite(5,4,96,100,left_paneData);
	putsprite(5,108,96,45,bottom_leftData);
	
	int stats_ability = NO_STATS;
	char *card_ability;
	
	int type = 0;
		
	if (pile_type == DISCARD_PILE_TYPE)
	{
		type = discard_pile_card_type[player][pos];
	}
	else
	{
		type = card_types[player][pos];
	}
	
	if (pile_type == SNK_CARD)
	{
		int hp;

		if (pile_type == DISCARD_PILE_TYPE)
		{
			card_ability = (char *)SNK_ABILITIES[discard_pile_cards[player][pos]+1];
			stats_ability = ability_stats(card_ability);
			hp = SNK_chars_hp[discard_pile_cards[player][pos]]/100;
			setup_char_sprite_data(0,(void*)SNK_char_offs[discard_pile_cards[player][pos]]);	//copies sprite image data to OAM Data Mem
			put_char_stats(0,hp*100,NO_STATS,stats_ability);
		}
		else
		{
			card_ability = (char *)SNK_ABILITIES[combat_decks[player][pos]+1];
			stats_ability = ability_stats(card_ability);
			hp = SNK_chars_hp[combat_decks[player][pos]]/100;
			setup_char_sprite_data(0,(void*)SNK_char_offs[combat_decks[player][pos]]);	//copies sprite image data to OAM Data Mem
			put_char_stats(0,hp*100,NO_STATS,stats_ability);
		}
		
		setup_char_sprite(22,4,0);
	}

	if (type == CAPCOM_CARD)
	{
		int hp;
		
		if (pile_type == DISCARD_PILE_TYPE)
		{
			card_ability = (char *)CAPCOM_ABILITIES[discard_pile_cards[player][pos]+1];
			stats_ability = ability_stats(card_ability);
			hp = Capcom_chars_hp[discard_pile_cards[player][pos]]/100;
			setup_char_sprite_data(0,(void*)Capcom_char_offs[discard_pile_cards[player][pos]]);	//copies sprite image data to OAM Data Mem
			put_char_stats(0,hp*100,NO_STATS,stats_ability);
		}
		else
		{
			card_ability = (char *)CAPCOM_ABILITIES[combat_decks[player][pos]+1];
			stats_ability = ability_stats(card_ability);
			hp = Capcom_chars_hp[combat_decks[player][pos]]/100;
			setup_char_sprite_data(0,(void*)Capcom_char_offs[combat_decks[player][pos]]);	//copies sprite image data to OAM Data Mem
			put_char_stats(0,hp*100,NO_STATS,stats_ability);
		}
		
		setup_char_sprite(22,4,0);
	}
}

void print_left_pane(int player, int card)
{
	putsprite(5,4,96,100,left_paneData);
	putsprite(5,108,96,45,bottom_leftData);

	print_area_chars(player, card, PRINT_CARD_IN_HAND);
	int card_num = hand_cards[player][card];

	if (hand_card_type[player][card] == SNK_CARD)
	{
		int stats_ability = NO_STATS;
		char *card_ability;
		card_ability = (char *)SNK_ABILITIES[hand_cards[player][card]+1];
		stats_ability = ability_stats(card_ability);
		
		int sp = SNK_chars_sp[card_num];
		int hp = SNK_chars_hp[card_num];
		put_char_stats(0,hp,sp,stats_ability);
	}

	if (hand_card_type[player][card] == CAPCOM_CARD)
	{
		int stats_ability = NO_STATS;
		char *card_ability;
		
		card_ability = (char *)CAPCOM_ABILITIES[hand_cards[player][card]+1];
		stats_ability = ability_stats(card_ability);

		int sp = Capcom_chars_sp[card_num];
		int hp = Capcom_chars_hp[card_num];
		put_char_stats(0,hp,sp,stats_ability);
	}
}

const int STATUS_NORMAL = 0;
const int STATUS_BLOCKING = 1;
const int STATUS_ATTACKING = 2;

void view_combat_area (int player, int status, int pos_1, int pos_2, int pos_3, int united_pos1, int united_pos2)
{
	if (status == STATUS_NORMAL)
	{
		for (int i=0; i <3; i++)
		{
			if (combat_area[player][i] == 255)
			{
				putsprite(54+i*20+4,4+(1-player)*50+7,24,36,empty_area);
			}
			else
			{
				if (combat_ready[player][i]==1) putsprite(54+i*20+4,4+(1-player)*50+7,24,36,card_small); else putsprite(54+i*20+4,4+(1-player)*50+7,24,36,card_new);
				if (freeze_status[player][i]) putsprite(54+i*20+4,4+(1-player)*50+7,24,36,card_frozen);
			}
		}		
	}
	
	if (status == STATUS_ATTACKING)
	{
		for (int i=0; i <3; i++)
		{
			putsprite(54+i*20+4,4+(1-player)*50+7,24,36,empty_area);
			
			if ((i+1)!=pos_1&&(i+1)!=pos_2&&(i+1)!=pos_3&&(i+1)!=united_pos1&&(i+1)!=united_pos2)
			{
				if (combat_area[player][i] != 255)
				{
					if (combat_ready[player][i]==1) putsprite(54+i*20+4,4+(1-player)*50+7,24,36,card_small); else putsprite(54+i*20+4,4+(1-player)*50+7,24,36,card_new);
					if (freeze_status[player][i]) putsprite(54+i*20+4,4+(1-player)*50+7,24,36,card_frozen);
				}
			}
			else
			{
				int y_factor = -7;
				
				if (player == COMPUTER_PLAYER) y_factor = -y_factor;
				
				if ((i+1) == pos_1||(i+1) == pos_2||(i+1) == pos_3) putsprite(54+i*20+4,4+(1-player)*50+7+y_factor,24,36,card_attacking);
				if ((i+1) == united_pos1) putsprite(54+i*20+4,4+(1-player)*50+7+y_factor,24,36,card_attacking_1);
				if ((i+1) == united_pos2) putsprite(54+i*20+4,4+(1-player)*50+7+y_factor,24,36,card_attacking_2);
			}
		}
	}
	
	if (status == STATUS_BLOCKING)
	{
		for (int i=0; i <3; i++)
		{
			putsprite(54+i*20+4,4+(1-player)*50+7,24,36,empty_area);
			
			if ((i+1)!=pos_1&&(i+1)!=pos_2&&(i+1)!=pos_3)
			{
				if (combat_area[player][i] != 255)
				{
					if (combat_ready[player][i]==1) putsprite(54+i*20+4,4+(1-player)*50+7,24,36,card_small); else putsprite(54+i*20+4,4+(1-player)*50+7,24,36,card_new);
					if (freeze_status[player][i]) putsprite(54+i*20+4,4+(1-player)*50+7,24,36,card_frozen);
				}
			}
			else
			{
				int y_factor = -7;			
				if (player == COMPUTER_PLAYER) y_factor = -y_factor;
				if ((i+1) == pos_1||(i+1) == pos_2||(i+1) == pos_3)
				{
					if (combat_ready[player][i]==1) putsprite(54+i*20+4,4+(1-player)*50+7+y_factor,24,36,card_small); else putsprite(54+i*20+4,4+(1-player)*50+7+y_factor,24,36,card_new);
				}
			}
		}
	}
}

const char player_name_01 [] = {"SHIN$"};
const char player_name_02 [] = {"CAP$"};
const char *player_names [] = {player_name_01,player_name_02};

void view_stats_area()
{
	putsprite(54,108,120,45,bottom_rightData);
	
	char hp_points [] = {"HP$"};
	char sp_points [] = {"SP$"};
	char max_hpsp  [] = {"MAX$"};
		
	for (int i=0; i<2; i++)
	{
		int hp = player_hp[i]/100;
		int sp = player_sp[i];
					
		PutString(62,114+i*16,(char *)player_names[i],0xF3);
		PutString(62,114+i*16+8,hp_points,0x9D);
		if (hp<60)
		{
			PutString(70,114+i*16+8,(char *)hpsp[hp],0x6B); 
			PutString(73,114+i*16+8,(char *)hpsp[0],0x6B);
			PutString(76,114+i*16+8,(char *)hpsp[0],0x6B);
		}
		else
		{
			PutString(70,114+i*16+8,max_hpsp,0x6B);
		}
		
		PutString(86,114+i*16+8,sp_points,0x9D);
		if (sp<60) PutString(92,114+i*16+8,(char *)hpsp[sp],0x6B); else PutString(92,114+i*16+8,max_hpsp,0x6B);
	}
}

void disp_combat_area(int player, int current_choice)
{
		InitializeSprites();
		putBG(back_offs[0]);
		putsprite(54,4,120,100,right_paneData);

		view_combat_area (HUMAN_PLAYER,STATUS_NORMAL,0,0,0,0,0);
		view_combat_area (COMPUTER_PLAYER,STATUS_NORMAL,0,0,0,0,0);
		view_stats_area();
		
		if (current_choice != 255)
		{
			putsprite(5,108,96,45,bottom_leftData);
			print_area_chars(player,current_choice,PRINT_CARD_IN_PLAY);
			putspriteBG(56+current_choice*20+6,1+(1-player)*50,18,28,0,selectData);
		}
		else
		{
			putsprite(5,4,96,100,left_paneData);
		}
}

void print_menu(int x, int y, int num_choices, int current_choice, int c, char* choices[])
{
	for (int i=0; i<num_choices; i++)
	{
		PutString(x,y+i*8,choices[i],c);
		if (current_choice == i) putspriteBG(x-15,(y-1)+i*8,28,18,0,small_handData);
	}	
}


const int SELECT_ANY = 0;
const int SELECT_OWN = 1;
const int SELECT_ENEMY = 2;
const int AI_SELECT_OWN = 3;
const int AI_SELECT_WEAK = 4;
const int AI_SELECT_STRONG = 5;
const int AI_SELECT_FROZEN = 6;
const int AI_SELECT_BACKUPS = 7;
const int AI_SELECT_PLAYER = 7;
const int AI_SELECT_ENEMY_WEAK = 8;
const int AI_SELECT_ENEMY_STRONG = 9;
const int AI_SELECT_ENEMY_WITH_BACKUPS = 10;
const int AI_SELECT_ENEMY_FROZEN = 11;

int select_char(int player, char *message, int SELECT_MODE)
{
	int return_flag = 0;
	
	if (player == HUMAN_PLAYER)
	{
		int exit_flag = 0;
		int current_choice = 0;
		
		if (SELECT_MODE == SELECT_ENEMY) player = 1-player;
		
		while (!exit_flag)
		{		
			disp_combat_area(player,current_choice);
			PutString(8,130,message,0x74);
			WaitFlipCopy();
		
			int key_pressed = 0;
		
			while (!key_pressed)
			{

				if (!(*KEYS&KEY_UP))
				{
					while (!(*KEYS&KEY_UP));
					if (SELECT_MODE == SELECT_ANY)
					{
						if (!player) player = 1 - player;
						key_pressed = 1;
					}
				}
			
				if (!(*KEYS&KEY_DOWN))
				{
					while (!(*KEYS&KEY_DOWN));
					if (SELECT_MODE == SELECT_ANY)
					{
						if (player) player = 1 - player;
						key_pressed = 1;
					}
				}

				if (!(*KEYS&KEY_LEFT))
				{
					while (!(*KEYS&KEY_LEFT));
					if (current_choice!=0) current_choice--;
					key_pressed = 1;
				}

				if (!(*KEYS&KEY_RIGHT))
				{
					while (!(*KEYS&KEY_RIGHT));
					if (current_choice!=2) current_choice++;
					key_pressed = 1;
				}

				if (!(*KEYS&KEY_A))
				{
					while (!(*KEYS&KEY_A));
					
					if (combat_area[player][current_choice]!=255)
					{
						return_flag = current_choice + 1;
						
						if (SELECT_MODE == SELECT_ANY)
						{
							// if other player's character was selected
							if (player == COMPUTER_PLAYER) return_flag += 10;
						}
						
						key_pressed = 1;
						exit_flag = 1;
					}
				}
			}
		}		
	}
	else
    	{
        	int enemy_max_hp = 0;
        	int enemy_max_hp_pos = 0;
        	int enemy_min_hp = 0;
        	int enemy_min_hp_pos = 0;
        	int max_hp = 0;
        	int max_hp_pos = 0;
        	int min_hp = 0;
        	int min_hp_pos = 0;

        	int min_frozen_pos = 0;
        	int min_frozen_hp = 0;

		int max_enemy_w_backup_pos = 0;
		int max_enemy_w_backup_hp = 0;

		int min_w_backup_pos = 0;
		int min_w_backup_hp = 0;

		int max_enemy_frozen_pos = 0;
		int max_enemy_frozen_hp = 0;

        	for (int i=0; i<3; i++)
        	{
                	if (combat_area[HUMAN_PLAYER][i]!=255)
			{
                		int char_hp = get_hp(HUMAN_PLAYER,i);
                     
                     		if (char_hp>enemy_max_hp)
                     		{
                        		enemy_max_hp = char_hp;
                          		enemy_max_hp_pos = i+1;
                     		}

				if (char_hp>max_enemy_w_backup_hp&&num_backups[HUMAN_PLAYER][i])
				{
					max_enemy_w_backup_hp = char_hp;
                        		max_enemy_w_backup_pos = i+1;
				}

				if (char_hp>max_enemy_frozen_hp&&freeze_status[HUMAN_PLAYER][i])
				{
					max_enemy_frozen_hp = char_hp;
                        		max_enemy_frozen_pos = i+1;
				}
			
                     		if (char_hp<enemy_min_hp||!enemy_min_hp)
                     		{
                        		enemy_min_hp = char_hp;
                          		enemy_min_hp_pos = i+1;
                     		}
			}
                
        		if (combat_area[COMPUTER_PLAYER][i]!=255)
        		{
        			int char_hp = get_hp(COMPUTER_PLAYER,i);
             			if (char_hp>max_hp)
                     		{
                          		max_hp = char_hp;
                          		max_hp_pos = i+1;
                     		}

                     		if (char_hp<enemy_min_hp||!enemy_min_hp)
                     		{
                          		min_hp = char_hp;
                          		min_hp_pos = i+1;
                     		}

				if ((char_hp<min_frozen_hp||!min_frozen_pos)&&freeze_status[COMPUTER_PLAYER][i])
                     		{
                          		min_frozen_hp = char_hp;
                          		min_frozen_pos = i+1;
                     		}

				if ((char_hp<min_w_backup_hp||!min_w_backup_pos)&&num_backups[COMPUTER_PLAYER][i])
                     		{
                          		min_w_backup_hp = char_hp;
                          		min_w_backup_pos = i+1;
                     		}
        		}
		}

		if (SELECT_MODE == AI_SELECT_ENEMY_STRONG) return_flag = enemy_max_hp_pos;
        	if (SELECT_MODE == AI_SELECT_ENEMY_WEAK) return_flag = enemy_min_hp_pos;
		if (SELECT_MODE == AI_SELECT_ENEMY_WITH_BACKUPS) return_flag = max_enemy_w_backup_pos;
		if (SELECT_MODE == AI_SELECT_ENEMY_FROZEN) return_flag = max_enemy_frozen_pos;

        	if (SELECT_MODE == AI_SELECT_STRONG) return_flag = max_hp_pos;
        	if (SELECT_MODE == AI_SELECT_WEAK) return_flag = min_hp_pos;
        	if (SELECT_MODE == AI_SELECT_FROZEN) return_flag = min_frozen_pos;
        	if (SELECT_MODE == AI_SELECT_BACKUPS) return_flag = min_w_backup_pos;

		
		if (SELECT_MODE == AI_SELECT_OWN)
		{
			int enemy_count = count_chars(HUMAN_PLAYER);
			int char_count = count_chars(COMPUTER_PLAYER);
		
			if (char_count > enemy_count)
			{
				if (player_hp[COMPUTER_PLAYER]>player_hp[HUMAN_PLAYER]) return_flag = min_hp_pos; else return_flag = max_hp_pos;
			}
			else
			{
				return_flag = max_hp_pos;
			}	
		}
	
		if (SELECT_MODE>AI_SELECT_PLAYER)  player = HUMAN_PLAYER; else player = COMPUTER_PLAYER;

		int current_choice = return_flag - 1;
		disp_combat_area(player,current_choice);
        	PutString(8,130,message,0x74);

		WaitFlipCopy();

		int key_pressed = 0;
	
		while (!key_pressed)
		{
			if (!(*KEYS&KEY_A))
			{
				while (!(*KEYS&KEY_A));
				key_pressed = 1;
			}
		}
	
	}	
	
	return return_flag;
}

int two_choice_menu(int player, int card_pos, char *msg, char * menu[])
{
	while (!(*KEYS&KEY_A));

	int choice = 0;
	int return_flag = 0;
	int exit_flag = 0;

	InitializeSprites();
	print_area_chars(player,card_pos,PRINT_CARD_IN_PLAY);

	while (!exit_flag)
	{
		putBG(back_offs[0]);
		putsprite(54,4,120,100,right_paneData);
		view_combat_area (HUMAN_PLAYER,STATUS_NORMAL,0,0,0,0,0);
		view_combat_area (COMPUTER_PLAYER,STATUS_NORMAL,0,0,0,0,0);
		view_stats_area();
		putsprite(5,108,96,45,bottom_leftData);
		PutString(8,114,msg,0x6B);
		putsprite(5,4,96,100,left_paneData);
		print_menu(16,122,2,choice,0x9D,menu);
		WaitFlipCopy();
		
		int key_pressed = 0;
		
		while(!key_pressed)
		{
			if (!(*KEYS&KEY_UP))
			{
				if (choice != 0) choice--;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_DOWN))
			{
				if (choice != 1) choice++;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_A))
			{
				while (!(*KEYS&KEY_A));
				return_flag = choice + 1;
				key_pressed = 1;
				exit_flag = 1;
			}
		}
	}
	
	return return_flag;
}

int two_choice_menu2(int player, char *msg, char * menu[],int card_pos,int pile_type)
{
	while (!(*KEYS&KEY_A));

	int choice = 0;
	int return_flag = 0;
	int exit_flag = 0;

	InitializeSprites();
	print_pile_char(player,card_pos,pile_type);

	while (!exit_flag)
	{
		putBG(back_offs[0]);
		putsprite(54,4,120,100,right_paneData);
		view_combat_area (HUMAN_PLAYER,STATUS_NORMAL,0,0,0,0,0);
		view_combat_area (COMPUTER_PLAYER,STATUS_NORMAL,0,0,0,0,0);
		view_stats_area();
		putsprite(5,108,96,45,bottom_leftData);
		PutString(8,114,msg,0x6B);
		putsprite(5,4,96,100,left_paneData);
		print_menu(16,122,2,choice,0x9D,menu);
		WaitFlipCopy();
		
		int key_pressed = 0;
		
		while(!key_pressed)
		{
			if (!(*KEYS&KEY_UP))
			{
				if (choice != 0) choice--;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_DOWN))
			{
				if (choice != 1) choice++;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_A))
			{
				while (!(*KEYS&KEY_A));
				return_flag = choice + 1;
				key_pressed = 1;
				exit_flag = 1;
			}
		}
	}
	
	return return_flag;
}

const char player_draws[] = {"DRAWS A CARD$"};
const char player_discards[] = {"DISCARDS$"};

void draw_card_animated(int player)
{
	int x = 240;
	InitializeSprites();	 //set all sprites off screen (stops artifact)

	setup_char_sprite_data(0,(void*)Card_back_Data);	//copies sprite image data to OAM Data Mem

	while(x>84)
	{
		putBG(back_offs[0]);
		putsprite(36,4,96,100,left_paneData);
		putsprite(36,108,96,45,bottom_leftData);
		PutString(39,118,(char *)player_names[player],0xF3);
		PutString(39,126,(char *)player_draws,0x9D);
		WaitForVsync();
		Flip();
		x-=10;
		setup_char_sprite(x,4,0);
		CopyOAM();
	}

	setup_char_sprite(84,4,0);
	CopyOAM();

	int delay_max = 30000;
	int do_nothing = 0;
	for (int i=0; i<delay_max; i++) do_nothing = 1 - do_nothing;
}

void discard_card_animated(int player)
{
	InitializeSprites();	 //set all sprites off screen (stops artifact)

	setup_char_sprite_data(0,(void*)Card_back_Data);	//copies sprite image data to OAM Data Mem

	putBG(back_offs[0]);
	putsprite(36,4,96,100,left_paneData);
	putsprite(36,108,96,45,bottom_leftData);
	PutString(39,118,(char *)player_names[player],0xF3);
	PutString(39,126,(char *)player_discards,0x9D);
	setup_char_sprite(84,4,0);
	WaitForVsync();
	Flip();
	CopyOAM();
	
	FadeOut(0);

	int delay_max = 30000;
	int do_nothing = 0;
	for (int i=0; i<delay_max; i++) do_nothing = 1 - do_nothing;
	
	InitializeSprites();
	putBG(back_offs[0]);
	putsprite(54,4,120,100,right_paneData);
	disp_combat_area(HUMAN_PLAYER,255);
	WaitFlipCopy();
}

const int MODE_CARD_IN_HAND = 0;
const int MODE_CARD_IN_PLAY = 1;
const char backups_label [] = {"BACKUPS$"};
const char no_backups_label [] = {"NONE$"};

void print_card_data(int player,int card_num,int card_type, int mode)
{
	int stats_ability = NO_STATS;
	
	if (mode == MODE_CARD_IN_HAND)
	{
		int hp = 0;
		int sp = 0;
		
		char *card_name;
		char *card_ability;
		char *card_ability_desc;
		
		if (card_type == SNK_CARD)
		{
			setup_char_sprite_data(0,(void*)SNK_char_offs[card_num]);	//copies sprite image data to OAM Data Mem
			hp = SNK_chars_hp[card_num]/100;
			sp = SNK_chars_sp[card_num];
			card_name =(char *)SNK_names[card_num];
			card_ability = (char *)SNK_ABILITIES[card_num+1];
			card_ability_desc = (char *)SNK_ABILITIES_DESC[card_num+1];
		}

		if (card_type == CAPCOM_CARD)
		{
			setup_char_sprite_data(0,(void*)Capcom_char_offs[card_num]);	//copies sprite image data to OAM Data Mem
			hp = Capcom_chars_hp[card_num]/100;
			sp = Capcom_chars_sp[card_num];
			card_name =(char *)capcom_names[card_num];
			card_ability = (char *)CAPCOM_ABILITIES[card_num+1];
			card_ability_desc = (char *)CAPCOM_ABILITIES_DESC[card_num+1];
		}

		unsigned short ability_color = 0xF3;
		stats_ability = ability_stats(card_ability);
		if (stats_ability == STATS_CIRCLE) ability_color = 0x44;
		if (stats_ability == STATS_SQUARE) ability_color = 0xE4;
		if (stats_ability == STATS_TRIANGLE) ability_color = 0x5D;
		if (stats_ability == NO_STATS) ability_color = 0xF3;
		
		if (card_type == SNK_CARD || card_type == CAPCOM_CARD) put_char_stats(0,hp*100,sp,stats_ability);

		InitializeSprites();
		putBG(back_offs[0]);
		putsprite(5,4,96,100,left_paneData);
		//putsprite(5,108,96,45,bottom_leftData);
		putsprite(54,4,120,100,right_paneData);
		putsprite(54,108,120,45,bottom_rightData);
		setup_char_sprite(22,4,0);
	
		
		if (stats_ability == NO_STATS) 
		{
			PutString(59,18,card_ability,ability_color); 
		}
		else 
		{
			if (stats_ability == STATS_CIRCLE) putspriteBG(58,18,8,8,0x00,card_circleData);
			if (stats_ability == STATS_SQUARE) putspriteBG(58,18,8,8,0x00,card_squareData);
			if (stats_ability == STATS_TRIANGLE) putspriteBG(58,18,8,8,0x00,card_triangleData);
			stats_ability = ability_stats(card_ability);
			PutString(63,18,card_ability+1,ability_color);
		}
		
		PutString(59,34,card_ability_desc,0x9D);

		int num_backups = 0;

		PutString(59,114,(char *)backups_label,0xF3);

		for (int i=0;i<3;i++)
		{
			char *backup_char;
			if (card_type == SNK_CARD)
			{
				int backup = SNK_backups[card_num*3+i];
				
				if (backup!=255)
				{
					if (backup>119)
					{
						backup_char = (char *)capcom_names[backup-120];
					}
					else
					{
						backup_char = (char *)SNK_names[backup];
					}
					
					PutString(59,122+num_backups*8,backup_char,0x9D);
					num_backups++;
				}

				if (!num_backups) PutString(59,122,(char *)no_backups_label,0x9D);
			}

			if (card_type == CAPCOM_CARD)
			{
				int backup = Capcom_backups[card_num*3+i];
				
				if (backup!=255)
				{
					if (backup>119)
					{
						backup_char = (char *)capcom_names[backup-120];
					}
					else
					{
						backup_char = (char *)SNK_names[backup];
					}
					
					PutString(59,122+num_backups*8,backup_char,0x9D);
					num_backups++;
				}
				
				if (!num_backups) PutString(59,122,(char *)no_backups_label,0x9D);
			}
		}

		WaitFlipCopy();
				
		int key_pressed = 0;
		while (!key_pressed)
		{
			if (!(*KEYS&KEY_START)||!(*KEYS&KEY_A)||!(*KEYS&KEY_B)||!(*KEYS&KEY_L)||!(*KEYS&KEY_R))
			{
				while (!(*KEYS&KEY_START)||!(*KEYS&KEY_A)||!(*KEYS&KEY_B)||!(*KEYS&KEY_L)||!(*KEYS&KEY_R));
				key_pressed = 1;
			}			
		}		
	}
	
	if (mode == MODE_CARD_IN_PLAY)
	{
		int hp = 0;
		
		char *card_name;
		char *card_ability;
		char *card_ability_desc;
		
		int type = combat_area_card_type[player][card_num];
		int num = combat_area[player][card_num];
		
		if (type == SNK_CARD)
		{
			setup_char_sprite_data(0,(void*)SNK_char_offs[num]);	//copies sprite image data to OAM Data Mem
			card_name =(char *)SNK_names[num];
			card_ability = (char *)SNK_ABILITIES[num+1];
			card_ability_desc = (char *)SNK_ABILITIES_DESC[num+1];
		}

		if (type == CAPCOM_CARD)
		{
			setup_char_sprite_data(0,(void*)Capcom_char_offs[num]);	//copies sprite image data to OAM Data Mem
			card_name =(char *)capcom_names[num];
			card_ability = (char *)CAPCOM_ABILITIES[num+1];
			card_ability_desc = (char *)CAPCOM_ABILITIES_DESC[num+1];
		}

		hp = get_hp(player,card_num)/100;

		unsigned short ability_color = 0xF3;
		stats_ability = ability_stats(card_ability);
		if (stats_ability == STATS_CIRCLE) ability_color = 0x44;
		if (stats_ability == STATS_SQUARE) ability_color = 0xE4;
		if (stats_ability == STATS_TRIANGLE) ability_color = 0x5D;
		if (stats_ability == NO_STATS) ability_color = 0xF3;
		put_char_stats(0,hp*100,NO_STATS,stats_ability);

		InitializeSprites();
		putBG(back_offs[0]);
		putsprite(5,4,96,100,left_paneData);
		putsprite(5,108,96,45,bottom_leftData);
		putsprite(54,4,120,100,right_paneData);
		putsprite(54,108,120,45,bottom_rightData);
		setup_char_sprite(22,4,0);
		
		
		if (card_ability[0] == 'N')
		{
			PutString(59,18,card_ability,ability_color); 
		}
		else 
		{
			if (stats_ability == STATS_CIRCLE) putspriteBG(58,18,8,8,0x00,card_circleData);
			if (stats_ability == STATS_SQUARE) putspriteBG(58,18,8,8,0x00,card_squareData);
			if (stats_ability == STATS_TRIANGLE) putspriteBG(58,18,8,8,0x00,card_triangleData);
			PutString(63,18,card_ability+1,ability_color);
		}

		PutString(59,34,card_ability_desc,0x9D);

		int num_backups = 0;
		
		PutString(59,114,(char *)backups_label,0xF3);

		for (int i=0;i<3;i++)
		{
			char *backup_char;
			int backup = combat_area_backups[player][card_num][i];   

				
			if (backup!=255)
			{
				if (backup>119)
				{
					backup_char = (char *)capcom_names[backup-120];
				}
				else
				{
					backup_char = (char *)SNK_names[backup];
				}
					
				PutString(59,122+num_backups*8,backup_char,0x9D);
				num_backups++;
			}
		}

		WaitFlipCopy();
				
		int key_pressed = 0;
		while (!key_pressed)
		{
			if (!(*KEYS&KEY_START)||!(*KEYS&KEY_A)||!(*KEYS&KEY_B)||!(*KEYS&KEY_L)||!(*KEYS&KEY_R))
			{
				while (!(*KEYS&KEY_START)||!(*KEYS&KEY_A)||!(*KEYS&KEY_B)||!(*KEYS&KEY_L)||!(*KEYS&KEY_R));
				key_pressed = 1;
			}			
		}		
	}
}	

const char select_data_001 [] = {"SELECT$"};
const char select_data_002 [] = {"DATA$"};
const char * select_data_menu_choices [] = {select_data_001,select_data_002};
	
int select_data_menu(int player, int card)
{
	int exit_flag = 0;
	int choice = 0;
	int return_flag = 0;

	InitializeSprites();

	while (!(*KEYS&KEY_A)); // clear keyboard buffer
	
	while (!exit_flag)
	{
		putBG(back_offs[0]);
		putsprite(5,4,96,100,left_paneData);
		print_right_pane(player,card);
		
		print_menu(16,14,2,choice,0x9D,(char **)select_data_menu_choices);
		WaitFlipCopy();
		
		int key_pressed = 0;
		
		while(!key_pressed)
		{
			if (!(*KEYS&KEY_UP))
			{
				if (choice != 0) choice--;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_DOWN))
			{
				if (choice != 1) choice++;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_A))
			{
				while (!(*KEYS&KEY_A));
				if (choice == 1) exit_flag = 3;
				if (choice == 0) exit_flag = 2;
				key_pressed = 1;
			}
			
			if (!(*KEYS&KEY_B))
			{
				while (!(*KEYS&KEY_B));
				key_pressed = 1;
				exit_flag = 1;
			}
		}
	}

	if (exit_flag == 3) print_card_data(player,hand_cards[player][card],hand_card_type[player][card],MODE_CARD_IN_HAND);
	if (exit_flag == 2)  return_flag = 1;
	
	return return_flag;
}

int select_data_menu_deck(int player, int card)
{
	int exit_flag = 0;
	int choice = 0;
	int return_flag = 0;

	InitializeSprites();

	while (!(*KEYS&KEY_A)); // clear keyboard buffer
	
	while (!exit_flag)
	{
		putBG(back_offs[0]);
		putsprite(5,4,96,100,left_paneData);
		print_right_pane_deck(player,card);
		
		print_menu(16,14,2,choice,0x9D,(char **)select_data_menu_choices);

		WaitFlipCopy();
		
		int key_pressed = 0;
		
		while(!key_pressed)
		{
			if (!(*KEYS&KEY_UP))
			{
				if (choice != 0) choice--;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_DOWN))
			{
				if (choice != 1) choice++;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_A))
			{
				while (!(*KEYS&KEY_A));
				if (choice == 1) exit_flag = 3;
				if (choice == 0) exit_flag = 2;
				key_pressed = 1;
			}
			
			if (!(*KEYS&KEY_B))
			{
				while (!(*KEYS&KEY_B));
				key_pressed = 1;
				exit_flag = 1;
			}
		}
	}

	if (exit_flag == 3) print_card_data(player,combat_decks[player][card],card_types[player][card],MODE_CARD_IN_HAND);
	if (exit_flag == 2)  return_flag = 1;
	
	return return_flag;
}

int select_data_menu_discard_pile(int player, int card)
{
	int exit_flag = 0;
	int choice = 0;
	int return_flag = 0;

	InitializeSprites();

	while (!(*KEYS&KEY_A)); // clear keyboard buffer
	
	while (!exit_flag)
	{
		putBG(back_offs[0]);
		putsprite(5,4,96,100,left_paneData);
		print_right_pane_discard_pile(player,card);
		
		print_menu(16,14,2,choice,0x9D,(char **)select_data_menu_choices);

		WaitFlipCopy();
		
		int key_pressed = 0;
		
		while(!key_pressed)
		{
			if (!(*KEYS&KEY_UP))
			{
				if (choice != 0) choice--;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_DOWN))
			{
				if (choice != 1) choice++;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_A))
			{
				while (!(*KEYS&KEY_A));
				if (choice == 1) exit_flag = 3;
				if (choice == 0) exit_flag = 2;
				key_pressed = 1;
			}
			
			if (!(*KEYS&KEY_B))
			{
				while (!(*KEYS&KEY_B));
				key_pressed = 1;
				exit_flag = 1;
			}
		}
	}

	if (exit_flag == 3) print_card_data(player,discard_pile_cards[player][card],discard_pile_card_type[player][card],MODE_CARD_IN_HAND);
	if (exit_flag == 2)  return_flag = 1;
	
	return return_flag;
}

const int HUMAN_MODE = 0;
const int HUMAN_BROWSE_MODE = 1;
const int AI_SELECT_STRONG_MODE = 2;
const int AI_SELECT_WEAK_MODE = 3;
const int AI_SELECT_STRONG_ACTION_MODE = 4;
const int AI_SELECT_WEAK_ACTION_MODE = 5;
const int AI_SELECT_STRONG_CARD_MODE = 6;
const int AI_SELECT_WEAK_CARD_MODE = 7;
const int AI_SELECT_ENEMY_STRONG_MODE = 8;
const int AI_SELECT_ENEMY_WEAK_MODE = 9;
const int AI_SELECT_ENEMY_STRONG_CARD_MODE = 10;
const int AI_SELECT_ENEMY_WEAK_CARD_MODE = 11;
const int ANY_CARD = 255;
const int ANY_CHAR_CARD = 3;

int select_card_from_deck(int player, int card_type, int mode)
{
	int return_flag = 0;
	
	if (mode == HUMAN_MODE || mode == HUMAN_BROWSE_MODE)
	{
		int start_index = deck_ptr[player];
		int end_index = NUM_CARDS-1;
		int end_list = NUM_CARDS-1;
		int card = 0;

		if ((NUM_CARDS-start_index)>9) end_index = start_index+9;
		
		int exit_flag = 0;

		if (deck_ptr[player]==NUM_CARDS) exit_flag = 1;
		
		while (!exit_flag)	
		{

			putBG(back_offs[0]);
			putsprite(5,4,96,100,left_paneData);
			
			for (int i=start_index;i<end_index+1;i++)
			{
				char card_set3[] = {"ACT    $"};
				char *ptr;
				
				if (card_types[player][i] == SNK_CARD) ptr = (char *)SNK_names[combat_decks[player][i]];
				if (card_types[player][i] == CAPCOM_CARD) ptr = (char *)capcom_names[combat_decks[player][i]];
				if (card_types[player][i] == ACTION_CARD) ptr = card_set3;
				
				if (card_types[player][start_index+card] != CAPCOM_CARD && card_types[player][start_index+card] != SNK_CARD)
				{
					itoa_lpad(ptr+3,4,combat_decks[player][i],' ');
					ptr[7]='$';
				}
				
				PutString(16,14+(i-start_index)*8,ptr,0x9D);
			
				if (card == (i-start_index)) putspriteBG(1,13+(i-start_index)*8,28,18,0,small_handData);
				if (end_index!=end_list) putsprite(25,94,10,5,down_ptrData);
				if (start_index!=deck_ptr[player]) putsprite(25,8,10,5,up_ptrData);
			}
			
			print_right_pane_deck(player,start_index+card);
			view_stats_area();
			
			char buf1[4];
			char buf2[4];
			
			itoa_lpad(buf1,3,(start_index+card-deck_ptr[player]+1),' ');
			itoa_lpad(buf2,3,(NUM_CARDS-deck_ptr[player]),' ');
			
			buf1[3] = '$';
			buf2[3] = '$';
			
			putsprite(42,95,8,8,slashData);
			PutString(34, 95,buf1,0x9D);
			PutString(43, 95,buf2,0x9D);
			
			char deck1 [] = {"SELECT CARD/FROM DECK$"};
			char deck2 [] = {"VIEW CARDS/IN DECK$"};
			
			putsprite(5,108,96,45,bottom_leftData);
			if (mode == HUMAN_MODE) PutString(8,114,deck1,0x6B); else PutString(8,114,deck2,0x6B);

			WaitFlipCopy();

			int key_pressed = 0;
			
			while(!key_pressed)
			{
				if (!(*KEYS&KEY_DOWN))
				{
				     if ((start_index+card)!=end_list)
				     {
				     	if (card == 9)
				     	{
				     	     start_index ++;
				     	     end_index ++;
				     	     key_pressed = 1;
				     	}
				     	else
				     	{
				     	     card++;
				     	     key_pressed = 1;
				     	}
				     }
				}
				
				if (!(*KEYS&KEY_UP))
				{
				     if (start_index+card!=deck_ptr[player])
				     {
				     	if (card == 0)
				     	{
				     	     start_index --;
				     	     end_index --;
				     	     key_pressed = 1;
				     	}
				     	else
				     	{
				     	     card--;
				     	     key_pressed = 1;
				     	}
				     }
				}
				
				if (!(*KEYS&KEY_A))
				{
					key_pressed = 1;
					if (mode!=HUMAN_BROWSE_MODE)
					{
						int selected_card = select_data_menu_deck(player,start_index+card);

						if (selected_card&&(card_types[player][start_index+card]==card_type||card_type==ANY_CARD))
						{
							return_flag = start_index+card + 1;
							exit_flag = 1;
						}

						if (selected_card&&(card_types[player][start_index+card]==CAPCOM_CARD||card_types[player][start_index+card]==SNK_CARD)&&card_type==ANY_CHAR_CARD)
						{
							return_flag = start_index+card + 1;
							exit_flag = 1;
						}
					}
					else
					{
						int card_data = start_index+card;
						print_card_data(player,combat_decks[player][card_data],card_types[player][card_data],MODE_CARD_IN_HAND);
					}
				}

				if (!(*KEYS&KEY_B)&&(mode==HUMAN_BROWSE_MODE))
				{
					key_pressed = 1;
					exit_flag = 1;
					return_flag = 0;
				}

			}
		}
		
		InitializeSprites();
		WaitForVsync();
		CopyOAM();
	}
	else
	{
		int strong_char = 0;
		int strong_char_hp = 0;
		
		int weak_char = 0;
		int weak_char_hp = 0;
		
		int ai_weak_action = 0;
		int weak_action_card = 0;
		
		int ai_strong_action = 0;
		int strong_action_card = 0;
		
		int ai_weak_card = 0;
		int weak_card = 0;
		
		int ai_strong_card = 0;
		int strong_card = 0;
		
		for (int i=deck_ptr[player]; i<NUM_CARDS; i++)
		{
			int overall_ai = 0;
			
			if (card_types[player][i]!=ACTION_CARD)
			{
				int hp = 0;
				int ai_value = 0;
				
				if (card_types[player][i]==SNK_CARD)
				{
					hp = SNK_chars_hp[combat_decks[player][i]];
				}
				else
				{
					hp = Capcom_chars_hp[combat_decks[player][i]];
				}
				
				for (int j = 0; j<3; j++) ai_value += check_card_in_deck_ai_value(player,i,j);
				
				if (hp>strong_char_hp)
				{
					strong_char_hp = hp;
					strong_char = i+1;
				}

				if (hp<weak_char_hp||!weak_char)
				{
					weak_char_hp = hp;
					weak_char = i+1;
				}
				
				overall_ai = ai_value/3;
			}
			else
			{
				int ai_card = check_card_in_deck_ai_value(player,i,AI_ABILITY);
				
				if (ai_card>ai_strong_action)
				{
					ai_strong_action = ai_card;
					strong_action_card = i+1;
				}
				
				if (ai_card<ai_weak_action||!weak_action_card)
				{
					ai_weak_action = ai_card;
					weak_action_card = i+1;
				}
				
				overall_ai = ai_card;
			}
			
			if (overall_ai>ai_strong_card)
			{
				ai_strong_card = overall_ai;
				strong_card = i+1;
			}

			if (overall_ai<ai_weak_card||!weak_card)
			{
				ai_weak_card = overall_ai;
				weak_card = i+1;
			}
		}

		if (mode == AI_SELECT_STRONG_MODE || mode == AI_SELECT_ENEMY_STRONG_MODE)
		{
			return_flag = strong_char;
		}
		
		if (mode == AI_SELECT_WEAK_MODE || mode == AI_SELECT_ENEMY_WEAK_MODE)
		{
			return_flag = weak_char;
		}
		
		if (mode == AI_SELECT_STRONG_ACTION_MODE) return_flag = strong_action_card;
		if (mode == AI_SELECT_WEAK_ACTION_MODE) return_flag = weak_action_card;
		if (mode == AI_SELECT_STRONG_CARD_MODE || mode == AI_SELECT_ENEMY_STRONG_CARD_MODE) return_flag = strong_card;
		if (mode == AI_SELECT_WEAK_CARD_MODE || mode == AI_SELECT_ENEMY_WEAK_CARD_MODE) return_flag = weak_card;
	}
	
	return return_flag;
}

int view_top_cards_from_deck(int player, int cards, int mode)
{
	int return_flag = 0;
	
	if (deck_ptr[player]==NUM_CARDS) return return_flag;
	
	int start_index = deck_ptr[player];
	int end_index = start_index+cards-1;
	int end_list = start_index+cards-1;
	
	if (end_index>NUM_CARDS-1) end_index=NUM_CARDS-1;
	if (end_list>NUM_CARDS-1) end_list=NUM_CARDS-1;
	int card = 0;

	if (mode == HUMAN_MODE || mode == HUMAN_BROWSE_MODE)
	{
		int exit_flag = 0;

		if (deck_ptr[player]==NUM_CARDS) exit_flag = 1;
		
		while (!exit_flag)	
		{

			putBG(back_offs[0]);
			putsprite(5,4,96,100,left_paneData);
			
			for (int i=start_index;i<end_index+1;i++)
			{
				char card_set3[] = {"ACT    $"};
				char *ptr;
				
				if (card_types[player][i] == SNK_CARD) ptr = (char *)SNK_names[combat_decks[player][i]];
				if (card_types[player][i] == CAPCOM_CARD) ptr = (char *)capcom_names[combat_decks[player][i]];
				if (card_types[player][i] == ACTION_CARD) ptr = card_set3;
				
				if (card_types[player][start_index+card] != CAPCOM_CARD && card_types[player][start_index+card] != SNK_CARD)
				{
					itoa_lpad(ptr+3,4,combat_decks[player][i],' ');
					ptr[7]='$';
				}
				
				PutString(16,14+(i-start_index)*8,ptr,0x9D);
			
				if (card == (i-start_index)) putspriteBG(1,13+(i-start_index)*8,28,18,0,small_handData);
				if (end_index!=end_list) putsprite(25,94,10,5,down_ptrData);
				if (start_index!=deck_ptr[player]) putsprite(25,8,10,5,up_ptrData);
			}
			
			print_right_pane_deck(player,start_index+card);
			view_stats_area();
			
			char buf1[4];
			char buf2[4];
			
			itoa_lpad(buf1,3,(start_index+card-deck_ptr[player]+1),' ');
			itoa_lpad(buf2,3,(NUM_CARDS-deck_ptr[player]),' ');
			
			buf1[3] = '$';
			buf2[3] = '$';
			
			putsprite(42,95,8,8,slashData);
			PutString(34, 95,buf1,0x9D);
			PutString(43, 95,buf2,0x9D);
			
			char deck [] = {"SELECT CARD/FROM DECK$"};
			putsprite(5,108,96,45,bottom_leftData);
			PutString(8,114,deck,0x6B);

			WaitFlipCopy();

			int key_pressed = 0;
			
			while(!key_pressed)
			{
				if (!(*KEYS&KEY_DOWN))
				{
				     if ((start_index+card)!=end_list)
				     {
				     	if (card == 9)
				     	{
				     	     start_index ++;
				     	     end_index ++;
				     	     key_pressed = 1;
				     	}
				     	else
				     	{
				     	     card++;
				     	     key_pressed = 1;
				     	}
				     }
				}
				
				if (!(*KEYS&KEY_UP))
				{
				     if (start_index+card!=deck_ptr[player])
				     {
				     	if (card == 0)
				     	{
				     	     start_index --;
				     	     end_index --;
				     	     key_pressed = 1;
				     	}
				     	else
				     	{
				     	     card--;
				     	     key_pressed = 1;
				     	}
				     }
				}
				
				if (!(*KEYS&KEY_A))
				{
					key_pressed = 1;
					if (mode!=HUMAN_BROWSE_MODE)
					{
						int selected_card = select_data_menu_deck(player,start_index+card);

						if (selected_card)
						{
							return_flag = start_index+card+1;
							exit_flag = 1;
						}
					}
					else
					{
						int card_data = start_index+card;
						print_card_data(player,combat_decks[player][card_data],card_types[player][card_data],MODE_CARD_IN_HAND);
					}
				}

				if (!(*KEYS&KEY_B)&&(mode==HUMAN_BROWSE_MODE))
				{
					key_pressed = 1;
					exit_flag = 1;
					return_flag = 0;
				}
			}
		}
		
		InitializeSprites();
		WaitForVsync();
		CopyOAM();
	}
	
	return return_flag;
}

int select_card_from_discard_pile(int player, int card_type, int mode)
{
	int start_index = 0;
	int end_index = discard_pile_ptr[player]-1;
	int end_list = discard_pile_ptr[player]-1;
	int return_flag = 0;
	int card = 0;

	if ((end_list+1)-start_index>9) end_index = start_index+9;
	
	if (mode == HUMAN_MODE || mode == HUMAN_BROWSE_MODE)
	{
		int exit_flag = 0;

		if (discard_pile_ptr[player]==0) exit_flag = 1;
		
		while (!exit_flag)	
		{

			putBG(back_offs[0]);
			putsprite(5,4,96,100,left_paneData);
			
			for (int i=start_index;i<end_index+1;i++)
			{
				char card_set3[] = {"ACT    $"};
				char *ptr;
				
				if (discard_pile_card_type[player][i] == SNK_CARD) ptr = (char *)SNK_names[discard_pile_cards[player][i]];
				if (discard_pile_card_type[player][i] == CAPCOM_CARD) ptr = (char *)capcom_names[discard_pile_cards[player][i]];
				if (discard_pile_card_type[player][i] == ACTION_CARD) ptr = card_set3;
				
				if (discard_pile_card_type[player][start_index+card] != CAPCOM_CARD && discard_pile_card_type[player][start_index+card] != SNK_CARD)
				{
					itoa_lpad(ptr+3,4,discard_pile_cards[player][i],' ');
					ptr[7]='$';
				}
				
				PutString(16,14+(i-start_index)*8,ptr,0x9D);

				if (card == (i-start_index)) putspriteBG(1,13+(i-start_index)*8,28,18,0,small_handData);
				if (end_index!=end_list) putsprite(25,94,10,5,down_ptrData);
				if (start_index!=0) putsprite(25,8,10,5,up_ptrData);
			}
			
			print_right_pane_discard_pile(player,start_index+card);
			view_stats_area();
			
			char buf1[4];
			char buf2[4];
			
			itoa_lpad(buf1,3,(start_index+card+1),' ');
			itoa_lpad(buf2,3,discard_pile_ptr[player],' ');
			
			buf1[3] = '$';
			buf2[3] = '$';
			
			putsprite(42,95,8,8,slashData);
			PutString(34, 95,buf1,0x9D);
			PutString(43, 95,buf2,0x9D);

			char discard_pile1 [] = {"SELECT CARD/FROM DISCARD/PILE$"};
			char discard_pile2 [] = {"VIEWING CARDS/FROM DISCARD/PILE$"};
			
			putsprite(5,108,96,45,bottom_leftData);
			if (mode == HUMAN_MODE) PutString(8,114,discard_pile1,0x6B); else PutString(8,114,discard_pile2,0x6B);

			WaitFlipCopy();

			int key_pressed = 0;
			
			while(!key_pressed)
			{
				if (!(*KEYS&KEY_DOWN))
				{
				     if ((start_index+card)!=end_list)
				     {
				     	if (card == 9)
				     	{
				     	     start_index ++;
				     	     end_index ++;
				     	     key_pressed = 1;
				     	}
				     	else
				     	{
				     	     card++;
				     	     key_pressed = 1;
				     	}
				     }
				}
				
				if (!(*KEYS&KEY_UP))
				{
				     if (start_index+card!=0)
				     {
				     	if (card == 0)
				     	{
				     	     start_index --;
				     	     end_index --;
				     	     key_pressed = 1;
				     	}
				     	else
				     	{
				     	     card--;
				     	     key_pressed = 1;
				     	}
				     }
				}
				
				if (!(*KEYS&KEY_A))
				{
					key_pressed = 1;
					if (mode!=HUMAN_BROWSE_MODE)
					{
						int selected_card = select_data_menu_discard_pile(player,start_index+card);

						if (selected_card&&(discard_pile_card_type[player][start_index+card]==card_type||card_type==ANY_CARD))
						{
							return_flag = start_index+card + 1;
							exit_flag = 1;
						}

						if (selected_card&&(discard_pile_card_type[player][start_index+card]==CAPCOM_CARD||card_types[player][start_index+card]==SNK_CARD)&&card_type==ANY_CHAR_CARD)
						{
							return_flag = start_index+card + 1;
							exit_flag = 1;
						}
					}
					else
					{
						int card_data = start_index+card;
						print_card_data(player,discard_pile_cards[player][card_data],discard_pile_card_type[player][card_data],MODE_CARD_IN_HAND);
					}
				}

				if (!(*KEYS&KEY_B)&&(mode==HUMAN_BROWSE_MODE))
				{
					key_pressed = 1;
					exit_flag = 1;
					return_flag = 0;
				}

			}
		}
		
		InitializeSprites();
		WaitForVsync();
		CopyOAM();
	}
	else
	{
		int strong_char = 0;
		int strong_char_hp = 0;
		
		int weak_char = 0;
		int weak_char_hp = 0;
		
		int ai_weak_action = 0;
		int weak_action_card = 0;
		
		int ai_strong_action = 0;
		int strong_action_card = 0;
		
		int ai_weak_card = 0;
		int weak_card = 0;
		
		int ai_strong_card = 0;
		int strong_card = 0;
		
		for (int i=0; i<discard_pile_ptr[player]; i++)
		{
			int overall_ai = 0;
			
			if (discard_pile_card_type[player][i]!=ACTION_CARD)
			{
				int hp = 0;
				int ai_value = 0;
				
				if (discard_pile_card_type[player][i]==SNK_CARD)
				{
					hp = SNK_chars_hp[discard_pile_cards[player][i]];
				}
				else
				{
					hp = Capcom_chars_hp[discard_pile_cards[player][i]];
				}
				
				for (int j = 0; j<3; j++) ai_value += check_card_in_discards_ai_value(player,i,j);
				
				if (hp>strong_char_hp)
				{
					strong_char_hp = hp;
					strong_char = i+1;
				}

				if (hp<weak_char_hp||!weak_char)
				{
					weak_char_hp = hp;
					weak_char = i+1;
				}
				
				overall_ai = ai_value/3;
			}
			else
			{
				int ai_card = check_card_in_discards_ai_value(player,i,AI_ABILITY);
				
				if (ai_card>ai_strong_action)
				{
					ai_strong_action = ai_card;
					strong_action_card = i+1;
				}
				
				if (ai_card<ai_weak_action||!weak_action_card)
				{
					ai_weak_action = ai_card;
					weak_action_card = i+1;
				}
				
				overall_ai = ai_card;
			}
			
			if (overall_ai>ai_strong_card)
			{
				ai_strong_card = overall_ai;
				strong_card = i+1;
			}

			if (overall_ai<ai_weak_card||!weak_card)
			{
				ai_weak_card = overall_ai;
				weak_card = i+1;
			}
		}

		if (mode == AI_SELECT_STRONG_MODE || mode == AI_SELECT_ENEMY_STRONG_MODE)
		{
			return_flag = strong_char;
		}
		
		if (mode == AI_SELECT_WEAK_MODE || mode == AI_SELECT_ENEMY_WEAK_MODE)
		{
			return_flag = weak_char;
		}
		
		if (mode == AI_SELECT_STRONG_ACTION_MODE) return_flag = strong_action_card;
		if (mode == AI_SELECT_WEAK_ACTION_MODE) return_flag = weak_action_card;
		if (mode == AI_SELECT_STRONG_CARD_MODE || mode == AI_SELECT_ENEMY_STRONG_CARD_MODE) return_flag = strong_card;
		if (mode == AI_SELECT_WEAK_CARD_MODE || mode == AI_SELECT_ENEMY_WEAK_CARD_MODE) return_flag = weak_card;
	}

	return return_flag;
}


int select_card_from_hand(int player, int mode)
{
	int start_index = 0;
	int end_index = 9;
	int current_card = 0;
	int return_flag = 0;
		
	count_hand(player);
	int hand_count = hand_size[player];
	if (hand_count < 10) end_index = hand_count - 1;
	int end_list = hand_count - 1;

	if (mode == HUMAN_MODE||mode == HUMAN_BROWSE_MODE)
	{
		int exit_flag = 0;
		count_hand(player);
		if (!hand_size[player]) exit_flag = 1;
		
		while (!exit_flag)	
		{

			putBG(back_offs[0]);
			putsprite(5,4,96,100,left_paneData);
			
			for (int i=start_index;i<end_index+1;i++)
			{
				int card = return_card(player,i+1);
				
				char card_set3[] = {"ACT    $"};
				char *ptr;
				
				if (hand_card_type[player][card] == SNK_CARD) ptr = (char *)SNK_names[hand_cards[player][card]];
				if (hand_card_type[player][card] == CAPCOM_CARD) ptr = (char *)capcom_names[hand_cards[player][card]];
				if (hand_card_type[player][card] == ACTION_CARD) ptr = card_set3;
				
				if (hand_card_type[player][card] != CAPCOM_CARD && hand_card_type[player][card] != SNK_CARD)
				{
					itoa_lpad(ptr+3,4,hand_cards[player][card],' ');
					ptr[7]='$';
				}
				
				PutString(16,14+(i-start_index)*8,ptr,0x9D);
			
				if (current_card == i) putspriteBG(1,13+(i-start_index)*8,28,18,0,small_handData);
				if (end_index!=end_list) putsprite(25,94,10,5,down_ptrData);
				if (start_index!=0) putsprite(25,8,10,5,up_ptrData);
			}
			
			int card = return_card(player,current_card+1);

			print_right_pane(player,card);
			view_stats_area();
			
			char buf1[4];
			char buf2[4];
			
			itoa_lpad(buf1,3,current_card+1,' ');
			itoa_lpad(buf2,3,hand_count,' ');
			
			buf1[3] = '$';
			buf2[3] = '$';
			
			putsprite(42,95,8,8,slashData);
			PutString(34, 95,buf1,0x9D);
			PutString(43, 95,buf2,0x9D);

			char hand1 [] = {"SELECT CARD/FROM HAND$"};
			char hand2 [] = {"VIEWING CARDS/IN HAND$"};
			
			putsprite(5,108,96,45,bottom_leftData);
			if (mode == HUMAN_MODE) PutString(8,114,hand1,0x6B); else PutString(8,114,hand2,0x6B);
			
			WaitFlipCopy();

			int key_pressed = 0;
			
			while(!key_pressed)
			{
				if (!(*KEYS&KEY_DOWN))
				{
				     if (current_card!=end_list)
				     {
				     	if ((current_card - start_index) == 9)
				     	{
				     	     start_index ++;
				     	     end_index ++;
				     	     current_card = end_index;	
				     	     key_pressed = 1;
				     	}
				     	else
				     	{
				     	     current_card++;
				     	     key_pressed = 1;
				     	}
				     }
				}
				
				if (!(*KEYS&KEY_UP))
				{
				     if (current_card!=0)
				     {
				     	if ((current_card - start_index) == 0)
				     	{
				     	     start_index --;
				     	     end_index --;
				     	     current_card = start_index;	
				     	     key_pressed = 1;
				     	}
				     	else
				     	{
				     	     current_card--;
				     	     key_pressed = 1;
				     	}
				     }
				}
				
				if (!(*KEYS&KEY_A))
				{
					key_pressed = 1;
					if (mode!=HUMAN_BROWSE_MODE)
					{
						if (select_data_menu(player,return_card(player,current_card+1)))
						{
							return_flag = current_card + 1;
							exit_flag = 1;
						}
					}
					else
					{
						int card_data = return_card(player,current_card+1);
						print_card_data(player,hand_cards[player][card_data],hand_card_type[player][card_data],MODE_CARD_IN_HAND);
					}
				}

				if (!(*KEYS&KEY_B)&&(mode==HUMAN_BROWSE_MODE))
				{
					key_pressed = 1;
					exit_flag = 1;
					return_flag = 0;
				}

			}
		}
		
		InitializeSprites();
		WaitForVsync();
		CopyOAM();
	}
	else
	{
		int strong_char = 0;
		int strong_char_hp = 0;
		
		int weak_char = 0;
		int weak_char_hp = 0;
		
		int ai_weak_action = 0;
		int weak_action_card = 0;
		
		int ai_strong_action = 0;
		int strong_action_card = 0;
		
		int ai_weak_card = 0;
		int weak_card = 0;
		
		int ai_strong_card = 0;
		int strong_card = 0;
		
		count_hand(player);
		
		for (int i=0; i<hand_size[player]; i++)
		{
			int overall_ai = 0;
			
			int card = return_card(player,i+1);
			
			if (hand_card_type[player][card]!=ACTION_CARD)
			{
				int hp = 0;
				int ai_value = 0;
				
				if (hand_card_type[player][card]==SNK_CARD)
				{
					hp = SNK_chars_hp[hand_cards[player][card]];
				}
				else
				{
					hp = Capcom_chars_hp[hand_cards[player][card]];
				}
				
				for (int j = 0; j<3; j++) ai_value += check_card_ai_value(player,i+1,j);
				
				if (hp>strong_char_hp)
				{
					strong_char_hp = hp;
					strong_char = i+1;
				}

				if (hp<weak_char_hp||!weak_char)
				{
					weak_char_hp = hp;
					weak_char = i+1;
				}
				
				overall_ai = ai_value/3;
			}
			else
			{
				int ai_card = check_card_ai_value(player,i+1,AI_ABILITY);
				
				if (ai_card>ai_strong_action)
				{
					ai_strong_action = ai_card;
					strong_action_card = i+1;
				}
				
				if (ai_card<ai_weak_action||!weak_action_card)
				{
					ai_weak_action = ai_card;
					weak_action_card = i+1;
				}
				
				overall_ai = ai_card;
			}
			
			if (overall_ai>ai_strong_card)
			{
				ai_strong_card = overall_ai;
				strong_card = i+1;
			}

			if (overall_ai<ai_weak_card||!weak_card)
			{
				ai_weak_card = overall_ai;
				weak_card = i+1;
			}
		}

		if (mode == AI_SELECT_STRONG_MODE || mode == AI_SELECT_ENEMY_STRONG_MODE)
		{
			return_flag = strong_char;
		}
		
		if (mode == AI_SELECT_WEAK_MODE || mode == AI_SELECT_ENEMY_WEAK_MODE)
		{
			return_flag = weak_char;
		}
		
		if (mode == AI_SELECT_STRONG_ACTION_MODE) return_flag = strong_action_card;
		if (mode == AI_SELECT_WEAK_ACTION_MODE) return_flag = weak_action_card;
		if (mode == AI_SELECT_STRONG_CARD_MODE || mode == AI_SELECT_ENEMY_STRONG_CARD_MODE) return_flag = strong_card;
		if (mode == AI_SELECT_WEAK_CARD_MODE || mode == AI_SELECT_ENEMY_WEAK_CARD_MODE) return_flag = weak_card;
	}

	return return_flag;
}

const char deploy_msg [] = {"DEPLOYS$"};
const char powered_msg [] = {"BACKS UP$"};
const char uses_ability_msg [] = {"ACTIVATES$"};

const int deploy = 0;
const int power = 1;
const int uses_abilities = 2;

void computer_deploys_card(int mode, int card)
{
	if (!card) return;
	
	int player = COMPUTER_PLAYER;
	
	while (!(*KEYS*KEY_A));	// clear keyboar buffer
	
	InitializeSprites();
	putBG(back_offs[0]);
	putsprite(54,4,120,100,right_paneData);
	view_combat_area (HUMAN_PLAYER,STATUS_NORMAL,0,0,0,0,0);
	view_combat_area (COMPUTER_PLAYER,STATUS_NORMAL,0,0,0,0,0);
	view_stats_area();

	print_area_chars(player,(card-1),PRINT_CARD_IN_PLAY);

	if (mode == deploy)
	{	
		PutString(8,133,(char *)deploy_msg,0x74);
	}

	if (mode == power)
	{
		PutString(8,133,(char *)powered_msg,0x22);
	}

	if (mode == power || mode == deploy)
	{
		char *card_name;
		if (combat_area_card_type[player][card-1] == SNK_CARD)
		{
			card_name = (char *)SNK_names[combat_area[player][card-1]];
		}
		else
		{
			card_name = (char *)capcom_names[combat_area[player][card-1]];
		}
		
		PutString(8,141,card_name,0x9D);
	}
	
	if (mode == uses_abilities)
	{

		char *card_ability_desc;
		
		if (combat_area_card_type[player][card-1] == SNK_CARD)
		{
			card_ability_desc = (char *)SNK_ABILITIES[combat_area[player][card-1]+1];
		}
		else
		{
			card_ability_desc = (char *)CAPCOM_ABILITIES[combat_area[player][card-1]+1];
		}

		PutString(8,133,(char *)uses_ability_msg,0x9D);
		PutString(8,141,card_ability_desc+1,0xE4);
	}
	
	putspriteBG(56+(card-1)*20+6,1+(1-player)*50,18,28,0,selectData);
	PutString(8,125,(char *)player_names[player],0x62);
	
	WaitFlipCopy();

	for (int delay_loop=0; delay_loop<6; delay_loop++)
	{
		int do_nothing = 0;
                for (int delay_max=0; delay_max<30000; delay_max++) do_nothing = 1 - do_nothing;
	}
}

#include "battle_engine.h"

const char error_msg [] = {"CANNOT BACKUP$"};
const char backup_msg [] = {"POWERED$"};
const char canbackup_msg [] = {"BACKUP//WITH$"};
const char withbackup_msg [] = {"WITH BACKUPS$"};
const char deploy_pos_msg [] = {"DEPLOY//HERE$"};

int use_card(int card)
{
	int backup_available = 0;
	int vacant_slots = 0;
	
	int player = HUMAN_PLAYER;
	
	int card_type = hand_card_type[player][card];
	
	if (card_type!=ACTION_CARD)
    	{

        	for (int i=0;i<3;i++) if (combat_area[player][i] == 255) vacant_slots++;
        
        	backup_available = check_for_backups(player,card);

        	if (!vacant_slots&&!backup_available) return 0;

                if (deploy_status[player]&&!backup_available) return 1;

    		InitializeSprites();
    		
    		int exit_flag = 0;
    		int current_choice = 0;
    		int error_flag = 0;
    		int backup_flag = 0;
		    		
    		while (!exit_flag)
    		{
			putBG(back_offs[0]);
			print_left_pane(player,card);
			putsprite(54,4,120,100,right_paneData);
			
			view_combat_area (HUMAN_PLAYER,STATUS_NORMAL,0,0,0,0,0);
			view_combat_area (COMPUTER_PLAYER,STATUS_NORMAL,0,0,0,0,0);
			view_stats_area();
			
			int key_pressed = 0;
			int can_backup_flag = 0;

			if (backup_flag)
			{
				PutString(8,141,(char *)backup_msg,0x22);
				exit_flag = 1;
				key_pressed = 1;
				do_backup(player,card,current_choice);
			}
			else
			{
				if (combat_area[player][current_choice]!=255)
				{
					can_backup_flag = check_backup(player,card,current_choice);
					if (can_backup_flag)
					{
						PutString(8,117,(char *)canbackup_msg,0x75);
						char *card_name;
						
						int card_type2 = combat_area_card_type[player][current_choice];

						if (card_type2 == SNK_CARD)
						{
							card_name = (char *)SNK_names[combat_area[player][current_choice]];
						}
						else
						{
							card_name = (char *)capcom_names[combat_area[player][current_choice]];
						}

						PutString(8,125,card_name,0x9D);
						
						if (card_type == SNK_CARD)
						{
							card_name = (char *)SNK_names[hand_cards[player][card]];
						}
						else
						{
							card_name = (char *)capcom_names[hand_cards[player][card]];
						}
		
						PutString(8,141,card_name,0x9D);
					}
				}
			}

			if (!error_flag&&!backup_flag&&!can_backup_flag&&!deploy_status[player])
			{
				PutString(8,125,(char *)deploy_pos_msg,0x74);
				
				char *card_name;
				
				if (card_type == SNK_CARD)
				{
					card_name = (char *)SNK_names[hand_cards[player][card]];
				}
				else
				{
					card_name = (char *)capcom_names[hand_cards[player][card]];
				}
		
				PutString(8,133,card_name,0x9D);
			}

			if (error_flag)
			{
				PutString(8,141,(char *)error_msg,0x22);
				error_flag = 0;
			}

			putspriteBG(56+current_choice*20+6,51,18,28,0,selectData);

			WaitFlipCopy();
			
			if (backup_flag)
			{
				int any_key = 0;
				
				while (!any_key)
				{
					if (!(*KEYS&KEY_A)||!(*KEYS&KEY_B)) any_key = 1;
				}
			}
			
			while(!key_pressed)
			{

				if (!(*KEYS&KEY_A))
				{
					while (!(*KEYS&KEY_A));
					
					if (combat_area[player][current_choice]!=255)
                			{
                     				if (!check_backup(player,card,current_choice))
                     				{
                     					error_flag = 1;
                     				}
                     				else
                     				{
                          				backup_flag = 1;
                     				}
                			}
                			else
                			{
                				if (!deploy_status[player])
                				{
							place_card(current_choice,player,card);
							player_deployed_card = current_choice + 1;
                     					set_deploy_status(player);
                     				}
						exit_flag = 1;
                			}
                			
                			key_pressed = 1;
				}

				if (!(*KEYS&KEY_LEFT))
				{
					while (!(*KEYS&KEY_LEFT));
					if (current_choice != 0) current_choice --;
					key_pressed = 1;
				}

				if (!(*KEYS&KEY_RIGHT))
				{
					while (!(*KEYS&KEY_RIGHT));
					if (current_choice != 2) current_choice ++;
					key_pressed = 1;
				}
				
				if (!(*KEYS&KEY_B))
				{
					while (!(*KEYS&KEY_B));
					key_pressed = 1;
					exit_flag = 1;
				}
			}
		}
	}    
	else
	{
		if (!action_cards_disabled[player]) use_special_card(player,card);
	}
	
	return 0;
}

const char use_data_001 [] = {"USE$"};
const char use_data_002 [] = {"DATA$"};
const char * use_data_menu_choices [] = {use_data_001,use_data_002};
	
void use_data_menu(int player, int card)
{
	int exit_flag = 0;
	int choice = 0;
	
	InitializeSprites();

	while (!(*KEYS&KEY_A));		// clear keyboard buffer
	
	while (!exit_flag)
	{
		putBG(back_offs[0]);
		putsprite(5,4,96,100,left_paneData);
		print_right_pane(player,card);
		
		print_menu(16,14,2,choice,0x9D,(char **)use_data_menu_choices);

		WaitFlipCopy();
		
		int key_pressed = 0;
		
		while(!key_pressed)
		{
			if (!(*KEYS&KEY_UP))
			{
				//while (!(*KEYS&KEY_UP));
				if (choice != 0) choice--;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_DOWN))
			{
				//while (!(*KEYS&KEY_DOWN));
				if (choice != 1) choice++;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_A))
			{
				while (!(*KEYS&KEY_A));
				if (choice == 1) exit_flag = 3;
				if (choice == 0) exit_flag = 2;
				key_pressed = 1;
			}
			
			if (!(*KEYS&KEY_B))
			{
				while (!(*KEYS&KEY_B));
				key_pressed = 1;
				exit_flag = 1;
			}
		}
		
		if (exit_flag == 3)
		{
			print_card_data(HUMAN_PLAYER,hand_cards[HUMAN_PLAYER][card],hand_card_type[HUMAN_PLAYER][card],MODE_CARD_IN_HAND);
			exit_flag = 0;
		}
	}
	
	if (exit_flag == 2) 
	{
		use_card(card);
	}
}

void view_hand(int player)
{
	int start_index = 0;
	int end_index = 9;
	int current_card = 0;
	
	count_hand(player);
	
	int hand_count = hand_size[player];
	if (hand_count < 10) end_index = hand_count - 1;
	int end_list = hand_count - 1;

	if (player == HUMAN_PLAYER)
	{
		int exit_flag = 0;
		count_hand(player);
		if (!hand_size[player]) exit_flag = 1;
		
		while (!exit_flag)	
		{

			putBG(back_offs[0]);
			putsprite(5,4,96,100,left_paneData);
			
			for (int i=start_index;i<end_index+1;i++)
			{
				int card = return_card(player,i+1);
				
				char card_set3[] = {"ACT    $"};
				char *ptr;
				
				if (hand_card_type[player][card] == SNK_CARD) ptr = (char *)SNK_names[hand_cards[player][card]];
				if (hand_card_type[player][card] == CAPCOM_CARD) ptr = (char *)capcom_names[hand_cards[player][card]];
				if (hand_card_type[player][card] == ACTION_CARD) ptr = card_set3;
				
				if (hand_card_type[player][card] != SNK_CARD&&hand_card_type[player][card] != CAPCOM_CARD) ptr = card_set3;
				
				if (hand_card_type[player][card] != CAPCOM_CARD && hand_card_type[player][card] != SNK_CARD)
				{
					itoa_lpad(ptr+3,4,hand_cards[player][card],' ');
					ptr[7]='$';
				}

				if (hand_cards[player][card]!=255) PutString(16,14+(i-start_index)*8,ptr,0x9D);
			
				if (current_card == i) putspriteBG(1,13+(i-start_index)*8,28,18,0,small_handData);
				if (end_index!=end_list) putsprite(25,94,10,5,down_ptrData);
				if (start_index!=0) putsprite(25,8,10,5,up_ptrData);
			}
			
			int card = return_card(player,current_card+1);
			
			print_right_pane(player,card);
			view_stats_area();
			
			char buf1[4];
			char buf2[4];
			
			itoa_lpad(buf1,3,current_card+1,' ');
			itoa_lpad(buf2,3,hand_count,' ');
			
			buf1[3] = '$';
			buf2[3] = '$';
			
			putsprite(42,95,8,8,slashData);
			PutString(34, 95,buf1,0x9D);
			PutString(43, 95,buf2,0x9D);
			
			WaitFlipCopy();

			int key_pressed = 0;
			
			while(!key_pressed)
			{
				if (!(*KEYS&KEY_DOWN))
				{
				     if (current_card!=end_list)
				     {
				     	if ((current_card - start_index) == 9)
				     	{
				     	     start_index ++;
				     	     end_index ++;
				     	     current_card = end_index;	
				     	     key_pressed = 1;
				     	}
				     	else
				     	{
				     	     current_card++;
				     	     key_pressed = 1;
				     	}
				     }
				}
				
				if (!(*KEYS&KEY_UP))
				{
				     if (current_card!=0)
				     {
				     	if ((current_card - start_index) == 0)
				     	{
				     	     start_index --;
				     	     end_index --;
				     	     current_card = start_index;	
				     	     key_pressed = 1;
				     	}
				     	else
				     	{
				     	     current_card--;
				     	     key_pressed = 1;
				     	}
				     }
				}
				
				if (!(*KEYS&KEY_B))
				{
					key_pressed = 1;
					exit_flag = 1;	
				}
				
				if (!(*KEYS&KEY_A))
				{
					key_pressed = 1;
					exit_flag = 2;
				}

			}
			
			if (exit_flag == 2)
			{
				use_data_menu(player,return_card(player,current_card+1));

				if (!deploy_status[player]) exit_flag = 0; else exit_flag = 1;

				start_index = 0;
				end_index = 9;
				current_card = 0;
				count_hand(player);
				hand_count = hand_size[player];
				if (hand_count < 10) end_index = hand_count - 1;
				end_list = hand_count - 1;
			}
		}
		
		InitializeSprites();
		WaitForVsync();
		CopyOAM();
	}
}

const char unite_01 [] = {"UNITED$"};
const char unite_02 [] = {"SINGLE$"};
const char *united_menu [] = {unite_01,unite_02};
const char computer_blocks [] = {"BLOCKS/ATTACKS$"};
const char computer_does_not_block [] = {"HAS NO/BLOCKERS$"};

void do_attack()
{
	int player = HUMAN_PLAYER;
	int current_choice = 0;
	int exit_flag = 0;
	int current_attack_status[3];
	int united_attackers[2];
	int attack_pos1 = 0;
    	int attack_pos2 = 0;
    	int make_united_attack = 0;
    	int attackers[3];
    	int num_attackers  = 0;
    
    	for (int i=0; i<3; i++)
    	{
		num_attackers += can_attack(player,i);
        	current_attack_status[i] = 0;
        	attackers[i] = 0;
    	}
    	
    	united_attackers[0] = 0;
    	united_attackers[1] = 0;

	if (!num_attackers) return;
	
	int attacker_flag = 0;
    	int current_num_attackers = 0;
	int united_choice = 0;
	int united_choice_done = 0;
		
	while(!exit_flag)
	{
		int key_pressed = 0;

		InitializeSprites();
		
		putBG(back_offs[0]);
		
		putsprite(54,4,120,100,right_paneData);

		view_combat_area (HUMAN_PLAYER,STATUS_ATTACKING,attackers[0],attackers[1],attackers[2],united_attackers[0],united_attackers[1]);
		view_combat_area (COMPUTER_PLAYER,STATUS_NORMAL,0,0,0,0,0);
		view_stats_area();
		
		print_area_chars(player,current_choice,PRINT_CARD_IN_PLAY);
		
		if (player == COMPUTER_PLAYER)
		{
			putspriteBG(56+current_choice*20+6,1+(1-player)*50,18,28,0,selectData);
		}
		else
		{
			putspriteBG(56+current_choice*20+6,1+(1-player)*50+40,18,28,0,pointerData);
		}

		if (attack_pos1&&attack_pos2&&current_num_attackers>=2&&player_sp[HUMAN_PLAYER]>=5&&!make_united_attack&&current_attack_status[attack_pos1-1]&&current_attack_status[attack_pos2-1])
                {
                	if (!united_choice_done)
                	{
                		InitializeSprites();
                		print_menu(16,14,2,united_choice,0x9D,(char **)united_menu);
                	}
                }

		WaitFlipCopy();

		if (attack_pos1&&attack_pos2&&current_num_attackers>=2&&player_sp[HUMAN_PLAYER]>=5&&!make_united_attack&&current_attack_status[attack_pos1-1]&&current_attack_status[attack_pos2-1])
		{
			int united_key = 0;
			
			while (!united_key&&!united_choice_done)
			{
				if (!(*KEYS&KEY_UP))
				{
					if (united_choice != 0) united_choice--;
					united_key = 1;
				}

				if (!(*KEYS&KEY_DOWN))
				{
					if (united_choice != 1) united_choice++;
					united_key = 1;
				}

				if (!(*KEYS&KEY_A))
				{
					while (!(*KEYS&KEY_A));
					if (united_choice == 0) make_united_attack = 1;
					if (united_choice == 1) make_united_attack = 0; 
					united_choice_done = 1;

					if (make_united_attack)
					{
						united_attackers[0] = attack_pos1;
						united_attackers[1] = attack_pos2;
					}
					else
					{
						united_attackers[0] = 0;
						united_attackers[1] = 0;
					}
				}

				key_pressed = 1;
			}
		}

		while(!key_pressed)
		{
			if (!(*KEYS&KEY_START))
			{
				while (!(*KEYS&KEY_START));
				key_pressed = 1;
				exit_flag = 1;
			}
			
			if (!(*KEYS&KEY_UP))
			{
				while (!(*KEYS&KEY_UP));
				if (!player) player = 1 - player;
				key_pressed = 1;
			}
			
			if (!(*KEYS&KEY_DOWN))
			{
				while (!(*KEYS&KEY_DOWN));
				if (player) player = 1 - player;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_LEFT))
			{
				while (!(*KEYS&KEY_LEFT));
				if (current_choice!=0) current_choice--;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_RIGHT))
			{
				while (!(*KEYS&KEY_RIGHT));
				if (current_choice!=2) current_choice++;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_B))
			{
				while (!(*KEYS&KEY_B));
				key_pressed = 1;
				exit_flag = 1;
				for (int i=0; i<3; i++) current_attack_status[i] = 0;
				for (int i=0; i<2; i++) united_attackers[i] = 0;
			}

			if (!(*KEYS&KEY_A))
			{
				while (!(*KEYS&KEY_A));
				key_pressed = 1;
				
				if (player == HUMAN_PLAYER)
				{
					if (combat_area[player][current_choice]!=255)
					{
						if (can_attack(player,current_choice))
                     				{
							if (current_attack_status[current_choice])
							{
								current_attack_status[current_choice] = 0;
								current_num_attackers--;
								
								if (current_num_attackers == 2)
                               					{
                                    					if (make_united_attack)
                                    					{
                                         					if (attack_pos1 == current_choice + 1 || attack_pos2 == current_choice+1)
                                         					{
                                              						make_united_attack = 0;
                                              						united_choice_done = 0;
                                              
                                              						if (attack_pos1 == current_choice + 1)
                                              						{
                                                   						attack_pos1 = attack_pos2;
                                         						}
                                         						
                                         						attack_pos2 = 0;	
                                              					}
                                    					}
                                    					else
                                    					{
                                         					if (attack_pos1 == current_choice + 1)
                                         					{
                                              						attack_pos1 = attack_pos2;
                                         						attack_pos2 = 0;
                                         					}
										else
                                         					{
                                         						if (attack_pos2 == current_choice + 1)
                                         						{
                                         							attack_pos2 = 0;
                                         						}
                                         					}
                                    					}
                               					}

                               					if (current_num_attackers == 1)
                               					{
									make_united_attack = 0;
									
                                         				if (attack_pos1 == current_choice+1)
                                         				{
										for (int i=0;i<3;i++)
                                         					{
                                         						if (current_attack_status[i]&&(i+1)!=attack_pos1) attack_pos1 = i+1;
										}
                                         				}

									attack_pos2 = 0;
                               					}
							}
							else
							{
								current_attack_status[current_choice] = 1;
								current_num_attackers++;
								
								if (current_num_attackers == 2)
                               					{
                                    						attack_pos2 = current_choice + 1;
                                    						united_choice_done = 0;
                               					}
                               					else
                               					{
                                    					if (current_num_attackers == 3)
                                    					{
                                         					if (!make_united_attack)
                                         					{
                                         						attack_pos2 = current_choice + 1;
                                         						united_choice_done = 0;
                                         					}
                                    					}
                                    					else
                                    					{
                                         					make_united_attack = 0;
                                         					attack_pos1 = current_choice + 1;
                                         					attack_pos2 = 0;
                                    					}
                               					}
							}
						}
					}
					
					if (current_attack_status[current_choice])
					{
						attackers[current_choice] = current_choice + 1;
					}
					else
					{
						attackers[current_choice] = 0;
					}
					
					if (attack_pos1&&attack_pos2&&make_united_attack)
					{
						united_attackers[0] = attack_pos1;
						united_attackers[1] = attack_pos2;
					}
					else
					{
						united_attackers[0] = 0;
						united_attackers[1] = 0;
					}
				}
			}
		}
	}

	for (int i=0; i<3; i++) if (current_attack_status[i]) attacker_flag ++;
	if (united_attackers[0]) attacker_flag++;
	
	if (attacker_flag)
	{
	    	if (!make_united_attack)
    		{
			int attack_pos[3];
                	int blockers = 0;
                	
        		for (int i=0; i<3; i++)
        		{
                		if (current_attack_status[i]) attack_pos[i] = i+1; else attack_pos[i] = 0;
        		}
        		
        		if (!(attack_pos[0]||attack_pos[1]||attack_pos[2])) return;

        		do_computer_block(attack_pos[0],attack_pos[1],attack_pos[2],0,0);
                
			int disp_blockers[3];
			
			for (int i=0; i<3; i++)  disp_blockers[i] = 0;
			
        		for (int i=0; i<3; i++)
        		{
                		if (attack_pos[i])
                		{
                			if (computer_block_pos[i]!=255&&computer_block_pos[i])
                			{
                				blockers++;
                				disp_blockers[i] = attack_pos[i];
                				swap_cards(COMPUTER_PLAYER,attack_pos[i],computer_block_pos[i]);
                				int temp = computer_block_pos[i];
                				int found = 0;
                				for (int j=0; j<3; j++)
                				{
                					if (!found)
                					{
                						if (computer_block_pos[j]==attack_pos[i]&&i!=j)
                						{
                							computer_block_pos[j] = temp;
                							found = 1;
                						}
                					}
                				}

                				computer_block_pos[i] = attack_pos[i];
                			}
                		}
        		}
        		
			InitializeSprites();
			putBG(back_offs[0]);
			putsprite(5,4,96,100,left_paneData);
			putsprite(54,4,120,100,right_paneData);
			
			if (blockers)
			{
				PutString(16,14,(char *)player_names[COMPUTER_PLAYER],0xF3); 
				PutString(16,22,(char *)computer_blocks,0x9D); 
			}
			else
			{
				PutString(16,14,(char *)player_names[COMPUTER_PLAYER],0xF3); 
				PutString(16,22,(char *)computer_does_not_block,0x9D);
			}

			view_combat_area (HUMAN_PLAYER,STATUS_ATTACKING,attack_pos[0],attack_pos[1],attack_pos[2],united_attackers[0],united_attackers[1]);
			view_combat_area (COMPUTER_PLAYER,STATUS_BLOCKING,disp_blockers[0],disp_blockers[1],disp_blockers[2],0,0);
			
			view_stats_area();
			
			WaitFlipCopy();

			int key_pressed_2 = 0;
			
			while (!key_pressed_2)
			{
				if (!(*KEYS&KEY_A))
				{
					while (!(*KEYS&KEY_A));
					key_pressed_2 = 1;
				}
			}
			
			for (int i=0; i<3; i++) if (attack_pos[i]&&player_hp[COMPUTER_PLAYER]) card_battle(player,attack_pos[i],computer_block_pos[i]);
    		}
    		else
    		{
    			int blockers = 0;
    			int disp_blockers[2];
    			
    			for (int i=0;i<2;i++) disp_blockers[i] = 0;
    			
        		if (current_num_attackers == 3)
        		{
                		int single_attacker = 0;
                		for (int i=0;i<3;i++) if (i+1!=attack_pos1&&i+1!=attack_pos2) single_attacker = i+1;
                     
                		do_computer_block(single_attacker,0,0,attack_pos1,attack_pos2);
                		if (computer_block_pos[1]!=255)
                		{
                			blockers ++;
                			disp_blockers[1] = attack_pos1;
					swap_cards(COMPUTER_PLAYER,computer_block_pos[1],attack_pos1);
                     			computer_block_pos[1] = attack_pos1;
                		}
                
                		if (computer_block_pos[0]!=255)
                		{
                			blockers ++;
                			disp_blockers[0] = single_attacker;
					swap_cards(COMPUTER_PLAYER,computer_block_pos[0],single_attacker);
                     			computer_block_pos[0] = single_attacker;
                		}

				InitializeSprites();
				putBG(back_offs[0]);
				putsprite(5,4,96,100,left_paneData);
				putsprite(54,4,120,100,right_paneData);
				
				if (blockers) 
				{
					PutString(16,14,(char *)player_names[COMPUTER_PLAYER],0xF3); 
					PutString(16,22,(char *)computer_blocks,0x9D); 
				}
				else
				{
					PutString(16,14,(char *)player_names[COMPUTER_PLAYER],0xF3); 
					PutString(16,22,(char *)computer_does_not_block,0x9D);
				}
				
				view_combat_area (HUMAN_PLAYER,STATUS_ATTACKING,single_attacker,0,0,attack_pos1,attack_pos2);
				view_combat_area (COMPUTER_PLAYER,STATUS_BLOCKING,disp_blockers[0],disp_blockers[1],0,0,0);
				view_stats_area();
				
				WaitFlipCopy();

				int key_pressed_2 = 0;
			
				while (!key_pressed_2)
				{
					if (!(*KEYS&KEY_A))
					{
						while (!(*KEYS&KEY_A));
						key_pressed_2 = 1;
					}
				}

                		united_attack(player,attack_pos1,attack_pos2,computer_block_pos[1]);
                		if (player_hp[COMPUTER_PLAYER]) card_battle(player,single_attacker,computer_block_pos[0]);
        		}
        		else
        		{
        			int blockers = 0;
                		do_computer_block(0,0,0,attack_pos1,attack_pos2);

                		if (computer_block_pos[1]!=255)
                		{
                			blockers ++;
					swap_cards(COMPUTER_PLAYER,computer_block_pos[1],attack_pos1);
                     			computer_block_pos[1] = attack_pos1;
                		}

				InitializeSprites();
				putBG(back_offs[0]);
			
				putsprite(5,4,96,100,left_paneData);
				putsprite(54,4,120,100,right_paneData);
				
				if (blockers) 
				{
					PutString(16,14,(char *)player_names[COMPUTER_PLAYER],0xF3); 
					PutString(16,22,(char *)computer_blocks,0x9D); 
				}
				else
				{
					PutString(16,14,(char *)player_names[COMPUTER_PLAYER],0xF3); 
					PutString(16,22,(char *)computer_does_not_block,0x9D);
				}
				
				view_combat_area (HUMAN_PLAYER,STATUS_ATTACKING,0,0,0,attack_pos1,attack_pos2);
				view_combat_area (COMPUTER_PLAYER,STATUS_BLOCKING,computer_block_pos[1],0,0,0,0);
				view_stats_area();
				
				WaitFlipCopy();

				int key_pressed_2 = 0;
			
				while (!key_pressed_2)
				{
					if (!(*KEYS&KEY_A))
					{
						while (!(*KEYS&KEY_A));
						key_pressed_2 = 1;
					}
				}

                		united_attack(player,attack_pos1,attack_pos2,computer_block_pos[1]);
        		}
    		}
	}
	
	InitializeSprites();	
	WaitForVsync();
	CopyOAM();
}

void view_combat_area_chars()
{
	int player = HUMAN_PLAYER;
	int current_choice = 0;
	int exit_flag = 0;
	while(!exit_flag)
	{
		disp_combat_area(player,current_choice);
		if (num_backups[player][current_choice]&&combat_area[player][current_choice]!=255) PutString(8,141,(char *)withbackup_msg,0x75);
		WaitFlipCopy();

		int key_pressed = 0;
		while(!key_pressed)
		{
			if (!(*KEYS&KEY_UP))
			{
				while (!(*KEYS&KEY_UP));
				if (!player) player = 1 - player;
				key_pressed = 1;
			}
			
			if (!(*KEYS&KEY_DOWN))
			{
				while (!(*KEYS&KEY_DOWN));
				if (player) player = 1 - player;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_LEFT))
			{
				while (!(*KEYS&KEY_LEFT));
				if (current_choice!=0) current_choice--;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_RIGHT))
			{
				while (!(*KEYS&KEY_RIGHT));
				if (current_choice!=2) current_choice++;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_A))
			{
				while (!(*KEYS&KEY_A));
				
				if (combat_area[player][current_choice]!=255)
				{
					print_card_data(player,current_choice,0,MODE_CARD_IN_PLAY);
				}
				
				key_pressed = 1;
			}
			
			if (!(*KEYS&KEY_B))
			{
				while (!(*KEYS&KEY_B));
				key_pressed = 1;
				exit_flag = 1;
			}
		}
	}
}

const char cannot_use_ability_msg [] = {"CANNOT USE/ABILITY$"};
void use_char_abilities()
{
	int player = HUMAN_PLAYER;
	int current_choice = 0;
	int exit_flag = 0;
	int error_flag = 0;
	
	while(!exit_flag)
	{
		disp_combat_area(player,current_choice);

		if (error_flag)
		{
			if (error_flag == 1) PutString(8,133,(char *)cannot_use_ability_msg,0xAA);
			error_flag = 0;
		}

		WaitFlipCopy();

		int key_pressed = 0;
		
		while(!key_pressed)
		{
			if (!(*KEYS&KEY_LEFT))
			{
				while (!(*KEYS&KEY_LEFT));
				if (current_choice!=0) current_choice--;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_RIGHT))
			{
				while (!(*KEYS&KEY_RIGHT));
				if (current_choice!=2) current_choice++;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_A))
			{
				while (!(*KEYS&KEY_A));
				if (combat_area[player][current_choice]!=255)
				{
					if (combat_ready[player][current_choice]==1&&!attack_status[player][current_choice]&&!freeze_status[player][current_choice]&&!counter_status[player][current_choice]&&!square_abilities_disabled[player])
					{
						use_special_abilities(player,current_choice);
						exit_flag = 1;
					}
					else
					{
						error_flag = 1;
					}
				}
				key_pressed = 1;
			}
			
			if (!(*KEYS&KEY_B))
			{
				while (!(*KEYS&KEY_B));
				key_pressed = 1;
				exit_flag = 1;
			}
		}
	}
}

#include "computer_attack.h"

const char main_menu_01 [] = {"VIEW HAND$"};
const char main_menu_02 [] = {"ATTACK$"};
const char main_menu_03 [] = {"ABILITY$"};
const char main_menu_04 [] = {"INFO$"};
const char main_menu_05 [] = {"SEARCH$"};
const char main_menu_06 [] = {"COMBAT AREA$"};
const char main_menu_07 [] = {"END$"};
const char * main_menu_choices [] = {main_menu_01,main_menu_02,main_menu_03,main_menu_04,main_menu_05,main_menu_06,main_menu_07};

const char main_menu_help01 [] = {"Select or view/cards in your/hand$"};
const char main_menu_help02 [] = {"Perform an/attack$"};
const char main_menu_help03 [] = {"Use ability of/a character in/your combat/area$"};
const char main_menu_help04 [] = {"Learn the game$"};
const char main_menu_help05 [] = {"View discarded/cards$"};
const char main_menu_help06 [] = {"Look at cards/in combat area$"};
const char main_menu_help07 [] = {"End your turn$"};
const char * main_menu_choices_help [] = {main_menu_help01,main_menu_help02,main_menu_help03,main_menu_help04,main_menu_help05,main_menu_help06,main_menu_help07};

void main_menu_visuals(void)
{
	InitializeSprites();
	putBG(back_offs[0]);
	putsprite(54,4,120,100,right_paneData);
	disp_combat_area(HUMAN_PLAYER,255);		
}

void search_menu(void)
{
	int choice = 0;
	int end_flag = 0;

	char *search_menu_choices = (char *)player_names;
	
	while (!end_flag)
	{
		main_menu_visuals();
		char search_label [] = {"SELECT DISCARD/PILE TO VIEW$"};
		PutString(8,14,search_label,0x6B);
		print_menu(16,40,2,choice,0x9D,(char **)search_menu_choices);

		WaitFlipCopy();	
		
		int key_pressed = 0;
		int made_choice = 0;
		
		while(!key_pressed)
		{
			if (!(*KEYS&KEY_UP))
			{
				if (choice != 0) choice--;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_DOWN))
			{
				if (choice != 1) choice++;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_A))
			{
				while (!(*KEYS&KEY_A));
				made_choice = 1;
				key_pressed = 1;
			}
			
			if (!(*KEYS&KEY_B))
			{
				while (!(*KEYS&KEY_B));
				key_pressed = 1;
				end_flag = 1;
			}
		}
		
		if (made_choice) select_card_from_discard_pile(choice,ANY_CARD,HUMAN_BROWSE_MODE);
	}
}

void main_menu(void)
{
	int choice = 0;
	int end_flag = 0;
		
	while (!end_flag)
	{
		if (player_deployed_card)
		{
			main_menu_visuals();		
			WaitFlipCopy();
			if (!triangle_abilities_disabled[HUMAN_PLAYER]) do_special_effects(HUMAN_PLAYER,player_deployed_card - 1);
			player_deployed_card = 0;
		}

		int key_pressed = 0;
		int made_choice = 0;
		
		if (!player_hp[COMPUTER_PLAYER]||!player_hp[HUMAN_PLAYER])
		{
			key_pressed = 1;
		}
		else
		{
			main_menu_visuals();
			print_menu(16,14,7,choice,0x9D,(char **)main_menu_choices);
			putsprite(5,108,96,45,bottom_leftData);
			PutString(8,114,(char *)main_menu_choices_help[choice],0x74);
			WaitFlipCopy();
		}

		while(!key_pressed)
		{
			if (!(*KEYS&KEY_UP))
			{
				if (choice != 0) choice--;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_DOWN))
			{
				if (choice != 6) choice++;
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_A))
			{
				while (!(*KEYS&KEY_A));
				
				if (choice == 6)
				{
					end_flag = 1;
                			do_end_of_turn_effects(HUMAN_PLAYER);
				}
				
				made_choice = 1;
				key_pressed = 1;
			}

			/*			
			// CHEAT CODE / DEBUG MODE
			if (!(*KEYS&KEY_L)&&!(*KEYS&KEY_R))
			{
				select_card_from_hand(COMPUTER_PLAYER,HUMAN_BROWSE_MODE);
				key_pressed = 1;
			}

			if (!(*KEYS&KEY_START)&&!(*KEYS&KEY_SELECT))
			{
				select_card_from_deck(COMPUTER_PLAYER,ANY_CARD,HUMAN_BROWSE_MODE);
				key_pressed = 1;
			}
			*/
		}

		if (made_choice&&(choice == 5)) view_combat_area_chars();
		if (made_choice&&(choice == 4)) search_menu();
		if (made_choice&&(choice == 2)) use_char_abilities();
		if (made_choice&&(choice == 1))
		{
			if (!attack_disabled[HUMAN_PLAYER]) do_attack();
		}

		if (made_choice&&(choice == 0)) view_hand(HUMAN_PLAYER);

		if (!player_hp[COMPUTER_PLAYER]||!player_hp[HUMAN_PLAYER]) end_flag = 1;
	}
}

const char win [] = {"WINS$"};

void begin_card_battle(void)
{
	DMAFastCopy((void *)Game_pal,(void *)BGPaletteMem,256);
	REG_TM1CNT = 0;
	REG_DMA2CNT = 0;
	//bgm_length = 0;
	REG_IME = 0x00;
			
	ai_scale_factor = 2;

	putBG(back_offs[0]);
	WaitForVsync();
	Flip();
	     		
	int current_player = HUMAN_PLAYER;
     		
    	build_random_deck(HUMAN_PLAYER);
    	build_random_deck(COMPUTER_PLAYER);

	init_game(3000,0,3000,0);
	init_ai();
						    			
	//debug
	//hand_cards[HUMAN_PLAYER][0] = 28;
	//hand_card_type[HUMAN_PLAYER][0] = SNK_CARD;
	//hand_cards[HUMAN_PLAYER][0] = 7;
	//hand_card_type[HUMAN_PLAYER][0] = SNK_CARD;
	//combat_decks[HUMAN_PLAYER][40] = 24;
	//card_types[HUMAN_PLAYER][40] = 0;
	//end debug
			
    	while (player_hp[HUMAN_PLAYER]&&player_hp[COMPUTER_PLAYER])
    	{
        	if (!draw_disabled[current_player])
        	{
        		draw_card(current_player);
			if (current_player == HUMAN_PLAYER) draw_card_animated(current_player);
        	}
        			
        	clear_deploy_status(current_player);
        	clear_attack_status(current_player);
				
		InitializeSprites();
		WaitForVsync();
		CopyOAM();
				
		putBG(back_offs[0]);
		putsprite(54,4,120,100,right_paneData);
		disp_combat_area(current_player,255);		
		WaitForVsync();
		Flip();
				
		for (int i=0;i<3;i++)
		{
			if ((freeze_status[current_player][i]==1)&&combat_area[current_player][i]!=255) defrost(current_player,i);
			if (combat_area[current_player][i]!=255&&(combat_ready[current_player][i]==0||combat_ready[current_player][i]==254)) twinkle(current_player,i);
		}
				
        	clear_freeze_status(current_player);
        	clear_backup_status(current_player);
        	enable_action_cards(current_player);
        	make_combat_ready(current_player);
        
        	if (current_player == HUMAN_PLAYER)
        	{
                	do_start_of_turn_effects(HUMAN_PLAYER);
        		main_menu();
        	}
        	else
        	{
                	do_start_of_turn_effects(COMPUTER_PLAYER);
                	do_computer_view_hand();

                	if (computer_backup)
                	{
                		computer_deploys_card(power,computer_backup_card);
                		computer_backup = 0;
                		computer_backup_card = 0;
                	}

                	if (deploy_status[COMPUTER_PLAYER])
                	{
                		computer_deploys_card(deploy,computer_deployed_card);
				if (!triangle_abilities_disabled[COMPUTER_PLAYER]) do_special_effects(COMPUTER_PLAYER,computer_deployed_card-1);
				InitializeSprites();
				WaitForVsync();
				CopyOAM();
				computer_deployed_card = 0;
                	}

			if (player_hp[HUMAN_PLAYER]) do_computer_use_card_abilities();
                	if (!attack_disabled[COMPUTER_PLAYER]) do_computer_attack();
                	if (player_hp[HUMAN_PLAYER]) do_computer_view_hand();

                	if (!deploy_status[COMPUTER_PLAYER])
                	{
                		if (player_hp[HUMAN_PLAYER])
                		{
                			if (!attack_disabled[COMPUTER_PLAYER])
                			{
                				do_computer_replace_card();
                				if (deploy_status[COMPUTER_PLAYER])
                				{
                					computer_deploys_card(deploy,computer_deployed_card);
                        				if (!triangle_abilities_disabled[COMPUTER_PLAYER]) do_special_effects(COMPUTER_PLAYER,computer_deployed_card-1);
                        				computer_deployed_card = 0;
                				}
                			}
                		}
                	}

			if (player_hp[HUMAN_PLAYER])
			{
				do_computer_use_card_abilities();
				if (!attack_disabled[COMPUTER_PLAYER]) do_computer_attack();	// check for killer instinct
			}
					
			if (player_hp[HUMAN_PLAYER])
			{
				do_computer_use_card_abilities();
				if (!attack_disabled[COMPUTER_PLAYER]) do_computer_attack();	// check for killer instinct
			}
			
			InitializeSprites();
			WaitForVsync();
			CopyOAM();
			disp_combat_area(current_player,255);
			WaitForVsync();
			Flip();
                	do_end_of_turn_effects(current_player);

			for (int delay_loop=0; delay_loop<3; delay_loop++)
			{
				int do_nothing = 0;
                		for (int delay_max=0; delay_max<30000; delay_max++) do_nothing = 1 - do_nothing;
			}
        	}
        
        	if (player_hp[COMPUTER_PLAYER]&&player_hp[HUMAN_PLAYER])
        	{
        		if (attack_disabled[current_player]) attack_disabled[current_player]--;
        		if (draw_disabled[current_player]) draw_disabled[current_player]--;
        		if (action_cards_disabled[current_player]) action_cards_disabled[current_player]--;
        				
                	current_player = 1 - current_player;               			
        	}
    	}
    			
	putBG(back_offs[0]);
	putsprite(36,4,96,100,left_paneData);
	putsprite(36,108,96,45,bottom_leftData);
			
	InitializeSprites();

	setup_face_sprite(88,22,0);
	setup_face_sprite_data(0,(void *)player_faces[0]);
						
	if (!player_hp[HUMAN_PLAYER])
	{
		setup_face_sprite_data(0,(void *)player_faces[1]);
		PutString(39,118,(char *)player_names[COMPUTER_PLAYER],0xF3);
	}
	else
	{
		setup_face_sprite_data(0,(void *)player_faces[0]);
		PutString(39,118,(char *)player_names[HUMAN_PLAYER],0xF3);
	}

	PutString(39,126,(char *)win,0x9D);

	int key_pressed = 0;
	WaitFlipCopy();
						
	while (!key_pressed)
	{
		if (!(*KEYS&KEY_START))
		{
			while (!(*KEYS&KEY_START)||!(*KEYS&KEY_SELECT)||!(*KEYS&KEY_A)||!(*KEYS&KEY_B)||!(*KEYS&KEY_L)||!(*KEYS&KEY_R));
			key_pressed = 1;
		}
	}				
}

const char press_start [] = {"PRESS START$"};
//const char gbax1[] = {"GBAX$"};
//const char gbax2[] = {"2004$"};

//#include "intro_gfx.h"
#include "gbaemu.h"

//#define WSCNT  *(volatile u16 *)0x04000204
//#define WSCNT_ROM31 0x0014
int main()
{
       	SetMode(MODE_4 | BG2_ENABLE | OBJ_ENABLE | OBJ_MAP_1D); 	//set mode 4 and enable sprites and 1d mapping
	DMAFastCopy((void *)Game_pal,(void *)OBJPaletteMem,256);	//Objects and Backgrounds have same Palette
	DMAFastCopy((void *)Game_pal,(void *)BGPaletteMem,256);
	
	//setfont(6,8,font6x8_WIDTH,font6x8_WIDTH,font6x8Data);

	int counter = 0;
	int put_msg = 0;
	//int intro_count = 0;
	//int do_nothing = 0;
	//int delay_max = 10000;

/*
	unsigned int OLD_ISR;
	OLD_ISR = REG_INTR_HANDLER;
	
	//create custom interrupt handler for vblank
	REG_IME = 0x00;
	REG_INTR_HANDLER  = (unsigned int)isr;
	REG_IE |= INT_VBLANK;
	REG_DISPSTAT |= 0x08;
	WSCNT = WSCNT_ROM31;
	
	unsigned char *sound_ptr;
    	unsigned int len;
	
	samples = find_first_gbfs_file((const void*)find_first_gbfs_file);
	if (!samples) while(1);

    	sound_ptr = (unsigned char *) gbfs_get_obj(samples, "duhast.pcm", &len);
	const SAMPLE duhast_bgm =
	{
		(s8*)sound_ptr,
		len,
	};

	REG_IME = 0x01;
	init_bgm_system();
	play_bgm(&duhast_bgm,8000);
*/
/*
	setfont(12,16,font12x16_WIDTH,font12x16_WIDTH,font12x16Data);

	while (intro_count<42)
	{
		if (!(*KEYS&KEY_START)) break;
		
		putBG(intro_01[intro_count]);
		
		if (intro_count<=18)
		{
			PutString(2,72,(char *)gbax1,0x9D);
			PutString(94,72,(char *)gbax2,0x9D);
		}

		if (intro_count<=39&&intro_count>21)
		{
			PutString(2,72,(char *)gbax1,0x9D);
			PutString(94,72,(char *)gbax2,0x9D);
		}

		if (intro_count > 40)
		{
			putspriteBG(32,120,112,10,0x00,gbaemuData);
		}
		
		WaitForVsync();
		Flip();
		intro_count++;
		
		for (int i=0; i<delay_max; i++) { do_nothing = 1 - do_nothing; }
		
		if (intro_count > 39) delay_max = 30000; else delay_max = 10000;

		if (intro_count > 18 && intro_count<21)	delay_max = 30000;
		if (intro_count == 21)
		{
			for (int i=0; i<delay_max*3; i++) { do_nothing = 1 - do_nothing; }
			for (int i=0; i<delay_max*3; i++) { do_nothing = 1 - do_nothing; }
			for (int i=0; i<delay_max*3; i++) { do_nothing = 1 - do_nothing; }
		}
	}


	for (int i=0; i<delay_max; i++) { do_nothing = 1 - do_nothing; }
	for (int i=0; i<delay_max; i++) { do_nothing = 1 - do_nothing; }
	for (int i=0; i<delay_max; i++) { do_nothing = 1 - do_nothing; }
*/
	setfont(6,8,font6x8_WIDTH,font6x8_WIDTH,font6x8Data);

	while(1)
	{
		InitializeSprites();	 //set all sprites off screen (stops artifact)
		putBG(intro_screenData);

		if (put_msg>10)
		{
			PutString(44,136,(char *)press_start,0xB0);
			
			if (put_msg>15) put_msg = 0; else put_msg++;
		}
		else
		{
			put_msg++;
		}

		putspriteBG(32,148,112,10,0x00,gbaemuData);
		
		WaitFlipCopy();

		if (counter == 32767) counter = 0; else counter++;
		
	     	if (!(*KEYS&KEY_START))
	     	{
	     		srand(counter);
	     		srand(counter+rand());
	     		begin_card_battle();
		}
	}	
}
