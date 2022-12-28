#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <ncurses.h>
#include <string.h>

typedef struct player {
    WINDOW * curwin; 
    char c; 
    int yLoc; 
    int xLoc; 
    char section_char;
} player_t; 

player_t init_player(WINDOW * u_win, int y, int x, char c); 

void mvup(player_t * player);

void mvdown(player_t * player);

void mvleft(player_t * player);

void mvright(player_t * player);

int getmv(player_t * player); 

void player_display(player_t * player);

void change_background(player_t * player);


#endif