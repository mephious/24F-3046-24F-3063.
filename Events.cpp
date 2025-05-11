// Events.cpp
#include "Stronghold.h"
#include <cstring>

const string PREDEFINED_KINGDOMS[MAX_KINGDOMS] = {
    "Valdoria",
    "Thalorium",
    "Zephyria",
    "Dragondale",
    "Elvenhold"
};

Events::Events() {
    eventChance = 30;
    eventSeverity = 1;
    currentEventIndex = 0;
    strcpy_s(currentEvent, 100, ""); // Initialize currentEvent as empty
}

Events::~Events() {
}

void Events::triggerRandomEvent(Population& pop, Society& soc, Military& mil,
    Economy& econ, Resources& res) {
    int roll = rand() % 100;
    if (roll >= eventChance) {
        strcpy_s(currentEvent, 100, "No significant events occurred this turn.");
        return;
    }
    int eventType = rand() % 10;  // Randomly selects type of the event to occur
    // Randomly selects severity
    eventSeverity = (rand() % 5) + 1;
    cout << "\n=== EVENT ALERT ===\n";
    switch (eventType) {
    case 0:
        triggerFamine(pop, res);
        break;
    case 1:
        triggerDisease(pop);
        break;
    case 2:
        triggerWar(mil, econ);
        break;
    case 3:
        triggerWeatherDisaster(res);
        break;
    case 4:
        triggerTradeOpportunity(res, econ);
        break;
    case 5:
        triggerPoliticalUnrest(soc, econ);
        break;
    case 6:
        triggerReligiousEvent(soc);
        break;
    case 7:
        triggerForeignDiplomacy(econ, mil);
        break;
    case 8:
        triggerBountifulHarvest(res);
        break;
    case 9:
        triggerTechnologicalAdvancement(res, econ, mil);
        break;
    }
    // Increase chance of event happening next turn if severity was low
    if (eventSeverity <= 2) {
        eventChance += 5;
    }
    else {
        eventChance = 30; // Reset chance after a significant event
    }
    if (eventChance > 70) eventChance = 70;
}

// Event Famine
void Events::triggerFamine(Population& pop, Resources& res) {
    strcpy_s(currentEvent, 100, "FAMINE STRIKES THE KINGDOM");
    cout << "=== FAMINE STRIKES ===\n";
    int foodLoss = static_cast<int>(res.getFood() * (eventSeverity * 0.1f));
    int popLoss = static_cast<int>(pop.getTotalPopulation() * (eventSeverity * 0.05f));
    res.consumeResources(foodLoss, 0);
    cout << "Food supplies are dwindling due to crop failure!\n";
    cout << "Severity level: " << eventSeverity << "/5\n";
    cout << "Lost " << foodLoss << " food reserves.\n";
    if (eventSeverity >= 3) {
        cout << "The famine has caused " << popLoss << " deaths in your population.\n";
        pop.applyDisease(popLoss);
    }
}

// Event Disease
void Events::triggerDisease(Population& pop) {
    strcpy_s(currentEvent, 100, "DISEASE OUTBREAK");
    cout << "=== DISEASE OUTBREAK ===\n";
    cout << "A disease is spreading among your people!\n";
    cout << "Severity level: " << eventSeverity << "/5\n";
    pop.applyDisease(eventSeverity);
    switch (eventSeverity) {
    case 1:
        cout << "A mild flu is affecting your population. Some are unable to work.\n";
        break;
    case 2:
        cout << "A more serious illness has broken out. Many are bedridden.\n";
        break;
    case 3:
        cout << "A dangerous epidemic is spreading. Quarantine measures are needed.\n";
        break;
    case 4:
        cout << "A severe plague has hit your kingdom. Many are dying daily.\n";
        break;
    case 5:
        cout << "BLACK DEATH! Your population is being decimated by a terrible plague.\n";
        break;
    }
}

// Event War
void Events::triggerWar(Military& mil, Economy& econ) {
    strcpy_s(currentEvent, 100, "WAR AGAINST ENEMY KINGDOM");
    cout << "=== WAR ===\n";
    int enemyStrength = static_cast<int>(mil.getSoldiers() * (0.5f + (eventSeverity * 0.2f)));
    bool battleSuccess = false;
    cout << "Enemy forces are attacking your kingdom!\n";
    cout << "Severity level: " << eventSeverity << "/5\n";
    cout << "Enemy strength: " << enemyStrength << " soldiers\n";
    mil.battle(enemyStrength, battleSuccess);
    econ.warImpact(eventSeverity);
    if (battleSuccess) {
        cout << "Your forces have prevailed! The enemy retreats in disarray.\n";
        if (rand() % 100 < 50) {
            int goldPlunder = 100 * eventSeverity;
            cout << "Your troops captured " << goldPlunder << " gold from the enemy!\n";
        }
    }
    else {
        cout << "Your forces have been defeated! The enemy plunders your lands.\n";
        int additionalEconomicDamage = 50 * eventSeverity;
        cout << "The enemy has caused " << additionalEconomicDamage << " additional gold in damages.\n";
    }
}

