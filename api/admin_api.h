#ifndef ADMIN_API_H
#define ADMIN_API_H

#include "crow.h"
// #include "user.h"
// #include "doctor.h"
// #include "appointment.h"
// #include "patient.h"
// #include "record.h"
// #include "report.h"
#include "admin.h"

void registerAdminRoutes(crow::SimpleApp& app){


CROW_ROUTE(app, "/admin/generate-report")
        .methods("POST"_method)([](const crow::request& req){
            auto json = crow::json::load(req.body);
            if (!json) {
                return crow::response(400, "Invalid JSON");
            }

            try {
                std::string reportType = json["report_type"].s();
                std::string startDate = json["start_date"].s();
                std::string endDate = json["end_date"].s();

                Admin admin(0, "", "", 0); // Dummy admin for demonstration
                auto reports = admin.generateReports(reportType, startDate, endDate);
                
                crow::json::wvalue result;
                for (size_t i = 0; i < reports.size(); i++) {
                    result[i]["id"] = reports[i].getReportID();
                    result[i]["details"] = reports[i].getDetails();
                }
                return crow::response{result};
            } catch (const std::exception& e) {
                return crow::response(500, e.what());
            }
        });

    }
    #endif