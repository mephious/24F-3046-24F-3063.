#include "Stronghold.h"

Leadership::Leadership() {
    leaderName = "Unnamed";
    stability = 50;     
    popularity = 50;     
    leadershipStyle = "Balanced";
}

Leadership::~Leadership() {
}

string Leadership::getLeaderName() const {
    return leaderName;
}

int Leadership::getStability() const {
    return stability;
}

int Leadership::getPopularity() const {
    return popularity;
}

void Leadership::newLeader(const string& name) {
    leaderName = name;

    int styleChoice = rand() % 4;
    switch (styleChoice) {
    case 0:
        leadershipStyle = "Benevolent";
        break;
    case 1:
        leadershipStyle = "Militaristic";
        break;
    case 2:
        leadershipStyle = "Economic";
        break;
    case 3:
        leadershipStyle = "Authoritarian";
        break;
    default:
        leadershipStyle = "Balanced";
    }

    stability = 40 + rand() % 21; 
    popularity = 40 + rand() % 21; 
}

void Leadership::changeStability(int value) {
    stability += value;

    if (stability > 100) stability = 100;
    if (stability < 0) stability = 0;
}

void Leadership::holdElection(const Society& society) {
    int supportLevel = 0;
    for (int i = 0; i < 4; i++) {
        int weight = (i == 2 || i == 3) ? 2 : 1;
        supportLevel += society.getHappiness(i) * weight;
    }
    supportLevel = supportLevel / 6;

    popularity = (popularity + supportLevel) / 2;

    stability += 20;
    if (stability > 100) stability = 100;

    cout << "\n===== ELECTION RESULTS =====\n";
    cout << "Leader " << leaderName << " has " << supportLevel << "% support from the people.\n";
    cout << "New popularity level: " << popularity << "/100\n";
    cout << "Stability increased to: " << stability << "/100\n";
}

bool Leadership::coupAttempt(int militarySupport) {
    int coupChance = militarySupport - popularity;

    coupChance += (100 - stability) / 2;

    bool coupSuccess = (rand() % 100) < coupChance;

    if (coupSuccess) {
        cout << "\n===== MILITARY COUP =====\n";
        cout << "The military has overthrown " << leaderName << "!\n";

        leaderName = "General " + leaderName.substr(0, 1) + ".";
        leadershipStyle = "Militaristic";

        stability = 30; 
        popularity = militarySupport; 
    }
    else {
        cout << "\n===== FAILED COUP ATTEMPT =====\n";
        cout << "The military's attempt to overthrow " << leaderName << " has failed!\n";

        stability -= 20;
        if (stability < 0) stability = 0;
    }

    return coupSuccess;
}

void Leadership::makeDecision(Economy& economy, Military& military) {
    cout << "\n===== LEADERSHIP DECISION =====\n";

    if (leadershipStyle == "Benevolent") {
        cout << leaderName << " focuses on public welfare!\n";
        economy.spendOnServices(economy.getGold() / 5);
    }
    else if (leadershipStyle == "Militaristic") {
        cout << leaderName << " prioritizes military strength!\n";
        military.train(5);
    }
    else if (leadershipStyle == "Economic") {
        cout << leaderName << " lowers tax rate to stimulate economy!\n";
        economy.adjustTaxRate(economy.getTaxRate() * 0.9f);
    }
    else if (leadershipStyle == "Authoritarian") {
        cout << leaderName << " increases military presence to maintain order!\n";
        military.recruit(50);
    }
    else {
        cout << leaderName << " maintains a balanced approach.\n";
        economy.spendOnServices(economy.getGold() / 10);
        military.train(2);
    }
}

void Leadership::displayLeadership() const {
    cout << "\n===== LEADERSHIP =====\n";
    cout << "Leader: " << leaderName << endl;
    cout << "Leadership Style: " << leadershipStyle << endl;
    cout << "Stability: " << stability << "/100" << endl;
    cout << "Popularity: " << popularity << "/100" << endl;

    cout << "Regime Status: ";
    if (stability > 80) cout << "Very Stable";
    else if (stability > 60) cout << "Stable";
    else if (stability > 40) cout << "Unstable";
    else if (stability > 20) cout << "Very Unstable";
    else cout << "On the Brink of Collapse";
    cout << endl;
}

void Leadership::saveToFile(ofstream& outFile) const {
    outFile << leaderName << endl;
    outFile << stability << endl;
    outFile << popularity << endl;
    outFile << leadershipStyle << endl;
}

void Leadership::loadFromFile(ifstream& inFile) {
    inFile >> leaderName;
    inFile >> stability;
    inFile >> popularity;
    inFile >> leadershipStyle;
}