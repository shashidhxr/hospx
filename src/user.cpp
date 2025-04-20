#include "user.h"
#include "database_handler.h"
#include <sqlite3.h>
#include <iostream>
#include <vector>

User::User(int id, const std::string& name, const std::string& contact, const std::string& type)
    : userID(id), name(name), contact(contact), type(type) {}

bool User::saveToDatabase() {
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    
    std::string sql = userID == 0 ?
        "INSERT INTO Users (name, contact, type) VALUES (?, ?, ?);" :
        "UPDATE Users SET name = ?, contact = ?, type = ? WHERE userID = ?;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, contact.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, type.c_str(), -1, SQLITE_TRANSIENT);
    
    if (userID != 0) {
        sqlite3_bind_int(stmt, 4, userID);
    }
    
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    if (success && userID == 0) {
        userID = sqlite3_last_insert_rowid(db);
    }
    
    sqlite3_finalize(stmt);
    return success;
}

User* User::getUserFromDatabase(int userID) {
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    
    std::string sql = "SELECT userID, name, contact, type FROM Users WHERE userID = ?;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    
    sqlite3_bind_int(stmt, 1, userID);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        sqlite3_finalize(stmt);
        
        // Return a new User object (note: in practice you'd want to return derived classes)
        return new User(id, name, contact, type);
    }
    
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<User*> User::getAllUsersFromDatabase() {
    std::vector<User*> users;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    
    std::string sql = "SELECT userID, name, contact, type FROM Users;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return users;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        users.push_back(new User(id, name, contact, type));
    }
    
    sqlite3_finalize(stmt);
    return users;
}

// Getters
int User::getUserID() const { return userID; }
std::string User::getName() const { return name; }
std::string User::getContact() const { return contact; }
std::string User::getType() const { return type; }

// Setters
void User::setName(const std::string& name) { this->name = name; }
void User::setContact(const std::string& contact) { this->contact = contact; }