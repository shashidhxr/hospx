cmake_minimum_required(VERSION 3.10)
project(hospx)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(SQLite3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

# Include directories
include_directories(
    include
    api
    ${SQLite3_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    lib/crow/include
)

# Source files
set(SOURCES
    src/main.cpp
    # src/api_server.cpp
    src/database_handler.cpp
    src/user.cpp
    src/patient.cpp
    src/doctor.cpp
    src/receptionist.cpp
    src/admin.cpp
    src/appointment.cpp
    src/record.cpp
    src/report.cpp
)

# Create executable
add_executable(hospx ${SOURCES})

# Link libraries
target_link_libraries(hospx
    ${SQLite3_LIBRARIES}
    ${Boost_LIBRARIES}
)