// This is the Server 
// This needs to be ran before player2.cpp 
// Player 1 always go first 

#include "battleship.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <poll.h>
#include <limits>
using namespace std ;

int main() {

    //define the fifos
    const char* player1fifo = "./player1moves" ; 
    const char* player2fifo = "./player2moves" ; 

    unlink(player1fifo) ; //remove the fifos if they already exist
    unlink(player2fifo) ;

    //make the fifos
    if (mkfifo(player1fifo, 0666) == -1) {
        perror("mkfifo error") ; 
        return 1 ; 
    }
    if (mkfifo(player2fifo, 0666) == -1) {
        perror("mkfifo error") ; 
        return 1 ; 
    }

    //open the fifos 
    int player1fd = open(player1fifo, O_WRONLY) ; //only write moves for player 1 
    int player2fd = open(player2fifo, O_RDONLY) ; //only read moves from player 2

    //let player 2 know that the game is ready to start 
    write(player1fd, "start", 5) ; 

    //instructions 
    cout << "FIFO BATTLESHIP. " << endl ; 
    cout << "The rules are as follows, you will be given a 10x10 grid where you will place your ships." << endl ; 
    cout << "Each player gets 3 ships that can be placed horizontally or vertically." << endl ; 
    cout << "The ship sizes are as follows: 3,4,5." << endl ; 
    cout << "Each turn, one player will select a coordinate from the grid to fire and the screen will let you know if there has been a hit or a miss." << endl ;
    cout << "Hits will be shown with the X character, / will mark the misses, and O will show your ships." << endl ;
    cout << "The game will continue until one player has hit all of the parts of the other player's ships." << endl ; 
    cout << endl ; 
    cout << "Please select your ships" << endl ; 
    cout << endl ; 

    //select ships
    battleship player1 = battleship() ;
    player1.setShips() ;

    //read from player2 or send message that you are ready 
    cout << endl ;
    char buffer[5] ;
    
    write(player1fd, "ready", 5) ;

    // Use poll to check if Player 1 is ready
    struct pollfd pfd;
    pfd.fd = player2fd;
    pfd.events = POLLIN;
    int ret;
    bool waitingMessagePrinted = false;

    while (true) {
        ret = poll(&pfd, 1, 1000); // 1 second timeout

        if (ret > 0) {
            // Data is available to read
            read(player2fd, buffer, 5);
            if (strcmp(buffer, "ready") == 0) {
                cout << "Both players are ready, Let's Begin !" << endl;
                break;
            }
        } else if (ret == 0 && !waitingMessagePrinted) {
            // Timeout occurred, print waiting message
            cout << "Waiting for Player 2 to finish selecting their ships... " << endl;
            waitingMessagePrinted = true;
        }
    }

    //now game is started 


    //game logic 
    //display board and misses
    //take shot 
    //show result, then wait for player 2 
    //send message of over whenever game is over 
    int count = 0 ; //count for opponent hits 
    bool tied = false ; //bool for end of game message 
    bool win = false ; 
    bool loss = false ; 


    while(count != 12) {
        read(player2fd,buffer,5) ; 
    
        if (strcmp(buffer, "turn") == 0) {
            //player turn 
            //keep track of shot for confirmation 
            char x ; 
            int y ; 

            player1.displayGameBoard() ;
            player1.takeshot(player1fd, x , y) ;
            read(player2fd, buffer, 5) ;
            player1.shotConfirmation(buffer, x, y) ; //end of turn
        }
    //wait for player2
    write(player1fd, "turn", 5) ;
    waitingMessagePrinted = false ;
    while (true) {
        ret = poll(&pfd, 1, 1000); // Waiting for Player 2 to fire 

        if (ret > 0) {
            // Data is available to read
            read(player2fd, buffer, 5);
            if (strncmp(buffer, "s", 1) == 0) {
                //read in the shot 
                if (player1.getShot(buffer) == true) {
                    count ++ ; 
                    write(player1fd, "hito", 5) ;
                }
                else {
                    write(player1fd, "miss", 5) ;
                }
                break;
            }
        }
        else if (ret == 0 && !waitingMessagePrinted) {
                // Timeout occurred, print waiting message
                cout << "Waiting for Player 2 to fire... " << endl;
                waitingMessagePrinted = true;
        }
    }
    //end of each turn , check if game is over
    if (count == 12) {
        write(player1fd, "over", 5) ;
        read(player2fd, buffer, 5) ;
        if (strcmp(buffer, "over") == 0) {
            cout << "You have tied the game!" << endl ; 
            break ; 
        }
        else {
            cout << "You have lost the game!" << endl ; 
            break ; 
        }
    }
    else {
        write(player1fd, "turn", 5) ;
        read(player2fd, buffer, 5) ;
        if (strcmp(buffer, "over") == 0) {
            cout << "You have won the game!" << endl ; 
            break ; 
        }
    }
}
    //close the fifos
    close(player1fd) ;
    close(player2fd) ;
    unlink(player1fifo) ;
    unlink(player2fifo) ;
    player1.closefifos() ;

    return 0 ; 
}