import { useState, useEffect } from 'react';
import PageHeader from '../components/PageHeader';
import { Users, UserCog, Calendar, DollarSign, Activity, TrendingUp, Clock } from 'lucide-react';

const Dashboard = () => {
  const [stats, setStats] = useState({
    totalPatients: 0,
    totalDoctors: 0,
    totalAppointments: 0,
    pendingAppointments: 0
  });
  
  const [recentAppointments, setRecentAppointments] = useState([]);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    const fetchData = async () => {
      try {
        // Fetch all data in parallel
        const [patientsRes, doctorsRes, appointmentsRes] = await Promise.all([
          fetch('http://localhost:8080/patients'),
          fetch('http://localhost:8080/doctors'),
          fetch('http://localhost:8080/appointments')
        ]);

        if (!patientsRes.ok || !doctorsRes.ok || !appointmentsRes.ok) {
          throw new Error('Failed to fetch dashboard data');
        }

        const [patients, doctors, appointments] = await Promise.all([
          patientsRes.json(),
          doctorsRes.json(),
          appointmentsRes.json()
        ]);

        // Convert object to array if needed
        const patientsArray = Array.isArray(patients) ? patients : Object.values(patients);
        const doctorsArray = Array.isArray(doctors) ? doctors : Object.values(doctors);
        const appointmentsArray = Array.isArray(appointments) ? appointments : Object.values(appointments);

        // Calculate pending appointments
        const pending = appointmentsArray.filter(appt => 
          appt.status === 'scheduled' || appt.status === 'pending'
        ).length;

        // Get recent appointments (last 4)
        const recent = appointmentsArray
          .sort((a, b) => new Date(b.date) - new Date(a.date))
          .slice(0, 4)
          .map(appt => ({
            id: appt.id,
            patientName: appt.patient_name || 'Unknown Patient',
            doctorName: appt.doctor_name || 'Unknown Doctor',
            date: appt.date,
            time: appt.time,
            status: appt.status || 'scheduled'
          }));

        setStats({
          totalPatients: patientsArray.length,
          totalDoctors: doctorsArray.length,
          totalAppointments: appointmentsArray.length,
          pendingAppointments: pending
        });

        setRecentAppointments(recent);
        setIsLoading(false);
      } catch (error) {
        console.error('Error fetching dashboard data:', error);
        setError(error.message);
        setIsLoading(false);
      }
    };
    
    fetchData();
  }, []);

  if (isLoading) {
    return (
      <div className="dashboard-page">
        <PageHeader title="Dashboard" />
        <div className="loading">Loading dashboard data...</div>
      </div>
    );
  }

  if (error) {
    return (
      <div className="dashboard-page">
        <PageHeader title="Dashboard" />
        <div className="error-message">{error}</div>
      </div>
    );
  }

  return (
    <div className="dashboard-page">
      <PageHeader title="Dashboard" />
      
      <div className="stats-grid">
        <div className="stat-card">
          <div className="icon patients-icon">
            <Users size={24} />
          </div>
          <h3>{stats.totalPatients}</h3>
          <p>Total Patients</p>
        </div>
        
        <div className="stat-card">
          <div className="icon doctors-icon">
            <UserCog size={24} />
          </div>
          <h3>{stats.totalDoctors}</h3>
          <p>Total Doctors</p>
        </div>
        
        <div className="stat-card">
          <div className="icon appointments-icon">
            <Calendar size={24} />
          </div>
          <h3>{stats.totalAppointments}</h3>
          <p>Total Appointments</p>
        </div>
        
        <div className="stat-card">
          <div className="icon revenue-icon">
            <Clock size={24} />
          </div>
          <h3>{stats.pendingAppointments}</h3>
          <p>Pending Appointments</p>
        </div>
      </div>
      
      <div className="chart-grid">
        <div className="chart-card">
          <div className="chart-header">
            <h3 className="chart-title">Appointment Statistics</h3>
            <div className="btn-group">
              <button className="btn outline">Weekly</button>
              <button className="btn primary">Monthly</button>
              <button className="btn outline">Yearly</button>
            </div>
          </div>
          <div className="chart-container">
            <div className="chart-placeholder">
              <TrendingUp size={48} color="#4361ee" />
              <span style={{ marginLeft: '1rem' }}>Appointment Trend Chart</span>
            </div>
          </div>
        </div>
        
        <div className="chart-card">
          <div className="chart-header">
            <h3 className="chart-title">Department Distribution</h3>
          </div>
          <div className="chart-container">
            <div className="chart-placeholder">
              <Activity size={48} color="#4361ee" />
              <span style={{ marginLeft: '1rem' }}>Department Distribution Chart</span>
            </div>
          </div>
        </div>
      </div>
      
      <div className="recent-section">
        <div className="recent-header">
          <h3 className="recent-title">Recent Appointments</h3>
          <button className="btn outline">View All</button>
        </div>
        
        <ul className="recent-list">
          {recentAppointments.map(appointment => (
            <li key={appointment.id} className="recent-item">
              <div className={`recent-item-icon bg-light`}>
                <Calendar size={20} color="#4361ee" />
              </div>
              <div className="recent-item-content">
                <h4 className="recent-item-title">{appointment.patientName}</h4>
                <div className="recent-item-meta">
                  <span>{appointment.doctorName}</span>
                  <span>•</span>
                  <span>{appointment.date} at {appointment.time}</span>
                </div>
              </div>
              <div className={`recent-item-status status-${appointment.status}`}>
                {appointment.status.charAt(0).toUpperCase() + appointment.status.slice(1)}
              </div>
            </li>
          ))}
        </ul>
      </div>
    </div>
  );
};

export default Dashboard;