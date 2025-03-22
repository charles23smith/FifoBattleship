#include "battleship.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <map>
using namespace std ; 

//default and only constructor
battleship:: battleship() {
    //loop and set each node to -
    for (auto& row : myArray) {
        for (char& index : row) {
            index = '-';
        }
    }
    for (auto& row : enemyBoard) {
        for (char& index : row) {
            index = '-';
        }
    }

}

void battleship :: openfifosP1(const char* player1fifo, const char* player2fifo) {
    player1fd = open(player1fifo, O_WRONLY) ; //only write moves for player 1 
    player2fd = open(player2fifo, O_RDONLY) ; //only read moves from player 2
}

void battleship :: openfifosP2(const char* player1fifo, const char* player2fifo) {
    player1fd = open(player1fifo, O_RDONLY) ; //only read moves from player 1
    player2fd = open(player2fifo, O_WRONLY) ; //only write moves for player 2 
}

void battleship :: closefifos() {
    close(player1fd) ; 
    close(player2fd) ; 
}


void battleship :: displayboard() { //prints player ships 
    int i = 0 ; 
    cout << "  " << "A B C D E F G H I J" << endl ;

     for (auto& row : myArray) {
            cout << i << " " ; 
           for (char& index : row) {
              cout << index << " " ; 
           }
           cout << endl ;  
           i ++ ; 
     }
}

//for the set ships function to make sure ship size is valid and that ship is placed in correct location 
bool battleship :: updateboard(char x, int y, int size, string direction) {
    int xindex = xMap[x] ; 
    if (direction == "up") {
        if (y - size + 1 < 0) {
            return false ; 
        }
        for (int i = 0; i < size; i++) {
            if (myArray[y - i][xindex] == 'O') {
                return false ; 
            }
        }
        for (int i = 0; i < size; i++) {
            myArray[y - i][xindex] = 'O' ; 
        }
    }
    else if (direction == "down") {
        if (y + size - 1 >= 10) {
            return false ; 
        }
        for (int i = 0; i < size; i++) {
            if (myArray[y + i][xindex] == 'O') {
                return false ; 
            }
        }
        for (int i = 0; i < size; i++) {
            myArray[y + i][xindex] = 'O' ; 
        }
    }
    else if (direction == "left") {
        if (xindex - size + 1 < 0) {
            return false ; 
        }
        for (int i = 0; i < size; i++) {
            if (myArray[y][xindex - i] == 'O') {
                return false ; 
            }
        }
        for (int i = 0; i < size; i++) {
            myArray[y][xindex - i ] = 'O' ; 
        }
    }
    else if (direction == "right") {
        if (xindex + size -1 >= 10) {
            return false ; 
        }
        for (int i = 0; i < size; i++) {
            if (myArray[y][xindex + i] == 'O') {
                return false ; 
            }
        }
        for (int i = 0; i < size; i++) {
            myArray[y][xindex + i] = 'O' ; 
        }
    }
    return true ;
}

//make sure y coordinate is valid 
bool battleship :: selectCoordinate(int y) {
    if (y > 9 || y < 0) {
        return false ; 
    }
    return true ; 
}

//make sure x coordinate is valid 
bool battleship :: selectCoordinate(char x) {
    if (xMap.find(x) == xMap.end()) {
        return false ; 
    }
    return true ; 
}

//make sure coordinate is not already taken
bool battleship :: takenCoordinate(char x, int y) {
    int xindex = xMap[x] ;
    if (myArray[y][xindex] == 'O') {
        return true ; 
    }
    return false ; 
}

