import { useState, useEffect } from 'react';
import { Plus, FileText, Search, Filter, Download, User } from 'lucide-react';
import PageHeader from '../components/PageHeader';
import DataTable from '../components/DataTable';
import Modal from '../components/Modal';

const MedicalRecords = () => {
  const [records, setRecords] = useState([]);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [currentRecord, setCurrentRecord] = useState(null);
  const [viewMode, setViewMode] = useState('all'); // 'all', 'recent', 'archived'
  const [patients, setPatients] = useState([]);
  const [selectedPatient, setSelectedPatient] = useState('');

  useEffect(() => {
    const fetchRecords = async () => {
      try {
        setIsLoading(true);
        // Replace with your API call
        const response = await fetch(`/api/records${selectedPatient ? `?patientId=${selectedPatient}` : ''}`);
        if (!response.ok) throw new Error('Failed to fetch medical records');
        const data = await response.json();
        setRecords(data);
        setIsLoading(false);
      } catch (err) {
        setError(err.message);
        setIsLoading(false);
      }
    };

    const fetchPatients = async () => {
      try {
        const response = await fetch('/api/patients');
        if (response.ok) {
          const data = await response.json();
          setPatients(data);
        }
      } catch (err) {
        console.error('Failed to fetch patients:', err);
      }
    };

    fetchRecords();
    fetchPatients();
  }, [selectedPatient]);

  const handleCreateRecord = (recordData) => {
    fetch('/api/records', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(recordData)
    })
      .then(response => {
        if (!response.ok) throw new Error('Failed to create medical record');
        return response.json();
      })
      .then(data => {
        setRecords([...records, data]);
        setIsModalOpen(false);
        setCurrentRecord(null);
      })
      .catch(err => setError(err.message));
  };

  const handleUpdateRecord = (recordData) => {
    fetch(`/api/records/${recordData.id}`, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(recordData)
    })
      .then(response => {
        if (!response.ok) throw new Error('Failed to update medical record');
        return response.json();
      })
      .then(data => {
        setRecords(records.map(record => 
          record.id === data.id ? data : record
        ));
        setIsModalOpen(false);
        setCurrentRecord(null);
      })
      .catch(err => setError(err.message));
  };

  const handleDeleteRecord = (id) => {
    if (window.confirm('Are you sure you want to delete this medical record?')) {
      fetch(`/api/records/${id}`, { method: 'DELETE' })
        .then(response => {
          if (!response.ok) throw new Error('Failed to delete medical record');
          setRecords(records.filter(record => record.id !== id));
        })
        .catch(err => setError(err.message));
    }
  };

  const handleSearch = (query) => {
    setIsLoading(true);
    fetch(`/api/records?search=${query}${selectedPatient ? `&patientId=${selectedPatient}` : ''}`)
      .then(response => {
        if (!response.ok) throw new Error('Search failed');
        return response.json();
      })
      .then(data => {
        setRecords(data);
        setIsLoading(false);
      })
      .catch(err => {
        setError(err.message);
        setIsLoading(false);
      });
  };

  const handleExport = () => {
    // Implement export functionality
    alert('Export functionality would be implemented here');
  };

  const handlePatientChange = (e) => {
    setSelectedPatient(e.target.value);
  };

  const filteredRecords = records.filter(record => {
    if (viewMode === 'all') return true;
    if (viewMode === 'recent') {
      const recordDate = new Date(record.date);
      const thirtyDaysAgo = new Date();
      thirtyDaysAgo.setDate(thirtyDaysAgo.getDate() - 30);
      return recordDate >= thirtyDaysAgo;
    }
    if (viewMode === 'archived') return record.archived;
    return true;
  });

  const columns = [
    { field: 'patientName', header: 'Patient Name', sortable: true },
    { field: 'doctorName', header: 'Doctor', sortable: true },
    { 
      field: 'date', 
      header: 'Date', 
      sortable: true,
      render: (row) => new Date(row.date).toLocaleDateString() 
    },
    { field: 'diagnosis', header: 'Diagnosis' },
    { field: 'recordType', header: 'Record Type' },
  ];

  return (
    <div className="medical-records-page">
      <PageHeader 
        title="Medical Records" 
        onSearch={handleSearch} 
      />
      
      <div className="content-container">
        <div className="page-toolbar">
          <div className="view-toggle-pills">
            <button 
              className={`pill-btn ${viewMode === 'all' ? 'active' : ''}`}
              onClick={() => setViewMode('all')}
            >
              All Records
            </button>
            <button 
              className={`pill-btn ${viewMode === 'recent' ? 'active' : ''}`}
              onClick={() => setViewMode('recent')}
            >
              Recent
            </button>
            <button 
              className={`pill-btn ${viewMode === 'archived' ? 'active' : ''}`}
              onClick={() => setViewMode('archived')}
            >
              Archived
            </button>
          </div>
          
          <div className="filter-section">
            <div className="patient-filter">
              <User size={16} />
              <select 
                value={selectedPatient} 
                onChange={handlePatientChange}
              >
                <option value="">All Patients</option>
                {patients.map(patient => (
                  <option key={patient.id} value={patient.id}>
                    {patient.name}
                  </option>
                ))}
              </select>
            </div>
          </div>
          
          <button 
            className="btn primary" 
            onClick={() => {
              setCurrentRecord(null);
              setIsModalOpen(true);
            }}
          >
            <Plus size={16} />
            <span>New Record</span>
          </button>
        </div>
        
        {isLoading ? (
          <div className="loading">Loading medical records...</div>
        ) : error ? (
          <div className="error-message">{error}</div>
        ) : (
          <DataTable 
            columns={columns} 
            data={filteredRecords}
            onView={(id) => {
              const record = records.find(rec => rec.id === id);
              setCurrentRecord(record);
              setIsModalOpen(true);
            }}
            onEdit={(id) => {
              const record = records.find(rec => rec.id === id);
              setCurrentRecord(record);
              setIsModalOpen(true);
            }}
            onDelete={handleDeleteRecord}
            onExport={handleExport}
          />
        )}
      </div>
      
      <Modal
        isOpen={isModalOpen}
        onClose={() => {
          setIsModalOpen(false);
          setCurrentRecord(null);
        }}
        title={currentRecord ? "View/Edit Medical Record" : "Create Medical Record"}
        size="large"
      >
        <MedicalRecordForm 
          record={currentRecord}
          onSubmit={currentRecord ? handleUpdateRecord : handleCreateRecord}
          onCancel={() => {
            setIsModalOpen(false);
            setCurrentRecord(null);
          }}
          patients={patients}
        />
      </Modal>
    </div>
  );
};