void Events::triggerWeatherDisaster(Resources& res) {
    strcpy_s(currentEvent, 100, "WEATHER DISASTER");
    cout << "=== WEATHER DISASTER ===\n";
    char disasterType[20];
    switch (eventSeverity) {
    case 1:
        strcpy_s(disasterType, 20, "Heavy rainfall");
        break;
    case 2:
        strcpy_s(disasterType, 20, "Drought");
        break;
    case 3:
        strcpy_s(disasterType, 20, "Hailstorm");
        break;
    case 4:
        strcpy_s(disasterType, 20, "Freezing winter");
        break;
    case 5:
        strcpy_s(disasterType, 20, "Hurricane");
        break;
    default:
        strcpy_s(disasterType, 20, "Storm");
        break;
    }
    cout << disasterType << " has struck your kingdom!\n";
    cout << "Severity level: " << eventSeverity << "/5\n";
    int foodLoss = static_cast<int>(res.getFood() * (eventSeverity * 0.1f));
    int woodLoss = static_cast<int>(res.getWood() * (eventSeverity * 0.05f));
    int stoneLoss = static_cast<int>(res.getStone() * (eventSeverity * 0.03f));
    int dummy = 0;
    res.trade(dummy, -foodLoss, -woodLoss, -stoneLoss, 0);
    cout << "The disaster has damaged your resources:\n";
    cout << "Food lost: " << foodLoss << endl;
    cout << "Wood lost: " << woodLoss << endl;
    cout << "Stone lost: " << stoneLoss << endl;
}

void Events::triggerTradeOpportunity(Resources& res, Economy& econ) {
    strcpy_s(currentEvent, 100, "MERCHANT CARAVAN ARRIVED");
    cout << "=== MERCHANT CARAVAN ARRIVED ===\n";
    cout << "Foreign merchants have arrived offering special trade deals!\n";
    cout << "Opportunity level: " << eventSeverity << "/5\n";
    int goldCost = 50 * eventSeverity;
    int resourceGain = 20 * eventSeverity;
    cout << "The merchants offer the following deals:\n";
    cout << "1. Buy " << resourceGain << " food for " << goldCost << " gold\n";
    cout << "2. Buy " << resourceGain << " wood for " << goldCost << " gold\n";
    cout << "3. Buy " << resourceGain << " stone for " << resourceGain / 2 << " iron\n";
    cout << "4. Decline all offers\n";

    int choice = 4;
    cout << "Merchants leave without any incident.\n";
}

// Event Political Unrest
void Events::triggerPoliticalUnrest(Society& soc, Economy& econ) {
    strcpy_s(currentEvent, 100, "POLITICAL UNREST");
    cout << "=== POLITICAL UNREST ===\n";
    cout << "Political tensions are rising in your kingdom!\n";
    cout << "Severity level: " << eventSeverity << "/5\n";
    int classAffected = rand() % 4;
    int happinessChange = -eventSeverity * 5;
    soc.affectHappiness(classAffected, happinessChange);
    cout << "The " << soc.getClassName(classAffected) << " are particularly upset.\n";
    cout << "Their happiness has decreased by " << -happinessChange << " points.\n";
    if (eventSeverity >= 3) {
        cout << "The unrest is disrupting trade and tax collection.\n";
    }
}

// Event Religious Event
void Events::triggerReligiousEvent(Society& soc) {
    strcpy_s(currentEvent, 100, "RELIGIOUS FESTIVAL");
    cout << "=== RELIGIOUS EVENT ===\n";
    bool isPositive = (rand() % 2 == 0);
    if (isPositive) {
        cout << "A popular religious festival is taking place in your kingdom!\n";
        cout << "Blessing level: " << eventSeverity << "/5\n";
        for (int i = 0; i < 4; i++) {
            soc.affectHappiness(i, eventSeverity);
        }
        cout << "The festival has boosted the happiness of all social classes.\n";
    }
    else {
        cout << "Religious tensions have erupted in your kingdom!\n";
        cout << "Severity level: " << eventSeverity << "/5\n";
        int classAffected1 = rand() % 4;
        int classAffected2 = (classAffected1 + 1 + rand() % 3) % 4;
        soc.affectHappiness(classAffected1, -eventSeverity * 2);
        soc.affectHappiness(classAffected2, -eventSeverity);
        cout << "The " << soc.getClassName(classAffected1) << " and ";
        cout << soc.getClassName(classAffected2) << " are in conflict.\n";
    }
}

