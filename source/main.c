#include <string.h>

#include <3ds.h>
#include "draw.h"
#include "data.h"
#include "main.h"

#define DEBUG

int mx,my;
enum MODE {
    MODE_NULL=0,
    MODE_INIT,
    MODE_GAME,
    MODE_CLEAR,
    MODE_ALL_CLEAR,
    MODE_END
};
int hcount;

int main()
{
    char level_data_line[20*16];
    LEVELDATA this_level_data = {
        {&level_data_line[0],   &level_data_line[20], &level_data_line[40], &level_data_line[60],
         &level_data_line[80], &level_data_line[100],&level_data_line[120],&level_data_line[140],
         &level_data_line[160],&level_data_line[180],&level_data_line[200],&level_data_line[220],
         &level_data_line[240],&level_data_line[260],&level_data_line[280],&level_data_line[300]},
        19,11
    };
    // Initializations
    srvInit();  // services
    aptInit();  // applets
    hidInit();  // input
	gfxInitDefault();
//	gfxSet3D(true); // uncomment if using stereoscopic 3D
    gfxSetDoubleBuffering(GFX_TOP, true);
    gfxSetDoubleBuffering(GFX_BOTTOM, false);

    PrintConsole /*topScreen, */bottomScreen;
    consoleInit(GFX_BOTTOM, &bottomScreen);
//    consoleInit(GFX_TOP, &topScreen);
    clear_char_stack();
    int level = 0;
    int mode = MODE_INIT;
//    int mode_status  = 0;
    hcount = 0;

int timer = 0;
    // Main loop
    while (aptMainLoop())
    {
        hidScanInput();
        u32 kDown = hidKeysDown();
        u32 kUp   = hidKeysUp();
        u32 kHeld = hidKeysHeld();
        if (kDown & KEY_START){
            break; // break in order to return to hbmenu
        }

        if (mode == MODE_INIT){
//            consoleSelect(&topScreen);
//            printf("\033[2J");
            copyGame(level,&this_level_data);
            gspWaitForVBlank();
            printGame(&this_level_data);
            mode = MODE_GAME;
        }else if (mode == MODE_CLEAR){
            if ((kUp & KEY_A) || (kUp & KEY_B) || (kUp & KEY_X) || (kUp & KEY_Y)){
                level++;
                if (level >= sizeof(level_data)/sizeof(LEVELDATA)) {
                    mode = MODE_ALL_CLEAR;
                }else{
                    mode = MODE_INIT;
                }
            }
            if (level < sizeof(level_data)/sizeof(LEVELDATA)) {
                consoleSelect(&bottomScreen);
                printf("\033[2J");
                printf("\x1b[21;12HStage cleared!!");
                printf("\x1b[23;12HPush A to next Next Stage");
            }
        }else if (mode == MODE_ALL_CLEAR){
            level = 0;
            consoleSelect(&bottomScreen);
            printf("\033[2J");
            printf("\x1b[21;12HGame cleared!!!");
            printf("\x1b[23;12HPush Start to exit game");
            mode = MODE_NULL;
        }else if (mode == MODE_GAME){

        // Your code goes here


#ifdef DEBUG
consoleSelect(&bottomScreen);
if (timer%10==0){
//if (1    != 0){printf("\x1b[10;12HkUp  =%08x",(unsigned int)kUp);}
//if (1    != 0){printf("\x1b[11;12HkDown=%08x",(unsigned int)kDown);}
//if (1    != 0){printf("\x1b[12;12HkHeld=%08x",(unsigned int)kHeld);}
//printf("\x1b[22;12HKEY_CPAD_UP=%x",KEY_UP);
//printf("\x1b[23;12HKEY_CPAD_DN=%x",KEY_DOWN);
printf("\x1b[20;12HRest: [SELECT]");
printf("\x1b[22;12HExit: [START]");
printf("\x1b[24;12HTime: %08d",timer);
}
timer++;
#endif
            if (kDown != 0 || kHeld != 0){
                if (kDown & KEY_SELECT){
                    mode = MODE_INIT;
                }else{
                    moveMan(kDown, kHeld,level,&this_level_data);
                    if(checkCleared(&this_level_data,level)==0){ mode = MODE_CLEAR;}
                    gspWaitForVBlank();
                    draw_char_stack(this_level_data,spriteData, sizeof(spriteData)/sizeof(SPRITEDATA));
                }
            }else{
                gspWaitForVBlank();
            }
        }

        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();
    }

    gfxExit();
    hidExit();
    aptExit();
    srvExit();
	return 0;
}

void copyGame(int level,LEVELDATA *this_level_data){
    this_level_data->height = level_data[level].height;
    this_level_data->width  = level_data[level].width;
    int x = 0, y= 0;
    for (y = 0; y<this_level_data->height; y++) {
        char *level_data_line = level_data[level].levelDataLine[y];
        char *this_level_data_line  = this_level_data->levelDataLine[y];
        for (x=0; x<this_level_data->width; x++) {
            if (*level_data_line == MANN_CHAR) {
                mx = x;
                my = y;
            }
                *this_level_data_line++ = *level_data_line++;
        }
    }
}

