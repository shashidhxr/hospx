#include "patient.h"
#include "database_handler.h"
#include "appointment.h"
#include "record.h"
#include "doctor.h"
#include <sqlite3.h>
#include <iostream>
#include <stdexcept>
#include <vector>

Patient::Patient(int userID, const std::string& name, const std::string& contact,
                 int patientID, int age, const std::string& gender)
    : User(userID, name, contact, "patient"), patientID(patientID), age(age), gender(gender) {}

bool Patient::saveToDatabase() {
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

        // Now save Patient-specific data
        std::string patientSql = patientID == 0 ?
            "INSERT INTO Patients (patientID, userID, age, gender) VALUES (?, ?, ?, ?);" :
            "UPDATE Patients SET age = ?, gender = ? WHERE patientID = ?;";

        sqlite3_stmt* patientStmt;
        if (sqlite3_prepare_v2(db, patientSql.c_str(), -1, &patientStmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare patient statement");
        }

        if (patientID == 0) {
            patientID = userID; // Use userID as patientID
            sqlite3_bind_int(patientStmt, 1, patientID);
            sqlite3_bind_int(patientStmt, 2, userID);
            sqlite3_bind_int(patientStmt, 3, age);
            sqlite3_bind_text(patientStmt, 4, gender.c_str(), -1, SQLITE_TRANSIENT);
        } else {
            sqlite3_bind_int(patientStmt, 1, age);
            sqlite3_bind_text(patientStmt, 2, gender.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(patientStmt, 3, patientID);
        }

        if (sqlite3_step(patientStmt) != SQLITE_DONE) {
            sqlite3_finalize(patientStmt);
            throw std::runtime_error("Failed to save patient data");
        }
        sqlite3_finalize(patientStmt);

        // Commit transaction
        DatabaseHandler::getInstance().execute("COMMIT;");
        return true;
    } catch (const std::exception& e) {
        DatabaseHandler::getInstance().execute("ROLLBACK;");
        std::cerr << "Error saving patient: " << e.what() << std::endl;
        return false;
    }
}

Patient* Patient::getPatientFromDatabase(int patientID) {
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    
    std::string sql = "SELECT p.userID, p.age, p.gender, u.name, u.contact "
                     "FROM Patients p JOIN Users u ON p.userID = u.userID "
                     "WHERE p.patientID = ?;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare select statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }
    
    sqlite3_bind_int(stmt, 1, patientID);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int userID = sqlite3_column_int(stmt, 0);
        int age = sqlite3_column_int(stmt, 1);
        std::string gender = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        
        sqlite3_finalize(stmt);
        return new Patient(userID, name, contact, patientID, age, gender);
    }
    
    sqlite3_finalize(stmt);
    return nullptr;
}

void Patient::bookAppointment(int doctorID, const std::string& date, const std::string& time) {
    Doctor* doctor = Doctor::getDoctorFromDatabase(doctorID);
    if (!doctor) {
        throw std::invalid_argument("Invalid doctor ID");
    }

    Appointment* appointment = new Appointment(0, this, doctor, date, time);
    if (!appointment->saveToDatabase()) {
        delete appointment;
        throw std::runtime_error("Failed to book appointment");
    }
    delete appointment;
}

std::vector<Appointment*> Patient::viewAppointments() {
    return Appointment::getAppointmentsForPatient(patientID);
}

std::vector<MedicalRecord*> Patient::viewMedicalRecords() {
    std::vector<MedicalRecord*> records;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    
    std::string sql = "SELECT recordID, doctorID, diagnosis, treatment, date "
                     "FROM MedicalRecords WHERE patientID = ? ORDER BY date DESC;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare medical records statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }
    
    sqlite3_bind_int(stmt, 1, patientID);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int recordID = sqlite3_column_int(stmt, 0);
        int doctorID = sqlite3_column_int(stmt, 1);
        std::string diagnosis = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string treatment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        
        Doctor* doctor = Doctor::getDoctorFromDatabase(doctorID);
        if (doctor) {
            records.push_back(new MedicalRecord(recordID, this, doctor, diagnosis, treatment));
        }
    }
    
    sqlite3_finalize(stmt);
    return records;
}

std::vector<Patient*> Patient::getAllPatientsFromDatabase() {
    std::vector<Patient*> patients;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    
    std::string sql = "SELECT p.patientID, p.userID, p.age, p.gender, u.name, u.contact "
                     "FROM Patients p JOIN Users u ON p.userID = u.userID "
                     "WHERE u.type = 'patient';";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare select all patients statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int patientID = sqlite3_column_int(stmt, 0);
        int userID = sqlite3_column_int(stmt, 1);
        int age = sqlite3_column_int(stmt, 2);
        std::string gender = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        std::string contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        
        patients.push_back(new Patient(userID, name, contact, patientID, age, gender));
    }
    
    sqlite3_finalize(stmt);
    return patients;
}

// Getters
int Patient::getPatientID() const { return patientID; }
int Patient::getAge() const { return age; }
std::string Patient::getGender() const { return gender; }

// Setters
void Patient::setAge(int age) { this->age = age; }
void Patient::setGender(const std::string& gender) { this->gender = gender; }