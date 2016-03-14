//
//  draw.h
//  sokoban
//
//  Created by kawaiyoshiharu on 2016/01/30.
//  Copyright © 2016年 Particle Inc. All rights reserved.
//

#ifndef draw_h
#define draw_h

#include <stdio.h>
#include <3ds.h>

typedef struct spriteDataStruct {
    const u8    *spriteDataPtr;
    u16         width,height;
    char        chara;
} SPRITEDATA;

typedef struct levelDataStruct {
    char *levelDataLine[16];
    u8 width,height;
} LEVELDATA;

typedef struct chara_stack{
    u16     x,y;
    char    chara;
}CHARA_STACK;

#define WALL_BIN (0)
#define MARK_BIN (1)
#define BOX_BIN  (2)
#define BOX_OK_BIN  (3)
#define MAN_BIN  (4)

#define WALL_CHAR '*'
#define MARK_CHAR 'X'
#define BOXX_CHAR 'O'
#define BOXK_CHAR 'B'
#define MANN_CHAR '@'
#define FLOR_CHAR '='

void clearScreen(gfxScreen_t screen, gfx3dSide_t side);
void drawCharacter(gfxScreen_t screen, gfx3dSide_t side, u8 *spriteData, s16 y,s16 x, u16 height,u16 width);

void clear_char_stack();
void push_char_stack(int x,int y,char chara);
void draw_char_stack(LEVELDATA this_level_data,SPRITEDATA *spriteData, int chars/*,PrintConsole bottomScreen*/);

#endif /* draw_h */
