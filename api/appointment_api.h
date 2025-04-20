#ifndef APPOINTMENT_API_H
#define APPOINTMENT_API_H

#include "crow.h"
// #include "user.h"
#include "doctor.h"
// #include "appointment.h"
#include "patient.h"

void registerAppointmentRoutes(crow::SimpleApp& app){


// CROW_ROUTE(app, "/appointments")
        // .methods("GET"_method)([](){
        //     auto appointments = Appointment::getAllAppointmentsFromDatabase();
        //     crow::json::wvalue result;
        //     for (size_t i = 0; i < appointments.size(); i++) {
        //         result[i]["id"] = appointments[i]->getAppointmentID();
        //         result[i]["patient_id"] = appointments[i]->getPatient()->getPatientID();
        //         result[i]["patient_name"] = appointments[i]->getPatient()->getName();
        //         result[i]["doctor_id"] = appointments[i]->getDoctor()->getDoctorID();
        //         result[i]["doctor_name"] = appointments[i]->getDoctor()->getName();
        //         result[i]["date"] = appointments[i]->getDate();
        //         result[i]["time"] = appointments[i]->getTime();
        //         delete appointments[i];
        //     }
        //     return crow::response{result};
        // });

        CROW_ROUTE(app, "/appointments")
        .methods("POST"_method)([](const crow::request& req){
            auto json = crow::json::load(req.body);
            if (!json) {
                return crow::response(400, "Invalid JSON");
            }

            try {
                int patientID = json["patient_id"].i();
                int doctorID = json["doctor_id"].i();
                std::string date = json["date"].s();
                std::string time = json["time"].s();

                Patient* patient = Patient::getPatientFromDatabase(patientID);
                Doctor* doctor = Doctor::getDoctorFromDatabase(doctorID);

                if (!patient || !doctor) {
                    return crow::response(404, "Patient or Doctor not found");
                }

                Appointment appointment(0, patient, doctor, date, time);
                if (appointment.saveToDatabase()) {
                    crow::json::wvalue result;
                    result["id"] = appointment.getAppointmentID();
                    return crow::response{result};
                }
                return crow::response(500, "Failed to save appointment");
            } catch (const std::exception& e) {
                return crow::response(500, e.what());
            }
        });

        CROW_ROUTE(app, "/appointments/<int>")
        .methods("GET"_method)([](int id){
            Appointment* appointment = Appointment::getAppointmentFromDatabase(id);
            if (!appointment) {
                return crow::response(404, "Appointment not found");
            }
            
            crow::json::wvalue result;
            result["id"] = appointment->getAppointmentID();
            result["patient_id"] = appointment->getPatient()->getPatientID();
            result["patient_name"] = appointment->getPatient()->getName();
            result["doctor_id"] = appointment->getDoctor()->getDoctorID();
            result["doctor_name"] = appointment->getDoctor()->getName();
            result["date"] = appointment->getDate();
            result["time"] = appointment->getTime();
            
            delete appointment;
            return crow::response{result};
        });

        CROW_ROUTE(app, "/appointments/<int>")
        .methods("PUT"_method)([](const crow::request& req, int id){
            auto json = crow::json::load(req.body);
            if (!json) {
                return crow::response(400, "Invalid JSON");
            }

            try {
                Appointment* appointment = Appointment::getAppointmentFromDatabase(id);
                if (!appointment) {
                    return crow::response(404, "Appointment not found");
                }

                if (json.has("date")) {
                    appointment->setDate(json["date"].s());
                }
                if (json.has("time")) {
                    appointment->setTime(json["time"].s());
                }

                if (appointment->saveToDatabase()) {
                    delete appointment;
                    return crow::response(200, "Appointment updated successfully");
                }
                
                delete appointment;
                return crow::response(500, "Failed to update appointment");
            } catch (const std::exception& e) {
                return crow::response(500, e.what());
            }
        });

        CROW_ROUTE(app, "/appointments/<int>")
        .methods("DELETE"_method)([](int id){
            Appointment* appointment = Appointment::getAppointmentFromDatabase(id);
            if (!appointment) {
                return crow::response(404, "Appointment not found");
            }

            if (appointment->deleteFromDatabase()) {
                delete appointment;
                return crow::response(200, "Appointment deleted successfully");
            }
            
            delete appointment;
            return crow::response(500, "Failed to delete appointment");
        });

    }

    #endif