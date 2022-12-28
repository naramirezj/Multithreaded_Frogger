#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <ncurses.h>
#include <math.h>
#include "player.h"
#include "obstacle.h"

// MACROS
#define GAME_WIDTH 60
#define GAME_HEIGHT 50
const short title = 1;
WINDOW *game_window;
pthread_t frogger = 0;
time_t t;

// characters
player_t froggy;
obstacle_t all_cars[10];
obstacle_t all_river_obs[10];
obstacle_t all_grass_obs[10];
obstacle_t all_grass_obs[10];
obstacle_t all_swamp_obs[10];

// lock variable
pthread_mutex_t lock;

// thread functions for sections
void *street_display(void *data);
void *river_display(void *data);
void *field_display(void *data);
void *swamp_display(void *data);

// time variables
WINDOW *clock_window;
#define CLOCK_WIDTH 9
#define CLOCK_HEIGHT 5
int minutes;
int seconds;

//Structure for leaderboard stats
typedef struct leader_stat
{
    char *username;
    int time;
} leader_stat_t;

//Array of structures for leaderboard
leader_stat_t current_leaderboard[15];

/**
 * @brief Initialization of the stat structure
 * 
 * @param str username
 * @param time 
 * @return leader_stat_t 
 */
leader_stat_t init_stat(char * str, int time)
{
    leader_stat_t player_stat;
    player_stat.username = str;
    player_stat.time = time;
    return player_stat;
}


/**
 * @brief boolean function that indicates if there has been a collision between the player thread and the obstacles in the section threads.
 * 
 * @return true when there is no collision.
 * @return false when there is a collision.
 */
bool check_collision()
{
    bool play_game = true;
    //for loops to check every position x and y of the obstacle with the ones of the player
    //Street
    for (int i = 0; i < 10; i++)
    {
        if ((all_cars[i].xLoc == froggy.xLoc || (all_cars[i].xLoc + 1) == froggy.xLoc || (all_cars[i].xLoc + 2) == froggy.xLoc || (all_cars[i].xLoc + 3) == froggy.xLoc || (all_cars[i].xLoc + 4) == froggy.xLoc || (all_cars[i].xLoc + 5) == froggy.xLoc || (all_cars[i].xLoc + 6) == froggy.xLoc) && all_cars[i].yLoc == froggy.yLoc)
        {
            play_game = false;
            return play_game;
        }
    }
    //River
    for (int i = 0; i < 10; i++)
    {
        if ((all_river_obs[i].xLoc == froggy.xLoc || all_river_obs[i].xLoc + 1 == froggy.xLoc || all_river_obs[i].xLoc + 2 == froggy.xLoc) && all_river_obs[i].yLoc == froggy.yLoc)
        {
            play_game = false;
            return play_game;
        }
    }
    //Grass
    for (int i = 0; i < 10; i++)
    {
        if ((all_grass_obs[i].xLoc == froggy.xLoc || all_grass_obs[i].xLoc + 1 == froggy.xLoc || all_grass_obs[i].xLoc + 2 == froggy.xLoc) && all_grass_obs[i].yLoc == froggy.yLoc)
        {
            play_game = false;
            return play_game;
        }
    }
    //Swamp
    for (int i = 0; i < 10; i++)
    {
        if ((all_swamp_obs[i].xLoc == froggy.xLoc || all_swamp_obs[i].xLoc + 1 == froggy.xLoc || all_swamp_obs[i].xLoc + 2 == froggy.xLoc) && all_swamp_obs[i].yLoc == froggy.yLoc)
        {
            play_game = false;
            return play_game;
        }
    }
    //return the boolean
    return play_game;
}

/**
 * @brief boolean function that indicates if the player reached the end of the screen.
 * 
 * @return true when the player has reached the desired coordinates.
 * @return false when the player hasn't reached the desired y-coordinates.
 */
bool check_win()
{
    bool winning = false;
    if (froggy.yLoc == (GAME_HEIGHT - 2))
    {
        winning = true;
    }
    return winning;
}

/**
 * @brief Setting up the main clock loop, it will run a clock until the specified ms time.
 * 
 */
void main_clock(ms)
{
    clock_t delay = ms + clock();
    while (delay > clock())
        ;
}

/**
 * @brief Printing the time into the clock_window next to the game_window
 * 
 * @param minutes 
 * @param seconds 
 */
