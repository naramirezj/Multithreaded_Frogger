#include "obstacle.h"

/**
 * @brief Initialize the obstacle object
 * 
 * @param u_win current window
 * @param y y-coordinates
 * @param x x-coordinates
 * @param str string representing the obstacle
 * @param section_chr section character
 * @return obstacle_t 
 */
obstacle_t init_obstacle(WINDOW * u_win, int y, int x, char* str, char* section_chr){
    obstacle_t my_obstacle; 
    my_obstacle.section_char = section_chr;
    my_obstacle.curwin = u_win; //window
    my_obstacle.yLoc = y; //current y position
    my_obstacle.xLoc = x; //currnet x position
    my_obstacle.str = str; //string being displayed
    my_obstacle.xMax = 60 - 1; 
    return my_obstacle;
}
/**
 * @brief Display the obstacle and move it through the screen from left to right
 * 
 * @param obstacle 
 */
void obstacle_display(obstacle_t* obstacle){
    //print the section character behind the obstacle
    mvwprintw(obstacle->curwin, obstacle->yLoc, obstacle->xLoc, obstacle->section_char);
    obstacle->xLoc = obstacle->xLoc + 1;
    //display obstacle
    mvwprintw(obstacle->curwin, obstacle->yLoc, obstacle->xLoc, obstacle->str);
    //go back to the beginning if it hits the end
    if (obstacle->xLoc > obstacle->xMax)
    {
        obstacle->xLoc = 1;
    }
}
