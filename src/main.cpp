#include <iostream>
#include "database_handler.h"
#include "api_server.h"

int main() {
    try {
        // Initialize database
        DatabaseHandler& dbHandler = DatabaseHandler::getInstance("hospital.db");
        dbHandler.initializeDatabase();
        
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