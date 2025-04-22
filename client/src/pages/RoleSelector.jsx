// src/pages/RoleSelector.jsx
import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { User, Shield, Stethoscope, ClipboardList, UserCheck } from 'lucide-react';

const RoleSelector = ({ onRoleSelect }) => {
  const navigate = useNavigate();
  const [selectedRole, setSelectedRole] = useState('');

  const handleSubmit = (e) => {
    e.preventDefault();
    if (selectedRole) {
      onRoleSelect(selectedRole);
      navigate('/dashboard');
    }
  };

  return (
    <div className="role-selector-container">
      <div className="role-selector-card">
        <h1>Hospital Management System</h1>
        <p>Select your role to continue</p>
        
        <form onSubmit={handleSubmit} className="role-form">
          <div className="role-options">
            <label className={`role-option ${selectedRole === 'admin' ? 'selected' : ''}`}>
              <input
                type="radio"
                name="role"
                value="admin"
                checked={selectedRole === 'admin'}
                onChange={() => setSelectedRole('admin')}
              />
              <div className="role-content">
                <Shield size={32} />
                <span>Admin</span>
              </div>
            </label>
            
            <label className={`role-option ${selectedRole === 'doctor' ? 'selected' : ''}`}>
              <input
                type="radio"
                name="role"
                value="doctor"
                checked={selectedRole === 'doctor'}
                onChange={() => setSelectedRole('doctor')}
              />
              <div className="role-content">
                <Stethoscope size={32} />
                <span>Doctor</span>
              </div>
            </label>
            
            <label className={`role-option ${selectedRole === 'receptionist' ? 'selected' : ''}`}>
              <input
                type="radio"
                name="role"
                value="receptionist"
                checked={selectedRole === 'receptionist'}
                onChange={() => setSelectedRole('receptionist')}
              />
              <div className="role-content">
                <ClipboardList size={32} />
                <span>Receptionist</span>
              </div>
            </label>
            
            <label className={`role-option ${selectedRole === 'patient' ? 'selected' : ''}`}>
              <input
                type="radio"
                name="role"
                value="patient"
                checked={selectedRole === 'patient'}
                onChange={() => setSelectedRole('patient')}
              />
              <div className="role-content">
                <User size={32} />
                <span>Patient</span>
              </div>
            </label>
          </div>
          
          <button 
            type="submit" 
            className="btn primary" 
            disabled={!selectedRole}
          >
            Continue
          </button>
        </form>
      </div>
    </div>
  );
};

export default RoleSelector;