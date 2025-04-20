#include "doctor.h"
#include "database_handler.h"
#include "patient.h"
#include "appointment.h"
#include "record.h"
#include <sqlite3.h>
#include <iostream>
#include <vector>

Doctor::Doctor(int userID, const std::string& name, const std::string& contact,
               int doctorID, const std::string& specialization)
    : User(userID, name, contact, "doctor"), doctorID(doctorID), specialization(specialization) {}

bool Doctor::saveToDatabase() {
    // First save the User part
    if (!User::saveToDatabase()) {
        return false;
    }

    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = doctorID == 0 ?
        "INSERT INTO Doctors (doctorID, userID, specialization) VALUES (?, ?, ?);" :
        "UPDATE Doctors SET specialization = ? WHERE doctorID = ?;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    if (doctorID == 0) {
        doctorID = userID; // Use userID as doctorID for consistency
        sqlite3_bind_int(stmt, 1, doctorID);
        sqlite3_bind_int(stmt, 2, userID);
        sqlite3_bind_text(stmt, 3, specialization.c_str(), -1, SQLITE_TRANSIENT);
    } else {
        sqlite3_bind_text(stmt, 1, specialization.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, doctorID);
    }

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

Doctor* Doctor::getDoctorFromDatabase(int doctorID) {
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "SELECT d.userID, d.specialization, u.name, u.contact "
                     "FROM Doctors d JOIN Users u ON d.userID = u.userID "
                     "WHERE d.doctorID = ?;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }

    sqlite3_bind_int(stmt, 1, doctorID);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int userID = sqlite3_column_int(stmt, 0);
        std::string specialization = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        sqlite3_finalize(stmt);
        return new Doctor(userID, name, contact, doctorID, specialization);
    }

    sqlite3_finalize(stmt);
    return nullptr;
}

void Doctor::prescribeMedicine(int patientID, const std::string& medicine, const std::string& dosage) {
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "INSERT INTO Prescriptions (doctorID, patientID, medicine, dosage, date) "
                     "VALUES (?, ?, ?, ?, date('now'));";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        throw std::runtime_error("Failed to prescribe medicine");
    }

    sqlite3_bind_int(stmt, 1, doctorID);
    sqlite3_bind_int(stmt, 2, patientID);
    sqlite3_bind_text(stmt, 3, medicine.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, dosage.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute prescription");
    }

    sqlite3_finalize(stmt);
}

void Doctor::updatePatientRecords(int patientID, const std::string& diagnosis, const std::string& treatment) {
    MedicalRecord* record = new MedicalRecord(0, 
                                           Patient::getPatientFromDatabase(patientID),
                                           this,
                                           diagnosis,
                                           treatment);
    if (!record->saveToDatabase()) {
        delete record;
        throw std::runtime_error("Failed to update patient records");
    }
    delete record;
}

std::vector<Appointment*> Doctor::viewAppointments() {
    std::vector<Appointment*> appointments;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "SELECT appointmentID, patientID, date, time, status "
                     "FROM Appointments WHERE doctorID = ? ORDER BY date, time;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return appointments;
    }

    sqlite3_bind_int(stmt, 1, doctorID);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int appointmentID = sqlite3_column_int(stmt, 0);
        int patientID = sqlite3_column_int(stmt, 1);
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        Patient* patient = Patient::getPatientFromDatabase(patientID);
        if (patient) {
            appointments.push_back(new Appointment(appointmentID, patient, this, date, time));
        }
    }

    sqlite3_finalize(stmt);
    return appointments;
}

// Getters
int Doctor::getDoctorID() const { return doctorID; }
std::string Doctor::getSpecialization() const { return specialization; }

// Setters
void Doctor::setSpecialization(const std::string& specialization) {
    this->specialization = specialization;
}