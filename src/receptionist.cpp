#include "receptionist.h"
#include "database_handler.h"
#include "patient.h"
#include "doctor.h"
#include "appointment.h"
#include <sqlite3.h>
#include <iostream>
#include <stdexcept>
#include <memory>

Receptionist::Receptionist(int userID, const std::string& name, const std::string& contact,
                           int receptionistID)
    : User(userID, name, contact, "receptionist"), receptionistID(receptionistID) {}

bool Receptionist::saveToDatabase() {
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    
    // Begin transaction
    DatabaseHandler::getInstance().execute("BEGIN TRANSACTION;");

    try {
        // First save the User part
        std::string userSql = userID == 0 ?
            "INSERT INTO Users (name, contact, type) VALUES (?, ?, ?);" :
            "UPDATE Users SET name = ?, contact = ?, type = ? WHERE userID = ?;";
        
        sqlite3_stmt* userStmt;
        if (sqlite3_prepare_v2(db, userSql.c_str(), -1, &userStmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare user statement");
        }

        sqlite3_bind_text(userStmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(userStmt, 2, contact.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(userStmt, 3, type.c_str(), -1, SQLITE_TRANSIENT);

        if (userID != 0) {
            sqlite3_bind_int(userStmt, 4, userID);
        }

        if (sqlite3_step(userStmt) != SQLITE_DONE) {
            sqlite3_finalize(userStmt);
            throw std::runtime_error("Failed to save user data");
        }

        if (userID == 0) {
            userID = sqlite3_last_insert_rowid(db);
        }
        sqlite3_finalize(userStmt);

        // Now save Receptionist-specific data
        std::string receptionistSql = receptionistID == 0 ?
            "INSERT INTO Receptionists (receptionistID, userID) VALUES (?, ?);" :
            "UPDATE Receptionists SET userID = ? WHERE receptionistID = ?;";

        sqlite3_stmt* receptionistStmt;
        if (sqlite3_prepare_v2(db, receptionistSql.c_str(), -1, &receptionistStmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare receptionist statement");
        }

        if (receptionistID == 0) {
            receptionistID = userID; // Use userID as receptionistID
            sqlite3_bind_int(receptionistStmt, 1, receptionistID);
            sqlite3_bind_int(receptionistStmt, 2, userID);
        } else {
            sqlite3_bind_int(receptionistStmt, 1, userID);
            sqlite3_bind_int(receptionistStmt, 2, receptionistID);
        }

        if (sqlite3_step(receptionistStmt) != SQLITE_DONE) {
            sqlite3_finalize(receptionistStmt);
            throw std::runtime_error("Failed to save receptionist data");
        }
        sqlite3_finalize(receptionistStmt);

        // Commit transaction
        DatabaseHandler::getInstance().execute("COMMIT;");
        return true;
    } catch (const std::exception& e) {
        DatabaseHandler::getInstance().execute("ROLLBACK;");
        std::cerr << "Error saving receptionist: " << e.what() << std::endl;
        return false;
    }
}

Receptionist* Receptionist::getReceptionistFromDatabase(int receptionistID) {
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "SELECT r.userID, u.name, u.contact "
                     "FROM Receptionists r JOIN Users u ON r.userID = u.userID "
                     "WHERE r.receptionistID = ?;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare select statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, receptionistID);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int userID = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        sqlite3_finalize(stmt);
        return new Receptionist(userID, name, contact, receptionistID);
    }

    sqlite3_finalize(stmt);
    return nullptr;
}

Patient* Receptionist::registerPatient(const std::string& name, const std::string& contact, 
                                      int age, const std::string& gender) {
    // Create and save the patient
    Patient* patient = new Patient(0, name, contact, 0, age, gender);
    if (!patient->saveToDatabase()) {
        delete patient;
        throw std::runtime_error("Failed to register new patient");
    }
    return patient;
}

Appointment* Receptionist::scheduleAppointment(int patientID, int doctorID, 
                                             const std::string& date, const std::string& time) {
    Patient* patient = Patient::getPatientFromDatabase(patientID);
    Doctor* doctor = Doctor::getDoctorFromDatabase(doctorID);

    if (!patient || !doctor) {
        throw std::invalid_argument("Invalid patient or doctor ID");
    }

    Appointment* appointment = new Appointment(0, patient, doctor, date, time);
    if (!appointment->saveToDatabase()) {
        delete appointment;
        throw std::runtime_error("Failed to schedule appointment");
    }
    return appointment;
}

std::vector<Appointment*> Receptionist::viewAllAppointments() {
    std::vector<Appointment*> appointments;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "SELECT a.appointmentID, a.patientID, a.doctorID, a.date, a.time, a.status "
                     "FROM Appointments a "
                     "ORDER BY a.date, a.time;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare appointments statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int patientID = sqlite3_column_int(stmt, 1);
        int doctorID = sqlite3_column_int(stmt, 2);
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        std::string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        Patient* patient = Patient::getPatientFromDatabase(patientID);
        Doctor* doctor = Doctor::getDoctorFromDatabase(doctorID);
        if (patient && doctor) {
            appointments.push_back(new Appointment(id, patient, doctor, date, time));
        }
    }

    sqlite3_finalize(stmt);
    return appointments;
}

int Receptionist::getReceptionistID() const { return receptionistID; }