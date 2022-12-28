#include "player.h"

//Macros for the width and height of the window
int xMax = 60;
int yMax = 50;

/**
 * @brief Initializes the object player
 * 
 * @param u_win current window
 * @param y y-coordinates
 * @param x x-coordinates
 * @param c character representing the player
 * @return player_t 
 */
player_t init_player(WINDOW *u_win, int y, int x, char c)
{
    player_t my_player;
    my_player.curwin = u_win;
    my_player.yLoc = y;
    my_player.xLoc = x;
    my_player.c = c;
    my_player.section_char = '-';
    return my_player;
}

/**
 * @brief Player moves up
 * 
 * @param player 
 */
void mvup(player_t * player)
{   //whenever the player moves it will print the section character behind
    mvwaddch(player->curwin, player->yLoc, player->xLoc, player->section_char); 
    //decrease one in the y coordinates
    player->yLoc = player->yLoc - 1;
    if (player->yLoc < 1)
    {
        player->yLoc = 1;
    }
}
/**
 * @brief Player moves down
 * 
 * @param player 
 */
void mvdown(player_t * player)
{   //whenever the player moves it will print the section character behind
    mvwaddch(player->curwin, player->yLoc, player->xLoc, player->section_char); 
    //increases one in the y coordinates
    player->yLoc = player->yLoc + 1;
    if (player->yLoc > yMax - 2)
    {
        player->yLoc = yMax - 2;
    }
}
/**
 * @brief Player moves left
 * 
 * @param player 
 */
void mvleft(player_t * player)
{   //whenever the player moves it will print the section character behind
    mvwaddch(player->curwin, player->yLoc, player->xLoc, player->section_char); 
    //decreases one in the x coordinates
    player->xLoc = player->xLoc - 1;
    if (player->xLoc < 1)
    {
        player->xLoc = 1;
    }
}
/**
 * @brief Player moves down
 * 
 * @param player 
 */
void mvright(player_t * player)
{   //whenever the player moves it will print the section character behind
    mvwaddch(player->curwin, player->yLoc, player->xLoc, player->section_char); 
    //increases one in the x coordinates
    player->xLoc = player->xLoc + 1;
    if (player->xLoc > xMax -2)
    {
        player->xLoc = xMax-2;
    }
}

/**
 * @brief Getting the players movement through user input
 * 
 * @param player 
 * @return int 
 */
int getmv(player_t * player){
    // enabling keypad operations
    keypad(player->curwin, true);
    int input = wgetch(player->curwin); 
    switch (input) {
        case KEY_UP:
            mvup(player); 
            break;
        case KEY_DOWN:
            mvdown(player); 
            break; 
        case KEY_LEFT: 
            mvleft(player); 
            break;
        case KEY_RIGHT: 
            mvright(player); 
            break;
        default: 
            break; 
    }

    return input; 

}

/**
 * @brief Displaying the player in its y and x cooridnates
 * 
 * @param player 
 */
void player_display(player_t * player)
{
    mvwaddch(player->curwin, player->yLoc, player->xLoc, player->c); 
}
/**
 * @brief Function to change the background characters depending on the section the player is in
 * 
 * @param player 
 */
void change_background(player_t* player){
    int tempYMax = yMax - 1;
    int section_height = (tempYMax / 4); 
    // street
    if (player->yLoc < section_height) {
        player->section_char =  '-';
    }
    // water
    else if (player->yLoc >= section_height && player->yLoc < (section_height * 2)) {
        player->section_char =  '~';
    }
    // grass
    else if (player->yLoc >= (section_height * 2) && player->yLoc < (section_height * 3)) {
        player->section_char =  '|';
    }
    // swamp
    else if (player->yLoc >= (section_height * 3) && player->yLoc < ((section_height * 4)-2)) {
        player->section_char =  'o';
    }
    else{
        player->section_char =  ' ';
    }
}
