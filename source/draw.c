//
//  draw.c
//  sokoban
//
//  Created by kawaiyoshiharu on 2016/01/30.
//  Copyright © 2016年 Particle Inc. All rights reserved.
//

#include <string.h>
#include "draw.h"

CHARA_STACK char_stack[10];
int char_stacks;


void clearScreen(gfxScreen_t screen, gfx3dSide_t side){
    u16 fbWidth  = 400;
    u16 fbHeight = 240;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);
    int yy,xx;
    for (yy = 0; yy< fbHeight; yy++) {
        u8 *ptr = &fbAdr[yy*fbWidth*3];
        for (xx = 0; xx< fbWidth; xx++) {
            *ptr++ = 0x80;
            *ptr++ = 0x80;
            *ptr++ = 0x80;
        }
    }
}


void drawCharacter(gfxScreen_t screen, gfx3dSide_t side, u8 *spriteData, s16 y,s16 x, u16 height,u16 width){
    if (spriteData==NULL) { return; }

    u16 fbWidth, fbHeight;
    u8 *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

    if (x + width  < 0 || fbWidth  <= x) { return; }
    if (y + height < 0 || fbHeight <= y) { return; }

    u16 xOffset = (x<0)? -x : 0;
    u16 yOffset = (y<0)? -y : 0;
    u16 widthDrawn  = ((x+width ) >= fbWidth )? fbWidth  - x : width;
    u16 heightDrawn = ((y+height) >= fbHeight)? fbHeight - y : height;
    widthDrawn  -= xOffset;
    heightDrawn -= yOffset;

    int yy;
    for (yy = yOffset; yy<(yOffset+heightDrawn); yy++) {
        memcpy(&fbAdr[((x+xOffset)+(y+yy)*fbWidth)*3], &spriteData[(xOffset +yy*width)*3], widthDrawn*3);
    }
}

void clear_char_stack(){
    char_stacks = 0;
}
void push_char_stack(int x,int y,char chara){
    char_stack[char_stacks].x = x;
    char_stack[char_stacks].y = y;
    char_stack[char_stacks].chara = chara;
    char_stacks++;
}
void draw_char_stack(LEVELDATA this_level_data,SPRITEDATA *spriteData, int chars/*,PrintConsole bottomScreen*/){
    int xOffset = 400/2 - this_level_data.width*15/2;
    int yOffset = 240/2 - this_level_data.height*15/2;
    int i,c,s;
    for (s=0;s<2;s++){
        gspWaitForVBlank();
        for (i=0; i<char_stacks; i++) {
            char chara = char_stack[i].chara;
            for (c=0;c<chars;c++){
                if(spriteData[c].chara == chara) break;
            }
//            consoleSelect(&bottomScreen);
//            printf("\x1b[22;12H(c,cs,cs,chara)=(%d,%d,%d,%x)",c,chars,sizeof(spriteData)/sizeof(SPRITEDATA),chara);
            if (c < chars)
                drawCharacter(GFX_TOP, GFX_LEFT, (u8*)spriteData[c].spriteDataPtr, char_stack[i].x*15+xOffset,(this_level_data.height-char_stack[i].y)*15+yOffset, spriteData[c].width, spriteData[c].height);
        }
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
    char_stacks = 0;
}
