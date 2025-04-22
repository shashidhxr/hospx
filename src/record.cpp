#include "record.h"
#include "database_handler.h"
#include <sqlite3.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>

MedicalRecord::MedicalRecord(int recordID, Patient* patient, Doctor* doctor,
                            const std::string& diagnosis, const std::string& treatment)
    : recordID(recordID), patient(patient), doctor(doctor),
      diagnosis(diagnosis), treatment(treatment) 
{
    // Set current date if not provided
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    date = oss.str();
}

bool MedicalRecord::saveToDatabase() {
    if (!patient || !doctor) {
        throw std::invalid_argument("Patient and Doctor must be valid");
    }

    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = recordID == 0 ?
        "INSERT INTO MedicalRecords (patientID, doctorID, diagnosis, treatment, date) VALUES (?, ?, ?, ?, ?);" :
        "UPDATE MedicalRecords SET diagnosis = ?, treatment = ?, date = ? WHERE recordID = ?;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare medical record statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }

    if (recordID == 0) {
        sqlite3_bind_int(stmt, 1, patient->getPatientID());
        sqlite3_bind_int(stmt, 2, doctor->getDoctorID());
        sqlite3_bind_text(stmt, 3, diagnosis.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, treatment.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, date.c_str(), -1, SQLITE_TRANSIENT);
    } else {
        sqlite3_bind_text(stmt, 1, diagnosis.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, treatment.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, recordID);
    }

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    if (success && recordID == 0) {
        recordID = sqlite3_last_insert_rowid(db);
    }

    sqlite3_finalize(stmt);
    return success;
}

bool MedicalRecord::deleteFromDatabase() {
    if (recordID == 0) return false;

    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "DELETE FROM MedicalRecords WHERE recordID = ?;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare delete statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, recordID);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);
    return success;
}

MedicalRecord* MedicalRecord::getRecordFromDatabase(int recordID) {
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "SELECT recordID, patientID, doctorID, diagnosis, treatment, date "
                     "FROM MedicalRecords WHERE recordID = ?;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare select statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, recordID);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int patientID = sqlite3_column_int(stmt, 1);
        int doctorID = sqlite3_column_int(stmt, 2);
        std::string diagnosis = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string treatment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        Patient* patient = Patient::getPatientFromDatabase(patientID);
        Doctor* doctor = Doctor::getDoctorFromDatabase(doctorID);

        if (!patient || !doctor) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Failed to load patient or doctor data");
        }

        MedicalRecord* record = new MedicalRecord(id, patient, doctor, diagnosis, treatment);
        record->date = date;
        sqlite3_finalize(stmt);
        return record;
    }

    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<MedicalRecord*> MedicalRecord::getRecordsForPatient(int patientID) {
    std::vector<MedicalRecord*> records;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "SELECT recordID, doctorID, diagnosis, treatment, date "
                     "FROM MedicalRecords WHERE patientID = ? "
                     "ORDER BY date DESC;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare patient records statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, patientID);

    Patient* patient = Patient::getPatientFromDatabase(patientID);
    if (!patient) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Invalid patient ID");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int doctorID = sqlite3_column_int(stmt, 1);
        std::string diagnosis = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string treatment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        Doctor* doctor = Doctor::getDoctorFromDatabase(doctorID);
        if (doctor) {
            MedicalRecord* record = new MedicalRecord(id, patient, doctor, diagnosis, treatment);
            record->date = date;
            records.push_back(record);
        }
    }

    sqlite3_finalize(stmt);
    return records;
}

std::vector<MedicalRecord*> MedicalRecord::getRecordsByDoctor(int doctorID) {
    std::vector<MedicalRecord*> records;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "SELECT recordID, patientID, diagnosis, treatment, date "
                     "FROM MedicalRecords WHERE doctorID = ? "
                     "ORDER BY date DESC;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare doctor records statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, doctorID);

    Doctor* doctor = Doctor::getDoctorFromDatabase(doctorID);
    if (!doctor) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Invalid doctor ID");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int patientID = sqlite3_column_int(stmt, 1);
        std::string diagnosis = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string treatment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        Patient* patient = Patient::getPatientFromDatabase(patientID);
        if (patient) {
            MedicalRecord* record = new MedicalRecord(id, patient, doctor, diagnosis, treatment);
            record->date = date;
            records.push_back(record);
        }
    }

    sqlite3_finalize(stmt);
    return records;
}

std::vector<MedicalRecord*> MedicalRecord::getAllRecordsFromDatabase() {
    std::vector<MedicalRecord*> records;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;

    std::string sql = "SELECT recordID, patientID, doctorID, diagnosis, treatment, date "
                     "FROM MedicalRecords ORDER BY date DESC;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare select all records statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int patientID = sqlite3_column_int(stmt, 1);
        int doctorID = sqlite3_column_int(stmt, 2);
        std::string diagnosis = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string treatment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        Patient* patient = Patient::getPatientFromDatabase(patientID);
        Doctor* doctor = Doctor::getDoctorFromDatabase(doctorID);

        if (patient && doctor) {
            MedicalRecord* record = new MedicalRecord(id, patient, doctor, diagnosis, treatment);
            record->date = date;
            records.push_back(record);
        }
    }

    sqlite3_finalize(stmt);
    return records;
}

// Getters
int MedicalRecord::getRecordID() const { return recordID; }
Patient* MedicalRecord::getPatient() const { return patient; }
Doctor* MedicalRecord::getDoctor() const { return doctor; }
std::string MedicalRecord::getDiagnosis() const { return diagnosis; }
std::string MedicalRecord::getTreatment() const { return treatment; }
std::string MedicalRecord::getDate() const {
    return date;
}


// Setters
void MedicalRecord::setDiagnosis(const std::string& diagnosis) { this->diagnosis = diagnosis; }
void MedicalRecord::setTreatment(const std::string& treatment) { this->treatment = treatment; }

