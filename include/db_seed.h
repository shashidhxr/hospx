#include <iostream>
#include "database_handler.h"
#include "api_server.h"

void initializeDatabaseSchema(DatabaseHandler& dbHandler) {
    try {
        dbHandler.execute("PRAGMA foreign_keys = ON;");

        // Create tables in dependency order
        dbHandler.execute(R"(
            CREATE TABLE IF NOT EXISTS Users (
                userID INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                contact TEXT NOT NULL,
                type TEXT NOT NULL CHECK(type IN ('patient', 'doctor', 'receptionist', 'admin'))
            );
        )");

        dbHandler.execute(R"(
            CREATE TABLE IF NOT EXISTS Patients (
                patientID INTEGER PRIMARY KEY AUTOINCREMENT,
                userID INTEGER NOT NULL UNIQUE,
                age INTEGER NOT NULL,
                gender TEXT NOT NULL,
                FOREIGN KEY (userID) REFERENCES Users(userID) ON DELETE CASCADE
            );
        )");

        dbHandler.execute(R"(
            CREATE TABLE IF NOT EXISTS Doctors (
                doctorID INTEGER PRIMARY KEY AUTOINCREMENT,
                userID INTEGER NOT NULL UNIQUE,
                specialization TEXT NOT NULL,
                FOREIGN KEY (userID) REFERENCES Users(userID) ON DELETE CASCADE
            );
        )");

        dbHandler.execute(R"(
            CREATE TABLE IF NOT EXISTS Receptionists (
                receptionistID INTEGER PRIMARY KEY AUTOINCREMENT,
                userID INTEGER NOT NULL UNIQUE,
                FOREIGN KEY (userID) REFERENCES Users(userID) ON DELETE CASCADE
            );
        )");

        dbHandler.execute(R"(
            CREATE TABLE IF NOT EXISTS Admins (
                adminID INTEGER PRIMARY KEY AUTOINCREMENT,
                userID INTEGER NOT NULL UNIQUE,
                FOREIGN KEY (userID) REFERENCES Users(userID) ON DELETE CASCADE
            );
        )");

        dbHandler.execute(R"(
            CREATE TABLE IF NOT EXISTS Appointments (
                appointmentID INTEGER PRIMARY KEY AUTOINCREMENT,
                patientID INTEGER NOT NULL,
                doctorID INTEGER NOT NULL,
                date TEXT NOT NULL,
                time TEXT NOT NULL,
                status TEXT DEFAULT 'scheduled' CHECK(status IN ('scheduled', 'completed', 'cancelled')),
                notes TEXT,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (patientID) REFERENCES Patients(patientID) ON DELETE CASCADE,
                FOREIGN KEY (doctorID) REFERENCES Doctors(doctorID) ON DELETE CASCADE
            );
        )");

        dbHandler.execute(R"(
            CREATE TABLE IF NOT EXISTS MedicalRecords (
                recordID INTEGER PRIMARY KEY AUTOINCREMENT,
                patientID INTEGER NOT NULL,
                doctorID INTEGER NOT NULL,
                diagnosis TEXT NOT NULL,
                treatment TEXT NOT NULL,
                date TEXT NOT NULL,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (patientID) REFERENCES Patients(patientID) ON DELETE CASCADE,
                FOREIGN KEY (doctorID) REFERENCES Doctors(doctorID) ON DELETE CASCADE
            );
        )");

        dbHandler.execute(R"(
            CREATE TABLE IF NOT EXISTS Prescriptions (
                prescriptionID INTEGER PRIMARY KEY AUTOINCREMENT,
                doctorID INTEGER NOT NULL,
                patientID INTEGER NOT NULL,
                medicine TEXT NOT NULL,
                dosage TEXT NOT NULL,
                date TEXT NOT NULL,
                FOREIGN KEY (doctorID) REFERENCES Doctors(doctorID),
                FOREIGN KEY (patientID) REFERENCES Patients(patientID)
            );
        )");

        dbHandler.execute(R"(
            CREATE TABLE IF NOT EXISTS Reports (
                reportID INTEGER PRIMARY KEY AUTOINCREMENT,
                doctorID INTEGER NOT NULL,
                details TEXT NOT NULL,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (doctorID) REFERENCES Doctors(doctorID) ON DELETE CASCADE
            );
        )");
    } catch (const std::exception& e) {
        throw std::runtime_error("Schema creation failed: " + std::string(e.what()));
    }
}

