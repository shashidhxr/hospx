#ifndef PATIENT_H
#define PATIENT_H

#include "user.h"
#include "appointment.h"
// #include "record.h"
#include "cors_config.h"

class MedicalRecord;  // forward declaration

#include <string>
#include <vector>

class Patient : public User {
private:
    int patientID;
    int age;
    std::string gender;

public:
    Patient(int userID, const std::string& name, const std::string& contact, 
            int patientID, int age, const std::string& gender);
    
    // override methods
    bool saveToDatabase();
    static Patient* getPatientFromDatabase(int patientID);
    void bookAppointment(int doctorID, const std::string& date, const std::string& time);
    std::vector<Appointment*> viewAppointments();
            
    // Patient specific methods
    static std::vector<Patient*> getAllPatientsFromDatabase();
    void bookAppointment();
    std::vector<MedicalRecord*> viewMedicalRecords();

    // Getters
    int getPatientID() const;
    int getAge() const;
    std::string getGender() const;

    // Setters
    void setAge(int age);
    void setGender(const std::string& gender);
};

#endif // PATIENT_H