// Event Foreign Diplomacy
void Events::triggerForeignDiplomacy(Economy& econ, Military& mil) {
    strcpy_s(currentEvent, 100, "FOREIGN DIPLOMACY");
    cout << "=== FOREIGN DIPLOMACY ===\n";
    bool isFavorable = (rand() % 2 == 0);
    if (isFavorable) {
        cout << "A neighboring kingdom offers an alliance!\n";
        cout << "Benefit level: " << eventSeverity << "/5\n";
        int moraleBoost = eventSeverity * 5;
        cout << "Military morale increased by " << moraleBoost << " points.\n";
        mil.updateMorale(0, true);
        cout << "The alliance offers new trade routes and diplomatic advantages.\n";
    }
    else {
        cout << "A neighboring kingdom issues threats and demands tribute!\n";
        cout << "Severity level: " << eventSeverity << "/5\n";
        int tributeDemand = eventSeverity * 100;
        cout << "They demand " << tributeDemand << " gold as tribute.\n";
        cout << "Paying would avoid conflict but drain your treasury.\n";
        cout << "You reluctantly pay the tribute to avoid war.\n";
        econ.spendOnServices(tributeDemand);
    }
}

// Event Bountiful Harvest
void Events::triggerBountifulHarvest(Resources& res) {
    strcpy_s(currentEvent, 100, "BOUNTIFUL HARVEST");
    cout << "=== BOUNTIFUL HARVEST ===\n";
    cout << "This season's harvest has been exceptionally good!\n";
    cout << "Bounty level: " << eventSeverity << "/5\n";
    int foodBonus = static_cast<int>(res.getFood() * (eventSeverity * 0.1f));
    if (foodBonus < 50) foodBonus = 50 * eventSeverity;
    res.consumeResources(-foodBonus, 0);
    cout << "Your kingdom has gained " << foodBonus << " additional food!\n";
    if (eventSeverity >= 3) {
        int woodBonus = static_cast<int>(res.getWood() * (eventSeverity * 0.05f));
        if (woodBonus < 20) woodBonus = 20 * eventSeverity;
        int dummy = 0;
        res.trade(dummy, 0, woodBonus, 0, 0);
        cout << "The excellent growing conditions have also yielded " << woodBonus << " extra wood!\n";
    }
}

void Events::triggerTechnologicalAdvancement(Resources& res, Economy& econ, Military& mil) {
    strcpy_s(currentEvent, 100, "TECHNOLOGICAL ADVANCEMENT");
    cout << "=== TECHNOLOGICAL ADVANCEMENT ===\n";
    cout << "Your kingdom has made a technological breakthrough!\n";
    cout << "Advancement level: " << eventSeverity << "/5\n";

    int advancementType = rand() % 5;
    switch (advancementType) {
    case 0:
        cout << "Agricultural techniques have improved!\n";
        cout << "Food production will increase by " << (eventSeverity * 5) << "%\n";
        // You might want to add a technology system to track this bonus
        break;
    case 1:
        cout << "Mining techniques have improved!\n";
        cout << "Stone and iron production will increase by " << (eventSeverity * 5) << "%\n";
        // Similar to above, consider adding a cost modifier
        break;
    case 2:
        cout << "Military training techniques have improved!\n";
        cout << "Soldier effectiveness increased by " << (eventSeverity * 5) << "%\n";
        // Now we can directly affect the game state
        mil.train(eventSeverity * 2);
        break;
    case 3:
        cout << "Economic innovations have been implemented!\n";
        cout << "Tax efficiency increased by " << (eventSeverity * 3) << "%\n";
        // This would require adding an efficiency modifier to the Economy class
        break;
    case 4:
        cout << "Construction techniques have improved!\n";
        cout << "Building costs reduced by " << (eventSeverity * 5) << "%\n";
        // This would require adding cost modifiers to relevant functions
        break;
    }
}

void Events::logEvent(const string& msg) {
    if (currentEventIndex < MAX_EVENTS) {
        eventLog[currentEventIndex++] = msg;
    }
    else {
        for (int i = 1; i < MAX_EVENTS; ++i) {
            eventLog[i - 1] = eventLog[i];
        }
        eventLog[MAX_EVENTS - 1] = msg;
    }
}

void Events::displayEvents() {
    cout << "\n--- Recent Events ---\n";
    for (int i = 0; i < currentEventIndex; ++i) {
        cout << "[Event] " << eventLog[i] << "\n";
    }
}

void Events::displayCurrentEvent() const {
    cout << "Current Event: " << currentEvent << endl;
}