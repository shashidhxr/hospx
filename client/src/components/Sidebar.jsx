import { NavLink, useNavigate } from 'react-router-dom';
import { useState } from 'react';
import { 
  User, Users, UserCog, Calendar, FileText, 
  Home, Menu, X, LogOut, Activity 
} from 'lucide-react';

const Sidebar = ({ userRole }) => {
  const [isCollapsed, setIsCollapsed] = useState(false);
  const navigate = useNavigate();

  const toggleSidebar = () => {
    setIsCollapsed(!isCollapsed);
  };
  
  const handleLogout = () => {
    localStorage.removeItem('userRole');
    navigate('/');
  };

  const menuItems = [
    { name: 'Dashboard', path: '/dashboard', icon: <Home size={20} />, roles: ['admin', 'doctor', 'receptionist', 'patient'] },
    { name: 'Patients', path: '/patients', icon: <Users size={20} />, roles: ['admin', 'doctor', 'receptionist'] },
    { name: 'Doctors', path: '/doctors', icon: <UserCog size={20} />, roles: ['admin', 'receptionist'] },
    { name: 'Appointments', path: '/appointments', icon: <Calendar size={20} />, roles: ['admin', 'doctor', 'receptionist', 'patient'] },
    { name: 'Medical Records', path: '/records', icon: <FileText size={20} />, roles: ['admin', 'doctor'] },
    { name: 'User Management', path: '/users', icon: <User size={20} />, roles: ['admin'] },
  ];

  return (
    <aside className={`sidebar ${isCollapsed ? 'collapsed' : ''}`}>
      <div className="sidebar-header">
        <div className="logo-container">
          {!isCollapsed && <h2>HMS</h2>}
          <Activity size={24} className="logo-icon" />
        </div>
        <button className="toggle-btn" onClick={toggleSidebar}>
          {isCollapsed ? <Menu size={20} /> : <X size={20} />}
        </button>
      </div>
      
      <nav className="sidebar-nav">
        <ul>
          {menuItems.map((item) => (
            item.roles.includes(userRole) && (
              <li key={item.path}>
                <NavLink 
                  to={item.path} 
                  className={({ isActive }) => isActive ? 'active' : ''}
                  end
                >
                  <span className="icon">{item.icon}</span>
                  {!isCollapsed && <span className="text">{item.name}</span>}
                </NavLink>
              </li>
            )
          ))}
        </ul>
      </nav>
      
      <div className="sidebar-footer">
        <button className="logout-btn" onClick={handleLogout}>
          <LogOut size={20} />
          {!isCollapsed && <span>Logout</span>}
        </button>
      </div>
    </aside>
  );
};

export default Sidebar;