void print_time(int minutes, int seconds)
{   //Converting both ints to strings so we can print them
    int sec_length = snprintf(NULL, 0, "%d", seconds);
    int min_length = snprintf(NULL, 0, "%d", minutes);
    char *sec = malloc(sec_length + 1);
    char *min = malloc(min_length + 1);
    snprintf(sec, sec_length + 1, "%d", seconds);
    snprintf(min, sec_length + 1, "%d", minutes);
    char *time_str = malloc(sec_length + min_length + 1);
    char colon[2] = ":";
    //Concatenating the time and formatting
    strcat(time_str, min);
    strcat(time_str, colon);
    strcat(time_str, sec);
    //Printing it to the clock_window
    pthread_mutex_lock(&lock);
    mvwprintw(clock_window, 2, 3, time_str);
    wrefresh(clock_window);
    pthread_mutex_unlock(&lock);
}
/**
 * @brief Thread function for the time in the game
 * 
 * @param args 
 * @return void* 
 */
void *game_time(void *args)
{   //intializing minutes and seconds
    minutes = 0;
    seconds = 0;
    //Creating the clock_window
    clock_window = newwin(CLOCK_HEIGHT, CLOCK_WIDTH, 0, GAME_WIDTH + 2);
    box(clock_window, 0, 0);
    pthread_mutex_lock(&lock);
    wrefresh(clock_window);
    pthread_mutex_unlock(&lock);
    //displaying time until there is a collision or a win
    while (check_collision() && !check_win())
    {   //getting minutes
        if (seconds > 59) 
        {
            seconds = 0;
            minutes = minutes + 1;
        }
        //printing the time to the terminal
        print_time(minutes, seconds);
        //making the clock wait a second
        main_clock(900000);
        //getting the seconds elapsed
        seconds = seconds + 1;
    }
    //after there's a collision we clear the window
    wclear(clock_window);
    wrefresh(clock_window);

    return NULL;
}
/**
 * @brief Writes username and time to the two leaderboard files, username is written to leaderboardU and time is written to leaderboardN
 * 
 * @param player_username 
 */
void write_leaderboard(char *player_username)
{
    // need to concat the player's minutes and seconds
    int temp_minutes = minutes * 100;
    int concat_time = temp_minutes + seconds;

    // writing username, writing time (minutes and seconds)
    FILE *leaderboardU;
    FILE *leaderboardN;
    // reading and writing (and doesn't overwrite current contents)
    leaderboardN = fopen("leaderboardN.txt", "a+");
    leaderboardU = fopen("leaderboardU.txt", "a+");
    if (leaderboardU == NULL)
    {
        printf("Error: file couldn't be opened.\n");
        exit(1);
    }
    if (leaderboardN == NULL)
    {
        printf("Error: file couldn't be opened.\n");
        exit(1);
    }
    // write to a fie
    fprintf(leaderboardU, "%s\n", player_username);
    fprintf(leaderboardN, "%d\n", concat_time);
    //close file
    fclose(leaderboardN);
    fclose(leaderboardU);
}
/**
 * @brief reading from both leaderboard files, sorting the times and displaying the leaderboard.
 * 
 */
