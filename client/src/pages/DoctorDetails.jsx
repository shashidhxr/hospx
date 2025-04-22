import { useState, useEffect } from 'react';
import { useParams } from 'react-router-dom';
import { Calendar, User, Phone, AtSign, Award, Clock } from 'lucide-react';
import PageHeader from '../components/PageHeader';
import Modal from '../components/Modal';

const DoctorDetails = () => {
  const { id } = useParams();
  const [doctor, setDoctor] = useState(null);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);
  const [isEditModalOpen, setIsEditModalOpen] = useState(false);
  const [appointments, setAppointments] = useState([]);

  useEffect(() => {
    // Fetch doctor details
    const fetchDoctorDetails = async () => {
      try {
        // Replace with your API call
        const response = await fetch(`/api/doctors/${id}`);
        if (!response.ok) throw new Error('Failed to fetch doctor details');
        const data = await response.json();
        setDoctor(data);
        
        // Fetch doctor's appointments
        const appointmentsResponse = await fetch(`/api/doctors/${id}/appointments`);
        if (appointmentsResponse.ok) {
          const appointmentsData = await appointmentsResponse.json();
          setAppointments(appointmentsData);
        }
        
        setIsLoading(false);
      } catch (err) {
        setError(err.message);
        setIsLoading(false);
      }
    };

    fetchDoctorDetails();
  }, [id]);

  const handleEditDoctor = (updatedData) => {
    // Update doctor API call
    fetch(`/api/doctors/${id}`, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(updatedData)
    })
      .then(response => {
        if (!response.ok) throw new Error('Failed to update doctor');
        return response.json();
      })
      .then(data => {
        setDoctor(data);
        setIsEditModalOpen(false);
      })
      .catch(err => setError(err.message));
  };

  if (isLoading) return <div className="loading">Loading doctor details...</div>;
  if (error) return <div className="error-message">{error}</div>;
  if (!doctor) return <div className="not-found">Doctor not found</div>;

  return (
    <div className="doctor-details-page">
      <PageHeader title="Doctor Details" />
      
      <div className="content-container">
        <div className="profile-header">
          <div className="profile-image">
            <User size={48} />
          </div>
          <div className="profile-info">
            <h2>{doctor.name}</h2>
            <p className="specialization">{doctor.specialization}</p>
            <div className="profile-actions">
              <button 
                className="btn primary" 
                onClick={() => setIsEditModalOpen(true)}
              >
                Edit Profile
              </button>
            </div>
          </div>
        </div>
        
        <div className="details-grid">
          <div className="detail-card">
            <h3>Personal Information</h3>
            <div className="detail-item">
              <User size={16} />
              <div>
                <p className="label">Full Name</p>
                <p className="value">{doctor.name}</p>
              </div>
            </div>
            <div className="detail-item">
              <Phone size={16} />
              <div>
                <p className="label">Contact</p>
                <p className="value">{doctor.contact}</p>
              </div>
            </div>
            <div className="detail-item">
              <AtSign size={16} />
              <div>
                <p className="label">Email</p>
                <p className="value">{doctor.email}</p>
              </div>
            </div>
            <div className="detail-item">
              <Award size={16} />
              <div>
                <p className="label">Specialization</p>
                <p className="value">{doctor.specialization}</p>
              </div>
            </div>
          </div>
          
          <div className="detail-card">
            <h3>Schedule Information</h3>
            <div className="schedule-details">
              <div className="schedule-day">
                <p className="day">Monday - Friday</p>
                <p className="time">{doctor.workingHours?.weekdays || '9:00 AM - 5:00 PM'}</p>
              </div>
              <div className="schedule-day">
                <p className="day">Saturday</p>
                <p className="time">{doctor.workingHours?.saturday || '9:00 AM - 1:00 PM'}</p>
              </div>
              <div className="schedule-day">
                <p className="day">Sunday</p>
                <p className="time">{doctor.workingHours?.sunday || 'Off'}</p>
              </div>
            </div>
          </div>
        </div>
        
        <div className="appointments-section">
          <div className="section-header">
            <h3>Upcoming Appointments</h3>
            <button className="btn secondary">View All</button>
          </div>
          
          <div className="appointments-list">
            {appointments.length > 0 ? (
              appointments.map(appointment => (
                <div key={appointment.id} className="appointment-card">
                  <div className="appointment-info">
                    <h4>{appointment.patientName}</h4>
                    <div className="appointment-details">
                      <div className="detail">
                        <Calendar size={14} />
                        <span>{new Date(appointment.date).toLocaleDateString()}</span>
                      </div>
                      <div className="detail">
                        <Clock size={14} />
                        <span>{appointment.time}</span>
                      </div>
                    </div>
                  </div>
                  <div className="appointment-status">
                    <span className={`status ${appointment.status.toLowerCase()}`}>
                      {appointment.status}
                    </span>
                  </div>
                </div>
              ))
            ) : (
              <p className="no-data">No upcoming appointments</p>
            )}
          </div>
        </div>
      </div>
      
      {isEditModalOpen && (
        <Modal
          isOpen={isEditModalOpen}
          onClose={() => setIsEditModalOpen(false)}
          title="Edit Doctor Information"
        >
          <DoctorForm 
            doctor={doctor} 
            onSubmit={handleEditDoctor} 
            onCancel={() => setIsEditModalOpen(false)} 
          />
        </Modal>
      )}
    </div>
  );
};

