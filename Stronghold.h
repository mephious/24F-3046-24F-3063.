#ifndef STRONGHOLD_H
#define STRONGHOLD_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstring>  // For strncpy, strtok
#include <cstdio>   // For FILE operations if needed

using namespace std;

// Forward declarations
class Society;
class Population;
class Military;
class Leadership;
class Economy;
class Banking;
class Resources;
class Events;
const int MAX_KINGDOM_NAME = 50;
//Map
const int MAP_SIZE = 10;          // Size of our map (MAP_SIZE x MAP_SIZE)
const int MAX_REGIONS = 5;       
const int MAX_REGION_NAME = 30;
// Relationship constants
const int RELATIONSHIP_NEUTRAL = 50;
const int RELATIONSHIP_ALLIANCE = 80;
const int RELATIONSHIP_WAR = 20;
const int RELATIONSHIP_MAX = 100;
const int RELATIONSHIP_MIN = 0;

// Trade system constants
const int TRADE_COST = 10;
const int SMUGGLING_CHANCE = 30;
const int EMBARGO_PENALTY = 50;

// Messaging system constants
const int MAX_MESSAGE_LENGTH = 256;
const int MAX_SENDER_NAME = 50;
const int MAX_RECEIVER_NAME = 50;
const int MAX_MESSAGES = 20;  // Maximum number of messages to store
const int MAX_KINGDOMS = 5;   // Maximum number of kingdoms

// Structs for multiplayer features
struct Message {
    char sender[MAX_SENDER_NAME];
    char receiver[MAX_RECEIVER_NAME];
    char content[MAX_MESSAGE_LENGTH];
    time_t timestamp;
};

enum TreatyType {
    PEACE,
    TRADE,
    DEFENSIVE_ALLIANCE,
    OFFENSIVE_ALLIANCE,
    WAR
};

struct Treaty {
    string kingdomName;
    TreatyType type;
    int duration; // turns remaining
    time_t startDate;
    bool isActive;
    string terms;
};

// Structure to track relationships with other kingdoms
struct KingdomRelationship {
    string name;
    int status; // 0-100, where 0 is enemy, 100 is ally
    Treaty activeTreaty;
    string lastInteractionType;
    time_t lastInteractionTime;
};

// Predefined kingdoms
extern const string PREDEFINED_KINGDOMS[MAX_KINGDOMS];

// Society class
class Society {
private:
    char classes[4][20];
    int happiness[4];
    int conflictLevel;
public:
    Society();
    ~Society();

    int getHappiness(int classIndex) const;
    int getConflictLevel() const;
    const char* getClassName(int index) const;

    void displaySociety() const;
    void affectHappiness(int index, int change);
    void resolveConflicts();
    bool isRevoltImminent() const;

    void saveToFile(ofstream& outFile) const;
    void loadFromFile(ifstream& inFile);
};

// Population class
class Population {
private:
    int totalPopulation;
    int sick;
    int revolting;
    float growthRate;
public:
    Population();
    ~Population();

    int getTotalPopulation() const;
    int getSick() const;
    int getRevolting() const;

    void updatePopulation(int foodSupply, int shelter);
    void applyDisease(int severity);
    void startRevolt(int unhappyPeople);
    void displayPopulation() const;

    void saveToFile(ofstream& outFile) const;
    void loadFromFile(ifstream& inFile);
};

// Military class
class Military {
private:
    int soldiers;
    int trainingLevel;
    int morale;
    int corruption;
public:
    Military();
    ~Military();

    int getSoldiers() const;
    int getMorale() const;
    int getCorruption() const;

    void recruit(int people);
    void train(int days);
    void updateMorale(int food, bool battleSuccess);
    void paySoldiers(int gold);
    void battle(int enemyStrength, bool& success);
    void displayMilitary() const;

    void saveToFile(ofstream& outFile) const;
    void loadFromFile(ifstream& inFile);
};

// Leadership class
class Leadership {
private:
    string leaderName;
    int stability;
    int popularity;
    string leadershipStyle;
public:
    Leadership();
    ~Leadership();

    string getLeaderName() const;
    int getStability() const;
    int getPopularity() const;

    void newLeader(const string& name);
    void changeStability(int value);
    void holdElection(const Society& society);
    bool coupAttempt(int militarySupport);
    void makeDecision(Economy& economy, Military& military);
    void displayLeadership() const;

    void saveToFile(ofstream& outFile) const;
    void loadFromFile(ifstream& inFile);
};

// Economy class
class Economy {
private:
    int gold;
    float taxRate;
    float inflation;
    int publicServices;
public:
    Economy();
    ~Economy();

    int getGold() const;
    float getTaxRate() const;
    float getInflation() const;

    void collectTax(int population);
    void adjustTaxRate(float rate);
    void spendOnServices(int amount);
    void warImpact(int severity);
    void updateInflation();
    void displayEconomy() const;

    void saveToFile(ofstream& outFile) const;
    void loadFromFile(ifstream& inFile);

    friend class Stronghold;
    void updateGold(int amount);
};

// Banking class
class Banking {
private:
    int treasury;
    int loans;
    float interestRate;
    int fraudLevel;
public:
    Banking();
    ~Banking();

    int getTreasury() const;
    int getLoans() const;
    float getInterestRate() const;

    void takeLoan(int amount);
    void repayLoan(int amount);
    void audit();
    void checkFraud();
    void updateInterest();
    void displayBanking() const;

    void saveToFile(ofstream& outFile) const;
    void loadFromFile(ifstream& inFile);
};

