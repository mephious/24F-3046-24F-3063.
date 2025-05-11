#include "Stronghold.h"

Banking::Banking() {
    treasury = 2000;     
    loans = 0;           
    interestRate = 0.05f; 
    fraudLevel = 0;     
}

Banking::~Banking() {
}

int Banking::getTreasury() const {
    return treasury;
}

int Banking::getLoans() const {
    return loans;
}

float Banking::getInterestRate() const {
    return interestRate;
}

void Banking::takeLoan(int amount) {
    if (amount <= 0) return;

    treasury += amount;

    loans += amount;

    cout << "Took a loan of " << amount << " gold at " << (interestRate * 100) << "% interest." << endl;
}

void Banking::repayLoan(int amount) {
    if (amount <= 0 || amount > treasury) return;

    if (amount > loans) amount = loans;

    loans -= amount;
    treasury -= amount;

    cout << "Repaid " << amount << " gold of loans." << endl;

    if (loans == 0) {
        interestRate = max(0.03f, interestRate - 0.01f);
    }
}

void Banking::audit() {
    if (fraudLevel > 0) {
        int recoveredAmount = fraudLevel * 50; 
        treasury += recoveredAmount;

        cout << "Audit uncovered " << fraudLevel << "% fraud in the treasury!" << endl;
        cout << "Recovered " << recoveredAmount << " gold." << endl;

        fraudLevel = 0;
    }
    else {
        cout << "Audit completed. No fraud detected." << endl;
    }
}

void Banking::checkFraud() {
    if (rand() % 100 < 5) { 
        int fraudAmount = rand() % 5 + 1;
        fraudLevel += fraudAmount;

        if (fraudLevel > 20) fraudLevel = 20;

        int stolenAmount = treasury * fraudLevel / 100;
        treasury -= stolenAmount;
    }
}

void Banking::updateInterest() {
    int interestAmount = static_cast<int>(loans * interestRate);
    loans += interestAmount;

    if (loans > 0) {
        cout << "Interest of " << interestAmount << " gold added to loans." << endl;
        cout << "Total loan amount is now " << loans << " gold." << endl;
    }

    if (loans > treasury * 2) {
        interestRate += 0.01f; 
        cout << "High debt level! Interest rate increased to " << (interestRate * 100) << "%." << endl;
    }

    if (interestRate > 0.15f) interestRate = 0.15f; 
}

void Banking::displayBanking() const {
    cout << "\n===== BANKING =====\n";
    cout << "Treasury: " << treasury << " gold" << endl;
    cout << "Loans: " << loans << " gold" << endl;
    cout << "Interest Rate: " << (interestRate * 100) << "%" << endl;

    float debtRatio = (treasury > 0) ? static_cast<float>(loans) / treasury : 99.9f;
    cout << "Debt to Treasury Ratio: " << debtRatio << endl;

    cout << "Financial Status: ";
    if (loans == 0) cout << "Debt-Free";
    else if (debtRatio < 0.5f) cout << "Healthy";
    else if (debtRatio < 1.0f) cout << "Manageable";
    else if (debtRatio < 2.0f) cout << "Concerning";
    else cout << "Crisis";
    cout << endl;
}

void Banking::saveToFile(ofstream& outFile) const {
    outFile << treasury << endl;
    outFile << loans << endl;
    outFile << interestRate << endl;
    outFile << fraudLevel << endl;
}

void Banking::loadFromFile(ifstream& inFile) {
    inFile >> treasury;
    inFile >> loans;
    inFile >> interestRate;
    inFile >> fraudLevel;
}