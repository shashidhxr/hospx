// src/components/DataTable.jsx
import { useState } from 'react';
import { ChevronLeft, ChevronRight, Filter, Download, Trash2, Edit, Eye, Search } from 'lucide-react';

const DataTable = ({ 
  columns, 
  data, 
  onView, 
  onEdit, 
  onDelete, 
  onExport, 
  searchable = true,
  pagination = true 
}) => {
  const [currentPage, setCurrentPage] = useState(1);
  const [searchQuery, setSearchQuery] = useState('');
  const [sortField, setSortField] = useState('');
  const [sortDirection, setSortDirection] = useState('asc');
  const [selectedRows, setSelectedRows] = useState([]);
  
  const itemsPerPage = 10;

  // Handle search
  const filteredData = data.filter(item => {
    if (!searchQuery) return true;
    return Object.values(item).some(value => 
      value && value.toString().toLowerCase().includes(searchQuery.toLowerCase())
    );
  });

  // Handle sorting
  const sortedData = [...filteredData].sort((a, b) => {
    if (!sortField) return 0;
    
    const fieldA = a[sortField];
    const fieldB = b[sortField];
    
    if (fieldA < fieldB) return sortDirection === 'asc' ? -1 : 1;
    if (fieldA > fieldB) return sortDirection === 'asc' ? 1 : -1;
    return 0;
  });

  // Handle pagination
  const totalPages = Math.ceil(sortedData.length / itemsPerPage);
  const startIndex = (currentPage - 1) * itemsPerPage;
  const paginatedData = pagination 
    ? sortedData.slice(startIndex, startIndex + itemsPerPage)
    : sortedData;

  const handleSort = (field) => {
    if (sortField === field) {
      setSortDirection(sortDirection === 'asc' ? 'desc' : 'asc');
    } else {
      setSortField(field);
      setSortDirection('asc');
    }
  };

  const handleRowSelect = (id) => {
    if (selectedRows.includes(id)) {
      setSelectedRows(selectedRows.filter(rowId => rowId !== id));
    } else {
      setSelectedRows([...selectedRows, id]);
    }
  };

  const handleSelectAll = () => {
    if (selectedRows.length === paginatedData.length) {
      setSelectedRows([]);
    } else {
      setSelectedRows(paginatedData.map(item => item.id || item._id));
    }
  };

  return (
    <div className="data-table-container">
      <div className="table-toolbar">
        {searchable && (
          <div className="search-box">
            <input
              type="text"
              placeholder="Search..."
              value={searchQuery}
              onChange={(e) => setSearchQuery(e.target.value)}
            />
            <Search size={18} />
          </div>
        )}
        
        <div className="table-actions">
          <button className="icon-button">
            <Filter size={18} />
            <span>Filter</span>
          </button>
          
          <button className="icon-button" onClick={onExport}>
            <Download size={18} />
            <span>Export</span>
          </button>
        </div>
      </div>
      
      <div className="table-responsive">
        <table className="data-table">
          <thead>
            <tr>
              <th className="checkbox-cell">
                <input 
                  type="checkbox" 
                  checked={selectedRows.length === paginatedData.length && paginatedData.length > 0} 
                  onChange={handleSelectAll}
                />
              </th>
              {columns.map(column => (
                <th 
                  key={column.field} 
                  onClick={() => column.sortable !== false && handleSort(column.field)}
                  className={column.sortable !== false ? 'sortable' : ''}
                >
                  {column.header}
                  {sortField === column.field && (
                    <span className={`sort-icon ${sortDirection}`}>▲</span>
                  )}
                </th>
              ))}
              <th className="actions-cell">Actions</th>
            </tr>
          </thead>
          <tbody>
            {paginatedData.length > 0 ? (
              paginatedData.map((row, index) => (
                <tr key={row.id || row._id || index}>
                  <td className="checkbox-cell">
                    <input 
                      type="checkbox" 
                      checked={selectedRows.includes(row.id || row._id)} 
                      onChange={() => handleRowSelect(row.id || row._id)}
                    />
                  </td>
                  {columns.map(column => (
                    <td key={`${row.id}-${column.field}`}>
                      {column.render ? column.render(row) : row[column.field]}
                    </td>
                  ))}
                  <td className="actions-cell">
                    {onView && (
                      <button 
                        className="icon-button small" 
                        onClick={() => onView(row.id || row._id)}
                      >
                        <Eye size={16} />
                      </button>
                    )}
                    {onEdit && (
                      <button 
                        className="icon-button small" 
                        onClick={() => onEdit(row.id || row._id)}
                      >
                        <Edit size={16} />
                      </button>
                    )}
                    {onDelete && (
                      <button 
                        className="icon-button small danger" 
                        onClick={() => onDelete(row.id || row._id)}
                      >
                        <Trash2 size={16} />
                      </button>
                    )}
                  </td>
                </tr>
              ))
            ) : (
              <tr>
                <td colSpan={columns.length + 2} className="no-data">
                  No data available
                </td>
              </tr>
            )}
          </tbody>
        </table>
      </div>
      
      {pagination && totalPages > 1 && (
        <div className="pagination">
          <button 
            className="pagination-btn" 
            disabled={currentPage === 1}
            onClick={() => setCurrentPage(prev => Math.max(prev - 1, 1))}
          >
            <ChevronLeft size={16} />
          </button>
          
          <div className="pagination-info">
            Page {currentPage} of {totalPages}
          </div>
          
          <button 
            className="pagination-btn" 
            disabled={currentPage === totalPages}
            onClick={() => setCurrentPage(prev => Math.min(prev + 1, totalPages))}
          >
            <ChevronRight size={16} />
          </button>
        </div>
      )}
    </div>
  );
};

export default DataTable;
