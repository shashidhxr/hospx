#include "report.h"
#include "database_handler.h"
#include "doctor.h"
#include <sqlite3.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>

Report::Report(int reportID, int doctorID, const std::string& details)
    : reportID(reportID), doctorID(doctorID), details(details) {}

bool Report::saveToDatabase() {
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    // Get current date/time
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::string currentDateTime = oss.str();

    std::string sql = reportID == 0 ?
        "INSERT INTO Reports (doctorID, details, createdDate) VALUES (?, ?, ?);" :
        "UPDATE Reports SET details = ? WHERE reportID = ?;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare report statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }

    if (reportID == 0) {
        sqlite3_bind_int(stmt, 1, doctorID);
        sqlite3_bind_text(stmt, 2, details.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, currentDateTime.c_str(), -1, SQLITE_TRANSIENT);
    } else {
        sqlite3_bind_text(stmt, 1, details.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, reportID);
    }

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    if (success && reportID == 0) {
        reportID = sqlite3_last_insert_rowid(db);
    }

    sqlite3_finalize(stmt);
    return success;
}

bool Report::deleteFromDatabase() {
    if (reportID == 0) return false;

    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "DELETE FROM Reports WHERE reportID = ?;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare delete statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, reportID);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return success;
}

Report* Report::getReportFromDatabase(int reportID) {
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "SELECT reportID, doctorID, details, createdDate "
                     "FROM Reports WHERE reportID = ?;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare select statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, reportID);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int docID = sqlite3_column_int(stmt, 1);
        std::string details = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string createdDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        sqlite3_finalize(stmt);
        return new Report(id, docID, details);
    }

    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Report*> Report::getReportsByDoctor(int doctorID) {
    std::vector<Report*> reports;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "SELECT reportID, details, createdDate "
                     "FROM Reports WHERE doctorID = ? "
                     "ORDER BY createdDate DESC;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare doctor reports statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, doctorID);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string details = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string createdDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        reports.push_back(new Report(id, doctorID, details));
    }

    sqlite3_finalize(stmt);
    return reports;
}

std::vector<Report*> Report::getAllReports() {
    std::vector<Report*> reports;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "SELECT reportID, doctorID, details, createdDate "
                     "FROM Reports ORDER BY createdDate DESC;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare all reports statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int docID = sqlite3_column_int(stmt, 1);
        std::string details = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string createdDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        reports.push_back(new Report(id, docID, details));
    }

    sqlite3_finalize(stmt);
    return reports;
}

// Getters
int Report::getReportID() const { return reportID; }
int Report::getDoctorID() const { return doctorID; }
std::string Report::getDetails() const { return details; }

// Setters
void Report::setDetails(const std::string& details) { this->details = details; }