// src/pages/Patients.jsx
import { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import { Plus, FileText } from 'lucide-react';
import PageHeader from '../components/PageHeader';
import DataTable from '../components/DataTable';
import Modal from '../components/Modal';
import PatientForm from '../components/PatientForm';

const Patients = () => {
  const navigate = useNavigate();
  const [patients, setPatients] = useState([]);
  const [isLoading, setIsLoading] = useState(true);
  // eslint-disable-next-line no-unused-vars
  const [searchQuery, setSearchQuery] = useState('');
  const [showModal, setShowModal] = useState(false);
  const [currentPatient, setCurrentPatient] = useState(null);

  // Mock data - replace with API calls
  useEffect(() => {
    // Simulate API call
    setTimeout(() => {
      setPatients([
        { id: 1, name: 'John Doe', age: 45, gender: 'Male', contact: '555-123-4567', address: '123 Main St' },
        { id: 2, name: 'Jane Smith', age: 32, gender: 'Female', contact: '555-987-6543', address: '456 Oak Ave' },
        { id: 3, name: 'Robert Johnson', age: 58, gender: 'Male', contact: '555-567-8901', address: '789 Pine Rd' },
        { id: 4, name: 'Emily Williams', age: 27, gender: 'Female', contact: '555-234-5678', address: '321 Elm St' },
        { id: 5, name: 'Michael Brown', age: 41, gender: 'Male', contact: '555-345-6789', address: '654 Maple Dr' },
      ]);
      setIsLoading(false);
    }, 500);
  }, []);
  
  const columns = [
    { field: 'name', header: 'Name', sortable: true },
    { field: 'age', header: 'Age', sortable: true },
    { field: 'gender', header: 'Gender', sortable: true },
    { field: 'contact', header: 'Contact', sortable: true },
    { 
      field: 'address', 
      header: 'Address', 
      sortable: true,
      render: (row) => row.address ? `${row.address.substring(0, 20)}${row.address.length > 20 ? '...' : ''}` : ''
    },
  ];

  const handleSearch = (query) => {
    setSearchQuery(query);
  };

  const handleView = (id) => {
    navigate(`/patients/${id}`);
  };

  const handleEdit = (id) => {
    const patient = patients.find(p => p.id === id);
    setCurrentPatient(patient);
    setShowModal(true);
  };

  const handleDelete = (id) => {
    if (window.confirm('Are you sure you want to delete this patient?')) {
      // In a real app, you'd call an API here
      setPatients(patients.filter(patient => patient.id !== id));
    }
  };

  const handleAddNew = () => {
    setCurrentPatient(null);
    setShowModal(true);
  };

  const handleFormSubmit = (data) => {
    if (currentPatient) {
      // Update existing patient
      setPatients(patients.map(p => p.id === currentPatient.id ? { ...data, id: currentPatient.id } : p));
    } else {
      // Add new patient
      const newId = Math.max(...patients.map(p => p.id), 0) + 1;
      setPatients([...patients, { ...data, id: newId }]);
    }
    setShowModal(false);
  };

  const handleExport = () => {
    // In a real app, this would generate a CSV or PDF
    alert('Exporting patient data...');
  };

  return (
    <div className="patients-page">
      <PageHeader 
        title="Patients" 
        onSearch={handleSearch} 
      />
      
      <div className="content-container">
        <div className="page-actions">
          <button className="btn primary" onClick={handleAddNew}>
            <Plus size={18} />
            <span>Add New Patient</span>
          </button>
          
          <button className="btn secondary" onClick={handleExport}>
            <FileText size={18} />
            <span>Export List</span>
          </button>
        </div>
        
        {isLoading ? (
          <div className="loading">Loading patients...</div>
        ) : (
          <DataTable 
            columns={columns} 
            data={patients} 
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
        title={currentPatient ? 'Edit Patient' : 'Add New Patient'}
      >
        <PatientForm 
          patient={currentPatient} 
          onSubmit={handleFormSubmit} 
          onCancel={() => setShowModal(false)} 
        />
      </Modal>
    </div>
  );
};

export default Patients;