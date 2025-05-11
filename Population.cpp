#include "Stronghold.h"

Population::Population() {
    totalPopulation = 1000; 
    sick = 0;                
    revolting = 0;           
    growthRate = 0.05f;     
}

Population::~Population() {
}

int Population::getTotalPopulation() const {
    return totalPopulation;
}

int Population::getSick() const {
    return sick;
}

int Population::getRevolting() const {
    return revolting;
}

void Population::updatePopulation(int foodSupply, int shelter) {
    int effectivePopulation = totalPopulation - sick - revolting;

    float foodFactor = min(1.0f, static_cast<float>(foodSupply) / effectivePopulation);
    float shelterFactor = min(1.0f, static_cast<float>(shelter) / effectivePopulation);

    float adjustedGrowthRate = growthRate * foodFactor * shelterFactor;

    int growth = static_cast<int>(effectivePopulation * adjustedGrowthRate);

    if (foodFactor < 0.5f) {
        int starvation = static_cast<int>(effectivePopulation * (0.5f - foodFactor) * 0.2f);
        growth -= starvation;
    }

    totalPopulation += growth;
    if (totalPopulation < 0) totalPopulation = 0;

    int recovery = sick / 4; 
    sick -= recovery;
    if (sick < 0) sick = 0;

    int calmDown = revolting / 5; 
    revolting -= calmDown;
    if (revolting < 0) revolting = 0;
}

void Population::applyDisease(int severity) {
    float sickRatio = static_cast<float>(severity) / 100.0f;
    int newSick = static_cast<int>((totalPopulation - sick) * sickRatio);

    sick += newSick;

    int deaths = sick / 10; 
    totalPopulation -= deaths;
    sick -= deaths;

    if (totalPopulation < 0) totalPopulation = 0;
    if (sick < 0) sick = 0;
}

void Population::startRevolt(int unhappyPeople) {
    revolting += unhappyPeople;

    if (revolting > totalPopulation) {
        revolting = totalPopulation;
    }
}

void Population::displayPopulation() const {
    cout << "\n===== POPULATION =====\n";
    cout << "Total Population: " << totalPopulation << endl;
    cout << "Healthy Population: " << (totalPopulation - sick - revolting) << endl;
    cout << "Sick: " << sick << " (" << (static_cast<float>(sick) / totalPopulation * 100) << "%)" << endl;
    cout << "Revolting: " << revolting << " (" << (static_cast<float>(revolting) / totalPopulation * 100) << "%)" << endl;
    cout << "Growth Rate: " << (growthRate * 100) << "% per turn" << endl;
}

void Population::saveToFile(ofstream& outFile) const {
    outFile << totalPopulation << endl;
    outFile << sick << endl;
    outFile << revolting << endl;
    outFile << growthRate << endl;
}

void Population::loadFromFile(ifstream& inFile) {
    inFile >> totalPopulation;
    inFile >> sick;
    inFile >> revolting;
    inFile >> growthRate;
}