// Placeholder for DoctorForm component - similar to PatientForm
const DoctorForm = ({ doctor, onSubmit, onCancel }) => {
  const [formData, setFormData] = useState({
    name: '',
    specialization: '',
    contact: '',
    email: '',
    ...doctor
  });

  const [errors, setErrors] = useState({});

  const validateForm = () => {
    const newErrors = {};
    if (!formData.name) newErrors.name = 'Name is required';
    if (!formData.specialization) newErrors.specialization = 'Specialization is required';
    if (!formData.contact) newErrors.contact = 'Contact is required';
    if (!formData.email) newErrors.email = 'Email is required';
    return newErrors;
  };

  const handleChange = (e) => {
    const { name, value } = e.target;
    setFormData(prev => ({ ...prev, [name]: value }));
    
    if (errors[name]) {
      setErrors(prev => ({ ...prev, [name]: undefined }));
    }
  };

  const handleSubmit = (e) => {
    e.preventDefault();
    const validationErrors = validateForm();
    
    if (Object.keys(validationErrors).length === 0) {
      onSubmit(formData);
    } else {
      setErrors(validationErrors);
    }
  };

  return (
    <form onSubmit={handleSubmit} className="form">
      <div className="form-group">
        <label htmlFor="name">Full Name</label>
        <input
          type="text"
          id="name"
          name="name"
          value={formData.name}
          onChange={handleChange}
          className={errors.name ? 'error' : ''}
        />
        {errors.name && <span className="error-message">{errors.name}</span>}
      </div>
      
      <div className="form-group">
        <label htmlFor="specialization">Specialization</label>
        <input
          type="text"
          id="specialization"
          name="specialization"
          value={formData.specialization}
          onChange={handleChange}
          className={errors.specialization ? 'error' : ''}
        />
        {errors.specialization && <span className="error-message">{errors.specialization}</span>}
      </div>
      
      <div className="form-group">
        <label htmlFor="contact">Contact Number</label>
        <input
          type="text"
          id="contact"
          name="contact"
          value={formData.contact}
          onChange={handleChange}
          className={errors.contact ? 'error' : ''}
        />
        {errors.contact && <span className="error-message">{errors.contact}</span>}
      </div>
      
      <div className="form-group">
        <label htmlFor="email">Email</label>
        <input
          type="email"
          id="email"
          name="email"
          value={formData.email}
          onChange={handleChange}
          className={errors.email ? 'error' : ''}
        />
        {errors.email && <span className="error-message">{errors.email}</span>}
      </div>
      
      <div className="form-actions">
        <button type="button" className="btn secondary" onClick={onCancel}>
          Cancel
        </button>
        <button type="submit" className="btn primary">
          Update Doctor
        </button>
      </div>
    </form>
  );
};

export default DoctorDetails;