void display_leaderboard()
{

    // setting up display
    wclear(game_window);
    wrefresh(game_window);
    box(game_window, 0, 0);
    wrefresh(game_window);
    //printing header
    mvwprintw(game_window, 1, ((game_window->_maxx / 2) - (strlen("LEADERBOARD") / 2)), "LEADERBOARD");
    mvwprintw(game_window, 2, ((game_window->_maxx / 2) - (strlen("Press any key to exit") / 2)), "Press any key to exit");
    wrefresh(game_window);

    FILE *leaderboardU;
    FILE *leaderboardN;
    // reading and writing (and doesn't overwrite current contents)
    leaderboardN = fopen("leaderboardN.txt", "r+");
    leaderboardU = fopen("leaderboardU.txt", "r+");

    rewind(leaderboardN);
    rewind(leaderboardU);
    // check for errors
    if (leaderboardN == NULL)
    {
        printf("Error: file couldn't be opened.\n");
        exit(1);
    }
    if (leaderboardU == NULL)
    {
        printf("Error: file couldn't be opened.\n");
        exit(1);
    }
    //reading the username and time, assigning it to the leaderboard stats structure array
    int index = 0;
    int entries = 0;
    while (!feof(leaderboardN) && !feof(leaderboardU))
    {
        char * cur_username = malloc(sizeof(char) * 20);
        int cur_time = 0;

        fscanf(leaderboardU, "%s", cur_username);

        fscanf(leaderboardN, "%d", &cur_time);
        //intializing the struct stats with the read username and time
        current_leaderboard[index] = init_stat(cur_username, cur_time);

        index = index + 1;
        entries = entries + 1;
    }

    // sorting it now
    leader_stat_t temp;
    int sorted = 0;
    while (!sorted)
    {   //boolean to determine if it's sorted
        sorted = 1;
        for (int i = 0; i < (entries - 1); i++)
        {
            if (current_leaderboard[i].time > current_leaderboard[i + 1].time)
            {   //if the array is not sorted the boolean is false
                sorted = 0;
                temp = current_leaderboard[i];
                current_leaderboard[i] = current_leaderboard[i + 1];
                current_leaderboard[i + 1] = temp;
            }
        }
    }

    double temp_min = 0;
    int temp_sec = 0;
    // display it to screen
    for (int i = 0; i < entries; i++) {
        temp_sec = current_leaderboard[i].time;
        //if the time is 0 ignore it and don't display it
        if (current_leaderboard[i].time  == 0) {
            continue;
        }
        //if the time is larger than a minute we get the minutes and seconds
        if (current_leaderboard[i].time > 100) {
            temp_min = floor(current_leaderboard[i].time / 100);
            temp_sec = current_leaderboard[i].time % (((int) temp_min) * 100);
        }
        //printing time to the leaderboard window
        mvwprintw(game_window, 4 + i, 1, " %s %d", current_leaderboard[i].username, current_leaderboard[i].time);
        wrefresh(game_window);
        usleep(900000);
    }
    //closing files
    fclose(leaderboardU);
    fclose(leaderboardN);
}

/**
 * @brief User input thread function, receives the user input, moves the frog until there's a collision or a win. Creates the display threads.
 * 
 * @param data 
 * @return void* 
 */
void *user_input(void *data)
{
    char *username = ((char *)data);
    // Clearing the menu
    wclear(game_window);
    wrefresh(game_window);
    // Creating the map margins
    box(game_window, 0, 0);
    wrefresh(game_window);
    // Initialize the player at the top of the screen
    froggy = init_player(game_window, 0, GAME_WIDTH / 2, '0');
    // Creating threads for each section
    pthread_t street;
    pthread_t river;
    pthread_t field;
    pthread_t swamp;
    pthread_t clock;

    if (pthread_create(&street, NULL, street_display, &froggy) != 0)
    {
        perror("Creating thread failed");
        exit(1);
    }

    if (pthread_create(&river, NULL, river_display, &froggy) != 0)
    {
        perror("Creating thread failed");
        exit(1);
    }

    if (pthread_create(&field, NULL, field_display, &froggy) != 0)
    {
        perror("Creating thread failed");
        exit(1);
    }

    if (pthread_create(&swamp, NULL, swamp_display, &froggy) != 0)
    {
        perror("Creating thread failed");
        exit(1);
    }

    if (pthread_create(&clock, NULL, game_time, &froggy) != 0)
    {
        perror("Creating thread failed");
        exit(1);
    }

    // In a while loop until the player collides or wins
    do
    {
        if (!check_collision())
        {
            // clear window after collision
            wclear(game_window);
            wrefresh(game_window);
            box(game_window, 0, 0);
            //printing the loss message
            wattrset(game_window, COLOR_PAIR(4));
            mvwprintw(game_window, 6, ((game_window->_maxx / 2) - (strlen("YOU LOST") / 2)), "YOU LOST");
            wrefresh(game_window);
            wattroff(game_window, COLOR_PAIR(4));
            break;
        }
        if (check_win())
        {
            // clear window after winning
            wclear(game_window);
            wrefresh(game_window);
            box(game_window, 0, 0);
            //printing the win message
            wattrset(game_window, COLOR_PAIR(1));
            mvwprintw(game_window, 6, ((game_window->_maxx / 2) - (strlen("YOU WON!") / 2)), "YOU WON!");
            wrefresh(game_window);
            wattroff(game_window, COLOR_PAIR(1));
            write_leaderboard(username);
            break;
        }
        // Player movement and background refresh
        pthread_mutex_lock(&lock);
        wattrset(game_window, COLOR_PAIR(1));
        player_display(&froggy);
        change_background(&froggy);
        wrefresh(game_window);
        wattroff(game_window, COLOR_PAIR(1));
        pthread_mutex_unlock(&lock);
    } while (getmv(&froggy) != 'x');

    return data;
}
/**
 * @brief Get the user object through user input.
 * 
 */