void seedDatabase(DatabaseHandler& dbHandler) {
    try {
        // Insert users first
        dbHandler.execute(R"(
            INSERT INTO Users (name, contact, type) VALUES 
            ('Admin User', 'admin@hospital.com', 'admin'),        -- userID 1
            ('Dr. Smith', 'dr.smith@hospital.com', 'doctor'),     -- userID 2
            ('Dr. Johnson', 'dr.johnson@hospital.com', 'doctor'), -- userID 3
            ('Receptionist 1', 'reception@hospital.com', 'receptionist'), -- userID 4
            ('John Doe', 'john@example.com', 'patient'),          -- userID 5
            ('Jane Smith', 'jane@example.com', 'patient'),        -- userID 6
            ('Michael Brown', 'michael@example.com', 'patient');  -- userID 7
        )");

        // Insert specialized users
        dbHandler.execute(R"(
            INSERT INTO Admins (userID) VALUES (1);
        )");

        dbHandler.execute(R"(
            INSERT INTO Doctors (userID, specialization) VALUES
            (2, 'Cardiology'),    -- doctorID 1
            (3, 'Neurology');     -- doctorID 2
        )");

        dbHandler.execute(R"(
            INSERT INTO Receptionists (userID) VALUES (4);
        )");

        dbHandler.execute(R"(
            INSERT INTO Patients (userID, age, gender) VALUES
            (5, 35, 'male'),      -- patientID 1
            (6, 28, 'female'),     -- patientID 2
            (7, 42, 'male');       -- patientID 3
        )");

        // Now insert appointments
        dbHandler.execute(R"(
            INSERT INTO Appointments (patientID, doctorID, date, time, status, notes) VALUES
            (1, 1, '2025-05-01', '09:00', 'scheduled', 'Routine checkup'),
            (2, 2, '2025-05-01', '10:30', 'scheduled', 'Follow-up appointment'),
            (1, 2, '2025-05-02', '14:00', 'completed', 'Migraine treatment'),
            (2, 1, '2025-05-03', '11:15', 'cancelled', 'Patient rescheduled'),
            (3, 1, '2025-05-04', '13:45', 'scheduled', 'Initial consultation');
        )");

        // Insert medical records
        dbHandler.execute(R"(
            INSERT INTO MedicalRecords (patientID, doctorID, diagnosis, treatment, date) VALUES
            (1, 1, 'Hypertension', 'Lifestyle changes and medication', '2025-04-15'),
            (2, 2, 'Migraine', 'Prescribed pain relief medication', '2025-04-10'),
            (3, 1, 'High cholesterol', 'Dietary changes and statins', '2025-04-20');
        )");

        // Insert prescriptions
        dbHandler.execute(R"(
            INSERT INTO Prescriptions (doctorID, patientID, medicine, dosage, date) VALUES
            (1, 1, 'Lisinopril', '10mg daily', '2025-04-15'),
            (2, 2, 'Sumatriptan', '50mg as needed', '2025-04-10'),
            (1, 3, 'Atorvastatin', '20mg daily', '2025-04-20');
        )");

        // Insert reports
        dbHandler.execute(R"(
            INSERT INTO Reports (doctorID, details) VALUES
            (1, 'Monthly cardiology department report'),
            (2, 'Neurology case study report'),
            (1, 'Patient statistics for Q2 2025');
        )");
    } catch (const std::exception& e) {
        throw std::runtime_error("Data seeding failed: " + std::string(e.what()));
    }
}