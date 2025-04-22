import { useState, useEffect } from 'react';
import { Plus, Calendar, Grid, Filter, User, Clock, MapPin } from 'lucide-react';
import PageHeader from '../components/PageHeader';
import DataTable from '../components/DataTable';
import Modal from '../components/Modal';

const Appointments = () => {
  const [appointments, setAppointments] = useState([]);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [currentAppointment, setCurrentAppointment] = useState(null);
  const [viewMode, setViewMode] = useState('list'); // 'list' or 'calendar'
  const [filterStatus, setFilterStatus] = useState('all');

  useEffect(() => {
    const fetchAppointments = async () => {
      try {
        // Replace with your API call
        const response = await fetch('/api/appointments');
        if (!response.ok) throw new Error('Failed to fetch appointments');
        const data = await response.json();
        setAppointments(data);
        setIsLoading(false);
      } catch (err) {
        setError(err.message);
        setIsLoading(false);
      }
    };

    fetchAppointments();
  }, []);

  const handleCreateAppointment = (appointmentData) => {
    fetch('/api/appointments', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(appointmentData)
    })
      .then(response => {
        if (!response.ok) throw new Error('Failed to create appointment');
        return response.json();
      })
      .then(data => {
        setAppointments([...appointments, data]);
        setIsModalOpen(false);
        setCurrentAppointment(null);
      })
      .catch(err => setError(err.message));
  };

  const handleUpdateAppointment = (appointmentData) => {
    fetch(`/api/appointments/${appointmentData.id}`, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(appointmentData)
    })
      .then(response => {
        if (!response.ok) throw new Error('Failed to update appointment');
        return response.json();
      })
      .then(data => {
        setAppointments(appointments.map(app => 
          app.id === data.id ? data : app
        ));
        setIsModalOpen(false);
        setCurrentAppointment(null);
      })
      .catch(err => setError(err.message));
  };

  const handleDeleteAppointment = (id) => {
    if (window.confirm('Are you sure you want to delete this appointment?')) {
      fetch(`/api/appointments/${id}`, { method: 'DELETE' })
        .then(response => {
          if (!response.ok) throw new Error('Failed to delete appointment');
          setAppointments(appointments.filter(app => app.id !== id));
        })
        .catch(err => setError(err.message));
    }
  };

  const handleSearch = (query) => {
    // Implement search functionality
    setIsLoading(true);
    fetch(`/api/appointments?search=${query}`)
      .then(response => {
        if (!response.ok) throw new Error('Search failed');
        return response.json();
      })
      .then(data => {
        setAppointments(data);
        setIsLoading(false);
      })
      .catch(err => {
        setError(err.message);
        setIsLoading(false);
      });
  };

  const filteredAppointments = filterStatus === 'all' 
    ? appointments 
    : appointments.filter(app => app.status.toLowerCase() === filterStatus);

  const columns = [
    { field: 'patientName', header: 'Patient Name', sortable: true },
    { field: 'doctorName', header: 'Doctor', sortable: true },
    { 
      field: 'date', 
      header: 'Date', 
      sortable: true,
      render: (row) => new Date(row.date).toLocaleDateString() 
    },
    { field: 'time', header: 'Time' },
    { 
      field: 'status', 
      header: 'Status', 
      render: (row) => (
        <span className={`status-badge ${row.status.toLowerCase()}`}>
          {row.status}
        </span>
      )
    },
  ];

  return (
    <div className="appointments-page">
      <PageHeader 
        title="Appointments" 
        onSearch={handleSearch} 
      />
      
      <div className="content-container">
        <div className="page-toolbar">
          <div className="view-toggle">
            <button 
              className={`toggle-btn ${viewMode === 'list' ? 'active' : ''}`}
              onClick={() => setViewMode('list')}
            >
              <Grid size={18} />
              <span>List</span>
            </button>
            <button 
              className={`toggle-btn ${viewMode === 'calendar' ? 'active' : ''}`}
              onClick={() => setViewMode('calendar')}
            >
              <Calendar size={18} />
              <span>Calendar</span>
            </button>
          </div>
          
          <div className="filter-section">
            <div className="filter-dropdown">
              <Filter size={16} />
              <select 
                value={filterStatus} 
                onChange={(e) => setFilterStatus(e.target.value)}
              >
                <option value="all">All Status</option>
                <option value="scheduled">Scheduled</option>
                <option value="completed">Completed</option>
                <option value="cancelled">Cancelled</option>
              </select>
            </div>
          </div>
          
          <button 
            className="btn primary" 
            onClick={() => {
              setCurrentAppointment(null);
              setIsModalOpen(true);
            }}
          >
            <Plus size={16} />
            <span>New Appointment</span>
          </button>
        </div>
        
        {isLoading ? (
          <div className="loading">Loading appointments...</div>
        ) : error ? (
          <div className="error-message">{error}</div>
        ) : (
          <>
            {viewMode === 'list' ? (
              <DataTable 
                columns={columns} 
                data={filteredAppointments}
                onView={(id) => {
                  const appointment = appointments.find(app => app.id === id);
                  setCurrentAppointment(appointment);
                  setIsModalOpen(true);
                }}
                onEdit={(id) => {
                  const appointment = appointments.find(app => app.id === id);
                  setCurrentAppointment(appointment);
                  setIsModalOpen(true);
                }}
                onDelete={handleDeleteAppointment}
                onExport={() => {/* Handle export */}}
              />
            ) : (
              <div className="calendar-view">
                {/* Calendar view implementation */}
                <div className="calendar-placeholder">
                  <Calendar size={48} />
                  <p>Calendar view would be implemented here</p>
                </div>
              </div>
            )}
          </>
        )}
      </div>
      
      <Modal
        isOpen={isModalOpen}
        onClose={() => {
          setIsModalOpen(false);
          setCurrentAppointment(null);
        }}
        title={currentAppointment ? "Edit Appointment" : "Create Appointment"}
      >
        <AppointmentForm 
          appointment={currentAppointment}
          onSubmit={currentAppointment ? handleUpdateAppointment : handleCreateAppointment}
          onCancel={() => {
            setIsModalOpen(false);
            setCurrentAppointment(null);
          }}
        />
      </Modal>
    </div>
  );
};

