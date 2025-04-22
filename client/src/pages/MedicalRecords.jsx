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
  const [patients, setPatients] = useState([]);
  const [doctors, setDoctors] = useState([]);
  const [selectedPatient, setSelectedPatient] = useState('');

  useEffect(() => {
    const fetchData = async () => {
      try {
        setIsLoading(true);
        
        // Fetch patients
        const patientsRes = await fetch('http://localhost:8080/patients');
        if (!patientsRes.ok) throw new Error('Failed to fetch patients');
        const patientsData = await patientsRes.json();
        setPatients(patientsData.map(p => ({
          id: p.patient_id,
          name: p.name
        })));

        // Fetch doctors
        const doctorsRes = await fetch('http://localhost:8080/doctors');
        if (!doctorsRes.ok) throw new Error('Failed to fetch doctors');
        const doctorsData = await doctorsRes.json();
        setDoctors(doctorsData.map(d => ({
          id: d.doctor_id,
          name: d.name,
          specialization: d.specialization || 'General'
        })));

        // Fetch records
        const url = selectedPatient 
          ? `http://localhost:8080/records/patient/${selectedPatient}`
          : 'http://localhost:8080/records';
        
        const recordsRes = await fetch(url);
        if (!recordsRes.ok) throw new Error('Failed to fetch medical records');
        const recordsData = await recordsRes.json();
        
        // Transform data to match frontend expectations
        const transformedRecords = recordsData.map(record => ({
          id: record.id || record.recordID,
          patientId: record.patient_id,
          patientName: record.patient_name,
          doctorId: record.doctor_id,
          doctorName: record.doctor_name,
          date: record.date,
          diagnosis: record.diagnosis,
          treatment: record.treatment,
          recordType: 'consultation' // Default since backend doesn't provide this
        }));
        
        setRecords(transformedRecords);
        setIsLoading(false);
      } catch (err) {
        setError(err.message);
        setIsLoading(false);
      }
    };

    fetchData();
  }, [selectedPatient]);

  const handleCreateRecord = async (recordData) => {
    try {
      const response = await fetch('http://localhost:8080/records', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          patient_id: parseInt(recordData.patientId),
          doctor_id: parseInt(recordData.doctorId),
          diagnosis: recordData.diagnosis,
          treatment: recordData.treatment
        })
      });

      if (!response.ok) throw new Error('Failed to create medical record');
      
      const data = await response.json();
      const newRecord = {
        id: data.id,
        patientId: recordData.patientId,
        patientName: patients.find(p => p.id === recordData.patientId)?.name || 'Unknown',
        doctorId: recordData.doctorId,
        doctorName: doctors.find(d => d.id === recordData.doctorId)?.name || 'Unknown',
        date: new Date().toISOString().split('T')[0], // Default date
        diagnosis: recordData.diagnosis,
        treatment: recordData.treatment,
        recordType: recordData.recordType
      };

      setRecords([...records, newRecord]);
      setIsModalOpen(false);
      setCurrentRecord(null);
    } catch (err) {
      setError(err.message);
    }
  };

  const handleUpdateRecord = async (recordData) => {
    try {
      const response = await fetch(`http://localhost:8080/records/${recordData.id}`, {
        method: 'PUT',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          diagnosis: recordData.diagnosis,
          treatment: recordData.treatment
        })
      });

      if (!response.ok) throw new Error('Failed to update medical record');
      
      const updatedRecord = {
        ...recordData,
        patientName: patients.find(p => p.id === recordData.patientId)?.name || 'Unknown',
        doctorName: doctors.find(d => d.id === recordData.doctorId)?.name || 'Unknown'
      };

      setRecords(records.map(record => 
        record.id === recordData.id ? updatedRecord : record
      ));
      setIsModalOpen(false);
      setCurrentRecord(null);
    } catch (err) {
      setError(err.message);
    }
  };

  const handleDeleteRecord = async (id) => {
    if (window.confirm('Are you sure you want to delete this medical record?')) {
      try {
        const response = await fetch(`http://localhost:8080/records/${id}`, { 
          method: 'DELETE' 
        });
        
        if (!response.ok) throw new Error('Failed to delete medical record');
        
        setRecords(records.filter(record => record.id !== id));
      } catch (err) {
        setError(err.message);
      }
    }
  };

  const handlePatientChange = (e) => {
    setSelectedPatient(e.target.value);
  };

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
        onSearch={(query) => {
          // Implement search if backend supports it
          console.log('Search:', query);
        }} 
      />
      
      <div className="content-container">
        <div className="page-toolbar">
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
            data={records}
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
          doctors={doctors}
        />
      </Modal>
    </div>
  );
};

const MedicalRecordForm = ({ record, onSubmit, onCancel, patients, doctors }) => {
  const [formData, setFormData] = useState({
    patientId: '',
    doctorId: '',
    date: new Date().toISOString().split('T')[0],
    diagnosis: '',
    treatment: '',
    recordType: 'consultation',
    ...record
  });
  
  const [errors, setErrors] = useState({});

  const validateForm = () => {
    const newErrors = {};
    if (!formData.patientId) newErrors.patientId = 'Patient is required';
    if (!formData.doctorId) newErrors.doctorId = 'Doctor is required';
    if (!formData.diagnosis) newErrors.diagnosis = 'Diagnosis is required';
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
            disabled={!!record}
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
          />
        </div>
        
        <div className="form-group">
          <label htmlFor="recordType">Record Type</label>
          <select
            id="recordType"
            name="recordType"
            value={formData.recordType}
            onChange={handleChange}
          >
            <option value="consultation">Consultation</option>
            <option value="lab_result">Lab Result</option>
            <option value="surgery">Surgery</option>
            <option value="follow_up">Follow Up</option>
            <option value="emergency">Emergency</option>
          </select>
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