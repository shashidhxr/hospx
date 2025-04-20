#ifndef ADMIN_H
#define ADMIN_H

#include "user.h"
#include "report.h"

class Admin : public User {
private:
    int adminID;

public:
    Admin(int userID, const std::string& name, const std::string& contact, 
          int adminID);
    
    // Admin specific methods
    void manageUser();
    void generateReports();

    // inheriterd abstract methods 
    bool saveToDatabase();

    // method from inherited class
    Admin* getAdminFromDatabase(int adminID); 
    void manageUser(int userID, const std::string& action, const std::string& newValue);
    std::vector<Report> generateReports(const std::string& reportType, const std::string& startDate, const std::string& endDate);

    // Getters
    int getAdminID() const;
};

#endif // ADMIN_H