#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <vector>
#include <string>

class Doctor;
class Patient;

class Appointment {
private:
    int appointmentID;
    Patient* patient;
    Doctor* doctor;
    std::string date;
    std::string time;

public:
    Appointment(int appointmentID, Patient* patient, Doctor* doctor, 
                const std::string& date, const std::string& time);
    
    // inherited abstrac methods 
    bool saveToDatabase();
    bool deleteFromDatabase();
    static Appointment* getAppointmentFromDatabase(int appointmentID); 
    static std::vector<Appointment*> getAppointmentsForPatient(int patientID);
    static std::vector<Appointment*> getAppointmentsForDoctor(int doctorID);
    static std::vector<Appointment*> getAllAppointmentsFromDatabase();
                
    // Getters
    int getAppointmentID() const;
    Patient* getPatient() const;
    Doctor* getDoctor() const;
    std::string getDate() const;
    std::string getTime() const;

    // Setters
    void setDate(const std::string& date);
    void setTime(const std::string& time);
};

#endif // APPOINTMENT_H