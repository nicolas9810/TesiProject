
cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Imposta il percorso della cartella degli header
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Trova i file sorgente nella cartella src
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Aggiungi l'eseguibile
add_executable(gestioneEnergetica ${SOURCES})

# # Aggiungi il percorso della libreria modbus
# link_directories(/usr/local/lib)

target_link_libraries(gestioneEnergetica PUBLIC modbus)


message(STATUS "Searching modbus.h....")
find_path(MODBUS_INCLUDE NAMES modbus/modbus.h)
if(NOT MODBUS_INCLUDE)
    message(FATAL_ERROR "libmodbus headers not found")
endif(NOT MODBUS_INCLUDE)
message(STATUS "-> Found " ${MODBUS_INCLUDE}/modbus/modbus.h)

target_include_directories(gestioneEnergetica PUBLIC "${MODBUS_INCLUDE}/modbus/")