void get_user()
{
    // Clearing the menu
    wclear(game_window);
    wrefresh(game_window);
    // Creating the map margins
    box(game_window, 0, 0);
    wrefresh(game_window);
    //setting up the buffer
    char username[20];
    mvwprintw(game_window, 2, ((game_window->_maxx / 2) - (strlen("Enter Username [MAX 20 Characters]") / 2)), "Enter Username [MAX 20 Characters]");
    wrefresh(game_window);
    int new_char = wgetch(game_window);
    int arr = 0;
    //getting the username through wgetch
    while (new_char != '\0' && new_char != '\n')
    {
        username[arr] = new_char;
        mvwaddch(game_window, 4, 20 + arr, new_char);
        wrefresh(game_window);
        new_char = wgetch(game_window);
        arr = arr + 1;
    }
    wclear(game_window);
    wrefresh(game_window);
    box(game_window, 0, 0);
    //displaying the username
    mvwprintw(game_window, 4, ((game_window->_maxx / 2) - (strlen("Welcome to our game") / 2)), "Welcome to our game");
    mvwprintw(game_window, 5, ((game_window->_maxx / 2) - (strlen(username) / 2)), username);
    wrefresh(game_window);
    usleep(900000);
    //Creates the frogger/user_input thread
    if (pthread_create(&frogger, NULL, user_input, &username) != 0)
    {
        perror("Creating thread failed");
        exit(1);
    }
}
/**
 * @brief Thread function to display the street section and obstacles
 * 
 * @param data 
 * @return void* 
 */
void *street_display(void *data)
{
    player_t *froggy = ((player_t *)data);
    // intialize random numbers
    srand((unsigned)time(&t)); 
    int random_x1, random_x2 = 0;
    //setting the character to display as the background for the section
    char street[2] = "-";

    int index = 0;
    // creating cars
    for (int k = 1; k < 6; k++)
    {   //randomizing the apparition
        random_x1 = rand() % GAME_WIDTH - 2;
        random_x2 = rand() % GAME_WIDTH - 2;
        all_cars[index] = init_obstacle(game_window, k * 2, random_x1, "{8__8}", "------");
        all_cars[index + 1] = init_obstacle(game_window, k * 2, random_x2, "{8__8}", "------");
        index = index + 2;
    }
    //Printing the background of the section first
    for (int i = 1; i < GAME_WIDTH - 1; i++)
    {
        for (int j = 1; j < GAME_HEIGHT / 4; j++)
        {
            pthread_mutex_lock(&lock);
            mvwprintw(game_window, j, i, street);
            pthread_mutex_unlock(&lock);
        }
    }
    pthread_mutex_lock(&lock);
    wrefresh(game_window);
    pthread_mutex_unlock(&lock);
    //while there is no collision or win the obstacles will keep moving
    while (check_collision() && !check_win())
    {
        for (int j = 0; j < 10; j++)
        {   //displaying the obstacle movement
            pthread_mutex_lock(&lock);
            obstacle_display(&(all_cars[j]));
            pthread_mutex_unlock(&lock);
        }
        pthread_mutex_lock(&lock);
        wrefresh(game_window);
        pthread_mutex_unlock(&lock);
        usleep(200000);
    }
    return data;
}
/**
 * @brief Thread function to display the river section and obstacles
 * 
 * @param data 
 * @return void* 
 */