const AppointmentForm = ({ appointment, onSubmit, onCancel }) => {
  const [formData, setFormData] = useState({
    patientId: '',
    doctorId: '',
    date: '',
    time: '',
    status: 'scheduled',
    notes: '',
    ...appointment
  });
  
  const [errors, setErrors] = useState({});
  const [patients, setPatients] = useState([]);
  const [doctors, setDoctors] = useState([]);
  
  useEffect(() => {
    // Fetch patients and doctors for dropdown
    const fetchData = async () => {
      try {
        const [patientsRes, doctorsRes] = await Promise.all([
          fetch('/api/patients'),
          fetch('/api/doctors')
        ]);
        
        if (patientsRes.ok) {
          const patientsData = await patientsRes.json();
          setPatients(patientsData);
        }
        
        if (doctorsRes.ok) {
          const doctorsData = await doctorsRes.json();
          setDoctors(doctorsData);
        }
      } catch (err) {
        console.error('Error fetching form data:', err);
      }
    };
    
    fetchData();
  }, []);

  const validateForm = () => {
    const newErrors = {};
    if (!formData.patientId) newErrors.patientId = 'Patient is required';
    if (!formData.doctorId) newErrors.doctorId = 'Doctor is required';
    if (!formData.date) newErrors.date = 'Date is required';
    if (!formData.time) newErrors.time = 'Time is required';
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
        <label htmlFor="patientId">Patient</label>
        <select
          id="patientId"
          name="patientId"
          value={formData.patientId}
          onChange={handleChange}
          className={errors.patientId ? 'error' : ''}
        >
          <option value="">Select Patient</option>
          {patients.map(patient => (
            <option key={patient.id} value={patient.id}>
              {patient.name}
            </option>
          ))}
        </select>
        {errors.patientId && <span className="error-message">{errors.patientId}</span>}
      </div>
      
      <div className="form-group">
        <label htmlFor="doctorId">Doctor</label>
        <select
          id="doctorId"
          name="doctorId"
          value={formData.doctorId}
          onChange={handleChange}
          className={errors.doctorId ? 'error' : ''}
        >
          <option value="">Select Doctor</option>
          {doctors.map(doctor => (
            <option key={doctor.id} value={doctor.id}>
              {doctor.name} - {doctor.specialization}
            </option>
          ))}
        </select>
        {errors.doctorId && <span className="error-message">{errors.doctorId}</span>}
      </div>
      
      <div className="form-row">
        <div className="form-group">
          <label htmlFor="date">Date</label>
          <input
            type="date"
            id="date"
            name="date"
            value={formData.date}
            onChange={handleChange}
            className={errors.date ? 'error' : ''}
          />
          {errors.date && <span className="error-message">{errors.date}</span>}
        </div>
        
        <div className="form-group">
          <label htmlFor="time">Time</label>
          <input
            type="time"
            id="time"
            name="time"
            value={formData.time}
            onChange={handleChange}
            className={errors.time ? 'error' : ''}
          />
          {errors.time && <span className="error-message">{errors.time}</span>}
        </div>
      </div>
      
      {appointment && (
        <div className="form-group">
          <label htmlFor="status">Status</label>
          <select
            id="status"
            name="status"
            value={formData.status}
            onChange={handleChange}
          >
            <option value="scheduled">Scheduled</option>
            <option value="completed">Completed</option>
            <option value="cancelled">Cancelled</option>
          </select>
        </div>
      )}
      
      <div className="form-group">
        <label htmlFor="notes">Notes</label>
        <textarea
          id="notes"
          name="notes"
          value={formData.notes || ''}
          onChange={handleChange}
          rows="3"
        />
      </div>
      
      <div className="form-actions">
        <button type="button" className="btn secondary" onClick={onCancel}>
          Cancel
        </button>
        <button type="submit" className="btn primary">
          {appointment ? 'Update Appointment' : 'Create Appointment'}
        </button>
      </div>
    </form>
  );
};

export default Appointments;