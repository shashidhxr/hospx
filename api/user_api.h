// include/routes/user_routes.h
#ifndef USER_API_H
#define USER_API_H

#include "crow.h"
#include "user.h"

void registerUserRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/users")
    .methods("GET"_method)([](){
        auto users = User::getAllUsersFromDatabase();
        crow::json::wvalue result;
        for (size_t i = 0; i < users.size(); i++) {
            result[i]["id"] = users[i]->getUserID();
            result[i]["name"] = users[i]->getName();
            result[i]["contact"] = users[i]->getContact();
            result[i]["type"] = users[i]->getType();
            delete users[i];
        }
        return crow::response{result};
    });

    CROW_ROUTE(app, "/users/<int>")
    .methods("GET"_method)([](int id){
        User* user = User::getUserFromDatabase(id);
        if (!user) {
            return crow::response(404, "User not found");
        }

        crow::json::wvalue result;
        result["id"] = user->getUserID();
        result["name"] = user->getName();
        result["contact"] = user->getContact();
        result["type"] = user->getType();

        delete user;
        return crow::response{result};
    });

    CROW_ROUTE(app, "/users")
    .methods("POST"_method)([](const crow::request& req){
        auto json = crow::json::load(req.body);
        if (!json) {
            return crow::response(400, "Invalid JSON");
        }

        try {
            std::string name = json["name"].s();
            std::string contact = json["contact"].s();
            std::string type = json["type"].s();

            if (type != "patient" && type != "doctor" && type != "receptionist" && type != "admin") {
                return crow::response(400, "Invalid user type");
            }

            User user(0, name, contact, type);
            if (!user.saveToDatabase()) {
                return crow::response(500, "Failed to save user");
            }

            crow::json::wvalue result;
            result["message"] = "User created successfully";
            result["name"] = name;
            result["contact"] = contact;
            result["type"] = type;

            return crow::response{201, result};
        } catch (const std::exception& e) {
            return crow::response(500, e.what());
        }
    });
}

#endif // USER_ROUTES_H
