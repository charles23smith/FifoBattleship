#ifndef BATTLESHIP 
#define BATTLESHIP 
#include <array>
#include <string>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std ; 

//battleship function definitions 

class battleship {
    public:
        battleship() ; //creates game board and sets winner to false 
        bool updateboard(char x, int y, int size, string direction) ;
        void setShips() ; //
        void displayboard() ; 
        bool getShot(char buffer[5]) ; //this is how the other computer will know where the shot lands 
        void takeshot( int playerfifo, char& x, int& y) ; 
        bool selectCoordinate(int y) ; //help with selection of valid coordinates
        bool selectCoordinate(char x) ; 
        bool takenCoordinate(char x, int y) ; //help with selection of valid coordinates
        void displayGameBoard() ; //shows player ships and misses  as well as shots taken on other board 

        void openfifosP1(const char* player1fifo, const char* player2fifo) ;
        void openfifosP2(const char* player1fifo, const char* player2fifo) ;
        void closefifos() ; 
        void shotConfirmation(char buffer[5], char x, int y) ;
        bool takenCoordinateEnemy(char x, int y) ;
        void defaults() ; //for debugging such that defualt ships are set 
        void defaults2() ; //for different default board

        private:
            array<array<char, 10>, 10> myArray ; 
            map<char,int> xMap = {
                {'A', 0},
                {'B', 1},
                {'C', 2},
                {'D', 3},
                {'E', 4},
                {'F', 5},
                {'G', 6},
                {'H', 7},
                {'I', 8},
                {'J', 9}
            } ;
            int player1fd ; 
            int player2fd ; 
            array<array<char, 10>, 10> enemyBoard ; 

};

#endif 