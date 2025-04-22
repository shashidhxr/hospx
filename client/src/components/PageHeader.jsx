// src/components/PageHeader.jsx
import { useState } from 'react';
import { User, Bell, Search } from 'lucide-react';

const PageHeader = ({ title, onSearch }) => {
  const [searchQuery, setSearchQuery] = useState('');

  const handleSearch = (e) => {
    e.preventDefault();
    if (onSearch) {
      onSearch(searchQuery);
    }
  };

  return (
    <header className="page-header">
      <h1>{title}</h1>
      
      <div className="header-actions">
        <form className="search-form" onSubmit={handleSearch}>
          <div className="search-input">
            <Search size={18} />
            <input
              type="text"
              placeholder="Search..."
              value={searchQuery}
              onChange={(e) => setSearchQuery(e.target.value)}
            />
          </div>
        </form>
        
        <div className="notification-icon">
          <Bell size={20} />
          <span className="badge">3</span>
        </div>
        
        <div className="user-profile">
          <div className="avatar">
            <User size={20} />
          </div>
        </div>
      </div>
    </header>
  );
};

export default PageHeader;
