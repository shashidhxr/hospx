// src/pages/Doctors.jsx
import { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import { Plus, FileText } from 'lucide-react';
import PageHeader from '../components/PageHeader';
import DataTable from '../components/DataTable';
import Modal from '../components/Modal';

const DoctorForm = ({ doctor, onSubmit, onCancel }) => {
  const [formData, setFormData] = useState({
    name: '',
    specialization: '',
    contact: '',
    email: '',
    ...doctor
  });

  const [errors, setErrors] = useState({});

  useEffect(() => {
    if (doctor) {
      setFormData({ ...doctor });
    }
  }, [doctor]);

  const validateForm = () => {
    const newErrors = {};
    if (!formData.name) newErrors.name = 'Name is required';
    if (!formData.specialization) newErrors.specialization = 'Specialization is required';
    if (!formData.contact) newErrors.contact = 'Contact is required';
    if (!formData.email) newErrors.email = 'Email is required';
    else if (!/\S+@\S+\.\S+/.test(formData.email)) newErrors.email = 'Invalid email format';
    return newErrors;
  };

  const handleChange = (e) => {
    const { name, value } = e.target;
    setFormData(prev => ({ ...prev, [name]: value }));
    
    // Clear error when field is edited
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
      
      <div className="form-row">
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
      </div>
      
      <div className="form-group">
        <label htmlFor="availability">Availability</label>
        <select
          id="availability"
          name="availability"
          value={formData.availability || 'weekdays'}
          onChange={handleChange}
        >
          <option value="weekdays">Weekdays</option>
          <option value="weekends">Weekends</option>
          <option value="all">All Days</option>
        </select>
      </div>
      
      <div className="form-actions">
        <button type="button" className="btn secondary" onClick={onCancel}>
          Cancel
        </button>
        <button type="submit" className="btn primary">
          {doctor ? 'Update Doctor' : 'Add Doctor'}
        </button>
      </div>
    </form>
  );
};

const Doctors = () => {
  const navigate = useNavigate();
  const [doctors, setDoctors] = useState([]);
  const [isLoading, setIsLoading] = useState(true);
  // eslint-disable-next-line no-unused-vars
  const [searchQuery, setSearchQuery] = useState('');
  const [showModal, setShowModal] = useState(false);
  const [currentDoctor, setCurrentDoctor] = useState(null);

  // Mock data - replace with API calls
  useEffect(() => {
    // Simulate API call
    setTimeout(() => {
      setDoctors([
        { id: 1, name: 'Dr. Sarah Smith', specialization: 'Cardiology', contact: '555-111-2222', email: 'smith@hospital.com', availability: 'weekdays' },
        { id: 2, name: 'Dr. James Johnson', specialization: 'Neurology', contact: '555-222-3333', email: 'johnson@hospital.com', availability: 'all' },
        { id: 3, name: 'Dr. Emily Williams', specialization: 'Pediatrics', contact: '555-333-4444', email: 'williams@hospital.com', availability: 'weekends' },
        { id: 4, name: 'Dr. Michael Brown', specialization: 'Orthopedics', contact: '555-444-5555', email: 'brown@hospital.com', availability: 'weekdays' },
        { id: 5, name: 'Dr. Lisa Davis', specialization: 'Dermatology', contact: '555-555-6666', email: 'davis@hospital.com', availability: 'all' },
      ]);
      setIsLoading(false);
    }, 500);
  }, []);
  
  const columns = [
    { field: 'name', header: 'Name', sortable: true },
    { field: 'specialization', header: 'Specialization', sortable: true },
    { field: 'contact', header: 'Contact', sortable: true },
    { field: 'email', header: 'Email', sortable: true },
    { 
      field: 'availability', 
      header: 'Availability', 
      sortable: true,
      render: (row) => {
        switch(row.availability) {
          case 'weekdays': return 'Mon-Fri';
          case 'weekends': return 'Sat-Sun';
          case 'all': return 'All Days';
          default: return row.availability;
        }
      }
    },
  ];

  const handleSearch = (query) => {
    setSearchQuery(query);
  };

  const handleView = (id) => {
    navigate(`/doctors/${id}`);
  };

  const handleEdit = (id) => {
    const doctor = doctors.find(d => d.id === id);
    setCurrentDoctor(doctor);
    setShowModal(true);
  };

  const handleDelete = (id) => {
    if (window.confirm('Are you sure you want to delete this doctor?')) {
      // In a real app, you'd call an API here
      setDoctors(doctors.filter(doctor => doctor.id !== id));
    }
  };

  const handleAddNew = () => {
    setCurrentDoctor(null);
    setShowModal(true);
  };

  const handleFormSubmit = (data) => {
    if (currentDoctor) {
      // Update existing doctor
      setDoctors(doctors.map(d => d.id === currentDoctor.id ? { ...data, id: currentDoctor.id } : d));
    } else {
      // Add new doctor
      const newId = Math.max(...doctors.map(d => d.id), 0) + 1;
      setDoctors([...doctors, { ...data, id: newId }]);
    }
    setShowModal(false);
  };

  const handleExport = () => {
    // In a real app, this would generate a CSV or PDF
    alert('Exporting doctor data...');
  };

  return (
    <div className="doctors-page">
      <PageHeader 
        title="Doctors" 
        onSearch={handleSearch} 
      />
      
      <div className="content-container">
        <div className="page-actions">
          <button className="btn primary" onClick={handleAddNew}>
            <Plus size={18} />
            <span>Add New Doctor</span>
          </button>
          
          <button className="btn secondary" onClick={handleExport}>
            <FileText size={18} />
            <span>Export List</span>
          </button>
        </div>
        
        {isLoading ? (
          <div className="loading">Loading doctors...</div>
        ) : (
          <DataTable 
            columns={columns} 
            data={doctors} 
            onView={handleView}
            onEdit={handleEdit}
            onDelete={handleDelete}
            onExport={handleExport}
          />
        )}
      </div>
      
      <Modal
        isOpen={showModal}
        onClose={() => setShowModal(false)}
        title={currentDoctor ? 'Edit Doctor' : 'Add New Doctor'}
      >
        <DoctorForm 
          doctor={currentDoctor} 
          onSubmit={handleFormSubmit} 
          onCancel={() => setShowModal(false)} 
        />
      </Modal>
    </div>
  );
};

export default Doctors;