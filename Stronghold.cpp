#include "Stronghold.h"
#include <iomanip>

// Constructor
Stronghold::Stronghold(const string& name)
    : resourceLogger("resource_log.txt"),
    populationLogger("population_log.txt"),
    messageSystem(name) {
    kingdomName = name;
    turn = 1;
    gameOver = false;
    messageSystem.setMessageCount(0);  // Use the setter to initialize messageCount
    initializeKingdomRelationships();
    regions[0] = { "Central Plains", 0, 0, "", 10, 5 }; 
    regionCount = 0;
    initializeMap();
    
    cout << "Map initialized with " << regionCount << " regions." << endl;
    cout << "=== STRONGHOLD: " << kingdomName << " ESTABLISHED ===" << endl;
    initialize();
}

// Destructor
Stronghold::~Stronghold() {
    cout << "Kingdom of " << kingdomName << " has ended after " << turn << " turns." << endl;
    logScore();
}

Leadership& Stronghold::getLeadership() {
    return leadership;
}

Events& Stronghold::getEvents() {
    return events;
}

// Initialize the game
void Stronghold::initialize() {
    srand(static_cast<unsigned int>(time(nullptr)));
    cout << "Initializing kingdom systems..." << endl;
    resourceLogger.log("Initial Food", resources.getFood());
    resourceLogger.log("Initial Wood", resources.getWood());
    resourceLogger.log("Initial Stone", resources.getStone());
    resourceLogger.log("Initial Iron", resources.getIron());
    populationLogger.log("Initial Population", population.getTotalPopulation());
    cout << "Kingdom initialized successfully!" << endl;
}

void Stronghold::processTurn() {
    cout << "\n=== TURN " << turn << " ===" << endl;
    resources.gatherResources();
    resourceLogger.log("Food after gathering", resources.getFood());
    resourceLogger.log("Wood after gathering", resources.getWood());
    resourceLogger.log("Stone after gathering", resources.getStone());
    resourceLogger.log("Iron after gathering", resources.getIron());
    resources.consumeResources(population.getTotalPopulation(), military.getSoldiers());
    resourceLogger.log("Food after consumption", resources.getFood());
    int shelterCapacity = (resources.getWood() / 10) + (resources.getStone() / 5); // Simple formula
    population.updatePopulation(resources.getFood(), shelterCapacity);
    populationLogger.log("Updated Population", population.getTotalPopulation());
    economy.collectTax(population.getTotalPopulation());
    economy.updateInflation();
    military.paySoldiers(economy.getGold());
    military.updateMorale(resources.getFood(), true);

    // Update society happiness based on various factors
    society.affectHappiness(0, resources.getFood() > population.getTotalPopulation() ? 1 : -1);
    society.affectHappiness(1, economy.getInflation() < 5.0f ? 1 : -1);
    society.affectHappiness(2, banking.getTreasury() > 1000 ? 1 : -1);
    society.resolveConflicts();

    if (society.isRevoltImminent()) {
        population.startRevolt(population.getTotalPopulation() / 10);
        cout << "WARNING: A portion of the population is revolting!" << endl;
    }

    banking.updateInterest();
    banking.checkFraud();
    events.triggerRandomEvent(population, society, military, economy, resources);
    leadership.makeDecision(economy, military);

    // Check if game should end
    if (population.getTotalPopulation() <= 0 || leadership.getStability() <= 0) {
        gameOver = true;
        cout << "Your kingdom has fallen!" << endl;
    }

    turn++;
}