const MedicalRecordForm = ({ record, onSubmit, onCancel, patients }) => {
  const [formData, setFormData] = useState({
    patientId: '',
    doctorId: '',
    date: new Date().toISOString().split('T')[0],
    diagnosis: '',
    treatment: '',
    prescription: '',
    notes: '',
    recordType: 'consultation',
    ...record
  });
  
  const [errors, setErrors] = useState({});
  const [doctors, setDoctors] = useState([]);
  
  useEffect(() => {
    // Fetch doctors for dropdown
    fetch('/api/doctors')
      .then(response => response.json())
      .then(data => setDoctors(data))
      .catch(err => console.error('Failed to fetch doctors:', err));
  }, []);

  const validateForm = () => {
    const newErrors = {};
    if (!formData.patientId) newErrors.patientId = 'Patient is required';
    if (!formData.doctorId) newErrors.doctorId = 'Doctor is required';
    if (!formData.date) newErrors.date = 'Date is required';
    if (!formData.diagnosis) newErrors.diagnosis = 'Diagnosis is required';
    if (!formData.recordType) newErrors.recordType = 'Record type is required';
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
      <div className="form-row">
        <div className="form-group">
          <label htmlFor="patientId">Patient</label>
          <select
            id="patientId"
            name="patientId"
            value={formData.patientId}
            onChange={handleChange}
            className={errors.patientId ? 'error' : ''}
            disabled={record}
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
          <label htmlFor="recordType">Record Type</label>
          <select
            id="recordType"
            name="recordType"
            value={formData.recordType}
            onChange={handleChange}
            className={errors.recordType ? 'error' : ''}
          >
            <option value="consultation">Consultation</option>
            <option value="lab_result">Lab Result</option>
            <option value="surgery">Surgery</option>
            <option value="follow_up">Follow Up</option>
            <option value="emergency">Emergency</option>
          </select>
          {errors.recordType && <span className="error-message">{errors.recordType}</span>}
        </div>
      </div>
      
      <div className="form-group">
        <label htmlFor="diagnosis">Diagnosis</label>
        <input
          type="text"
          id="diagnosis"
          name="diagnosis"
          value={formData.diagnosis}
          onChange={handleChange}
          className={errors.diagnosis ? 'error' : ''}
        />
        {errors.diagnosis && <span className="error-message">{errors.diagnosis}</span>}
      </div>
      
      <div className="form-group">
        <label htmlFor="treatment">Treatment</label>
        <textarea
          id="treatment"
          name="treatment"
          value={formData.treatment || ''}
          onChange={handleChange}
          rows="3"
        />
      </div>
      
      <div className="form-group">
        <label htmlFor="prescription">Prescription</label>
        <textarea
          id="prescription"
          name="prescription"
          value={formData.prescription || ''}
          onChange={handleChange}
          rows="3"
        />
      </div>
      
      <div className="form-group">
        <label htmlFor="notes">Additional Notes</label>
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
          {record ? 'Update Record' : 'Create Record'}
        </button>
      </div>
    </form>
  );
};

export default MedicalRecords;