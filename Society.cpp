#include "Stronghold.h"
#include <string.h>

Society::Society() {
    strcpy_s(classes[0], "Peasants");
    strcpy_s(classes[1], "Merchants");
    strcpy_s(classes[2], "Nobles");
    strcpy_s(classes[3], "Clergy");

    for (int i = 0; i < 4; i++) {
        happiness[i] = 50; 
    }

    conflictLevel = 0; 
}

Society::~Society() {
}

int Society::getHappiness(int classIndex) const {
    if (classIndex < 0 || classIndex > 3) {
        return 0;
    }
    return happiness[classIndex];
}

int Society::getConflictLevel() const {
    return conflictLevel;
}

const char* Society::getClassName(int index) const {
    if (index < 0 || index > 3) {
        return "Invalid"; 
    }
    return classes[index];
}

void Society::displaySociety() const {
    cout << "\n===== SOCIAL STRUCTURE =====\n";
    for (int i = 0; i < 4; i++) {
        cout << classes[i] << " - Happiness: " << happiness[i] << "/100";

        cout << " [";
        for (int j = 0; j < 10; j++) {
            if (j < happiness[i] / 10) cout << "#";
            else cout << "-";
        }
        cout << "]" << endl;
    }
    cout << "Conflict Level: " << conflictLevel << "/100" << endl;

    if (isRevoltImminent()) {
        cout << "WARNING: Society is on the verge of revolt!" << endl;
    }
}

void Society::affectHappiness(int index, int change) {
    if (index >= 0 && index < 4) {
        happiness[index] += change;

        if (happiness[index] > 100) happiness[index] = 100;
        if (happiness[index] < 0) happiness[index] = 0;

        resolveConflicts();
    }
}

void Society::resolveConflicts() {
    conflictLevel = 0;

    int totalHappiness = 0;
    for (int i = 0; i < 4; i++) {
        totalHappiness += happiness[i];
    }
    int avgHappiness = totalHappiness / 4;

    for (int i = 0; i < 4; i++) {
        int deviation = abs(happiness[i] - avgHappiness);
        conflictLevel += deviation;
    }

    conflictLevel = conflictLevel / 4;

    for (int i = 0; i < 4; i++) {
        if (happiness[i] < 20) {
            conflictLevel += (20 - happiness[i]) / 2;
        }
    }

    if (conflictLevel > 100) conflictLevel = 100;
}

bool Society::isRevoltImminent() const {
    return (conflictLevel > 75);
}

void Society::saveToFile(ofstream& outFile) const {
    
    for (int i = 0; i < 4; i++) {
        outFile << classes[i] << endl;
    }

    for (int i = 0; i < 4; i++) {
        outFile << happiness[i] << endl;
    }

    outFile << conflictLevel << endl;
}

void Society::loadFromFile(ifstream& inFile) {
    for (int i = 0; i < 4; i++) {
        inFile >> classes[i];
    }

    for (int i = 0; i < 4; i++) {
        inFile >> happiness[i];
    }

    inFile >> conflictLevel;
}