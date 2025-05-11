#include "Stronghold.h"

Military::Military() {
    soldiers = 100;     
    trainingLevel = 50; 
    morale = 70;        
    corruption = 10;     
}

Military::~Military() {
}

int Military::getSoldiers() const {
    return soldiers;
}

int Military::getMorale() const {
    return morale;
}

int Military::getCorruption() const {
    return corruption;
}

void Military::recruit(int people) {
    if (people <= 0) return;

    soldiers += people;

    trainingLevel = (trainingLevel * (soldiers - people) + 10 * people) / soldiers;

    if (trainingLevel > 100) trainingLevel = 100;
    if (trainingLevel < 0) trainingLevel = 0;
}

void Military::train(int days) {
    if (days <= 0) return;

    trainingLevel += days * 2;

    if (trainingLevel > 100) trainingLevel = 100;

    morale += days;
    if (morale > 100) morale = 100;
}

void Military::updateMorale(int food, bool battleSuccess) {
    float foodFactor = static_cast<float>(food) / soldiers;
    if (foodFactor >= 1.0f) {
        morale += 5;
    }
    else {
        morale -= static_cast<int>((1.0f - foodFactor) * 20); 
    }

    if (battleSuccess) {
        morale += 10; 
    }
    else {
        morale -= 15; 
    }

    morale -= corruption / 10;

    if (morale > 100) morale = 100;
    if (morale < 0) morale = 0;
}

void Military::paySoldiers(int gold) {
    int expectedPay = 2;
    int totalExpectedPay = soldiers * expectedPay;

    float paymentRatio = static_cast<float>(gold) / totalExpectedPay;

    if (paymentRatio >= 1.0f) {
        morale += 5;
        corruption -= 2;
        if (corruption < 0) corruption = 0;
    }
    else if (paymentRatio <= 0.5f) {
        morale -= static_cast<int>((1.0f - paymentRatio) * 20);

        corruption += static_cast<int>((1.0f - paymentRatio) * 10);
    }

    if (morale > 100) morale = 100;
    if (morale < 0) morale = 0;
    if (corruption > 100) corruption = 100;
}

void Military::battle(int enemyStrength, bool& success) {
    int armyStrength = soldiers * (trainingLevel + morale) / 100;

    armyStrength = static_cast<int>(armyStrength * (1.0f - corruption / 100.0f));

    success = (armyStrength > enemyStrength);

    float casualtyRatio;
    if (success) {
        casualtyRatio = static_cast<float>(enemyStrength) / armyStrength * 0.1f; 
    }
    else {
        casualtyRatio = static_cast<float>(enemyStrength) / armyStrength * 0.2f;
    }

    if (casualtyRatio > 0.5f) casualtyRatio = 0.5f;

    int casualties = static_cast<int>(soldiers * casualtyRatio);
    soldiers -= casualties;

    if (soldiers < 0) soldiers = 0;

    updateMorale(soldiers, success);
}

void Military::displayMilitary() const {
    cout << "\n===== MILITARY =====\n";
    cout << "Soldiers: " << soldiers << endl;
    cout << "Training Level: " << trainingLevel << "/100" << endl;
    cout << "Morale: " << morale << "/100" << endl;
    cout << "Corruption: " << corruption << "/100" << endl;

    cout << "Army Strength: ";
    int strength = soldiers * (trainingLevel + morale) / 200;
    if (strength > 80) cout << "Formidable";
    else if (strength > 60) cout << "Strong";
    else if (strength > 40) cout << "Average";
    else if (strength > 20) cout << "Weak";
    else cout << "Very Weak";
    cout << endl;
}

void Military::saveToFile(ofstream& outFile) const {
    outFile << soldiers << endl;
    outFile << trainingLevel << endl;
    outFile << morale << endl;
    outFile << corruption << endl;
}

void Military::loadFromFile(ifstream& inFile) {
    inFile >> soldiers;
    inFile >> trainingLevel;
    inFile >> morale;
    inFile >> corruption;
}