void *river_display(void *data)
{
    player_t *froggy = ((player_t *)data);
    srand((unsigned)time(&t)); // intialize random numbers
    int random_x1, random_x2 = 0;
    //setting the character to display as the background for the section
    char river[2] = "~";

    int index = 0;
    // creating obstacles
    for (int k = 1; k < 6; k++)
    {
        random_x1 = rand() % GAME_WIDTH - 2;
        random_x2 = rand() % GAME_WIDTH - 2;
        all_river_obs[index] = init_obstacle(game_window, (k * 2) + GAME_HEIGHT / 4, random_x1, ":0:", "~~~");
        all_river_obs[index + 1] = init_obstacle(game_window, (k * 2) + GAME_HEIGHT / 4, random_x2, "==", "~~~");
        index = index + 2;
    }
    //Printing the background of the section first
    for (int i = 1; i < GAME_WIDTH - 1; i++)
    {
        for (int j = (GAME_HEIGHT / 4); j < (GAME_HEIGHT / 4 * 2); j++)
        {
            pthread_mutex_lock(&lock);
            mvwprintw(game_window, j, i, river);
            pthread_mutex_unlock(&lock);
        }
    }
    pthread_mutex_lock(&lock);
    wrefresh(game_window);
    pthread_mutex_unlock(&lock);
    //while there is no collision or win the obstacles will keep moving
    while (check_collision() && !check_win())
    {
        for (int j = 0; j < 10; j++)
        {
            pthread_mutex_lock(&lock);
            obstacle_display(&(all_river_obs[j]));
            pthread_mutex_unlock(&lock);
        }
        pthread_mutex_lock(&lock);
        wrefresh(game_window);
        pthread_mutex_unlock(&lock);
        usleep(200000);
    }
    return data;
}
/**
 * @brief Thread function to display the field section and obstacles
 * 
 * @param data 
 * @return void* 
 */
void *field_display(void *data)
{
    player_t *froggy = ((player_t *)data);
    srand((unsigned)time(&t)); // intialize random numbers
    int random_x1, random_x2 = 0;
     //setting the character to display as the background for the section
    char grass[2] = "|";

    int index = 0;
    // creating osbtacles
    for (int k = 1; k < 6; k++)
    {
        random_x1 = rand() % GAME_WIDTH - 2;
        random_x2 = rand() % GAME_WIDTH - 2;
        all_grass_obs[index] = init_obstacle(game_window, (k * 2) + (GAME_HEIGHT / 4) * 2, random_x1, "^O^", "|||");
        all_grass_obs[index + 1] = init_obstacle(game_window, (k * 2) + (GAME_HEIGHT / 4 * 2), random_x2, "_0-", "|||");
        index = index + 2;
    }
     //Printing the background of the section first
    for (int i = 1; i < GAME_WIDTH - 1; i++)
    {
        for (int j = (GAME_HEIGHT / 4 * 2); j < (GAME_HEIGHT / 4 * 3); j++)
        {
            pthread_mutex_lock(&lock);
            mvwprintw(game_window, j, i, grass);
            pthread_mutex_unlock(&lock);
        }
    }
    pthread_mutex_lock(&lock);
    wrefresh(game_window);
    pthread_mutex_unlock(&lock);
     //while there is no collision or win the obstacles will keep moving
    while (check_collision() && !check_win())
    {
        for (int j = 0; j < 10; j++)
        {
            pthread_mutex_lock(&lock);
            obstacle_display(&(all_grass_obs[j]));
            pthread_mutex_unlock(&lock);
        }
        pthread_mutex_lock(&lock);
        wrefresh(game_window);
        pthread_mutex_unlock(&lock);
        usleep(200000);
    }
    return data;
}

/**
 * @brief Thread function to display the swamp section and obstacles
 * 
 * @param data 
 * @return void* 
 */
