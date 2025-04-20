#include "database_handler.h"
#include <iostream>

DatabaseHandler* DatabaseHandler::instance = nullptr;

DatabaseHandler::DatabaseHandler(const std::string& dbName) {
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
    }
}

DatabaseHandler::~DatabaseHandler() {
    sqlite3_close(db);
}

DatabaseHandler& DatabaseHandler::getInstance(const std::string& dbName) {
    if (!instance) {
        instance = new DatabaseHandler(dbName);
    }
    return *instance;
}

sqlite3* DatabaseHandler::getDatabase() const {
    return db;
}

void DatabaseHandler::execute(const std::string& sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string error = "SQL error: " + std::string(errMsg);
        sqlite3_free(errMsg);
        throw std::runtime_error(error);
    }
}

void DatabaseHandler::initializeDatabase() {
    // Create tables if they don't exist
    execute("BEGIN TRANSACTION;");
    
    execute("CREATE TABLE IF NOT EXISTS Users ("
           "userID INTEGER PRIMARY KEY AUTOINCREMENT, "
           "name TEXT NOT NULL, "
           "contact TEXT NOT NULL, "
           "type TEXT NOT NULL CHECK(type IN ('patient', 'doctor', 'receptionist', 'admin')));");

    execute("CREATE TABLE IF NOT EXISTS Patients ("
           "patientID INTEGER PRIMARY KEY, "
           "userID INTEGER NOT NULL, "
           "age INTEGER NOT NULL, "
           "gender TEXT NOT NULL, "
           "FOREIGN KEY (userID) REFERENCES Users(userID) ON DELETE CASCADE);");

    // Similar CREATE TABLE statements for other tables...
    
    execute("COMMIT;");
}