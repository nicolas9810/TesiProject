
cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Imposta il percorso della cartella degli header
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${PROFPGA}/include/)  # Include directory specificata

# Trova i file sorgente nella cartella src
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)




# Aggiungi l'eseguibile
add_executable(gestioneEnergetica ${SOURCES})

# # Aggiungi il percorso della libreria modbus
# link_directories(/usr/local/lib)

target_link_libraries(gestioneEnergetica PUBLIC modbus)
target_link_libraries(gestioneEnergetica PRIVATE Threads::Threads)
target_include_directories(gestioneEnergetica PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(gestioneEnergetica PRIVATE ${Boost_LIBRARIES})
# Collega le librerie specifiche del profpga


message(STATUS "Searching modbus.h....")
find_path(MODBUS_INCLUDE NAMES modbus/modbus.h)
if(NOT MODBUS_INCLUDE)
    message(FATAL_ERROR "libmodbus headers not found")
endif(NOT MODBUS_INCLUDE)
message(STATUS "-> Found " ${MODBUS_INCLUDE}/modbus/modbus.h)

target_include_directories(gestioneEnergetica PUBLIC "${MODBUS_INCLUDE}/modbus/")

# Aggiungi le opzioni del compilatore per il debugging
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

