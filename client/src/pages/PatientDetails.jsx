// src/pages/PatientDetails.jsx
import { useState, useEffect } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { ChevronLeft, Edit, Calendar, FileText, Clock } from 'lucide-react';
import PageHeader from '../components/PageHeader';
import Modal from '../components/Modal';
import PatientForm from '../components/PatientForm';
import DataTable from '../components/DataTable';

const PatientDetails = () => {
  const { id } = useParams();
  const navigate = useNavigate();
  const [patient, setPatient] = useState(null);
  const [appointments, setAppointments] = useState([]);
  const [medicalRecords, setMedicalRecords] = useState([]);
  const [isLoading, setIsLoading] = useState(true);
  const [activeTab, setActiveTab] = useState('overview');
  const [showEditModal, setShowEditModal] = useState(false);

  // Mock data - replace with API calls
  useEffect(() => {
    // Simulate API call
    setTimeout(() => {
      setPatient({
        id: parseInt(id),
        name: 'John Doe',
        age: 45,
        gender: 'Male',
        contact: '555-123-4567',
        address: '123 Main St, Springfield, IL',
        bloodType: 'O+',
        allergies: 'Penicillin',
        emergencyContact: 'Jane Doe (Wife) - 555-987-6543'
      });
      
      setAppointments([
        { id: 101, date: '2025-04-22', time: '10:00 AM', doctorId: 1, doctorName: 'Dr. Smith', status: 'Scheduled' },
        { id: 102, date: '2025-04-15', time: '2:30 PM', doctorId: 2, doctorName: 'Dr. Johnson', status: 'Completed' },
        { id: 103, date: '2025-03-30', time: '11:15 AM', doctorId: 1, doctorName: 'Dr. Smith', status: 'Completed' }
      ]);
      
      setMedicalRecords([
        { id: 201, date: '2025-04-15', doctorName: 'Dr. Johnson', diagnosis: 'Common Cold', prescription: 'Antihistamines, Rest' },
        { id: 202, date: '2025-03-30', doctorName: 'Dr. Smith', diagnosis: 'Annual Checkup', prescription: 'None' },
        { id: 203, date: '2025-02-12', doctorName: 'Dr. Williams', diagnosis: 'Sprained Ankle', prescription: 'Pain relievers, Compression bandage' }
      ]);
      
      setIsLoading(false);
    }, 500);
  }, [id]);

  const handleGoBack = () => {
    navigate('/patients');
  };

  const handleEditPatient = () => {
    setShowEditModal(true);
  };

  const handleUpdatePatient = (updatedData) => {
    setPatient({ ...patient, ...updatedData });
    setShowEditModal(false);
  };

  const handleBookAppointment = () => {
    navigate('/appointments', { state: { patientId: parseInt(id) } });
  };

  const appointmentColumns = [
    { field: 'date', header: 'Date', sortable: true },
    { field: 'time', header: 'Time', sortable: true },
    { field: 'doctorName', header: 'Doctor', sortable: true },
    { field: 'status', header: 'Status', 
      sortable: true,
      render: (row) => (
        <span className={`status-badge ${row.status.toLowerCase()}`}>
          {row.status}
        </span>
      )
    }
  ];

  const recordColumns = [
    { field: 'date', header: 'Date', sortable: true },
    { field: 'doctorName', header: 'Doctor', sortable: true },
    { field: 'diagnosis', header: 'Diagnosis', sortable: true },
    { field: 'prescription', header: 'Prescription', sortable: true }
  ];

  if (isLoading) {
    return <div className="loading">Loading patient details...</div>;
  }

  return (
    <div className="patient-details-page">
      <PageHeader title="Patient Details" />
      
      <div className="content-container">
        <div className="back-button" onClick={handleGoBack}>
          <ChevronLeft size={20} />
          <span>Back to Patients</span>
        </div>
        
        <div className="patient-header">
          <div className="patient-info">
            <h2>{patient.name}</h2>
            <div className="patient-meta">
              <span>ID: {patient.id}</span>
              <span>Age: {patient.age}</span>
              <span>Gender: {patient.gender}</span>
            </div>
          </div>
          
          <div className="patient-actions">
            <button className="btn secondary" onClick={handleEditPatient}>
              <Edit size={18} />
              <span>Edit Patient</span>
            </button>
            
            <button className="btn primary" onClick={handleBookAppointment}>
              <Calendar size={18} />
              <span>Book Appointment</span>
            </button>
          </div>
        </div>
        
        <div className="tabs">
          <button 
            className={`tab ${activeTab === 'overview' ? 'active' : ''}`}
            onClick={() => setActiveTab('overview')}
          >
            Overview
          </button>
          <button 
            className={`tab ${activeTab === 'appointments' ? 'active' : ''}`}
            onClick={() => setActiveTab('appointments')}
          >
            Appointments
          </button>
          <button 
            className={`tab ${activeTab === 'records' ? 'active' : ''}`}
            onClick={() => setActiveTab('records')}
          >
            Medical Records
          </button>
        </div>
        
        <div className="tab-content">
          {activeTab === 'overview' && (
            <div className="patient-details">
              <div className="detail-section">
                <h3>Personal Information</h3>
                <div className="detail-grid">
                  <div className="detail-item">
                    <span className="label">Full Name</span>
                    <span className="value">{patient.name}</span>
                  </div>
                  <div className="detail-item">
                    <span className="label">Age</span>
                    <span className="value">{patient.age}</span>
                  </div>
                  <div className="detail-item">
                    <span className="label">Gender</span>
                    <span className="value">{patient.gender}</span>
                  </div>
                  <div className="detail-item">
                    <span className="label">Contact</span>
                    <span className="value">{patient.contact}</span>
                  </div>
                  <div className="detail-item">
                    <span className="label">Address</span>
                    <span className="value">{patient.address}</span>
                  </div>
                  <div className="detail-item">
                    <span className="label">Blood Type</span>
                    <span className="value">{patient.bloodType}</span>
                  </div>
                  <div className="detail-item">
                    <span className="label">Allergies</span>
                    <span className="value">{patient.allergies || 'None'}</span>
                  </div>
                  <div className="detail-item">
                    <span className="label">Emergency Contact</span>
                    <span className="value">{patient.emergencyContact}</span>
                  </div>
                </div>
              </div>
              
              <div className="detail-section">
                <h3>Recent Activity</h3>
                <div className="activity-timeline">
                  {appointments.slice(0, 3).map(appointment => (
                    <div key={appointment.id} className="activity-item">
                      <div className="activity-icon">
                        <Clock size={16} />
                      </div>
                      <div className="activity-content">
                        <div className="activity-title">
                          Appointment with {appointment.doctorName}
                        </div>
                        <div className="activity-meta">
                          {appointment.date} at {appointment.time}
                        </div>
                        <div className={`activity-status ${appointment.status.toLowerCase()}`}>
                          {appointment.status}
                        </div>
                      </div>
                    </div>
                  ))}
                </div>
              </div>
            </div>
          )}
          
          {activeTab === 'appointments' && (
            <div className="appointments-list">
              <div className="section-header">
                <h3>Patient Appointments</h3>
                <button className="btn primary sm" onClick={handleBookAppointment}>
                  <Calendar size={16} />
                  <span>New Appointment</span>
                </button>
              </div>
              
              <DataTable 
                columns={appointmentColumns} 
                data={appointments}
                pagination={false}
              />
            </div>
          )}
          
          {activeTab === 'records' && (
            <div className="records-list">
              <div className="section-header">
                <h3>Medical Records</h3>
                <button className="btn secondary sm">
                  <FileText size={16} />
                  <span>Export Records</span>
                </button>
              </div>
              
              <DataTable 
                columns={recordColumns} 
                data={medicalRecords}
                pagination={false}
              />
            </div>
          )}
        </div>
      </div>
      
      <Modal
        isOpen={showEditModal}
        onClose={() => setShowEditModal(false)}
        title="Edit Patient Information"
      >
        <PatientForm 
          patient={patient} 
          onSubmit={handleUpdatePatient} 
          onCancel={() => setShowEditModal(false)} 
        />
      </Modal>
    </div>
  );
};

export default PatientDetails;