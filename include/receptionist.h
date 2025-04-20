#ifndef RECEPTIONIST_H
#define RECEPTIONIST_H

#include "user.h"
#include "patient.h"
#include "appointment.h"


class Receptionist : public User {
private:
    int receptionistID;

public:
    Receptionist(int userID, const std::string& name, const std::string& contact, 
                 int receptionistID);
    
    // inherited methods declaration
    bool saveToDatabase();
    Receptionist* getReceptionistFromDatabase(int receptionistID);
    Patient* registerPatient(const std::string& name, const std::string& contact, 
        int age, const std::string& gender);
    Appointment* scheduleAppointment(int patientID, int doctorID, const std::string& date, const std::string& time);
    std::vector<Appointment*> viewAllAppointments();
        
    // Receptionist specific methods
    void registerPatient();
    void scheduleAppointment();

    // Getters
    int getReceptionistID() const;
};

#endif // RECEPTIONIST_H