void *swamp_display(void *data)
{
    player_t *froggy = ((player_t *)data);
    srand((unsigned)time(&t)); // intialize random numbers
    int random_x1, random_x2 = 0;
     //setting the character to display as the background for the section
    char swamp[2] = "o";

    int index = 0;
    // creating osbtacles
    for (int k = 1; k < 6; k++)
    {
        random_x1 = rand() % GAME_WIDTH - 2;
        random_x2 = rand() % GAME_WIDTH - 2;
        all_swamp_obs[index] = init_obstacle(game_window, ((k * 2) + (GAME_HEIGHT / 4 * 3)) - 2, random_x1, "(-)", "ooo");
        all_swamp_obs[index + 1] = init_obstacle(game_window, ((k * 2) + (GAME_HEIGHT / 4 * 3)) - 2, random_x2, "{^^}", "oooo");
        index = index + 2;
    }
    //Printing the background of the section first
    for (int i = 1; i < GAME_WIDTH - 1; i++)
    {
        for (int j = (GAME_HEIGHT / 4 * 3); j < ((GAME_HEIGHT / 4 * 4) - 2); j++)
        {
            pthread_mutex_lock(&lock);
            mvwprintw(game_window, j, i, swamp);
            pthread_mutex_unlock(&lock);
        }
    }
    pthread_mutex_lock(&lock);
    wrefresh(game_window);
    pthread_mutex_unlock(&lock);
     //while there is no collision or win the obstacles will keep moving
    while (check_collision() && !check_win())
    {
        for (int j = 0; j < 10; j++)
        {
            pthread_mutex_lock(&lock);
            obstacle_display(&(all_swamp_obs[j]));
            pthread_mutex_unlock(&lock);
        }
        pthread_mutex_lock(&lock);
        wrefresh(game_window);
        pthread_mutex_unlock(&lock);
        usleep(200000);
    }
    return data;
}

/**
 * @brief Displaying the menu at the beginning of the game
 * 
 * @return void* 
 */
void *display()
{
    //setting up colors
    if (has_colors() == FALSE)
    {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }

    // colors for the game
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_GREEN);
    init_pair(2, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    init_pair(4, COLOR_WHITE, COLOR_RED);
    init_pair(5, COLOR_WHITE, COLOR_BLUE);
    init_color(COLOR_GREEN, 0, 999, 0);

    // setting up window and larger display of the game
    game_window = newwin(GAME_HEIGHT, GAME_WIDTH, 0, 0);
    box(game_window, 0, 0);
    wrefresh(game_window);

    // display title
    wattrset(game_window, COLOR_PAIR(1));
    mvwprintw(game_window, 1, ((game_window->_maxx / 2) - (strlen("FROGGER") / 2)), "FROGGER");
    wattroff(game_window, COLOR_PAIR(1));
    wattrset(game_window, COLOR_PAIR(2));
    mvwprintw(game_window, 2, ((game_window->_maxx / 2) - (strlen("NAT AND MIA STYLE") / 2)), "NAT AND MIA STYLE");
    wattroff(game_window, COLOR_PAIR(2));
    wrefresh(game_window);
    //declaring the menu choices
    char choices[2][15] = {"Start", "Leader Board"};
    int choice;
    int highlight = 0;

    //menu display in a while loop until the user selects an option
    while (1)
    {
        for (int i = 0; i < 2; i++)
        {   //setting up the display with colors
            if (i == highlight)
            {
                wattron(game_window, A_REVERSE);
                wattrset(game_window, COLOR_PAIR(1));
            }
            mvwprintw(game_window, i + 4, ((game_window->_maxx / 2) - (strlen(choices[i]) / 2)), choices[i]);
            wattroff(game_window, A_REVERSE);
            wattroff(game_window, COLOR_PAIR(1));
        }
        keypad(game_window, true);
        choice = wgetch(game_window);
    //switch statement for input choice
        switch (choice)
        {
        //if key up the highlight decreases one place
        case KEY_UP:
            highlight--;
            if (highlight == -1)
            {
                highlight = 0;
            }
            break;
        //if key down the highlight increases one place
        case KEY_DOWN:
            highlight++;
            if (highlight == 2)
            {
                highlight = 1;
            }
            break;
        default:
            break;
        }
        if (choice == 10) // if the user presses "enter"
        {
            if (highlight == 0) // and if highlight is on the first option
            {                   // Creating the display thread
                get_user();
            }
            if (highlight == 1) //if highlight is on the second option
            {
                display_leaderboard(); //display the leaderboard
            }
            break;
        }
    }

    return NULL;
}

int main(int argc, char **argv)
{   //initialize screen
    initscr(); 
    //make it break when pressing CNTRL + C
    cbreak();
    noecho();
    //calling the display function for the menu
    display();
    //game ends when joining the frogger thread is succesful
    if (pthread_join(frogger, NULL) == 0)
    {
        getchar();
        wclear(game_window);
        wrefresh(game_window);
        endwin();
    }
    else
    {
        //if no threads created and cannot be joined, terminate when pressing a character
         getchar();
         endwin();
    }

    return 0;
}
