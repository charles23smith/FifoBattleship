//this is the client 
//player 2 always goes second 
//needs to be ran after the sever (player1.cpp)

#include "battleship.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <poll.h>
using namespace std ;

int main() {

    //define fifos
    const char* player1fifo = "./player1moves" ; 
    const char* player2fifo = "./player2moves" ; 

    //open the fifos
    int player1fd = open(player1fifo, O_RDONLY) ; //only read moves from player 1
    int player2fd = open(player2fifo, O_WRONLY) ; //only write moves for player 2 

    //read from player 1 that the game has started 
    char buffer[5] ; 
    read(player1fd, buffer, 5) ;
    if (strcmp(buffer, "start") == 0) {

        //instructions 
        cout << "FIFO BATTLESHIP." << endl ; 
        cout << "The rules are as follows, you will be given a 10x10 grid where you will place your ships." << endl ; 
        cout << "Each player gets 3 ships that can be placed horizontally or vertically." << endl ; 
        cout << "The ship sizes are as follows: 3,4,5." << endl ; 
        cout << "Each turn, one player will select a coordinate from the grid to fire and the screen will let you know if there has been a hit or a miss." << endl ;
        cout << "Hits will be shown with the X character, / will mark the misses, and O will show your ships." << endl ;
        cout << "The game will continue until one player has hit all of the parts of the other player's ships." << endl ; 
        cout << endl ; 
        cout << "Please select your ships" << endl ; 
        cout << endl ; 
    }

    //select the ships 
    battleship player2 ; 
    player2.setShips() ;
    cout << endl ; 

    //write fto player 1 that we are ready 
    write(player2fd, "ready", 5) ;

    // Use poll to check if Player 1 is ready
    struct pollfd pfd;
    pfd.fd = player1fd;
    pfd.events = POLLIN;
    int ret;
    bool waitingMessagePrinted = false;

    while (true) {
        ret = poll(&pfd, 1, 1000); // 1 second timeout

        if (ret > 0) {
            // Data is available to read
            read(player1fd, buffer, 5);
            if (strcmp(buffer, "ready") == 0) {
                cout << "Both players are ready, Let's Begin !" << endl;
                break;
            }
        } else if (ret == 0 && !waitingMessagePrinted) {
            // Timeout occurred, print waiting message
            cout << "Waiting for Player 1 to finish selecting their ships... " << endl;
            waitingMessagePrinted = true;
        }
    }
   

    //now game is started 
    int count = 0 ;
    while(count != 12) {
        waitingMessagePrinted = false ; 
        write(player2fd, "turn", 5) ;
        while (true) {
            ret = poll(&pfd, 1, 1000); // Waiting for Player 1 to fire 
    
            if (ret > 0) {
                // Data is available to read
                read(player1fd, buffer, 5);
                if (strncmp(buffer, "s", 1) == 0) {
                    //read in the shot 
                    if (player2.getShot(buffer) == true) {
                        count ++ ; 
                        write(player2fd, "hito", 5) ;
                        break ; 
                    }
                    else {
                        write(player2fd, "miss", 5) ;
                        break ; 
                    }
                }
            }
            else if (ret == 0 && !waitingMessagePrinted) {
                // Timeout occurred, print waiting message
                cout << "Waiting for Player 1 to fire... " << endl;
                waitingMessagePrinted = true;
            }
        }
        //time to take shot 
        read(player1fd, buffer, 5) ;
        if (strcmp(buffer, "over") == 0) {
            if (count == 12) {
                cout << "You have tied the game ! " << endl ;
                break ; 
            }
            else {
                cout << "You have won the game!" << endl ; 
                break ; 
            }
        }
        if (strcmp(buffer, "turn") == 0) {
            //player turn 

            //keep track of shot for shot confirmation 
            char x ; 
            int y ; 

            player2.displayGameBoard() ;
            player2.takeshot(player2fd, x, y) ;
            read(player1fd, buffer, 5) ;
            player2.shotConfirmation(buffer, x , y) ; //end of turn
        
        }
        //end of turns, check to see if there is a winner
        read(player1fd, buffer, 5) ;
        if (strcmp(buffer, "over") == 0) {
            if (count == 12) {
                write(player2fd, "over", 5) ;
                cout << "You have tied the game!" << endl ; 
                break ; 
            }
            else {
                write(player2fd, "turn", 5) ;
                cout << "You have won the game!" << endl ; 
                break ; 
            }
        }
        else {
            if (count == 12) {
                write(player2fd, "over", 5) ;
                cout << "You have lost the game!" << endl ; 
                break ; 
            }
            else {
                write(player2fd, "turn", 5) ;
            }
        }
    }
    
    //cloe the fifos
    close(player1fd) ;
    close(player2fd) ;
    unlink(player1fifo) ;
    unlink(player2fifo) ;
    player2.closefifos() ;

    


    return 0 ; 
}