void Stronghold::handleUserCommands() {
    int choice;
    bool turnEnded = false;

    while (!turnEnded) {
        cout << "\n=== KINGDOM MANAGEMENT ===" << endl;
        cout << "1. View Kingdom Status" << endl;
        cout << "2. Adjust Tax Rate" << endl;
        cout << "3. Assign Workers" << endl;
        cout << "4. Recruit Soldiers" << endl;
        cout << "5. Trade Resources" << endl;
        cout << "6. Banking Operations" << endl;
        cout << "7. Trade Commands" << endl;
        cout << "8. Multiplayer Commands" << endl;
        cout << "9. Manage Diplomacy" << endl;
        cout << "10. End Turn" << endl;
        cout << "11. Save Game" << endl;
        cout << "12. Exit Game" << endl;
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            displayGameState();
            break;
        case 2: {
            float newRate;
            cout << "Current tax rate: " << economy.getTaxRate() << "%" << endl;
            cout << "Enter new tax rate (0-20%): ";
            cin >> newRate;
            if (newRate >= 0 && newRate <= 20) {
                economy.adjustTaxRate(newRate);
                cout << "Tax rate adjusted to " << newRate << "%" << endl;
            }
            else {
                cout << "Invalid tax rate. Must be between 0 and 20%." << endl;
            }
            break;
        }
        case 3: {
            int food, wood, stone, iron;
            int availableWorkers = population.getTotalPopulation() -
                (population.getSick() + population.getRevolting() + military.getSoldiers());
            cout << "Available workers: " << availableWorkers << endl;
            cout << "Assign workers to Food: ";
            cin >> food;
            cout << "Assign workers to Wood: ";
            cin >> wood;
            cout << "Assign workers to Stone: ";
            cin >> stone;
            cout << "Assign workers to Iron: ";
            cin >> iron;

            if (food + wood + stone + iron <= availableWorkers) {
                resources.assignWorkers(food, wood, stone, iron);
                cout << "Workers assigned successfully!" << endl;
            }
            else {
                cout << "Error: Total exceeds available workers!" << endl;
            }
            break;
        }
        case 4: {
            int recruits;
            int availableWorkers = population.getTotalPopulation() -
                (population.getSick() + population.getRevolting() + military.getSoldiers());
            cout << "Available people for recruitment: " << availableWorkers << endl;
            cout << "How many to recruit: ";
            cin >> recruits;

            if (recruits <= availableWorkers && recruits >= 0) {
                military.recruit(recruits);
                cout << recruits << " people recruited into the army!" << endl;
            }
            else {
                cout << "Invalid number of recruits!" << endl;
            }
            break;
        }
        case 5: {
            int foodAmount, woodAmount, stoneAmount, ironAmount;
            cout << "Enter amounts to trade (negative to sell, positive to buy):" << endl;
            cout << "Food: ";
            cin >> foodAmount;
            cout << "Wood: ";
            cin >> woodAmount;
            cout << "Stone: ";
            cin >> stoneAmount;
            cout << "Iron: ";
            cin >> ironAmount;

            int goldBefore = economy.getGold();
            resources.trade(goldBefore, foodAmount, woodAmount, stoneAmount, ironAmount);
            economy.updateGold(goldBefore);
            cout << "Trade complete. Gold changed from " << goldBefore << " to " << economy.getGold() << endl;
            break;
        }
        case 6: {
            int bankChoice;
            cout << "\n=== BANKING ===" << endl;
            cout << "1. Take Loan" << endl;
            cout << "2. Repay Loan" << endl;
            cout << "3. Audit Treasury" << endl;
            cout << "4. Back to Main Menu" << endl;
            cout << "Enter choice: ";
            cin >> bankChoice;

            switch (bankChoice) {
            case 1: {
                int amount;
                cout << "Enter loan amount: ";
                cin >> amount;
                banking.takeLoan(amount);
                break;
            }
            case 2: {
                int amount;
                cout << "Enter repayment amount: ";
                cin >> amount;
                banking.repayLoan(amount);
                break;
            }
            case 3:
                banking.audit();
                break;
            case 4:
                break;
            default:
                cout << "Invalid banking option!" << endl;
            }
            break;
        }
        case 7:
            handleTradeCommands();
            break;
        case 8:
            handleMultiplayerCommands();
            break;
        case 9:
            manageDiplomacy();
            break;
        case 10: 
            handleMapCommands();
            break;
        case 11:
            turnEnded = true;
            break;
        case 12: {
            string filename;
            cout << "Enter save filename: ";
            cin >> filename;
            saveGame(filename);
            break;
        }
        case 13:
            gameOver = true;
            turnEnded = true;
            cout << "Exiting game..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

// Display diplomacy menu
void Stronghold::displayDiplomacyMenu() {
    cout << "\n=== DIPLOMACY MENU ===" << endl;
    cout << "1. View Relationship Status" << endl;
    cout << "2. Send Message" << endl;
    cout << "3. Improve Relationship" << endl;
    cout << "4. Worsen Relationship" << endl;
    cout << "5. Propose Alliance" << endl;
    cout << "6. Declare War" << endl;
    cout << "7. Back to Main Menu" << endl;
    cout << "Enter your choice: ";
}

// Manage diplomatic actions
void Stronghold::manageDiplomacy() {
    int choice;
    bool done = false;

    while (!done) {
        displayDiplomacyMenu();

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            displayRelationshipStatus();
            break;
        case 2:
            sendDiplomaticMessage();
            break;
        case 3: {
            int kingdomIndex;
            cout << "Select kingdom (1-" << MAX_KINGDOMS << "): ";
            cin >> kingdomIndex;

            if (kingdomIndex >= 1 && kingdomIndex <= MAX_KINGDOMS) {
                int improvement;
                cout << "How much improvement (1-20): ";
                cin >> improvement;

                if (improvement < 1) improvement = 1;
                if (improvement > 20) improvement = 20;

                updateRelationship(kingdomIndex - 1, improvement);
                relationships[kingdomIndex - 1].lastInteractionType = "Improved relations";
                relationships[kingdomIndex - 1].lastInteractionTime = time(0);
                cout << "Relationship with " << relationships[kingdomIndex - 1].name
                    << " improved by " << improvement << "." << endl;
            }
            break;
        }
        case 4: {
            int kingdomIndex;
            cout << "Select kingdom (1-" << MAX_KINGDOMS << "): ";
            cin >> kingdomIndex;

            if (kingdomIndex >= 1 && kingdomIndex <= MAX_KINGDOMS) {
                int deterioration;
                cout << "How much deterioration (1-20): ";
                cin >> deterioration;

                if (deterioration < 1) deterioration = 1;
                if (deterioration > 20) deterioration = 20;

                updateRelationship(kingdomIndex - 1, -deterioration);
                relationships[kingdomIndex - 1].lastInteractionType = "Tensions increased";
                relationships[kingdomIndex - 1].lastInteractionTime = time(0);
                cout << "Relationship with " << relationships[kingdomIndex - 1].name
                    << " worsened by " << deterioration << "." << endl;
            }
            break;
        }
        case 5: {
            int kingdomIndex;
            cout << "Propose alliance to which kingdom (1-" << MAX_KINGDOMS << "): ";
            cin >> kingdomIndex;

            if (kingdomIndex >= 1 && kingdomIndex <= MAX_KINGDOMS) {
                // Simulate random acceptance chance based on current relationship
                int acceptanceChance = min(relationships[kingdomIndex - 1].status * 2, 100);
                if (rand() % 100 < acceptanceChance) {
                    relationships[kingdomIndex - 1].status = RELATIONSHIP_ALLIANCE;
                    relationships[kingdomIndex - 1].lastInteractionType = "Alliance formed";
                    relationships[kingdomIndex - 1].lastInteractionTime = time(0);

                    // Create treaty
                    relationships[kingdomIndex - 1].activeTreaty.kingdomName = relationships[kingdomIndex - 1].name;
                    relationships[kingdomIndex - 1].activeTreaty.type = DEFENSIVE_ALLIANCE;
                    relationships[kingdomIndex - 1].activeTreaty.duration = 10; // 10 turns
                    relationships[kingdomIndex - 1].activeTreaty.startDate = time(0);
                    relationships[kingdomIndex - 1].activeTreaty.terms = "Mutual defense pact";
                    relationships[kingdomIndex - 1].activeTreaty.isActive = true;

                    cout << relationships[kingdomIndex - 1].name
                        << " has accepted your alliance!" << endl;
                }
                else {
                    cout << relationships[kingdomIndex - 1].name
                        << " has declined your alliance offer." << endl;
                    // Some risk of worsening relations
                    if (rand() % 100 < 20) {
                        updateRelationship(kingdomIndex - 1, -5); // Slight decrease
                        relationships[kingdomIndex - 1].lastInteractionType = "Offended by alliance request";
                        relationships[kingdomIndex - 1].lastInteractionTime = time(0);
                        cout << "They are offended by your request. Relationship worsened." << endl;
                    }
                }
            }
            break;
        }
        case 6: {
            int kingdomIndex;
            cout << "Declare war on which kingdom (1-" << MAX_KINGDOMS << "): ";
            cin >> kingdomIndex;

            if (kingdomIndex >= 1 && kingdomIndex <= MAX_KINGDOMS) {
                // Check if already at war
                if (relationships[kingdomIndex - 1].status > RELATIONSHIP_WAR) {
                    cout << "You have declared war on "
                        << relationships[kingdomIndex - 1].name << "!" << endl;
                    relationships[kingdomIndex - 1].status = RELATIONSHIP_MIN;
                    relationships[kingdomIndex - 1].lastInteractionType = "War declared";
                    relationships[kingdomIndex - 1].lastInteractionTime = time(0);

                    // Other kingdoms might react
                    for (int i = 0; i < MAX_KINGDOMS; i++) {
                        if (i != kingdomIndex - 1 && relationships[i].status > RELATIONSHIP_ALLIANCE) {
                            // Allies of our enemy might turn against us
                            updateRelationship(i, -10);
                            cout << relationships[i].name << " disapproves of your war." << endl;
                        }
                    }
                }
                else {
                    cout << "You are already at war with "
                        << relationships[kingdomIndex - 1].name << "!" << endl;
                }
            }
            break;
        }
        case 7:
            done = true;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

// Initialize kingdom relationships
void Stronghold::initializeKingdomRelationships() {
    for (int i = 0; i < MAX_KINGDOMS; i++) {
        relationships[i].name = PREDEFINED_KINGDOMS[i];
        relationships[i].status = RELATIONSHIP_NEUTRAL;
        relationships[i].lastInteractionTime = time(0);
        relationships[i].activeTreaty.isActive = false;
        relationships[i].activeTreaty.type = PEACE;
    }
}

// Display relationship status
void Stronghold::displayRelationshipStatus() const {
    cout << "\n=== RELATIONSHIP STATUS ===" << endl;
    for (int i = 0; i < MAX_KINGDOMS; i++) {
        cout << i + 1 << ". " << relationships[i].name << ": "
            << getRelationshipStatus(relationships[i].status)
            << " (" << relationships[i].status << "/100)" << endl;
    }
}

// Get relationship status as text
string Stronghold::getRelationshipStatus(int value) const {
    if (value >= 90) return "Best Allies";
    if (value >= 75) return "Close Allies";
    if (value >= 60) return "Friendly";
    if (value >= 40) return "Neutral";
    if (value >= 25) return "Hostile";
    if (value >= 10) return "Aggressive";
    return "At War";
}

// Send diplomatic message
void Stronghold::sendDiplomaticMessage() {
    int receiverIndex;
    string message;

    cout << "Send message to:" << endl;
    for (int i = 0; i < MAX_KINGDOMS; i++) {
        cout << i + 1 << ". " << PREDEFINED_KINGDOMS[i] << endl;
    }
    cout << "Select recipient (1-" << MAX_KINGDOMS << "): ";
    cin >> receiverIndex;

    if (receiverIndex < 1 || receiverIndex > MAX_KINGDOMS) {
        cout << "Invalid selection." << endl;
        return;
    }

    cin.ignore(); // Clear newline from previous input
    cout << "Enter your message: ";
    getline(cin, message);

    // In a real game, you'd send this message somewhere
    // For now, we'll just log it and possibly affect relationship
    cout << "Message sent to " << PREDEFINED_KINGDOMS[receiverIndex - 1] << ": " << message << endl;

    // Simple logic to simulate effect of message
    if (message.find("peace") != string::npos ||
        message.find("alliance") != string::npos ||
        message.find("friend") != string::npos) {
        updateRelationship(receiverIndex - 1, 5);
        relationships[receiverIndex - 1].lastInteractionType = "Peaceful message received";
    }
    else if (message.find("threat") != string::npos ||
        message.find("war") != string::npos ||
        message.find("attack") != string::npos) {
        updateRelationship(receiverIndex - 1, -10);
        relationships[receiverIndex - 1].lastInteractionType = "Hostile message received";
    }
    else {
        relationships[receiverIndex - 1].lastInteractionType = "Neutral message received";
    }

    relationships[receiverIndex - 1].lastInteractionTime = time(0);
}

// Update relationship value
void Stronghold::updateRelationship(int index, int changeAmount) {
    if (index >= 0 && index < MAX_KINGDOMS) {
        relationships[index].status += changeAmount;

        // Clamp to valid range
        if (relationships[index].status > RELATIONSHIP_MAX)
            relationships[index].status = RELATIONSHIP_MAX;
        if (relationships[index].status < RELATIONSHIP_MIN)
            relationships[index].status = RELATIONSHIP_MIN;
    }
}

// Handle multiplayer commands
void Stronghold::handleMultiplayerCommands() {
    int choice;
    bool done = false;

    while (!done) {
        cout << "\n=== MULTIPLAYER MENU ===" << endl;
        cout << "1. Send Message" << endl;
        cout << "2. View Messages" << endl;
        cout << "3. Back to Main Menu" << endl;
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            sendMessages();
            break;
        case 2:
            viewMessages();
            break;
        case 3:
            done = true;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

// Send a message
void Stronghold::sendMessages() {
    cout << "\n=== SEND MESSAGE ===" << endl;
    string targetKingdom, message;

    cout << "Enter recipient kingdom: ";
    cin.ignore();  // Clear newline from previous input
    getline(cin, targetKingdom);

    cout << "Enter your message: ";
    getline(cin, message);

    if (messageSystem.sendMessage(targetKingdom, message)) {
        cout << "Message sent successfully!" << endl;
    }
    else {
        cout << "Failed to send message. The message system might be full." << endl;
    }
}

// View messages
void Stronghold::viewMessages() {
    messageSystem.displayMessages();
}

// Handle trade commands
void Stronghold::handleTradeCommands() {
    int choice;
    bool done = false;

    while (!done) {
        cout << "\n=== TRADE SYSTEM ===" << endl;
        cout << "1. Initiate Trade Deal" << endl;
        cout << "2. Execute Smuggling Operation" << endl;
        cout << "3. Check Trade Routes" << endl;
        cout << "4. Apply Embargo" << endl;
        cout << "5. Back to Main Menu" << endl;
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            initiateTrade();
            break;
        case 2:
            executeSmuggling();
            break;
        case 3:
            checkTradeRoutes();
            break;
        case 4:
            applyEmbargo();
            break;
        case 5:
            done = true;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

// Initiate trade deal
void Stronghold::initiateTrade() {
    int receiverIndex;
    cout << "Initiate trade with:" << endl;
    for (int i = 0; i < MAX_KINGDOMS; i++) {
        cout << i + 1 << ". " << PREDEFINED_KINGDOMS[i] << endl;
    }
    cout << "Select partner (1-" << MAX_KINGDOMS << "): ";
    cin >> receiverIndex;

    if (receiverIndex < 1 || receiverIndex > MAX_KINGDOMS) {
        cout << "Invalid selection." << endl;
        return;
    }

    // Check if we can trade with this kingdom
    if (relationships[receiverIndex - 1].status < RELATIONSHIP_MIN + 10) {
        cout << "Relations with " << relationships[receiverIndex - 1].name
            << " are too poor to establish trade." << endl;
        return;
    }

    // Simulate trade negotiation
    int baseValue = static_cast<int>(100 * (relationships[receiverIndex - 1].status / 100.0f));
    int availableGold = economy.getGold();
    cout << "Trading with " << relationships[receiverIndex - 1].name << endl;
    cout << "Base trade value: " << baseValue << " gold" << endl;
    cout << "Available gold: " << availableGold << endl;

    if (availableGold < TRADE_COST) {
        cout << "Insufficient funds to initiate trade." << endl;
        return;
    }

    economy.spendOnServices(TRADE_COST);

    // Determine what kind of trade deal we got
    int dealQuality = rand() % 100;
    if (dealQuality < 20) {
        cout << "Poor trade conditions. Minimal benefits." << endl;
        economy.adjustTaxRate(-0.01f); // Small economic boost
        resources.assignWorkers(0, 5, 0, 0); // Additional wood workers
    }
    else if (dealQuality < 60) {
        cout << "Standard trade agreement established." << endl;
        economy.adjustTaxRate(-0.02f);
        resources.assignWorkers(0, 10, 0, 0);
    }
    else {
        cout << "Favorable trade agreement secured!" << endl;
        economy.adjustTaxRate(-0.03f);
        resources.assignWorkers(0, 15, 0, 0);
    }

    cout << "Your kingdom will benefit from this trade agreement for several turns." << endl;
}

// Execute smuggling operation
void Stronghold::executeSmuggling() {
    int receiverIndex;
    cout << "Execute smuggling operation with:" << endl;
    for (int i = 0; i < MAX_KINGDOMS; i++) {
        cout << i + 1 << ". " << PREDEFINED_KINGDOMS[i] << endl;
    }
    cout << "Select partner (1-" << MAX_KINGDOMS << "): ";
    cin >> receiverIndex;

    if (receiverIndex < 1 || receiverIndex > MAX_KINGDOMS) {
        cout << "Invalid selection." << endl;
        return;
    }

    // Calculate success chance based on relationship
    int successChance = relationships[receiverIndex - 1].status / 2;
    // Random factor
    int roll = rand() % 100;

    if (roll < successChance) {
        cout << "Smuggling operation successful! Your kingdom gains hidden benefits." << endl;
        economy.spendOnServices(50); // Hidden resource gain
        resources.assignWorkers(0, 10, 0, 0); // Secret trade boost

        // But there's a cost...
        if (rand() % 100 < 30) {
            cout << "The authorities discovered part of your operation!" << endl;
            relationships[receiverIndex - 1].status -= 5; // Risk of deteriorating relations
        }
    }
    else {
        cout << "Smuggling operation failed! The authorities caught wind of your plans." << endl;
        relationships[receiverIndex - 1].status -= 10; // Relations worsen
        cout << "Your relationship with " << relationships[receiverIndex - 1].name
            << " has worsened due to the failed operation." << endl;
    }
}
void Stronghold::initializeMap() {
    // Initialize some default regions with random positions and bonuses
    const char* regionNames[MAX_REGIONS] = { "Riverlands", "Highlands", "Black Forest", "Coastal Plains", "Mountain Pass" };

    for (int i = 0; i < MAX_REGIONS; i++) {
        strcpy_s(regions[i].name, MAX_REGION_NAME, regionNames[i]);
        regions[i].positionX = rand() % MAP_SIZE + 1;  // Random X position between 1-MAP_SIZE
        regions[i].positionY = rand() % MAP_SIZE + 1;  // Random Y position between 1-MAP_SIZE

        // Random resource and military bonuses
        regions[i].resourceBonus = rand() % 20 + 5;  // 5-25% bonus
        regions[i].militaryBonus = rand() % 15 + 3; // 3-18% bonus

        // Initially all regions are neutral
        strcpy_s(regions[i].controller, MAX_KINGDOM_NAME, "Neutral");
    }

    regionCount = MAX_REGIONS;
}

void Stronghold::displayMap() const {
    cout << "\n=== KINGDOM MAP ===" << endl;
    cout << "Current Kingdom: " << kingdomName << endl;

    // Create a grid representation of the map
    for (int y = 1; y <= MAP_SIZE; y++) {
        for (int x = 1; x <= MAP_SIZE; x++) {
            // Check if this position has any region
            bool hasRegion = false;
            int regionIndex = -1;

            for (int r = 0; r < regionCount; r++) {
                if (regions[r].positionX == x && regions[r].positionY == y) {
                    hasRegion = true;
                    regionIndex = r;
                    break;
                }
            }

            if (hasRegion) {
                // Check who controls this region
                if (strcmp(regions[regionIndex].controller, "Neutral") == 0) {
                    cout << "[N]";  // Neutral region
                }
                else if (strcmp(regions[regionIndex].controller, kingdomName.c_str()) == 0) {
                    cout << "[Y]";  // Your territory
                }
                else {
                    // Determine if it's an ally or enemy
                    bool isAlly = false;
                    for (int i = 0; i < MAX_KINGDOMS; i++) {
                        if (relationships[i].status >= RELATIONSHIP_ALLIANCE &&
                            strcmp(relationships[i].name.c_str(), regions[regionIndex].controller) == 0) {
                            isAlly = true;
                            break;
                        }
                    }

                    if (isAlly) {
                        cout << "[A]";  // Ally's territory
                    }
                    else {
                        cout << "[E]";  // Enemy territory
                    }
                }
            }
            else {
                cout << "[ ]";  // No region at this location
            }
        }
        cout << endl;
    }

    // Display strategic information
    cout << "\nLegend:" << endl;
    cout << "[N] - Neutral Region" << endl;
    cout << "[Y] - Your Territory" << endl;
    cout << "[A] - Ally Territory" << endl;
    cout << "[E] - Enemy Territory" << endl;

    cout << "\nControlled Regions:" << endl;
    int controlledRegions = 0;
    for (int i = 0; i < regionCount; i++) {
        if (strcmp(regions[i].controller, kingdomName.c_str()) == 0) {
            cout << "- " << regions[i].name
                << " (" << regions[i].positionX << ", " << regions[i].positionY << ")"
                << endl;
            controlledRegions++;
        }
    }

    if (controlledRegions > 0) {
        cout << "Your controlled regions provide +" << controlledRegions * 5 << "% resource production." << endl;
    }

    // Show proximity to other kingdoms
    cout << "\nProximity to Other Kingdoms:" << endl;
    for (int i = 0; i < MAX_KINGDOMS; i++) {
        // Calculate distance to each region controlled by this kingdom
        int minDistance = MAP_SIZE * 2;  // Start with maximum possible distance

        // Find closest region controlled by this kingdom
        for (int myRegion = 0; myRegion < regionCount; myRegion++) {
            // Skip regions not controlled by us
            if (strcmp(regions[myRegion].controller, kingdomName.c_str()) != 0) continue;

            for (int otherRegion = 0; otherRegion < regionCount; otherRegion++) {
                // Skip regions not controlled by this foreign kingdom
                if (strcmp(regions[otherRegion].controller, relationships[i].name.c_str()) != 0) continue;

                int dx = regions[myRegion].positionX - regions[otherRegion].positionX;
                int dy = regions[myRegion].positionY - regions[otherRegion].positionY;
                int distance = static_cast<int>(sqrt(dx * dx + dy * dy));

                if (distance < minDistance) {
                    minDistance = distance;
                }
            }
        }

        cout << PREDEFINED_KINGDOMS[i] << ": "
            << "Distance: " << minDistance
            << " | Status: " << getRelationshipStatus(relationships[i].status) << endl;
    }
}
bool Stronghold::moveTroops(int fromRegion, int toRegion, int troops) {
    // Validate region indices
    if (fromRegion < 0 || fromRegion >= regionCount ||
        toRegion < 0 || toRegion >= regionCount) {
        cout << "Invalid region index." << endl;
        return false;
    }

    // Check if we control the source region
    if (strcmp(regions[fromRegion].controller, kingdomName.c_str()) != 0) {
        cout << "You don't control the source region. Cannot move troops." << endl;
        return false;
    }

    // Calculate distance between regions
    int dx = regions[fromRegion].positionX - regions[toRegion].positionY;
    int dy = regions[fromRegion].positionY - regions[toRegion].positionY;
    float distance = sqrtf(static_cast<float>(dx * dx + dy * dy));

    // Determine how many troops arrive based on distance
    int arrivalPercentage = max(50, 100 - static_cast<int>(distance * 10));
    int arrivingTroops = static_cast<int>(troops * (arrivalPercentage / 100.0f));

    // Update troop counts
    military.recruit(arrivingTroops);  // Simplified - assuming this increases morale slightly

    cout << "\nMoving troops from " << regions[fromRegion].name
        << " to " << regions[toRegion].name << "." << endl;
    cout << "Only " << arrivalPercentage << "% of troops arrived due to distance ("
        << distance << " units)." << endl;

    return true;
}

void Stronghold::claimRegion(int regionIndex) {
    if (regionIndex < 0 || regionIndex >= regionCount) {
        cout << "Invalid region selected." << endl;
        return;
    }

    // Check if already claimed
    if (strcmp(regions[regionIndex].controller, kingdomName.c_str()) == 0) {
        cout << "You already control " << regions[regionIndex].name << "." << endl;
        return;
    }

    // Check if it's neutral
    if (strcmp(regions[regionIndex].controller, "Neutral") == 0) {
        // Simple conquest logic - no battle needed for neutral territories
        cout << "Your forces have claimed " << regions[regionIndex].name << "!" << endl;
        strcpy_s(regions[regionIndex].controller, MAX_KINGDOM_NAME, kingdomName.c_str());

        // Apply regional bonuses to our kingdom
        economy.spendOnServices(regions[regionIndex].resourceBonus);  // Boost resource production
        military.paySoldiers(regions[regionIndex].militaryBonus);      // Improve soldier pay efficiency
    }
    else {
        // This is controlled by another kingdom - need to attack!
        cout << "This region is controlled by " << regions[regionIndex].controller
            << ". You must conquer it through battle." << endl;

        // Simulate battle for control
        bool battleSuccess;
        int enemyStrength = military.getSoldiers() * 0.75f; // Weaker than usual enemies
        military.battle(enemyStrength, battleSuccess);

        if (battleSuccess) {
            cout << "Victory! Your forces have taken control of "
                << regions[regionIndex].name << "." << endl;
            strcpy_s(regions[regionIndex].controller, MAX_KINGDOM_NAME, kingdomName.c_str());

            // Apply regional bonuses to our kingdom
            economy.spendOnServices(regions[regionIndex].resourceBonus);
            military.paySoldiers(regions[regionIndex].militaryBonus);
        }
        else {
            cout << "Defeat! Your attempt to take control has failed." << endl;
            leadership.changeStability(-5); // Political instability
            society.affectHappiness(0, -5); // Peasants suffer losses
        }
    }
}

void Stronghold::checkTerritoryStatus() {
    cout << "\n=== TERRITORIAL STATUS ===" << endl;
    int ourRegions = 0;
    int allyRegions = 0;
    int enemyRegions = 0;

    for (int i = 0; i < regionCount; i++) {
        if (strcmp(regions[i].controller, kingdomName.c_str()) == 0) {
            ourRegions++;
        }
        else if (strcmp(regions[i].controller, "Neutral") == 0) {
            // Do nothing - neutral
        }
        else {
            // Check if it's an ally or enemy
            bool isAlly = false;
            for (int j = 0; j < MAX_KINGDOMS; j++) {
                if (relationships[j].status >= RELATIONSHIP_ALLIANCE &&
                    strcmp(relationships[j].name.c_str(), regions[i].controller) == 0) {
                    allyRegions++;
                    isAlly = true;
                    break;
                }
            }

            if (!isAlly) {
                enemyRegions++;
            }
        }
    }

    cout << "You control " << ourRegions << " out of " << regionCount << " regions." << endl;
    cout << "Allies control " << allyRegions << " regions." << endl;
    cout << "Enemies control " << enemyRegions << " regions." << endl;

    if (ourRegions > 0) {
        cout << "Controlled regions provide bonuses to resource production and military effectiveness." << endl;
        economy.adjustTaxRate(economy.getTaxRate() * 0.95f); // 5% more efficient tax collection
        military.train(1); // Slight improvement in training
    }

    // Check if we control a majority
    if (ourRegions > regionCount / 2) {
        cout << "You control more than half the map!" << endl;
        economy.adjustTaxRate(economy.getTaxRate() * 0.9f); // 10% more efficient
        military.train(2); // Better training with more resources
    }
}
void Stronghold::saveMapToFile(const string& filename) const {
    ofstream outFile(filename, ios::app);  // Append mode

    if (!outFile.is_open()) {
        cout << "Error: Could not open map file for writing!" << endl;
        return;
    }

    for (int i = 0; i < regionCount; i++) {
        outFile << regions[i].name << "|";
        outFile << regions[i].positionX << "|";
        outFile << regions[i].positionY << "|";
        outFile << regions[i].controller << "|";
        outFile << regions[i].resourceBonus << "|";
        outFile << regions[i].militaryBonus << "|" << endl;
    }

    outFile.close();
}

void Stronghold::loadMapFromFile(const string& filename) {
    ifstream inFile(filename);

    if (!inFile.is_open()) {
        // This is normal if no map data has been saved yet
        return;
    }

    char line[300];  // Should be big enough for one line

    while (inFile.getline(line, 300)) {
        // Parse the line into its components
        char* name = strtok_s(line, "|", nullptr);
        char* posX = strtok_s(nullptr, "|", nullptr);
        char* posY = strtok_s(nullptr, "|", nullptr);
        char* controller = strtok_s(nullptr, "|", nullptr);
        char* resBonus = strtok_s(nullptr, "|", nullptr);
        char* milBonus = strtok_s(nullptr, "|", nullptr);

        if (name && posX && posY && controller && resBonus && milBonus) {
            for (int i = 0; i < regionCount; i++) {
                if (strcmp(name, regions[i].name) == 0) {
                    regions[i].positionX = atoi(posX);
                    regions[i].positionY = atoi(posY);
                    strcpy_s(regions[i].controller, MAX_KINGDOM_NAME, controller);
                    regions[i].resourceBonus = atoi(resBonus);
                    regions[i].militaryBonus = atoi(milBonus);
                    break;
                }
            }
        }
    }

    inFile.close();
}
// Implement handleMapCommands in Stronghold.cpp
void Stronghold::handleMapCommands() {
    int choice;
    bool done = false;

    while (!done) {
        cout << "\n=== MAP COMMANDS ===" << endl;
        cout << "1. View Map" << endl;
        cout << "2. Claim Region" << endl;
        cout << "3. Move Troops Between Territories" << endl;
        cout << "4. Check Territorial Status" << endl;
        cout << "5. Back to Main Menu" << endl;
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            displayMap();
            break;
        case 2: {
            int regionIndex;
            cout << "Available regions:" << endl;

            for (int i = 0; i < regionCount; i++) {
                cout << i + 1 << ". " << regions[i].name << " (controlled by "
                    << regions[i].controller << ")" << endl;
            }

            cout << "Select region to claim (1-" << regionCount << "): ";
            cin >> regionIndex;

            if (regionIndex >= 1 && regionIndex <= regionCount) {
                claimRegion(regionIndex - 1);
            }
            else {
                cout << "Invalid selection." << endl;
            }
            break;
        }
        case 3: {
            int fromRegion, toRegion, troops;
            cout << "Move troops between territories" << endl;
            cout << "From region (1-" << regionCount << "): ";
            cin >> fromRegion;
            cout << "To region (1-" << regionCount << "): ";
            cin >> toRegion;
            cout << "Number of troops to move: ";
            cin >> troops;

            if (fromRegion >= 1 && fromRegion <= regionCount &&
                toRegion >= 1 && toRegion <= regionCount) {
                moveTroops(fromRegion - 1, toRegion - 1, troops);
            }
            else {
                cout << "Invalid region selection." << endl;
            }
            break;
        }
        case 4:
            checkTerritoryStatus();
            break;
        case 5:
            done = true;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}
// Check trade routes
void Stronghold::checkTradeRoutes() {
    cout << "\n=== CURRENT TRADE ROUTES ===" << endl;
    int activeTrades = 0;

    for (int i = 0; i < MAX_KINGDOMS; i++) {
        if (relationships[i].activeTreaty.isActive &&
            relationships[i].activeTreaty.type == TRADE) {
            activeTrades++;
            cout << "- Trade route with " << relationships[i].name
                << " (benefits remaining: " << relationships[i].activeTreaty.duration << " turns)" << endl;
        }
    }

    if (activeTrades == 0) {
        cout << "No active trade routes." << endl;
    }
}

// Apply embargo
void Stronghold::applyEmbargo() {
    int targetIndex;
    cout << "Apply embargo against:" << endl;
    for (int i = 0; i < MAX_KINGDOMS; i++) {
        cout << i + 1 << ". " << PREDEFINED_KINGDOMS[i] << endl;
    }
    cout << "Select target (1-" << MAX_KINGDOMS << "): ";
    cin >> targetIndex;

    if (targetIndex < 1 || targetIndex > MAX_KINGDOMS) {
        cout << "Invalid selection." << endl;
        return;
    }

    // Applying an embargo hurts our own economy but more so the target
    cout << "You have imposed an embargo against " << relationships[targetIndex - 1].name << "." << endl;
    cout << "This will hurt your economy as well as theirs." << endl;

    // Our economy takes a small hit
    economy.spendOnServices(EMBARGO_PENALTY);
    // Their economy takes a bigger hit
    relationships[targetIndex - 1].status -= 10;

    // Notify everyone
    cout << relationships[targetIndex - 1].name << " is angered by your embargo." << endl;
}

// Display game state
void Stronghold::displayGameState() const {
    cout << "\n=== KINGDOM OF " << kingdomName << " - TURN " << turn << " ===" << endl;
    cout << "\n--- SOCIETY ---" << endl;
    society.displaySociety();
    cout << "\n--- POPULATION ---" << endl;
    population.displayPopulation();
    cout << "\n--- MILITARY ---" << endl;
    military.displayMilitary();
    cout << "\n--- LEADERSHIP ---" << endl;
    leadership.displayLeadership();
    cout << "\n--- ECONOMY ---" << endl;
    economy.displayEconomy();
    cout << "\n--- BANKING ---" << endl;
    banking.displayBanking();
    cout << "\n--- RESOURCES ---" << endl;
    resources.displayResources();
    cout << "\n--- CURRENT EVENTS ---" << endl;
    events.displayCurrentEvent();
    cout << "\n--- RELATIONSHIPS ---" << endl;
    displayRelationshipStatus();
    cout << "\n--- MESSAGES ---" << endl;
    messageSystem.displayMessages();
}

// Save game
void Stronghold::saveGame(const string& filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cout << "Error: Could not open file for saving game!" << endl;
        return;
    }

    try {
        outFile << kingdomName << endl;
        outFile << turn << endl;
        society.saveToFile(outFile);
        population.saveToFile(outFile);
        military.saveToFile(outFile);
        leadership.saveToFile(outFile);
        economy.saveToFile(outFile);
        banking.saveToFile(outFile);
        resources.saveToFile(outFile);
        saveRelationshipsToFile("relationships_" + filename);
        cout << "Game saved successfully to " << filename << endl;
    }
    catch (const exception& e) {
        cout << "Error saving game: " << e.what() << endl;
    }
    outFile.close();
}

// Load game
void Stronghold::loadGame(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "Error: Could not open file for loading game!" << endl;
        return;
    }

    try {
        getline(inFile, kingdomName);
        inFile >> turn;
        inFile.ignore();
        society.loadFromFile(inFile);
        population.loadFromFile(inFile);
        military.loadFromFile(inFile);
        leadership.loadFromFile(inFile);
        economy.loadFromFile(inFile);
        banking.loadFromFile(inFile);
        resources.loadFromFile(inFile);
        loadRelationshipsFromFile("relationships_" + filename);
        cout << "Game loaded successfully from " << filename << endl;
    }
    catch (const exception& e) {
        cout << "Error loading game: " << e.what() << endl;
    }
    inFile.close();
}

// Log game score
void Stronghold::logScore() {
    ofstream scoreFile("score.txt", ios::app);
    if (!scoreFile.is_open()) {
        cout << "Error: Could not open score file!" << endl;
        return;
    }

    int finalScore = population.getTotalPopulation() * 2 +
        economy.getGold() +
        military.getSoldiers() * 5 +
        resources.getFood() + resources.getWood() + resources.getStone() + resources.getIron() +
        leadership.getStability() * 10 -
        banking.getLoans();

    time_t now = time(0);
    char timeStr[26];
#ifdef _WIN32
    ctime_s(timeStr, sizeof(timeStr), &now);
#else
    char* result = ctime(&now);
    strncpy(timeStr, result, sizeof(timeStr));
    timeStr[sizeof(timeStr) - 1] = '\0';
#endif

    scoreFile << "Kingdom: " << kingdomName
        << " | Date: " << timeStr
        << " | Turns Survived: " << turn
        << " | Final Score: " << finalScore << endl;
    scoreFile << "Final Population: " << population.getTotalPopulation()
        << " | Gold: " << economy.getGold()
        << " | Military: " << military.getSoldiers() << endl;
    scoreFile << "-----------------------------------------------------" << endl;
    scoreFile.close();
    cout << "Final Score: " << finalScore << " (logged to score.txt)" << endl;
}

// Save relationships
void Stronghold::saveRelationshipsToFile(const string& filename) const {
    ofstream outFile(filename, ios::app);  // Append mode

    if (!outFile.is_open()) {
        cout << "Error: Could not open relationships file for saving!" << endl;
        return;
    }

    for (int i = 0; i < MAX_KINGDOMS; i++) {
        // Only save active relationships
        if (relationships[i].status != RELATIONSHIP_NEUTRAL || relationships[i].activeTreaty.isActive) {
            outFile << relationships[i].name << "|";
            outFile << relationships[i].status << "|";
            outFile << relationships[i].lastInteractionType << "|";
            outFile << relationships[i].lastInteractionTime << "|";

            // Save treaty information
            outFile << relationships[i].activeTreaty.kingdomName << "|";
            outFile << relationships[i].activeTreaty.type << "|";
            outFile << relationships[i].activeTreaty.duration << "|" << endl;
        }
    }

    outFile.close();
}

// Load relationships
void Stronghold::loadRelationshipsFromFile(const string& filename) {
    ifstream inFile(filename);

    if (!inFile.is_open()) {
        // This is normal if no relationships have been saved yet
        return;
    }

    char line[300];  // Should be big enough for one line
    while (inFile.getline(line, 300) && messageSystem.getMessageCount() < MAX_MESSAGES) {
        char* context = nullptr;
        // Parse the line into its components using strtok_s
        char* name = strtok_s(line, "|", &context);
        char* statusStr = strtok_s(nullptr, "|", &context);
        char* interactionType = strtok_s(nullptr, "|", &context);
        char* timestampStr = strtok_s(nullptr, "|", &context);
        char* treatyKingdom = strtok_s(nullptr, "|", &context);
        char* treatyTypeStr = strtok_s(nullptr, "|", &context);
        char* treatyDurationStr = strtok_s(nullptr, "|", &context);

        if (name && statusStr && interactionType && timestampStr && treatyKingdom && treatyTypeStr && treatyDurationStr) {
            // Find matching kingdom in our predefined list
            for (int i = 0; i < MAX_KINGDOMS; i++) {
                if (strcmp(name, PREDEFINED_KINGDOMS[i].c_str()) == 0) {
                    relationships[i].name = name;
                    relationships[i].status = atoi(statusStr);
                    relationships[i].lastInteractionType = interactionType;
                    relationships[i].lastInteractionTime = static_cast<time_t>(atol(timestampStr));

                    // Load treaty information
                    relationships[i].activeTreaty.kingdomName = treatyKingdom;
                    relationships[i].activeTreaty.type = static_cast<TreatyType>(atoi(treatyTypeStr));
                    relationships[i].activeTreaty.duration = atoi(treatyDurationStr);
                    relationships[i].activeTreaty.isActive = (relationships[i].activeTreaty.duration > 0);
                    relationships[i].activeTreaty.startDate = time(0);
                    relationships[i].activeTreaty.terms = "Not implemented in this version";

                    break;
                }
            }
        }
    }

    inFile.close();
}

// Display timestamp
void Stronghold::displayTimestamp(time_t timestamp) const {
    char buffer[26];  // Buffer for date/time string

#ifdef _WIN32
    ctime_s(buffer, sizeof(buffer), &timestamp);
#else
    char* result = ctime(&timestamp);
    strncpy(buffer, result, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination
#endif

    cout << buffer;
}

// Is game over?
bool Stronghold::isGameOver() const {
    return gameOver;
}

// Check if in war
bool Stronghold::isInWar() const {
    for (int i = 0; i < MAX_KINGDOMS; i++) {
        if (relationships[i].status < RELATIONSHIP_WAR) {
            return true;
        }
    }
    return false;
}

// Check if has alliance
bool Stronghold::hasAlliance() const {
    for (int i = 0; i < MAX_KINGDOMS; i++) {
        if (relationships[i].status >= RELATIONSHIP_ALLIANCE) {
            return true;
        }
    }
    return false;
}