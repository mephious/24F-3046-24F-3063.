#include "Stronghold.h"
#include <fstream>
#include <ctime>
#include <iomanip>

MessageSystem::MessageSystem(const string& kingdomName) : messageCount(0), currentKingdom(kingdomName) {
    for (int i = 0; i < MAX_MESSAGES; i++) {
        messages[i].sender[0] = '\0';
        messages[i].receiver[0] = '\0';
        messages[i].content[0] = '\0';
        messages[i].timestamp = 0;
    }
}

MessageSystem::~MessageSystem() {
    // No dynamic memory to clean up
}

bool MessageSystem::sendMessage(const string& receiver, const string& content) {
    if (messageCount >= MAX_MESSAGES) {
        cout << "Error: Message system is full. Cannot send new messages." << endl;
        return false;
    }

    // Add the message to our array
    strncpy_s(messages[messageCount].sender, currentKingdom.c_str(), MAX_SENDER_NAME - 1);
    messages[messageCount].sender[MAX_SENDER_NAME - 1] = '\0';
    strncpy_s(messages[messageCount].receiver, receiver.c_str(), MAX_RECEIVER_NAME - 1);
    messages[messageCount].receiver[MAX_RECEIVER_NAME - 1] = '\0';
    strncpy_s(messages[messageCount].content, content.c_str(), MAX_MESSAGE_LENGTH - 1);
    messages[messageCount].content[MAX_MESSAGE_LENGTH - 1] = '\0';
    messages[messageCount].timestamp = time(0);

    messageCount++;

    return true;
}

void MessageSystem::displayMessages() const {
    if (messageCount == 0) {
        cout << "No messages in your inbox." << endl;
        return;
    }

    cout << "\n=== MESSAGE INBOX ===" << endl;
    int unreadCount = 0;

    for (int i = 0; i < messageCount; i++) {
        // Check if this message is for the current kingdom
        if (strcmp(messages[i].receiver, currentKingdom.c_str()) == 0) {
            cout << "\n--- Message from " << messages[i].sender << " ---" << endl;
            displayTimestamp(messages[i].timestamp);

            cout << "Message: " << messages[i].content << endl;

            // Check if it's unread
            // In a real implementation, we would track read/unread status
            // For simplicity, we're assuming all messages are unread initially
            unreadCount++;
        }
    }

    if (unreadCount == 0) {
        cout << "\nNo new messages." << endl;
    }
}

void MessageSystem::saveToFile(const string& filename) const {
    ofstream outFile(filename, ios::app);  // Append mode

    if (!outFile.is_open()) {
        cout << "Error: Could not open message file for writing!" << endl;
        return;
    }

    for (int i = 0; i < messageCount; i++) {
        // Only save messages for this kingdom
        if (strcmp(messages[i].receiver, currentKingdom.c_str()) == 0 ||
            strcmp(messages[i].sender, currentKingdom.c_str()) == 0) {

            outFile << messages[i].sender << "|";
            outFile << messages[i].receiver << "|";
            outFile << messages[i].content << "|";
            outFile << messages[i].timestamp << endl;
        }
    }

    outFile.close();
}

void MessageSystem::loadFromFile(const string& filename) {
    ifstream inFile(filename);

    if (!inFile.is_open()) {
        return; // File doesn't exist yet
    }

    char line[300];  // Buffer for reading each line

    while (inFile.getline(line, 300) && messageCount < MAX_MESSAGES) {
        char* context = nullptr;
        // Parse the line into its components using strtok_s
        char* sender = strtok_s(line, "|", &context);
        char* receiver = strtok_s(nullptr, "|", &context);
        char* content = strtok_s(nullptr, "|", &context);
        char* timestampStr = strtok_s(nullptr, "|", &context);

        if (sender && receiver && content && timestampStr) {
            if (strcmp(receiver, currentKingdom.c_str()) == 0) {
                strncpy_s(messages[messageCount].sender, sender, MAX_SENDER_NAME - 1);
                messages[messageCount].sender[MAX_SENDER_NAME - 1] = '\0';
                strncpy_s(messages[messageCount].receiver, receiver, MAX_RECEIVER_NAME - 1);
                messages[messageCount].receiver[MAX_RECEIVER_NAME - 1] = '\0';
                strncpy_s(messages[messageCount].content, content, MAX_MESSAGE_LENGTH - 1);
                messages[messageCount].content[MAX_MESSAGE_LENGTH - 1] = '\0';
                messages[messageCount].timestamp = static_cast<time_t>(atol(timestampStr));

                messageCount++;
            }
        }
    }

    inFile.close();
}

void MessageSystem::displayTimestamp(time_t timestamp) const {
    char buffer[26];  // Buffer for date/time string

#ifdef _WIN32
    ctime_s(buffer, sizeof(buffer), &timestamp);
#else
    char* result = ctime(&timestamp);
    strncpy(buffer, result, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';  // Ensure null-termination
#endif

    cout << "Sent: " << buffer;
}

int MessageSystem::countUnreadMessages() const {
    int count = 0;

    for (int i = 0; i < messageCount; i++) {
        // In a more complete implementation, we'd track read/unread status
        // For now, assume all messages are unread
        if (strcmp(messages[i].receiver, currentKingdom.c_str()) == 0) {
            count++;
        }
    }

    return count;
}

bool MessageSystem::hasMessages() const {
    return messageCount > 0;
}