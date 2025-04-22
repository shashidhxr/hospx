// src/services/api.js
import axios from 'axios';

const API_URL = 'http://localhost:5000/api'; // Replace with your actual API URL

// Create axios instance
const api = axios.create({
  baseURL: API_URL,
  headers: {
    'Content-Type': 'application/json',
  },
});

// Add auth token interceptor
api.interceptors.request.use(
  (config) => {
    const token = localStorage.getItem('token');
    if (token) {
      config.headers.Authorization = `Bearer ${token}`;
    }
    return config;
  },
  (error) => Promise.reject(error)
);

// Auth services
export const login = (credentials) => api.post('/auth/login', credentials);
export const register = (userData) => api.post('/auth/register', userData);

// User services
export const getUsers = () => api.get('/users');
export const getUser = (id) => api.get(`/users/${id}`);
export const createUser = (userData) => api.post('/users', userData);
export const updateUser = (id, userData) => api.put(`/users/${id}`, userData);
export const deleteUser = (id) => api.delete(`/users/${id}`);

// Patient services
export const getPatients = () => api.get('/patients');
export const getPatient = (id) => api.get(`/patients/${id}`);
export const createPatient = (patientData) => api.post('/patients', patientData);
export const updatePatient = (id, patientData) => api.put(`/patients/${id}`, patientData);
export const deletePatient = (id) => api.delete(`/patients/${id}`);
export const getPatientAppointments = (id) => api.get(`/patients/${id}/appointments`);
export const getPatientRecords = (id) => api.get(`/patients/${id}/records`);

// Doctor services
export const getDoctors = () => api.get('/doctors');
export const getDoctor = (id) => api.get(`/doctors/${id}`);
export const createDoctor = (doctorData) => api.post('/doctors', doctorData);
export const updateDoctor = (id, doctorData) => api.put(`/doctors/${id}`, doctorData);
export const deleteDoctor = (id) => api.delete(`/doctors/${id}`);
export const getDoctorAppointments = (id) => api.get(`/doctors/${id}/appointments`);
export const createPrescription = (doctorId, prescriptionData) => 
  api.post(`/doctors/${doctorId}/prescribe`, prescriptionData);

// Appointment services
export const getAppointments = () => api.get('/appointments');
export const getAppointment = (id) => api.get(`/appointments/${id}`);
export const createAppointment = (appointmentData) => api.post('/appointments', appointmentData);
export const updateAppointment = (id, appointmentData) => api.put(`/appointments/${id}`, appointmentData);
export const cancelAppointment = (id) => api.delete(`/appointments/${id}`);

// Medical record services
export const getMedicalRecords = () => api.get('/records');
export const getMedicalRecord = (id) => api.get(`/records/${id}`);
export const createMedicalRecord = (recordData) => api.post('/records', recordData);

export default api;