#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <sqlite3.h>
#include <string>
#include <stdexcept>

class DatabaseHandler {
private:
    sqlite3* db;
    static DatabaseHandler* instance;
    DatabaseHandler(const std::string& dbName);

public:
    ~DatabaseHandler();
    static DatabaseHandler& getInstance(const std::string& dbName = "hospital.db");
    sqlite3* getDatabase() const;

    void execute(const std::string& sql);
    void initializeDatabase();
};

#endif // DATABASEHANDLER_H
