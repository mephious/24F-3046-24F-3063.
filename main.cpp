// main.cpp
#include "Stronghold.h"
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace std;

// Function prototypes
void displayTitle();
void displayInstructions();
void startNewGame();
void loadExistingGame();


int main() {
	Events events; 
    try {
        srand(static_cast<unsigned int>(time(nullptr)));

        displayTitle();

        int choice = 0;
        bool exitGame = false;

        while (!exitGame) {
            cout << "\n=== MAIN MENU ===" << endl;
            cout << "1. New Game" << endl;
            cout << "2. Load Game" << endl;
            cout << "3. Instructions" << endl;
            cout << "4. Exit" << endl;
            cout << "Enter your choice: ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number." << endl;
                continue;
            }

            switch (choice) {
            case 1:
                startNewGame();
                break;

            case 2:
                loadExistingGame();
                break;

            case 3:
                displayInstructions();
                break;

            case 4:
                exitGame = true;
                cout << "Thank you for playing Stronghold! Exiting..." << endl;
                break;

            default:
                cout << "Invalid choice. Please try again." << endl;
            }
        }

        return 0;
    }
    catch (const exception& e) {
        cerr << "FATAL ERROR: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cerr << "UNKNOWN FATAL ERROR" << endl;
        return 1;
    }
}

void displayTitle() {
    cout << "=================================================" << endl;
    cout << "                  STRONGHOLD                     " << endl;
    cout << "       Medieval Kingdom Management Simulator     " << endl;
    cout << "               OOP Project - 2025                " << endl;
    cout << "    Developed by Ahmad Sufyan and Hamna Maryiam   " << endl;
    cout << "=================================================" << endl;
}


void displayInstructions() {
    cout << "\n=== STRONGHOLD INSTRUCTIONS ===" << endl;
    cout << "Stronghold is a medieval kingdom management simulator where you" << endl;
    cout << "must balance social classes, population needs, military power," << endl;
    cout << "economic decisions, and resource management to survive." << endl;
    cout << "\nYOUR ROLE:" << endl;
    cout << "As the ruler, you make strategic decisions about:" << endl;
    cout << "- Resource allocation (food, wood, stone, iron)" << endl;
    cout << "- Tax rates and economic policies" << endl;
    cout << "- Military recruitment and defense" << endl;
    cout << "- Social class stability and happiness" << endl;
    cout << "\nGAME MECHANICS:" << endl;
    cout << "- All systems are interconnected - decisions in one area affect others" << endl;
    cout << "- Random events can disrupt your plans" << endl;
    cout << "- Proper resource management is critical for survival" << endl;
    cout << "- Social unrest can lead to revolts if class happiness is too low" << endl;
    cout << "\nWINNING:" << endl;
    cout << "Your goal is to build a prosperous kingdom that survives as long as possible." << endl;
    cout << "The game ends if your population reaches zero or if your leadership fails." << endl;
    cout << "\nPress Enter to return to the main menu...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void startNewGame() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string kingdomName;
    cout << "\nEnter your kingdom name: ";
    getline(cin, kingdomName);
    string leaderName;
    cout << "Enter your leader's name: ";
    getline(cin, leaderName);

    Stronghold game(kingdomName);
    game.getLeadership().newLeader(leaderName);


    while (!game.isGameOver()) {
        game.displayGameState();
        game.handleUserCommands();
        game.processTurn();
    }
    game.getEvents().displayEvents();
	game.getEvents().displayCurrentEvent();
    cout << "\n=== GAME OVER ===" << endl;
}

void loadExistingGame() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string filename;
    cout << "\nEnter save file name: ";
    getline(cin, filename);

    ifstream checkFile(filename);
    if (!checkFile.good()) {
        cout << "Error: Save file '" << filename << "' not found!" << endl;
        checkFile.close();
        return;
    }
    checkFile.close();

    Stronghold game("Temporary");
    game.loadGame(filename);

    // Game loop
    while (!game.isGameOver()) {
        game.displayGameState();
        game.handleUserCommands();
        game.processTurn();
    }

    cout << "\n=== GAME OVER ===" << endl;
}