//beginning call to set up the ships for each player 
void battleship :: setShips() {

    //required varirables 
    int y, size ; 
    char x ; 
    vector<int> usedShipSizes ; 
    string direction ; 
    int count = 0 ; 

   while(count < 3) { //make sure user inputs correct values for inputs and record each input 
        displayboard() ; 
        cout << endl ; 

        bool takenCoordinateBool = true ;
        bool PrintRepeatMessage  = false ; 

        while (takenCoordinateBool) { //loop to make sure coordinate is not already taken
            if (PrintRepeatMessage) {
                cout << "You have already placed a ship at this coordinate. Please select a new one. "  << endl ; 
                cout << endl ; 
            }

        cout << "Select the x coordinate of the nose of the ship: " ; 
        while(!(cin >> x) || !isalpha(x) || !selectCoordinate(x) || cin.peek() != '\n') {
            cin.clear() ;
            cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
            cout << "Please enter a valid x coordinate (A-J): " ; 
            //if (x == 'q') {return;} for debugging purposes
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;

        cout << "Select the y coordinate of the nose of the ship: " ;
        while(!(cin >> y) || cin.peek() != '\n' || !selectCoordinate(y)) {
            cin.clear() ;
            cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
            cout << "Please enter a valid y coordinate (0-9): " ; 
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;

        takenCoordinateBool = takenCoordinate(x, y) ;
        PrintRepeatMessage = true ;
    }

        /*//default debugging conditions
        if ( x == 'A' && y == 0) {
            defaults() ; 
            count = 3 ; 
            break ; 
        }
        if ( x == 'B'  && y == 0) {
            defaults2() ; 
            count = 3 ; 
            break ; 
        } */

        cout << "Select the size of the ship: " ;
        while(!(cin >> size) || cin.peek() != '\n' || (size !=  3 && size != 5 && size != 4) || find(usedShipSizes.begin(), usedShipSizes.end(), size) != usedShipSizes.end()) {
            cin.clear() ;
            cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
            cout << "Please enter a valid size of the ship (3, 4, 5 & Cannot be previously used): " ; 
        }
        usedShipSizes.push_back(size) ;
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;

        cout << "Select the direction of the ship from the following: up, down, left, right: " ;
        while(!(cin >> direction) || (direction != "up" && direction != "down" && direction != "left" && direction != "right" ) || updateboard(x, y, size, direction) == false) {
            cin.clear() ;
            cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
            cout << "Please enter a valid direction of the ship: " ; 
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;

        count ++ ;
        //show updated board ; 
        cout << "Here is your updated board: " << endl ;
        cout << endl ; 
   }
   displayboard() ; 
}

void battleship :: takeshot(int playerfifo, char& x, int& y) {

    //require variables to make sure shot is not already taken 
    bool takenCoordinate = true ; 
    int count = 0 ; 

    while (takenCoordinate) {
    if (count == 1) {
        cout << "You have already shot at this coordinate. Please select a new one. "  << endl ; 
        cout << endl ; 
    }

    cout << "Select the x coordinate of the shot: " ; 
    while(!(cin >> x) || !selectCoordinate(x) || cin.peek() != '\n') {
        cin.clear() ;
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
        cout << "Please enter a valid x coordinate: " ; 
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n') ;

    cout << "Select the y coordinate of the shot: " ;
    while(!(cin >> y) || !selectCoordinate(y)) {
        cin.clear() ;
        cin.ignore(numeric_limits<streamsize>::max(), '\n') ;
        cout << "Please enter a valid y coordinate: " ; 
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n') ;

    takenCoordinate = takenCoordinateEnemy(x, y) ;
    count = 1 ; 

}

    //send the shot to the other player 
    char buffer[5] ;
    snprintf(buffer, sizeof(buffer), "s%c%d", x, y);
    write(playerfifo, buffer, 5) ; 
}

bool battleship :: getShot(char buffer[5]) {

    //print where the other player shot 
    cout << "The other player has taken a shot at " << buffer[1] << " " << buffer[2] << endl ; 

    //find the coordinates where the player has shot 
    char x = buffer[1] ; 
    int xindex = xMap[x] ;
    int y = buffer[2] - '0' ; 

    //find if the shot is a hit or a miss 
    if (myArray[y][xindex] == 'O') {
        myArray[y][xindex] = 'X' ; 
        cout << "Hit!" << endl ; 
        cout << endl ; 
        return true ; 
    }
    else {
        myArray[y][xindex] = '/' ; 
        cout << "Miss!" << endl ; 
        cout << endl ; 
        return false ; 
    }
}

//this is for the enemy board to show if the taken shot is a hit or a miss 
void battleship :: shotConfirmation(char buffer[5], char x, int y) {
    if (buffer[0] == 'h') {
        cout << endl ; 
        cout << "You have hit the other player's ship!" << endl ; 
        cout << endl ; 
    }
    else {
        cout << endl ; 
        cout << "You have missed the other player's ship!" << endl ; 
        cout << endl ; 
    }

    int xindex = xMap[x] ;
    if (buffer[0] == 'h') {
        enemyBoard[y][xindex] = 'X' ; 
    }
    else {
        enemyBoard[y][xindex] = '/' ; 
    }
}

//track the shot at the enemy to make sure its not repeated 
bool battleship :: takenCoordinateEnemy(char x, int y) {
    int xindex = xMap[x] ;
    if (enemyBoard[y][xindex] == 'X' || enemyBoard[y][xindex] == '/') {
        return true ; 
    }
    return false ; 
}

//debugging defaults for player 1
void battleship :: defaults() {
    updateboard('A', 0, 5, "down") ; 
    updateboard('E', 5, 3, "right") ; 
    updateboard('J', 9, 4, "up") ; 
}

//debugging defaults for player 2
void battleship :: defaults2() {
    updateboard('B', 0, 5, "down") ; 
    updateboard('F', 5, 3, "right") ; 
    updateboard('J', 0, 4, "down") ; 
}

//show the game board for the player and the player's shots (the enemy board )
void battleship :: displayGameBoard() {
    cout << "Your Ships: " << endl ; 
    cout << endl ; 
    cout << "  " << "A B C D E F G H I J" << endl ;
    int i = 0 ; 
    for (auto& row : myArray) {
        cout << i << " " ; 
        for (char& index : row) {
            cout << index << " " ; 
        }
        cout << endl ;  
        i ++ ; 
    }
    cout << endl ; 
    cout << endl ; 
    cout << "Enemy Ships: " << endl ; 
    cout << endl ; 
    cout << "  " << "A B C D E F G H I J" << endl ;
    i = 0 ; 
    for (auto& row : enemyBoard) {
        cout << i << " " ; 
        for (char& index : row) {
            cout << index << " " ; 
        }
        cout << endl ;  
        i ++ ; 
    }
    cout << endl ; 
}