#include "admin.h"
#include "database_handler.h"
#include "user.h"
#include "doctor.h"
#include "patient.h"
#include "receptionist.h"
#include "report.h"
#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <memory>

Admin::Admin(int userID, const std::string& name, const std::string& contact,
             int adminID)
    : User(userID, name, contact, "admin"), adminID(adminID) {}

bool Admin::saveToDatabase() {
    // First save the User part
    if (!User::saveToDatabase()) {
        return false;
    }

    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = adminID == 0 ?
        "INSERT INTO Admins (adminID, userID) VALUES (?, ?);" :
        "UPDATE Admins SET userID = ? WHERE adminID = ?;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    if (adminID == 0) {
        adminID = userID; // Use userID as adminID for consistency
        sqlite3_bind_int(stmt, 1, adminID);
        sqlite3_bind_int(stmt, 2, userID);
    } else {
        sqlite3_bind_int(stmt, 1, userID);
        sqlite3_bind_int(stmt, 2, adminID);
    }

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

Admin* Admin::getAdminFromDatabase(int adminID) {
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "SELECT a.userID, u.name, u.contact "
                     "FROM Admins a JOIN Users u ON a.userID = u.userID "
                     "WHERE a.adminID = ?;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }

    sqlite3_bind_int(stmt, 1, adminID);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int userID = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        sqlite3_finalize(stmt);
        return new Admin(userID, name, contact, adminID);
    }

    sqlite3_finalize(stmt);
    return nullptr;
}

void Admin::manageUser(int userID, const std::string& action, const std::string& newValue) {
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    std::string sql;

    if (action == "delete") {
        sql = "DELETE FROM Users WHERE userID = ?;";
    } else if (action == "update_name") {
        sql = "UPDATE Users SET name = ? WHERE userID = ?;";
    } else if (action == "update_contact") {
        sql = "UPDATE Users SET contact = ? WHERE userID = ?;";
    } else {
        throw std::invalid_argument("Invalid management action");
    }

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare management statement");
    }

    if (action == "delete") {
        sqlite3_bind_int(stmt, 1, userID);
    } else {
        sqlite3_bind_text(stmt, 1, newValue.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, userID);
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute user management");
    }

    sqlite3_finalize(stmt);
}

std::vector<Report> Admin::generateReports(const std::string& reportType, const std::string& startDate, const std::string& endDate) {
    std::vector<Report> reports;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    std::string sql;

    if (reportType == "appointments") {
        sql = "SELECT a.appointmentID, a.date, a.time, p.name AS patient_name, d.name AS doctor_name "
              "FROM Appointments a "
              "JOIN Patients pt ON a.patientID = pt.patientID "
              "JOIN Users p ON pt.userID = p.userID "
              "JOIN Doctors dr ON a.doctorID = dr.doctorID "
              "JOIN Users d ON dr.userID = d.userID "
              "WHERE a.date BETWEEN ? AND ? "
              "ORDER BY a.date, a.time;";
    } else if (reportType == "patients") {
        sql = "SELECT p.patientID, u.name, u.contact, p.age, p.gender, COUNT(a.appointmentID) as appointment_count "
              "FROM Patients p "
              "JOIN Users u ON p.userID = u.userID "
              "LEFT JOIN Appointments a ON p.patientID = a.patientID "
              "GROUP BY p.patientID;";
    } else {
        throw std::invalid_argument("Invalid report type");
    }

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare report statement");
    }

    if (reportType == "appointments") {
        sqlite3_bind_text(stmt, 1, startDate.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, endDate.c_str(), -1, SQLITE_TRANSIENT);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int reportID = sqlite3_column_int(stmt, 0);
        std::string details;
    
        if (reportType == "appointments") {
            details = "Appointment on " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))) +
                      " at " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))) +
                      " with Patient: " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))) +
                      " and Doctor: " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        } else if (reportType == "patients") {
            details = "Patient: " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))) +
                      " | Contact: " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))) +
                      " | Age: " + std::to_string(sqlite3_column_int(stmt, 3)) +
                      " | Gender: " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))) +
                      " | Appointments: " + std::to_string(sqlite3_column_int(stmt, 5));
        }
    
        // Provide a dummy doctorID if you don't have it
        Report report(reportID, /*dummy*/ 0, details);
        reports.push_back(report);
    }

    sqlite3_finalize(stmt);
    return reports;
}

int Admin::getAdminID() const { return adminID; }