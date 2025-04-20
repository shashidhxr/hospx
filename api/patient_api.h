#ifndef PATIENT_API
#define PATIENT_API

#include "crow.h"
#include "patient.h"
// #include "appointment.h"
#include "doctor.h"
#include "record.h"

void registerPatientRoutes(crow::SimpleApp& app){

 // Patient endpoints
        // CROW_ROUTE(app, "/patients")
        // .methods("GET"_method)([](){
        //     auto patients = Patient::getAllPatientsFromDatabase();
        //     crow::json::wvalue result;
        //     for (size_t i = 0; i < patients.size(); i++) {
        //         result[i]["id"] = patients[i]->getPatientID();
        //         result[i]["user_id"] = patients[i]->getUserID();
        //         result[i]["name"] = patients[i]->getName();
        //         result[i]["contact"] = patients[i]->getContact();
        //         result[i]["age"] = patients[i]->getAge();
        //         result[i]["gender"] = patients[i]->getGender();
        //         delete patients[i];
        //     }
        //     return crow::response{result};
        // });

        CROW_ROUTE(app, "/patients")
        .methods("POST"_method)([](const crow::request& req){
            auto json = crow::json::load(req.body);
            if (!json) {
                return crow::response(400, "Invalid JSON");
            }

            try {
                std::string name = json["name"].s();
                std::string contact = json["contact"].s();
                int age = json["age"].i();
                std::string gender = json["gender"].s();

                Patient patient(0, name, contact, 0, age, gender);
                if (patient.saveToDatabase()) {
                    crow::json::wvalue result;
                    result["id"] = patient.getPatientID();
                    return crow::response{result};
                }
                return crow::response(500, "Failed to save patient");
            } catch (const std::exception& e) {
                return crow::response(500, e.what());
            }
        });

        CROW_ROUTE(app, "/patients/<int>")
        .methods("GET"_method)([](int id){
            Patient* patient = Patient::getPatientFromDatabase(id);
            if (!patient) {
                return crow::response(404, "Patient not found");
            }
            
            crow::json::wvalue result;
            result["id"] = patient->getPatientID();
            result["user_id"] = patient->getUserID();
            result["name"] = patient->getName();
            result["contact"] = patient->getContact();
            result["age"] = patient->getAge();
            result["gender"] = patient->getGender();
            
            delete patient;
            return crow::response{result};
        });

        CROW_ROUTE(app, "/patients/<int>/appointments")
        .methods("GET"_method)([](int id){
            auto appointments = Appointment::getAppointmentsForPatient(id);
            crow::json::wvalue result;
            for (size_t i = 0; i < appointments.size(); i++) {
                result[i]["id"] = appointments[i]->getAppointmentID();
                result[i]["patient_id"] = appointments[i]->getPatient()->getPatientID();
                result[i]["doctor_id"] = appointments[i]->getDoctor()->getDoctorID();
                result[i]["doctor_name"] = appointments[i]->getDoctor()->getName();
                result[i]["date"] = appointments[i]->getDate();
                result[i]["time"] = appointments[i]->getTime();
                delete appointments[i];
            }
            return crow::response{result};
        });

        CROW_ROUTE(app, "/patients/<int>/records")
        .methods("GET"_method)([](int id){
            auto records = MedicalRecord::getRecordsForPatient(id);
            crow::json::wvalue result;
            for (size_t i = 0; i < records.size(); i++) {
                result[i]["id"] = records[i]->getRecordID();
                result[i]["doctor_id"] = records[i]->getDoctor()->getDoctorID();
                result[i]["doctor_name"] = records[i]->getDoctor()->getName();
                result[i]["diagnosis"] = records[i]->getDiagnosis();
                result[i]["treatment"] = records[i]->getTreatment();
                result[i]["date"] = records[i]->getDate();
                delete records[i];
            }
            return crow::response{result};
        });
    }

#endif