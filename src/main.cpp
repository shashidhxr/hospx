#include <iostream>
#include "database_handler.h"
#include "api_server.h"
#include "db_seed.h"
#include <cstdlib> // for std::remove

int main() {
    try {
        // Delete existing database file to start fresh
        std::remove("hospital.db");
        
        // Initialize database
        DatabaseHandler& dbHandler = DatabaseHandler::getInstance("hospital.db");
        
        // Create schema and seed data in correct order
        initializeDatabaseSchema(dbHandler);
        seedDatabase(dbHandler);
        
        // Create API server
        ApiServer server;
        
        std::cout << "Starting hospital management system API server on port 8080..." << std::endl;
        server.run(8080);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}