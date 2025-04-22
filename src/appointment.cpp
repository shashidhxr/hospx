#include "appointment.h"
#include "database_handler.h"
#include "patient.h"
#include "doctor.h"
#include <sqlite3.h>
#include <iostream>
#include <stdexcept>

Appointment::Appointment(int appointmentID, Patient* patient, Doctor* doctor,
                         const std::string& date, const std::string& time)
    : appointmentID(appointmentID), patient(patient), doctor(doctor),
      date(date), time(time) {}

bool Appointment::saveToDatabase() {
    if (!patient || !doctor) {
        throw std::invalid_argument("Patient and Doctor must be valid");
    }

    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    
    std::string sql = appointmentID == 0 ?
        "INSERT INTO Appointments (patientID, doctorID, date, time, status) VALUES (?, ?, ?, ?, 'scheduled');" :
        "UPDATE Appointments SET patientID = ?, doctorID = ?, date = ?, time = ? WHERE appointmentID = ?;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare appointment statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }
    
    if (appointmentID == 0) {
        sqlite3_bind_int(stmt, 1, patient->getPatientID());
        sqlite3_bind_int(stmt, 2, doctor->getDoctorID());
        sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, time.c_str(), -1, SQLITE_TRANSIENT);
    } else {
        sqlite3_bind_int(stmt, 1, patient->getPatientID());
        sqlite3_bind_int(stmt, 2, doctor->getDoctorID());
        sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, time.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 5, appointmentID);
    }
    
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    if (success && appointmentID == 0) {
        appointmentID = sqlite3_last_insert_rowid(db);
    }
    
    sqlite3_finalize(stmt);
    return success;
}

bool Appointment::deleteFromDatabase() {
    if (appointmentID == 0) return false;

    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    
    std::string sql = "DELETE FROM Appointments WHERE appointmentID = ?;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare delete statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }
    
    sqlite3_bind_int(stmt, 1, appointmentID);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    
    sqlite3_finalize(stmt);
    return success;
}

Appointment* Appointment::getAppointmentFromDatabase(int appointmentID) {
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    
    std::string sql = "SELECT a.appointmentID, a.patientID, a.doctorID, a.date, a.time, a.status "
                     "FROM Appointments a "
                     "WHERE a.appointmentID = ?;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare select statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }
    
    sqlite3_bind_int(stmt, 1, appointmentID);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int patientID = sqlite3_column_int(stmt, 1);
        int doctorID = sqlite3_column_int(stmt, 2);
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        std::string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        
        Patient* patient = Patient::getPatientFromDatabase(patientID);
        Doctor* doctor = Doctor::getDoctorFromDatabase(doctorID);
        
        if (!patient || !doctor) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Failed to load patient or doctor data");
        }
        
        Appointment* appointment = new Appointment(id, patient, doctor, date, time);
        sqlite3_finalize(stmt);
        return appointment;
    }
    
    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<Appointment*> Appointment::getAppointmentsForPatient(int patientID) {
    std::vector<Appointment*> appointments;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    
    std::string sql = "SELECT a.appointmentID, a.doctorID, a.date, a.time, a.status "
                     "FROM Appointments a "
                     "WHERE a.patientID = ? "
                     "ORDER BY a.date, a.time;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare select statement: " + 
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
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        Doctor* doctor = Doctor::getDoctorFromDatabase(doctorID);
        if (doctor) {
            appointments.push_back(new Appointment(id, patient, doctor, date, time));
        }
    }
    
    sqlite3_finalize(stmt);
    return appointments;
}

std::vector<Appointment*> Appointment::getAppointmentsForDoctor(int doctorID) {
    std::vector<Appointment*> appointments;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    
    std::string sql = "SELECT a.appointmentID, a.patientID, a.date, a.time, a.status "
                     "FROM Appointments a "
                     "WHERE a.doctorID = ? "
                     "ORDER BY a.date, a.time;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare select statement: " + 
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
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        
        Patient* patient = Patient::getPatientFromDatabase(patientID);
        if (patient) {
            appointments.push_back(new Appointment(id, patient, doctor, date, time));
        }
    }
    
    sqlite3_finalize(stmt);
    return appointments;
}

std::vector<Appointment*> Appointment::getAllAppointmentsFromDatabase() {
    std::vector<Appointment*> appointments;
    sqlite3* db = DatabaseHandler::getInstance().getDatabase();
    sqlite3_stmt* stmt;
    
    std::string sql = "SELECT a.appointmentID, a.patientID, a.doctorID, a.date, a.time, a.status "
                     "FROM Appointments a "
                     "ORDER BY a.date, a.time;";
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare select statement: " + 
                               std::string(sqlite3_errmsg(db)));
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int patientID = sqlite3_column_int(stmt, 1);
        int doctorID = sqlite3_column_int(stmt, 2);
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        
        Patient* patient = Patient::getPatientFromDatabase(patientID);
        Doctor* doctor = Doctor::getDoctorFromDatabase(doctorID);
        
        if (patient && doctor) {
            appointments.push_back(new Appointment(id, patient, doctor, date, time));
        } else {
            // Clean up if we couldn't get both patient and doctor
            if (patient) delete patient;
            if (doctor) delete doctor;
        }
    }
    
    sqlite3_finalize(stmt);
    return appointments;
}

// Getters
int Appointment::getAppointmentID() const { return appointmentID; }
Patient* Appointment::getPatient() const { return patient; }
Doctor* Appointment::getDoctor() const { return doctor; }
std::string Appointment::getDate() const { return date; }
std::string Appointment::getTime() const { return time; }

// Setters
void Appointment::setDate(const std::string& date) { this->date = date; }
void Appointment::setTime(const std::string& time) { this->time = time; }