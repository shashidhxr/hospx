#ifndef REPORT_H
#define REPORT_H

#include <string>
#include <vector>

class Report {
private:
    int reportID;
    int doctorID;
    std::string details;

public:
    Report(int reportID, int doctorID, const std::string& details);

    // inherited methods declartion
    bool saveToDatabase();
    bool deleteFromDatabase();
    static Report* getReportFromDatabase(int reportID);
    std::vector<Report*> getReportsByDoctor(int doctorID);
    std::vector<Report*> getAllReports();
    
    // Getters
    int getReportID() const;
    int getDoctorID() const;
    std::string getDetails() const;

    // Setters
    void setDetails(const std::string& details);
};

#endif // REPORT_H