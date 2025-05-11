#include "Stronghold.h"

Logger::Logger(const string& name) : logName(name) {
    logFile.open(name + ".log", ios::app);

    if (logFile.tellp() == 0) {
        logFile << "=== " << name << " Log ===" << endl;
        logFile << "Turn\tAction\tValue" << endl;
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::log(const string& action, int value) {
    if (logFile.is_open()) {
        logFile << action << "\t" << value << endl;
    }
}

void Logger::displayLog() {
    if (logFile.is_open()) {
        logFile.close();
    }

    // Open for reading
    ifstream readLog(logName + ".log");

    if (readLog.is_open()) {
        cout << "\n===== " << logName << " LOG =====\n";

        string line;
        while (getline(readLog, line)) {
            cout << line << endl;
        }

        readLog.close();
    }
    else {
        cout << "Could not open log file for reading." << endl;
    }

    // Reopen for appending
    logFile.open(logName + ".log", ios::app);
}