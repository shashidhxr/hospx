// // include/routes/login.h
// #ifndef LOGIN_API_H
// #define LOGIN_API_H

// #include "crow.h"
// #include "user.h"

// void registerLoginRoutes(crow::SimpleApp& app) {
//     // Simple login endpoint
//     CROW_ROUTE(app, "/auth")
//     .methods("POST"_method)([](const crow::request& req) {
//         auto json = crow::json::load(req.body);
//         if (!json) {
//             return crow::response(400, "Invalid JSON");
//         }

//         try {
//             std::string contact = json["contact"].s();
//             std::string password = json["password"].s();

//             // Simple authentication (NOT secure - for testing only)
//             User* user = User::getUserByContactFromDatabase(contact);
//             if (!user || user->getPassword() != password) {  // Plain text comparison
//                 if (user) delete user;
//                 return crow::response(401, "Invalid credentials");
//             }

//             // Successful login
//             crow::json::wvalue response;
//             response["success"] = true;
//             response["redirect"] = "/dashboard";
//             response["user"] = {
//                 {"id", user->getUserID()},
//                 {"name", user->getName()},
//                 {"type", user->getType()}
//             };

//             delete user;
//             return crow::response{response};
//         } catch (const std::exception& e) {
//             return crow::response(500, e.what());
//         }
//     });
// }

// #endif // LOGIN_API_H