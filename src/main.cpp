#include "gba.h"
#include "Game_pal.h"
#include "Capcom_chars_offs.h"
#include "Capcom_chars_xc.h"
#include "SNK_chars_offs.h"
#include "SNK_chars_xc.h"
#include "bottom.h"
#include "bottom_box.h"
#include "Capcom_chars_hp.h"
#include "Capcom_chars_sp.h"
#include "SNK_chars_hp.h"
#include "SNK_chars_sp.h"
#include "explode_offs.h"
#include "font.h"
#include "sfx.h"
#include "gbfs.h"

const GBFS_FILE *samples;

s32 sound_length;
s32 bgm_length;
int sound_played = 0;
unsigned int entry = 0;
int BGM = 0;
int BGM_INDEX = 0;

void DMAFastCopy( void *source, void *dest, unsigned int count);

void isr(void)
{
	//disable/store interrupts
	REG_IME = 0x00;
	int INT_FLAGS = REG_IF;
	if((REG_IF & INT_VBLANK) == INT_VBLANK)
	{
		entry++;
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
				if (BGM) BGM=0;
		}

	}
	
	REG_IF = INT_FLAGS;
	REG_IME = 0x01;
}

void itoa_lpad(char *buf, int len, unsigned int n, int lpad_chr)
{
  buf[len] = 0;
  if(n == 0)
    n = 0;

  do 
  {
    buf[--len] = (n % 10) + '0';
    n = n / 10;
  } while(n && len);

  while(len) buf[--len] = lpad_chr;
}

