#ifndef API_SERVER_H
#define API_SERVER_H

#include "crow.h"
#include "database_handler.h"
#include "patient.h"
#include "doctor.h"
#include "receptionist.h"
#include "admin.h"
#include "appointment.h"
#include "record.h"
#include "report.h"

#include <memory>

#include "user_api.h"
#include "patient_api.h"
#include "doctor_api.h"
#include "appointment_api.h"
#include "record_api.h"
#include "report_api.h"
#include "admin_api.h"
// #include "login_api.h"

// inline void add_cors_headers(crow::response& res) {
//     res.add_header("Access-Control-Allow-Origin", "*");
//     res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
//     res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
// }

class ApiServer {
public:
    ApiServer() : app() {
        setupRoutes();
    }

    void run(int port = 8080) {
        app.port(port).multithreaded().run();
    }

private:
    crow::SimpleApp app;

    void setupRoutes() {
        // Health check endpoint
        CROW_ROUTE(app, "/")([](){
            return "Hospital Management System API is running";
        });

        // User endpoints
        registerUserRoutes(app);

        // Patient endpoints
        registerPatientRoutes(app);

        // Doctor endpoints
        registerDoctorRoutes(app);
       
        // Appointment endpoints
        registerAppointmentRoutes(app);

        // Medical Record endpoints
        registerRecordRoutes(app);

        // Report endpoints
        registerReportRoutes(app);
        
        // Admin endpoints
        registerAdminRoutes(app);

        // login endpoints
        // registerLoginRoutes(app);
    }
};

#endif // API_SERVER_H