import { useState } from 'react';
import { BrowserRouter as Router, Routes, Route, Navigate } from 'react-router-dom';
import Sidebar from './components/Sidebar';
import Dashboard from './pages/Dashboard';
import Patients from './pages/Patients';
import PatientDetails from './pages/PatientDetails';
import Doctors from './pages/Doctors';
import DoctorDetails from './pages/DoctorDetails';
import Appointments from './pages/Appointments.jsx';
import MedicalRecords from './pages/MedicalRecords';
import Users from './pages/Users.jsx';
import RoleSelector from './pages/RoleSelector';
import './App.css';

function App() {
  const [userRole, setUserRole] = useState(localStorage.getItem('userRole') || '');

  const handleRoleSelect = (role) => {
    setUserRole(role);
    localStorage.setItem('userRole', role);
  };

  const ProtectedRoute = ({ children, allowedRoles = [] }) => {
    if (!userRole) return <Navigate to="/" />;
    if (allowedRoles.length > 0 && !allowedRoles.includes(userRole)) {
      return <Navigate to="/dashboard" />;
    }
    return children;
  };

  return (
    <Router>
      <div className="app">
        {userRole && <Sidebar userRole={userRole} />}
        <div className="content">
          <Routes>
            <Route 
              path="/" 
              element={
                userRole ? (
                  <Navigate to="/dashboard" />
                ) : (
                  <RoleSelector onRoleSelect={handleRoleSelect} />
                )
              } 
            />
            
            <Route 
              path="/dashboard" 
              element={
                <ProtectedRoute>
                  <Dashboard />
                </ProtectedRoute>
              } 
            />
            
            <Route 
              path="/patients" 
              element={
                <ProtectedRoute allowedRoles={['admin', 'doctor', 'receptionist']}>
                  <Patients />
                </ProtectedRoute>
              } 
            />
            
            <Route 
              path="/patients/:id" 
              element={
                <ProtectedRoute allowedRoles={['admin', 'doctor', 'receptionist']}>
                  <PatientDetails />
                </ProtectedRoute>
              } 
            />
            
            <Route 
              path="/doctors" 
              element={
                <ProtectedRoute allowedRoles={['admin', 'receptionist']}>
                  <Doctors />
                </ProtectedRoute>
              } 
            />
            
            <Route 
              path="/doctors/:id" 
              element={
                <ProtectedRoute allowedRoles={['admin', 'receptionist']}>
                  <DoctorDetails />
                </ProtectedRoute>
              } 
            />
            
            <Route 
              path="/appointments" 
              element={
                <ProtectedRoute allowedRoles={['admin', 'doctor', 'receptionist', 'patient']}>
                  <Appointments />
                </ProtectedRoute>
              } 
            />
            
            <Route 
              path="/records" 
              element={
                <ProtectedRoute allowedRoles={['admin', 'doctor']}>
                  <MedicalRecords />
                </ProtectedRoute>
              } 
            />
            
            <Route 
              path="/users" 
              element={
                <ProtectedRoute allowedRoles={['admin']}>
                  <Users />
                </ProtectedRoute>
              } 
            />
          </Routes>
        </div>
      </div>
    </Router>
  );
}

export default App;