#include "Stronghold.h"

Resources::Resources() {
    food = 1000;    
    wood = 500;     
    stone = 300;      
    iron = 100;      

    foodWorkers = 50;
    woodWorkers = 30;
    stoneWorkers = 15;
    ironWorkers = 5;
}

Resources::~Resources() {
}

int Resources::getFood() const {
    return food;
}

int Resources::getWood() const {
    return wood;
}

int Resources::getStone() const {
    return stone;
}

int Resources::getIron() const {
    return iron;
}

void Resources::assignWorkers(int food, int wood, int stone, int iron) {
    if (food < 0 || wood < 0 || stone < 0 || iron < 0) {
        return;
    }

    int total = food + wood + stone + iron;
    if (total > 500) { 
        return;
    }

    foodWorkers = food;
    woodWorkers = wood;
    stoneWorkers = stone;
    ironWorkers = iron;

    cout << "Workers reassigned: " << endl;
    cout << "Food: " << foodWorkers << ", Wood: " << woodWorkers
        << ", Stone: " << stoneWorkers << ", Iron: " << ironWorkers << endl;
}

void Resources::gatherResources() {
    int foodGathered = foodWorkers * 10;
    int woodGathered = woodWorkers * 8;
    int stoneGathered = stoneWorkers * 5;
    int ironGathered = ironWorkers * 3;

    foodGathered = static_cast<int>(foodGathered * (0.9f + (rand() % 20) / 100.0f));
    woodGathered = static_cast<int>(woodGathered * (0.9f + (rand() % 20) / 100.0f));
    stoneGathered = static_cast<int>(stoneGathered * (0.9f + (rand() % 20) / 100.0f));
    ironGathered = static_cast<int>(ironGathered * (0.9f + (rand() % 20) / 100.0f));

    food += foodGathered;
    wood += woodGathered;
    stone += stoneGathered;
    iron += ironGathered;

    cout << "\n===== RESOURCES GATHERED =====\n";
    cout << "Food: +" << foodGathered << " (Total: " << food << ")" << endl;
    cout << "Wood: +" << woodGathered << " (Total: " << wood << ")" << endl;
    cout << "Stone: +" << stoneGathered << " (Total: " << stone << ")" << endl;
    cout << "Iron: +" << ironGathered << " (Total: " << iron << ")" << endl;
}

void Resources::consumeResources(int population, int military) {
    int foodConsumed = population * 1 + military * 2;
    int woodConsumed = population / 20 + military / 10; 
    int stoneConsumed = population / 100; 
    int ironConsumed = military / 20; 

    food -= foodConsumed;
    wood -= woodConsumed;
    stone -= stoneConsumed;
    iron -= ironConsumed;

    if (food < 0) food = 0;
    if (wood < 0) wood = 0;
    if (stone < 0) stone = 0;
    if (iron < 0) iron = 0;

    cout << "\n===== RESOURCES CONSUMED =====\n";
    cout << "Food: -" << foodConsumed << " (Remaining: " << food << ")" << endl;
    cout << "Wood: -" << woodConsumed << " (Remaining: " << wood << ")" << endl;
    cout << "Stone: -" << stoneConsumed << " (Remaining: " << stone << ")" << endl;
    cout << "Iron: -" << ironConsumed << " (Remaining: " << iron << ")" << endl;

    if (food < population / 2) {
        cout << "WARNING: Food shortage detected!" << endl;
    }
}

bool Resources::isFamine() const {
    return (food <= 0);
}

void Resources::trade(int& gold, int foodAmount, int woodAmount, int stoneAmount, int ironAmount) {
    int foodCost = foodAmount * 2; 
    int woodCost = woodAmount * 3; 
    int stoneCost = stoneAmount * 5;
    int ironCost = ironAmount * 10; 

    int totalCost = foodCost + woodCost + stoneCost + ironCost;

    if (foodAmount > 0 && foodAmount > food)
        return; 
    if (woodAmount > 0 && woodAmount > wood)
        return;
    if (stoneAmount > 0 && stoneAmount > stone) 
        return; 
    if (ironAmount > 0 && ironAmount > iron)
        return; 
    if (totalCost < 0 && -totalCost > gold)
        return;

    food -= foodAmount;
    wood -= woodAmount;
    stone -= stoneAmount;
    iron -= ironAmount;

    gold -= totalCost;

    cout << "\n===== TRADE EXECUTED =====\n";
    if (totalCost > 0) {
        cout << "Resources sold for " << totalCost << " gold." << endl;
    }
    else {
        cout << "Resources purchased for " << -totalCost << " gold." << endl;
    }
}

void Resources::displayResources() const {
    cout << "\n===== RESOURCES =====\n";
    cout << "Food: " << food << " (Workers: " << foodWorkers << ")" << endl;
    cout << "Wood: " << wood << " (Workers: " << woodWorkers << ")" << endl;
    cout << "Stone: " << stone << " (Workers: " << stoneWorkers << ")" << endl;
    cout << "Iron: " << iron << " (Workers: " << ironWorkers << ")" << endl;

    cout << "\nResource Status: ";
    int totalWorkers = foodWorkers + woodWorkers + stoneWorkers + ironWorkers;
    if (totalWorkers == 0) {
        cout << "No workers assigned";
    }
    else if (food < 100) {
        cout << "Critical food shortage!";
    }
    else if (foodWorkers < 20) {
        cout << "Food production is low";
    }
    else {
        cout << "Balanced";
    }
    cout << endl;
}

void Resources::saveToFile(ofstream& outFile) const {
    outFile << food << endl;
    outFile << wood << endl;
    outFile << stone << endl;
    outFile << iron << endl;
    outFile << foodWorkers << endl;
    outFile << woodWorkers << endl;
    outFile << stoneWorkers << endl;
    outFile << ironWorkers << endl;
}

void Resources::loadFromFile(ifstream& inFile) {
    inFile >> food;
    inFile >> wood;
    inFile >> stone;
    inFile >> iron;
    inFile >> foodWorkers;
    inFile >> woodWorkers;
    inFile >> stoneWorkers;
    inFile >> ironWorkers;
}