void printGame(LEVELDATA *this_level_data){
    int s = 0, x = 0, y= 0, c= 0;
    int xOffset = 400/2 - this_level_data->width*15/2;
    int yOffset = 240/2 - this_level_data->height*15/2;
    for (s=0;s<2;s++){
        gspWaitForVBlank();
        clearScreen(GFX_TOP, GFX_LEFT);
        for (y = 0; y<this_level_data->height; y++) {
            char *level_data_line = this_level_data->levelDataLine[y];
            for (x=0; x<this_level_data->width; x++) {
                char chara = *level_data_line++;
                for (c=0;c<sizeof(spriteData)/sizeof(SPRITEDATA);c++){
                    if(spriteData[c].chara == chara) break;
                }
                if (c < sizeof(spriteData)/sizeof(SPRITEDATA))
                    drawCharacter(GFX_TOP, GFX_LEFT, (u8*)spriteData[c].spriteDataPtr, x*15+xOffset,(this_level_data->height-y)*15+yOffset, spriteData[c].width, spriteData[c].height);
            }
        }
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
}


int checkCleared(LEVELDATA *this_level_data,int level){
    int max = 0, match = 0, x, y;
    for (y = 0; y<this_level_data->height; y++) {
        char *level_data_line = level_data[level].levelDataLine[y];
        for (x=0; x<this_level_data->width; x++) {
            if (*level_data_line == MARK_CHAR) {
                max++;
            }
            level_data_line++;
        }
    }
    for (y = 0; y<this_level_data->height; y++) {
        char *level_data_line = level_data[level].levelDataLine[y];
        char *this_level_data_line = this_level_data->levelDataLine[y];
        for (x=0; x<this_level_data->width; x++) {
            if (*level_data_line == MARK_CHAR &&
                *this_level_data_line == BOXX_CHAR) {
                match++;
            }
            level_data_line++;
            this_level_data_line++;
        }
    }
    return max - match;
}


void moveMan(u32 kDown,u32 kHeld,int level,LEVELDATA *this_level_data){
    if (kDown & KEY_UP || kHeld & KEY_UP) {
        if (my > 0) {
            char *upline_orig = level_data[level].levelDataLine[my-1];
            char *upline      = this_level_data->levelDataLine[my-1];
            if(upline[mx] == BOXX_CHAR &&
               my > 1){
                upline_orig = level_data[level].levelDataLine[my-2];
                upline      = this_level_data->levelDataLine[my-2];
                if (upline[mx] == FLOR_CHAR ||
                    upline[mx] == MARK_CHAR) {
                    if (kDown & KEY_UP) {
                        upline[mx] = BOXX_CHAR;
                        if (upline_orig[mx] == MARK_CHAR) {
                            push_char_stack(mx,my-2,BOXK_CHAR);
                        }else{
                            push_char_stack(mx,my-2,BOXX_CHAR);
                        }
                        upline = this_level_data->levelDataLine[my-1];
                        upline[mx] = MANN_CHAR;
                        upline = this_level_data->levelDataLine[my];
                        upline[mx] = FLOR_CHAR;
                        upline_orig = level_data[level].levelDataLine[my];
                        if (upline_orig[mx] == MARK_CHAR) {
                            push_char_stack(mx,my,MARK_CHAR);
                        }else {
                            push_char_stack(mx,my,FLOR_CHAR);
                        }
                        push_char_stack(mx,my-1,  MANN_CHAR);
                        my--;
                    }
                }
            }else if (upline[mx]      == FLOR_CHAR ||
                      upline_orig[mx] == MARK_CHAR) {
                if (hcount > 0) {
                    hcount--;
                }else{
                    upline = this_level_data->levelDataLine[my];
                    upline[mx] = FLOR_CHAR;
                    upline_orig = level_data[level].levelDataLine[my];
                    if (upline_orig[mx] == MARK_CHAR) {
                        push_char_stack(mx,my,MARK_CHAR);
                    }else {
                        push_char_stack(mx,my,FLOR_CHAR);
                    }
                    push_char_stack(mx,my-1,  MANN_CHAR);
                    my--;
                    hcount = H_COUNT_MAX;
                }
            }
        }
    }else if ((kDown & KEY_DOWN) || (kHeld & KEY_DOWN)) {
        if (my < this_level_data->height-1) {
            char *upline_orig = level_data[level].levelDataLine[my+1];
            char *upline = this_level_data->levelDataLine[my+1];
            if(upline[mx] == BOXX_CHAR &&
               my < this_level_data->height-2){
                upline_orig = level_data[level].levelDataLine[my+2];
                upline      = this_level_data->levelDataLine[my+2];
                if (upline[mx] == FLOR_CHAR ||
                    upline[mx] == MARK_CHAR) {
                    if (kDown & KEY_DOWN){
                        upline[mx] = BOXX_CHAR;
                        if (upline_orig[mx] == MARK_CHAR) {
                            push_char_stack(mx,my+2,BOXK_CHAR);
                        }else{
                            push_char_stack(mx,my+2,BOXX_CHAR);
                        }
                        upline = this_level_data->levelDataLine[my+1];
                        upline[mx] = MANN_CHAR;
                        upline = this_level_data->levelDataLine[my];
                        upline[mx] = FLOR_CHAR;
                        upline_orig = level_data[level].levelDataLine[my];
                        if (upline_orig[mx] == MARK_CHAR) {
                            push_char_stack(mx,my,MARK_CHAR);
                        }else {
                            push_char_stack(mx,my,FLOR_CHAR);
                        }
                        push_char_stack(mx,my+1,  MANN_CHAR);
                        my++;
                    }
                }
            }else if (upline[mx]      == FLOR_CHAR ||
                upline_orig[mx] == MARK_CHAR) {
                if (hcount > 0) {
                    hcount--;
                }else{
                    upline = this_level_data->levelDataLine[my];
                    upline[mx] = FLOR_CHAR;
                    upline_orig = level_data[level].levelDataLine[my];
                    if (upline_orig[mx] == MARK_CHAR) {
                        push_char_stack(mx,my,MARK_CHAR);
                    }else{
                        push_char_stack(mx,my,FLOR_CHAR);
                    }
                    push_char_stack(mx,my+1,  MANN_CHAR);
                    my++;
                    hcount = H_COUNT_MAX;
                }
            }
        }
    }else if ((kDown & KEY_LEFT) || (kHeld & KEY_LEFT)) {
        if (mx > 0) {
            char *upline_orig = level_data[level].levelDataLine[my];
            char *upline      = this_level_data->levelDataLine[my];
            if (upline[mx-1] == BOXX_CHAR &&
                mx > 1){
                if (upline[mx-2] == FLOR_CHAR ||
                    upline[mx-2] == MARK_CHAR) {
                    if (kDown & KEY_LEFT) {
                        upline[mx] = FLOR_CHAR;
                        upline[mx-1] = MANN_CHAR;
                        upline[mx-2] = BOXX_CHAR;
                        if (upline_orig[mx] == MARK_CHAR) {
                            push_char_stack(mx--,my,MARK_CHAR);
                        }else {
                            push_char_stack(mx--,my,FLOR_CHAR);
                        }
                        push_char_stack(mx,my,  MANN_CHAR);
                        if (upline_orig[mx-1] == MARK_CHAR){
                            push_char_stack(mx-1,my,BOXK_CHAR);
                        }else{
                            push_char_stack(mx-1,my,BOXX_CHAR);
                        }
                    }
                }
            }else if (upline[mx-1] == FLOR_CHAR ||
                      upline[mx-1] == MARK_CHAR) {
                if (hcount > 0) {
                    hcount--;
                }else{
                    upline[mx] = FLOR_CHAR;
                    if (upline_orig[mx] == MARK_CHAR) {
                        push_char_stack(mx--,my,MARK_CHAR);
                    }else{
                        push_char_stack(mx--,my,FLOR_CHAR);
                    }
                    push_char_stack(mx,my,  MANN_CHAR);
                    hcount = H_COUNT_MAX;
                }
            }
        }
    }else if ((kDown & KEY_RIGHT) || (kHeld & KEY_RIGHT)) {
        if (my < this_level_data->width-1) {
            char *upline_orig = level_data[level].levelDataLine[my];
            char *upline = this_level_data->levelDataLine[my];
            if (upline[mx+1] == BOXX_CHAR &&
                mx < this_level_data->width-2){
                if (upline[mx+2] == FLOR_CHAR ||
                    upline[mx+2] == MARK_CHAR) {
                    if (kDown & KEY_RIGHT){
                        upline[mx]   = FLOR_CHAR;
                        upline[mx+1] = MANN_CHAR;
                        upline[mx+2] = BOXX_CHAR;
                        if (upline_orig[mx] == MARK_CHAR) {
                            push_char_stack(mx++,my,MARK_CHAR);
                        }else{
                            push_char_stack(mx++,my,FLOR_CHAR);
                        }
                        push_char_stack(mx,my,  MANN_CHAR);
                        if (upline_orig[mx+1] == MARK_CHAR){
                            push_char_stack(mx+1,my,BOXK_CHAR);
                        }else{
                            push_char_stack(mx+1,my,BOXX_CHAR);
                        }
                    }
                }
            }else if (upline[mx+1] == FLOR_CHAR ||
                      upline[mx+1] == MARK_CHAR) {
                upline[mx] = FLOR_CHAR;
                if (hcount > 0) {
                    hcount--;
                }else{
                    if (upline_orig[mx] == MARK_CHAR) {
                        push_char_stack(mx++,my,MARK_CHAR);
                    }else{
                        push_char_stack(mx++,my,FLOR_CHAR);
                    }
                    push_char_stack(mx,my,  MANN_CHAR);
                    hcount = H_COUNT_MAX;
                }
            }
        }
    }
}



