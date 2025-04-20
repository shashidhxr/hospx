#ifndef USER_H
#define USER_H

#include <string>
#include <vector>

class User {
protected:
    int userID;
    std::string name;
    std::string contact;
    std::string type;

public:
    User(int id, const std::string& name, const std::string& contact, const std::string& type);
    virtual ~User() = default;

    // Database operations
    virtual bool saveToDatabase();
    static User* getUserFromDatabase(int userID);
    static std::vector<User*> getAllUsersFromDatabase();

    // Getters and setters
    int getUserID() const;
    std::string getName() const;
    std::string getContact() const;
    std::string getType() const;

    void setName(const std::string& name);
    void setContact(const std::string& contact);
};

#endif // USER_H