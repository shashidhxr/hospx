import { useState, useEffect } from 'react';
import { Plus, UserPlus, Filter, Shield, UserCheck, Key } from 'lucide-react';
import PageHeader from '../components/PageHeader';
import DataTable from '../components/DataTable';
import Modal from '../components/Modal';

const Users = () => {
  const [users, setUsers] = useState([]);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [currentUser, setCurrentUser] = useState(null);
  const [filterType, setFilterType] = useState('all');
  const [isResetPasswordModalOpen, setIsResetPasswordModalOpen] = useState(false);
  const [userToResetPassword, setUserToResetPassword] = useState(null);

  useEffect(() => {
    const fetchUsers = async () => {
      try {
        const response = await fetch('http://localhost:8080/users');
        if (!response.ok) throw new Error('Failed to fetch users');
        const data = await response.json();
        // Convert the array-like object to a proper array
        const usersArray = Object.values(data).map(user => ({
          id: user.id,
          name: user.name,
          contact: user.contact,
          type: user.type,
          active: true // Assuming all users are active by default
        }));
        setUsers(usersArray);
        setIsLoading(false);
      } catch (err) {
        setError(err.message);
        setIsLoading(false);
      }
    };

    fetchUsers();
  }, []);

  const handleCreateUser = (userData) => {
    fetch('http://localhost:8080/users', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        name: userData.name,
        contact: userData.contact,
        type: userData.role // Changed from 'role' to 'type' to match backend
      })
    })
      .then(response => {
        if (!response.ok) throw new Error('Failed to create user');
        return response.json();
      })
      .then(data => {
        const newUser = {
          id: data.id || Date.now(), // Use generated ID or fallback
          name: data.name,
          contact: data.contact,
          type: data.type,
          active: true
        };
        setUsers([...users, newUser]);
        setIsModalOpen(false);
        setCurrentUser(null);
      })
      .catch(err => setError(err.message));
  };

  const handleUpdateUser = (userData) => {
    // Note: Your backend doesn't currently have an update endpoint
    // This is a placeholder implementation
    fetch(`http://localhost:8080/users/${userData.id}`, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        name: userData.name,
        contact: userData.contact,
        type: userData.role
      })
    })
      .then(response => {
        if (!response.ok) throw new Error('Failed to update user');
        return response.json();
      })
      .then(data => {
        setUsers(users.map(user => 
          user.id === data.id ? {
            ...user,
            name: data.name,
            contact: data.contact,
            type: data.type
          } : user
        ));
        setIsModalOpen(false);
        setCurrentUser(null);
      })
      .catch(err => setError(err.message));
  };

  const handleDeleteUser = (id) => {
    if (window.confirm('Are you sure you want to delete this user?')) {
      // Note: Your backend doesn't currently have a delete endpoint
      // This is a placeholder implementation
      fetch(`http://localhost:8080/users/${id}`, { method: 'DELETE' })
        .then(response => {
          if (!response.ok) throw new Error('Failed to delete user');
          setUsers(users.filter(user => user.id !== id));
        })
        .catch(err => setError(err.message));
    }
  };

  const handleResetPassword = (password) => {
    if (!userToResetPassword) return;
    
    // Note: Your backend doesn't currently have a password reset endpoint
    // This is a placeholder implementation
    fetch(`http://localhost:8080/users/${userToResetPassword.id}/reset-password`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ password })
    })
      .then(response => {
        if (!response.ok) throw new Error('Failed to reset password');
        setIsResetPasswordModalOpen(false);
        setUserToResetPassword(null);
        alert('Password reset successfully');
      })
      .catch(err => setError(err.message));
  };

  const filteredUsers = filterType === 'all' 
    ? users 
    : users.filter(user => user.type === filterType);

  const columns = [
    { field: 'name', header: 'Name', sortable: true },
    { field: 'contact', header: 'Contact', sortable: true },
    { 
      field: 'type', 
      header: 'Type', 
      sortable: true,
      render: (row) => (
        <span className={`type-badge ${row.type.toLowerCase()}`}>
          {row.type.charAt(0).toUpperCase() + row.type.slice(1)}
        </span>
      )
    },
    { 
      field: 'status', 
      header: 'Status', 
      render: (row) => (
        <span className={`status-badge ${row.active ? 'active' : 'inactive'}`}>
          {row.active ? 'Active' : 'Inactive'}
        </span>
      )
    },
  ];

  const handleEditUserClick = (id) => {
    const user = users.find(user => user.id === id);
    setCurrentUser({
      ...user,
      role: user.type // Map 'type' to 'role' for the form
    });
    setIsModalOpen(true);
  };

  const handleResetPasswordClick = (id) => {
    const user = users.find(user => user.id === id);
    setUserToResetPassword(user);
    setIsResetPasswordModalOpen(true);
  };

  const customActions = (id) => (
    <button 
      className="icon-button small" 
      onClick={() => handleResetPasswordClick(id)}
      title="Reset Password"
    >
      <Key size={16} />
    </button>
  );

  return (
    <div className="users-page">
      <PageHeader 
        title="User Management" 
      />
      
      <div className="content-container">
        <div className="page-toolbar">
          <div className="type-filter">
            <div className="filter-dropdown">
              <Filter size={16} />
              <select 
                value={filterType} 
                onChange={(e) => setFilterType(e.target.value)}
              >
                <option value="all">All Types</option>
                <option value="admin">Admin</option>
                <option value="doctor">Doctor</option>
                <option value="receptionist">Receptionist</option>
                <option value="patient">Patient</option>
              </select>
            </div>
          </div>
          
          <button 
            className="btn primary" 
            onClick={() => {
              setCurrentUser(null);
              setIsModalOpen(true);
            }}
          >
            <UserPlus size={16} />
            <span>Add User</span>
          </button>
        </div>
        
        {isLoading ? (
          <div className="loading">Loading users...</div>
        ) : error ? (
          <div className="error-message">{error}</div>
        ) : (
          <DataTable 
            columns={columns} 
            data={filteredUsers}
            onEdit={handleEditUserClick}
            onDelete={handleDeleteUser}
            customActions={customActions}
          />
        )}
      </div>
      
      <Modal
        isOpen={isModalOpen}
        onClose={() => {
          setIsModalOpen(false);
          setCurrentUser(null);
        }}
        title={currentUser ? "Edit User" : "Create User"}
      >
        <UserForm 
          user={currentUser}
          onSubmit={currentUser ? handleUpdateUser : handleCreateUser}
          onCancel={() => {
            setIsModalOpen(false);
            setCurrentUser(null);
          }}
        />
      </Modal>
      
      <Modal
        isOpen={isResetPasswordModalOpen}
        onClose={() => {
          setIsResetPasswordModalOpen(false);
          setUserToResetPassword(null);
        }}
        title="Reset Password"
        size="small"
      >
        <ResetPasswordForm 
          user={userToResetPassword}
          onSubmit={handleResetPassword}
          onCancel={() => {
            setIsResetPasswordModalOpen(false);
            setUserToResetPassword(null);
          }}
        />
      </Modal>
    </div>
  );
};

// UserForm and ResetPasswordForm components remain the same as in your original code
// Only change needed is to ensure the role field matches the backend's 'type' field

export default Users;