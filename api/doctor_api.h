#ifndef DOCTOR_API_H
#define DOCTOR_API_H

#include "crow.h"
// #include "user.h"
#include "doctor.h"
// #include "appointment.h"
#include "patient.h"
// #include "cors_config.h"

void registerDoctorRoutes(crow::SimpleApp& app){

        CROW_ROUTE(app, "/doctors")
        .methods("GET"_method)([](){
            auto doctors = Doctor::getAllDoctorsFromDatabase();
            crow::json::wvalue result;
            for (size_t i = 0; i < doctors.size(); i++) {
                result[i]["id"] = doctors[i]->getDoctorID();
                result[i]["user_id"] = doctors[i]->getUserID();
                result[i]["name"] = doctors[i]->getName();
                result[i]["contact"] = doctors[i]->getContact();
                result[i]["specialization"] = doctors[i]->getSpecialization();
                delete doctors[i];
            }
            auto res = crow::response{result};
            add_cors_headers(res);
            return res;
        });

        CROW_ROUTE(app, "/doctors")
        .methods("POST"_method)([](const crow::request& req){
            auto json = crow::json::load(req.body);
            if (!json) {
                return crow::response(400, "Invalid JSON");
            }

            try {
                std::string name = json["name"].s();
                std::string contact = json["contact"].s();
                std::string specialization = json["specialization"].s();

                Doctor doctor(0, name, contact, 0, specialization);
                if (doctor.saveToDatabase()) {
                    crow::json::wvalue result;
                    result["id"] = doctor.getDoctorID();
                    return crow::response{result};
                }
                return crow::response(500, "Failed to save doctor");
            } catch (const std::exception& e) {
                auto res = crow::response(500, e.what());
                add_cors_headers(res);
                return res;
            }
        });

        CROW_ROUTE(app, "/doctors/<int>")
        .methods("GET"_method)([](int id){
            Doctor* doctor = Doctor::getDoctorFromDatabase(id);
            if (!doctor) {
                return crow::response(404, "Doctor not found");
            }
            
            crow::json::wvalue result;
            result["id"] = doctor->getDoctorID();
            result["user_id"] = doctor->getUserID();
            result["name"] = doctor->getName();
            result["contact"] = doctor->getContact();
            result["specialization"] = doctor->getSpecialization();
            
            delete doctor;

            auto res = crow::response{result};
            add_cors_headers(res);
            return res;    
        });

        CROW_ROUTE(app, "/doctors/<int>/appointments")
        .methods("GET"_method)([](int id){
            auto appointments = Appointment::getAppointmentsForDoctor(id);
            crow::json::wvalue result;
            for (size_t i = 0; i < appointments.size(); i++) {
                result[i]["id"] = appointments[i]->getAppointmentID();
                result[i]["patient_id"] = appointments[i]->getPatient()->getPatientID();
                result[i]["patient_name"] = appointments[i]->getPatient()->getName();
                result[i]["date"] = appointments[i]->getDate();
                result[i]["time"] = appointments[i]->getTime();
                delete appointments[i];
            }
            return crow::response{result};
        });

        CROW_ROUTE(app, "/doctors/<int>/prescribe")
        .methods("POST"_method)([](const crow::request& req, int id){
            auto json = crow::json::load(req.body);
            if (!json) {
                return crow::response(400, "Invalid JSON");
            }

            try {
                int patientID = json["patient_id"].i();
                std::string medicine = json["medicine"].s();
                std::string dosage = json["dosage"].s();

                Doctor* doctor = Doctor::getDoctorFromDatabase(id);
                if (!doctor) {
                    return crow::response(404, "Doctor not found");
                }

                doctor->prescribeMedicine(patientID, medicine, dosage);
                delete doctor;
                
                auto res = crow::response(200, "Prescription created successfully");
                add_cors_headers(res);
                return res;
            } catch (const std::exception& e) {
                auto res = crow::response(500, e.what());
                add_cors_headers(res);
                return res;
            }
        });

    }
#endif