void Flip(void) // flips between the back/front buffer
{
    if(REG_DISPCNT & BACKBUFFER)    //back buffer is the current buffer so we need to switch it to the front buffer
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

void WaitForVblank(void)	        
{
	while (*ScanlineCounter<160);
	while (*ScanlineCounter==160);
	 
}

void PlotPixel(int x,int y, u16 c)
{	
	if (y>=0&&y<=SCREEN_HEIGHT&&x>=0&&x<=SCREEN_WIDTH/2-1)	VideoBuffer[y*SCREEN_WIDTH/2+x] = c;
}

// Mode 4 is 240(120)x160 by 8bit for software sprites
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

void PutString(int x, int y,char *buff, unsigned short c, unsigned short int bg)
{
	int start_x = x;
	int fx = font_sizex>>1;

	while (*buff!='$')
	{
		int font_num=0;
		
		if (*buff>='A'&&*buff<='Z')
		{
			font_num = *buff - 'A';
		}

		if (*buff>='a'&&*buff<='z')
		{
			font_num = *buff - 'a' + 26;
		}

		if (*buff>='0'&&*buff<='9')
		{
			font_num = *buff - '0' + 52;
		}
	
		if ((*buff!=' ')&&(*buff!='-')) {PutFontBG(start_x,y,font_num,c,bg); start_x+=fx; }
		if ((*buff==' ')) {start_x += fx;}
		if ((*buff=='-')) {y+=font_sizey; start_x = x;}
		buff++;
	}
}

void cls(u8 c)
{
	for (int y=0;y<SCREEN_HEIGHT;y++)
	{
		for (int x=0;x<SCREEN_WIDTH/2;x++)
		{
			VideoBuffer[y*SCREEN_WIDTH/2+x] = (c<<8)+c;
		}
	}
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

void doexplosion(int x, int y, int state)
{
	putspriteBG(x,y,explode_WIDTH,explode_HEIGHT,0,explode_data[state]);	
}

void PutCard(int x, int y,int card_num,int set_num,int hp, int sp)
{
	char buf[16];
	int xc = 0;
	int sx = 0;
	int sy = 0;
	const u16* char_ptr = 0;

	itoa_lpad(buf, 4,hp, ' ');
	buf[4] = '$';

	if (set_num==0)
	{
		sx = Capcom_chars_width[card_num];
		sy = Capcom_chars_height[card_num];
		char_ptr = Capcom_chars_offs[card_num];
		xc = Capcom_chars_xc[card_num];
	}
	
	if (set_num==1)
	{
		sx = SNK_chars_width[card_num];
		sy = SNK_chars_height[card_num];
		char_ptr = SNK_chars_offs[card_num];
		xc = SNK_chars_xc[card_num];
	}	

	putsprite(x,y,sx,sy,char_ptr);
	if (xc%2)
	{
		int xf=x+((xc-1)>>1);
		int yf=(y+sy)-bottom2_HEIGHT;
		putsprite(xf,yf,bottom2_WIDTH,bottom2_HEIGHT,bottom2Data);
		putsprite(xf+5,yf,bpsp2_WIDTH,bpsp2_HEIGHT,bpsp2Data);
		PutString(xf+15,yf+1,buf,101,0x4e);
	}
	else
	{
		int xf=x+(xc>>1);
		int yf=(y+sy)-bottom_HEIGHT;
		putsprite(xf,yf,bottom_WIDTH,bottom_HEIGHT,bottomData);
		putsprite(xf+5,yf,bpsp_WIDTH,bpsp_HEIGHT,bpspData);
		PutString(xf+15,yf+1,buf,101,0x4e);
	}
}

int STATE = 0;
int STATE_CARDS_IN = 5;	

// moves the cards into battle
int battle_x1;
int battle_x2;
int moving_cards = 0;
int explode_card = 0;
int card_hp1=0;
int card_hp2=0;
int move_cards(int card1, int card2, int set1, int set2, int hp1, int hp2);

// Textbox Before Battle
void Battle_TextBox(int card1, int card2, int set1, int set2);
int InBattleTextBox = 1;

#define WSCNT  *(volatile u16 *)0x04000204
#define WSCNT_ROM31 0x0014

int main(void)
{
	unsigned char *sound_ptr;

	SetMode(MODE_4 | BG2_ENABLE); //set mode 4 and enable background 2
 	DMAFastCopy((void*)Game_pal,(void*)BGPaletteMem,256);

	int x1,x2;
	int y1,y2;
	int sx1,sx2;
	int sy1,sy2;
	
	int max_chars=119;
	int char1 = 0;
	int char2 = 0;
	
	int explode1=0;
	int explode2=0;
	
	int ex1=0;
	int ex2=0;
	
	sx1 = Capcom_chars_width[char1];
	sx2 = SNK_chars_width[char2];
	
	sy1 = Capcom_chars_height[char1];
	sy2 = SNK_chars_height[char2];
	
	x1=0; y1=0;
	x2=SCREEN_WIDTH/2-(sx2>>1); y2=SCREEN_HEIGHT-sy2;
	
	u8 SCREEN_BG=0x4e;

	setfont(6,8,font6x8_WIDTH,font6x8_WIDTH,font6x8Data);

	int set1 = 0;
	int set2 = 1;
	int hp1 = 0;
	int hp2 = 0;
	int sp1 = 0;
	int sp2 = 0;
	
	samples = find_first_gbfs_file((const void*)find_first_gbfs_file);
	if (!samples) while(1);
	
    	unsigned int len;

    	sound_ptr = (unsigned char *)gbfs_get_obj(samples, "bomb.pcm", &len);
	const SAMPLE bomb_sfx =
	{
		(s8*)sound_ptr,
		len,
	};

    	sound_ptr = (unsigned char *) gbfs_get_obj(samples, "duhast.pcm", &len);
	const SAMPLE duhast_bgm =
	{
		(s8*)sound_ptr,
		len,
	};

    	sound_ptr = (unsigned char *) gbfs_get_obj(samples, "dragula.pcm", &len);
	const SAMPLE dragula_bgm =
	{
		(s8*)sound_ptr,
		len,
	};

    	sound_ptr = (unsigned char *)gbfs_get_obj(samples, "hmetal.pcm", &len);
	const SAMPLE hmetal_bgm =
	{
		(s8*)sound_ptr,
		len,
	};

	const SAMPLE *BG_MUSIC [] = {&duhast_bgm,&dragula_bgm,&hmetal_bgm};
	
	unsigned int OLD_ISR;
	OLD_ISR = REG_INTR_HANDLER;
	
	//create custom interrupt handler for vblank
	REG_IME = 0x00;
	REG_INTR_HANDLER  = (unsigned int)isr;
	REG_IE |= INT_VBLANK;
	REG_DISPSTAT |= 0x08;
	WSCNT = WSCNT_ROM31;

	if (set2 == 0) hp2 = Capcom_chars_hp[char2]; else hp2 = SNK_chars_hp[char2]; 
	if (set2 == 0) sp2 = Capcom_chars_sp[char2]; else sp2 = SNK_chars_sp[char2]; 
	if (set1 == 0) hp1 = Capcom_chars_hp[char1]; else hp1 = SNK_chars_hp[char1]; 
	if (set1 == 0) sp1 = Capcom_chars_sp[char1]; else sp1 = SNK_chars_sp[char1]; 
	
	while (1)
	{	
		cls(SCREEN_BG);

		//itoa_lpad(buf2,7,entry, ' ');
		//buf2[7]='$';
		//PutString(0,120,buf2,255-SCREEN_BG,0x4e);

		char buf2 [] = {"Multiple Line-String$"};
		PutString(0,120,buf2,255-SCREEN_BG,0x4e);
		
		//if (sound_length>=0)
		//{
		//	itoa_lpad(buf2,7,sound_length, ' ');
		//	buf2[7]='$';
		//	PutString(0,128,buf2,255-SCREEN_BG,0x4e);
		//}

		//if (bgm_length>=0)
		//{
		//	itoa_lpad(buf2,7,bgm_length, ' ');
		//	buf2[7]='$';
		//	PutString(0,136,buf2,255-SCREEN_BG,0x4e);
		//}

		if (STATE==0)
		{
			if (!BGM) REG_IME = 0x00;
			x1 = 0;
			
			if (set2 == 1)
			{	
				x2 = SCREEN_WIDTH/2-(SNK_chars_width[char2]>>1); 
			}
			else 
			{
				x2 = SCREEN_WIDTH/2-(Capcom_chars_width[char2]>>1);
			}
			
			sound_played = 0;
			PutCard(x1,8,char1,set1,hp1,sp1);
			PutCard(x2,8,char2,set2,hp2,sp2);
			sound_played = 0;
			explode1 = explode2 = ex1 = ex2 = 0;
		}
		
		if (STATE==STATE_CARDS_IN)
		{
						
			int loser = move_cards(char1,char2,set1,set2,hp1,hp2);

			if (loser)
			{
				if (loser == 1)
				{
					if (!sound_played)
					{
						if (!REG_IME) REG_IME = 0x01;
						init_sfx_system();
						play_sfx(&bomb_sfx,8000);
						sound_played = 1;
					}

					char buf[] = {"Card 1 DIES$"};	
					PutString(3,160-bottom_box_HEIGHT+9,buf,0x9D,0x0);
					if (!ex1) ex1 = battle_x1;				
				}
			
				if (loser == 2)
				{
					if (!sound_played)
					{
						if (!REG_IME) REG_IME = 0x01;
						init_sfx_system();
						play_sfx(&bomb_sfx,8000);
						sound_played = 1;
					}

					char buf[] = {"Card 2 DIES$"};	
					PutString(3,160-bottom_box_HEIGHT+9,buf,0x9D,0x0);
					if (!ex2) ex2 = battle_x2;				
				}
			
				if (loser == 3)
				{
					if (!sound_played)
					{
						if (!REG_IME) REG_IME = 0x01;
						init_sfx_system();
						play_sfx(&bomb_sfx,8000);
						sound_played = 1;
					}

					char buf[] = {"Both Fighters DIE$"};	
					PutString(3,160-bottom_box_HEIGHT+9,buf,0x9D,0x0);
					ex1 = battle_x1;
					ex2 = battle_x2;
				}
			
				if (sound_played&(sound_length<0)&!BGM) REG_IME = 0x00;
			
				if (ex1)
				{
					if (explode1<=20) { doexplosion(ex1,-14,explode1); explode1+=4; }
				}
			
				if (ex2)
				{
					if (explode2<=20) { doexplosion(ex2,-14,explode2); explode2+=4; }
				}
			}
		}
		
		//char CopyRight[] = {"Lord Dael 2004$"};
		//PutString(6,148,CopyRight,255-SCREEN_BG,0x0);

		WaitForVblank();
		Flip();
		
		if (!(*KEYS&KEY_START))  
		{
			if ((STATE==STATE_CARDS_IN)&&InBattleTextBox) InBattleTextBox = 0;
			if (STATE!=STATE_CARDS_IN) STATE = STATE_CARDS_IN;
		}
		
		if (!(*KEYS&KEY_A)) 
		{
			if (STATE == 0)
			{
				if (char1 == max_chars) char1 = 0; else char1++;	
				if (set1 == 0) hp1 = Capcom_chars_hp[char1]; else hp1 = SNK_chars_hp[char1]; 
				if (set1 == 0) sp1 = Capcom_chars_sp[char1]; else sp1 = SNK_chars_sp[char1]; 
			}
		}
		
		if (!(*KEYS&KEY_B)) 
		{
			if (STATE == 0)
			{
				if (char2 == max_chars) char2 = 0; else char2++;	
				if (set2 == 0) hp2 = Capcom_chars_hp[char2]; else hp2 = SNK_chars_hp[char2]; 
				if (set2 == 0) sp2 = Capcom_chars_sp[char2]; else sp2 = SNK_chars_sp[char2]; 
			}
		}

		if (!(*KEYS&KEY_L))
		{
			if (BGM_INDEX==2) BGM_INDEX=0; else (BGM_INDEX++);
			if (BGM)
			{
				init_bgm_system();
				play_bgm(BG_MUSIC[BGM_INDEX],8000);
			}
		}
				
		if (!(*KEYS&KEY_SELECT))
		{
			if (STATE == 0)
			{
				if (!BGM)
				{
					if (!REG_IME) REG_IME = 0x01;
					init_bgm_system();
					play_bgm(BG_MUSIC[BGM_INDEX],8000);
					BGM = 1;
				}
				else
				{
					REG_TM1CNT = 0;
					REG_DMA2CNT = 0;
					bgm_length = 0;
					BGM = 0;
					REG_IME = 0x00;
				}
			}	
		}
	}

	return 0;	
}


int move_cards(int card1, int card2, int set1, int set2, int hp1, int hp2)
{
	putsprite(0,160-bottom_box_HEIGHT,bottom_box_WIDTH,bottom_box_HEIGHT,bottom_boxData);

	if (explode_card)
	{
		PutCard(battle_x1,8,card1,set1,card_hp1,0);
		PutCard(battle_x2,8,card2,set2,card_hp2,0);

		if (explode_card>=20) 
		{
			STATE = 0; 
			explode_card = 0;
			moving_cards = 0;
			card_hp1 = 0;
			card_hp2 = 0;
			battle_x1 = 0;
			battle_x2 = 0;
			InBattleTextBox  = 1;
			
			return 0;
		}
		else 
		{	
			explode_card+=4;
		}
		
		if (card_hp1<card_hp2)
		{	
			card_hp2 -= card_hp1;
			card_hp1=0;
			 
			return 1; 
		}

		if (card_hp2<card_hp1) 
		{
			card_hp1 -= card_hp2;
			card_hp2 = 0;
			return 2;
		}
		
		if (card_hp1==card_hp2)
		{
			card_hp1 = 0;
			card_hp2 = 0;
			return 3;
		}
	}
	
	if (moving_cards)
	{
		PutCard(battle_x1,8,card1,set1,card_hp1,0);
		PutCard(battle_x2,8,card2,set2,card_hp2,0);
		
		battle_x1+=5;
		battle_x2-=5;
		
		moving_cards++;
		if (moving_cards > 5) explode_card = 1;
	}
	else
	{
		card_hp1 = hp1;
		card_hp2 = hp2;
		explode_card = 0;
		
		battle_x1 = 0;
		
		if (set2 == 0)
		{
			battle_x2 = SCREEN_WIDTH/2-(Capcom_chars_width[card2]>>1);
		}

		if (set2 == 1)
		{
			battle_x2 = SCREEN_WIDTH/2-(SNK_chars_width[card2]>>1);
		}
		
		PutCard(battle_x1,8,card1,set1,card_hp1,0);
		PutCard(battle_x2,8,card2,set2,card_hp2,0);

		if (InBattleTextBox) Battle_TextBox(card1,card2,set1,set2); else moving_cards++;
	}

	return 0;
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

void DMAFastCopy(void *source, void *dest, unsigned int count)
{
		REG_DMA3SAD = (unsigned int)source;
		REG_DMA3DAD = (unsigned int)dest;
		REG_DMA3CNT = count|ENABLE_DMA|HALF_WORD_DMA;
}

void Battle_TextBox(int card1, int card2, int set1, int set2)
{
	char card_set1[] = {"CAP    $"};	
	char card_set2[] = {"SNK    $"};
	char attack_string[] ={"ATTACKS         PRESS START$"};

	char *ptr_1;
	char *ptr_2;
	
	if (set1==0) ptr_1 = card_set1; else ptr_1 = card_set2;
	if (set2==0) ptr_2 = card_set1; else ptr_2 = card_set2;

	int by = 160-bottom_box_HEIGHT+9;
	itoa_lpad(ptr_1+3,4,card1,' ');
	ptr_1[7]='$';
	PutString(3,by,ptr_1,0x9D,0x00);
	PutString(27,by,attack_string,0x9D,0x00);
	itoa_lpad(ptr_2+3,4,card2,' ');
	ptr_2[7]='$';
	PutString(51,by,ptr_2,0x9D,0x00);
}
