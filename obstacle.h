#ifndef _OBSTACLE_H_
#define _OBSTACLE_H_

#include <ncurses.h>
#include <string.h>


typedef struct obstacle {
    WINDOW * curwin; 
    char* str; 
    int yLoc; 
    int xLoc; 
    int xMax;
   // int time;
    char* section_char;
} obstacle_t; 

obstacle_t init_obstacle(WINDOW * u_win, int y, int x, char* str, char* section_chr); 

void obstacle_display(obstacle_t* obstacle);

#endif