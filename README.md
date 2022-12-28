# Multithreaded_Frogger
The game consists of a moving frog whose objective is to get to the end of a simulated path without being hit by moving obstacles. The frog will jump and move by user input. Moreover, the graphics component of the game depends on thread parallelism and synchronization.

A Makefile is provided so the game can be compiled with the following command:
~~~C
make
~~~

The game can be ran as:
~~~C
./game.c
~~~

When ran, the game displays a menu with two options:
START and LEADER BOARD

If START is pressed (with the enter key) the game will then ask for your username and then display a welcome message.
After, the game will start. The user is represented by a green 0. 
The user can move with the UP, DOWN, RIGHT, and LEFT keys in the keyboard.
The goal is to get to the other side of the window without colliding with the obstacles in each section.
If the player collides with an obstacle the game will finish and display a loss message.
If the player manages to get to the end of the window a win message will be displayed and the time will be listed in the leaderboard.

If LEADERBOARD is pressed (with the enter key), the game will show a sorted list of all the user that have played the games and their times.
The first place is the shortest time that someone took to finish and win the game.