// Resources class
class Resources {
private:
    int food;
    int wood;
    int stone;
    int iron;

    int foodWorkers;
    int woodWorkers;
    int stoneWorkers;
    int ironWorkers;
public:
    Resources();
    ~Resources();

    int getFood() const;
    int getWood() const;
    int getStone() const;
    int getIron() const;

    void assignWorkers(int food, int wood, int stone, int iron);
    void gatherResources();
    void consumeResources(int population, int military);
    bool isFamine() const;
    void trade(int& gold, int foodAmount, int woodAmount, int stoneAmount, int ironAmount);
    void displayResources() const;

    void saveToFile(ofstream& outFile) const;
    void loadFromFile(ifstream& inFile);
};

// Events class
class Events {
private:
    int eventChance;
    int eventSeverity;
    char currentEvent[100];
    static const int MAX_EVENTS = 10;
    string eventLog[MAX_EVENTS];
    int currentEventIndex;

public:
    Events();
    ~Events();

    void triggerRandomEvent(Population& pop, Society& soc, Military& mil,
        Economy& econ, Resources& res);
    void triggerFamine(Population& pop, Resources& res);
    void triggerDisease(Population& pop);
    void triggerWar(Military& mil, Economy& econ);
    void triggerWeatherDisaster(Resources& res);
    void triggerTradeOpportunity(Resources& res, Economy& econ);
    void triggerPoliticalUnrest(Society& soc, Economy& econ);
    void triggerReligiousEvent(Society& soc);
    void triggerForeignDiplomacy(Economy& econ, Military& mil);
    void triggerBountifulHarvest(Resources& res);
    void triggerTechnologicalAdvancement(Resources& res, Economy& econ, Military& mil);

    void logEvent(const string& msg);
    void displayEvents();
    void displayCurrentEvent() const;
};

// Logger class
class Logger {
private:
    string logName;
    ofstream logFile;
public:
    Logger(const string& name);
    ~Logger();

    void log(const string& action, int value);
    void displayLog();
};

// MessageSystem class
class MessageSystem {
private:
    int messageCount;
    Message messages[MAX_MESSAGES];
    string currentKingdom;

public:
    MessageSystem() : messageCount(0) {
        for (int i = 0; i < MAX_MESSAGES; i++) {
            messages[i].sender[0] = '\0';
            messages[i].receiver[0] = '\0';
            messages[i].content[0] = '\0';
            messages[i].timestamp = 0;
        }
    }
    MessageSystem(const string& kingdomName);
    ~MessageSystem();

    // Getter and setter for messageCount
    int getMessageCount() const { return messageCount; }
    void setMessageCount(int count) { messageCount = count; }

    // Messaging functions
    bool sendMessage(const string& receiver, const string& content);
    void displayMessages() const;
    void saveToFile(const string& filename) const;
    void loadFromFile(const string& filename);

    // Utility functions
    void displayTimestamp(time_t timestamp) const;

    int countUnreadMessages() const;
    bool hasMessages() const;
};
struct MapRegion {
    char name[MAX_REGION_NAME];      // Name of the region
    int positionX;                   // X coordinate on the map
    int positionY;                   // Y coordinate on the map
    char controller[MAX_KINGDOM_NAME]; // Current kingdom controlling this region
    int resourceBonus;               // Resource production bonus (percentage)
    int militaryBonus;              // Military advantage (e.g., fortifications)
};
// Stronghold class
class Stronghold {
private:
    Society society;
    Population population;
    Military military;
    Leadership leadership;
    Economy economy;
    Banking banking;
    Resources resources;
    Events events;
    MessageSystem messageSystem;

    // Multiplayer and Diplomacy Engine
    KingdomRelationship relationships[MAX_KINGDOMS];  // Relationships with other kingdoms

    int turn;
    bool gameOver;
    string kingdomName;

    Logger resourceLogger;
    Logger populationLogger;
    MapRegion regions[MAX_REGIONS];  // Our map consisting of multiple regions
    int regionCount;                // Number of active regions

    void initializeMap();
    void displayMap() const;
    bool moveTroops(int fromRegion, int toRegion, int troops);
    void claimRegion(int regionIndex);
    void checkTerritoryStatus();

public:
    Stronghold(const string& name);
    ~Stronghold();

    Leadership& getLeadership();
    Events& getEvents();

    void initialize();
    void processTurn();
    void handleUserCommands();
    bool isGameOver() const;
    void displayGameState() const;

    void saveGame(const string& filename);
    void loadGame(const string& filename);
    void logScore();

    // Multiplayer functions
    void handleMultiplayerCommands();
    void sendMessages();
    void viewMessages();

    // Diplomacy functions
    void manageDiplomacy();
    void displayDiplomacyMenu();
    void sendDiplomaticMessage();
    void updateRelationship(int index, int changeAmount);
    void displayRelationshipStatus() const;
    string getRelationshipStatus(int value) const;
    void initiateTrade();
    void executeSmuggling();
    void checkTradeRoutes();
    void applyEmbargo();
    void handleTradeCommands();

    // Helper functions
    void displayTimestamp(time_t timestamp) const;
    void saveRelationshipsToFile(const string& filename) const;
    void loadRelationshipsFromFile(const string& filename);
    void initializeKingdomRelationships();
    void saveMapToFile(const string& filename) const;
    void loadMapFromFile(const string& filename);
    void handleMapCommands();
    // Status checks
    bool isInWar() const;
    bool hasAlliance() const;
};

#endif