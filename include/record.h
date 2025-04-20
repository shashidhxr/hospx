#ifndef RECORD_H
#define RECORD_H

#include <string>
#include "patient.h"
#include "doctor.h"

class MedicalRecord {
private:
    int recordID;
    Patient* patient;
    Doctor* doctor;
    std::string diagnosis;
    std::string treatment;
    std::string date;

public:
    MedicalRecord(int recordID, Patient* patient, Doctor* doctor, 
                  const std::string& diagnosis, const std::string& treatment);
    
    // inhreited methods declaration
    bool saveToDatabase();
    bool deleteFromDatabase();
    static MedicalRecord* getRecordFromDatabase(int recordID);
    static std::vector<MedicalRecord*> getRecordsForPatient(int patientID);
    std::vector<MedicalRecord*> getRecordsByDoctor(int doctorID);
                  
    // Getters
    int getRecordID() const;
    Patient* getPatient() const;
    Doctor* getDoctor() const;
    std::string getDiagnosis() const;
    std::string getTreatment() const;
    std::string getDate() const;

    // Setters
    void setDiagnosis(const std::string& diagnosis);
    void setTreatment(const std::string& treatment);
};

#endif // RECORD_H