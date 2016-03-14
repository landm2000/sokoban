//
//  main.h
//  sokoban
//
//  Created by kawaiyoshiharu on 2016/02/07.
//  Copyright © 2016年 Particle Inc. All rights reserved.
//

#ifndef main_h
#define main_h

#define H_COUNT_MAX 3

void copyGame(int level,LEVELDATA *this_level_data);
void printGame(LEVELDATA *this_level_data);
void moveMan(u32 kDown,u32 kHeld,int level,LEVELDATA *this_level_data);
int checkCleared(LEVELDATA *this_level_data,int level);

#endif /* main_h */
