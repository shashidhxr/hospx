#ifndef REPORT_API_H
#define REPORT_API_H

#include "crow.h"
// #include "user.h"
// #include "doctor.h"
// #include "appointment.h"
// #include "patient.h"
// #include "record.h"
#include "report.h"

void registerReportRoutes(crow::SimpleApp& app){

// CROW_ROUTE(app, "/reports")
        // .methods("GET"_method)([](){
        //     auto reports = Report::getAllReportsFromDatabase();
        //     crow::json::wvalue result;
        //     for (size_t i = 0; i < reports.size(); i++) {
        //         result[i]["id"] = reports[i]->getReportID();
        //         result[i]["doctor_id"] = reports[i]->getDoctorID();
        //         result[i]["details"] = reports[i]->getDetails();
        //         delete reports[i];
        //     }
        //     return crow::response{result};
        // });

        CROW_ROUTE(app, "/reports")
        .methods("POST"_method)([](const crow::request& req){
            auto json = crow::json::load(req.body);
            if (!json) {
                return crow::response(400, "Invalid JSON");
            }

            try {
                int doctorID = json["doctor_id"].i();
                std::string details = json["details"].s();

                Report report(0, doctorID, details);
                if (report.saveToDatabase()) {
                    crow::json::wvalue result;
                    result["id"] = report.getReportID();
                    return crow::response{result};
                }
                return crow::response(500, "Failed to save report");
            } catch (const std::exception& e) {
                return crow::response(500, e.what());
            }
        });

        CROW_ROUTE(app, "/reports/<int>")
        .methods("GET"_method)([](int id){
            Report* report = Report::getReportFromDatabase(id);
            if (!report) {
                return crow::response(404, "Report not found");
            }
            
            crow::json::wvalue result;
            result["id"] = report->getReportID();
            result["doctor_id"] = report->getDoctorID();
            result["details"] = report->getDetails();
            
            delete report;
            return crow::response{result};
        });

    }
    #endif