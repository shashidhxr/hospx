#ifndef RECORD_API_H
#define RECORD_API_H

#include "crow.h"
// #include "user.h"
#include "doctor.h"
// #include "appointment.h"
#include "patient.h"
#include "record.h"

void registerRecordRoutes(crow::SimpleApp& app){


// CROW_ROUTE(app, "/records")
        // .methods("GET"_method)([](){
        //     auto records = MedicalRecord::getAllRecordsFromDatabase();
        //     crow::json::wvalue result;
        //     for (size_t i = 0; i < records.size(); i++) {
        //         result[i]["id"] = records[i]->getRecordID();
        //         result[i]["patient_id"] = records[i]->getPatient()->getPatientID();
        //         result[i]["patient_name"] = records[i]->getPatient()->getName();
        //         result[i]["doctor_id"] = records[i]->getDoctor()->getDoctorID();
        //         result[i]["doctor_name"] = records[i]->getDoctor()->getName();
        //         result[i]["diagnosis"] = records[i]->getDiagnosis();
        //         result[i]["treatment"] = records[i]->getTreatment();
        //         result[i]["date"] = records[i]->date;
        //         delete records[i];
        //     }
        //     return crow::response{result};
        // });

        CROW_ROUTE(app, "/records")
        .methods("POST"_method)([](const crow::request& req){
            auto json = crow::json::load(req.body);
            if (!json) {
                return crow::response(400, "Invalid JSON");
            }

            try {
                int patientID = json["patient_id"].i();
                int doctorID = json["doctor_id"].i();
                std::string diagnosis = json["diagnosis"].s();
                std::string treatment = json["treatment"].s();

                Patient* patient = Patient::getPatientFromDatabase(patientID);
                Doctor* doctor = Doctor::getDoctorFromDatabase(doctorID);

                if (!patient || !doctor) {
                    return crow::response(404, "Patient or Doctor not found");
                }

                MedicalRecord record(0, patient, doctor, diagnosis, treatment);
                if (record.saveToDatabase()) {
                    crow::json::wvalue result;
                    result["id"] = record.getRecordID();
                    return crow::response{result};
                }
                return crow::response(500, "Failed to save medical record");
            } catch (const std::exception& e) {
                return crow::response(500, e.what());
            }
        });

        CROW_ROUTE(app, "/records/<int>")
        .methods("GET"_method)([](int id){
            MedicalRecord* record = MedicalRecord::getRecordFromDatabase(id);
            if (!record) {
                return crow::response(404, "Medical record not found");
            }
            
            crow::json::wvalue result;
            result["id"] = record->getRecordID();
            result["patient_id"] = record->getPatient()->getPatientID();
            result["patient_name"] = record->getPatient()->getName();
            result["doctor_id"] = record->getDoctor()->getDoctorID();
            result["doctor_name"] = record->getDoctor()->getName();
            result["diagnosis"] = record->getDiagnosis();
            result["treatment"] = record->getTreatment();
            result["date"] = record->getDate();
            
            delete record;
            return crow::response{result};
        });

    }
    #endif