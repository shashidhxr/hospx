#ifndef DOCTOR_H
#define DOCTOR_H

#include "user.h"
#include "appointment.h"

#include <string>

class Doctor : public User {
private:
    int doctorID;
    std::string specialization;

public:
    Doctor(int userID, const std::string& name, const std::string& contact, 
           int doctorID, const std::string& specialization);

    // override abstrat class methods from user class
    bool saveToDatabase();
    User* getUserFromDatabase(int userID);
    std::vector<User*> getAllUsersFromDatabase();
    
    // inherited methods
    static Doctor* getDoctorFromDatabase(int doctorID);
    void prescribeMedicine(int patientID, const std::string& medicine, const std::string& dosage);
    void updatePatientRecords(int patientID, const std::string& diagnosis, const std::string& treatment);
    std::vector<Appointment*> viewAppointments();
    
    // Doctor specific methods
    void prescribeMedicine();
    void updatePatientRecords();
    void viewAppointment();

    // Getters
    int getDoctorID() const;
    std::string getSpecialization() const;

    // Setters
    void setSpecialization(const std::string& specialization);
};

#endif // DOCTOR_H