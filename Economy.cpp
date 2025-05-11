// Economy.cpp
#include "Stronghold.h"
#include <iomanip>

Economy::Economy() {
    gold = 1000;
    taxRate = 0.10f;
    inflation = 0.02f;
    publicServices = 50;
}

Economy::~Economy() {
}

int Economy::getGold() const {
    return gold;
}

float Economy::getTaxRate() const {
    return taxRate;
}

float Economy::getInflation() const {
    return inflation;
}

void Economy::collectTax(int population) {
    int baseRevenue = static_cast<int>(population * taxRate * 2);
    int actualRevenue = static_cast<int>(baseRevenue * (1.0f - inflation));
    gold += actualRevenue;
    cout << "Collected " << actualRevenue << " gold in taxes." << endl;
}

void Economy::adjustTaxRate(float rate) {
    if (rate < 0.0f) rate = 0.0f;
    if (rate > 0.5f) rate = 0.5f;

    taxRate = rate;

    if (taxRate > 0.25f) {
        inflation += 0.01f;
    }
    else if (taxRate < 0.1f) {
        inflation -= 0.005f;
    }

    if (inflation < 0.0f) inflation = 0.0f;
    if (inflation > 0.5f) inflation = 0.5f;
}

void Economy::spendOnServices(int amount) {
    if (amount <= 0 || amount > gold) return;

    gold -= amount;

    int serviceIncrease = amount / 20;
    publicServices += serviceIncrease;

    if (publicServices > 100) publicServices = 100;

    cout << "Spent " << amount << " gold on public services." << endl;
    cout << "Public services level is now " << publicServices << "/100." << endl;
}

void Economy::warImpact(int severity) {
    gold -= severity * 50;
    inflation += severity * 0.005f;
    publicServices -= severity * 2;

    if (gold < 0) gold = 0;
    if (publicServices < 0) publicServices = 0;

    if (inflation > 0.5f) inflation = 0.5f;
}

void Economy::updateInflation() {
    if (inflation > 0.05f) {
        inflation -= 0.005f;
    }
    else if (inflation < 0.02f) {
        inflation += 0.002f;
    }

    if (publicServices > 70) {
        inflation -= 0.002f;
    }

    if (inflation < 0.0f) inflation = 0.0f;
    if (inflation > 0.5f) inflation = 0.5f;
}

void Economy::displayEconomy() const {
    cout << "\n===== ECONOMY =====" << endl;
    cout << "Treasury: " << gold << " gold" << endl;
    cout << "Tax Rate: " << (taxRate * 100) << "%" << endl;
    cout << "Inflation: " << (inflation * 100) << "%" << endl;
    cout << "Public Services: " << publicServices << "/100" << endl;

    cout << "Economic Status: ";
    if (gold > 5000) cout << "Flourishing";
    else if (gold > 2000) cout << "Prosperous";
    else if (gold > 1000) cout << "Stable";
    else if (gold > 500) cout << "Struggling";
    else cout << "In Crisis";
    cout << endl;
}

void Economy::saveToFile(ofstream& outFile) const {
    outFile << gold << endl;
    outFile << taxRate << endl;
    outFile << inflation << endl;
    outFile << publicServices << endl;
}

void Economy::loadFromFile(ifstream& inFile) {
    inFile >> gold;
    inFile >> taxRate;
    inFile >> inflation;
    inFile >> publicServices;
}

void Economy::updateGold(int amount) {
    gold = amount;
}