PROJECT(Skel Fortran)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# configure Fortran compiler
GET_FILENAME_COMPONENT(Fortran_COMPILER_NAME ${CMAKE_Fortran_COMPILER} NAME)

IF (Fortran_COMPILER_NAME MATCHES "f95")
    SET(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -Wall") 
ENDIF()

MESSAGE(STATUS "Fortran_COMPILER_NAME        : " ${Fortran_COMPILER_NAME})
MESSAGE(STATUS "CMAKE_Fortran_FLAGS          : " ${CMAKE_Fortran_FLAGS})
MESSAGE(STATUS "CMAKE_Fortran_FLAGS_RELEASE  : " ${CMAKE_Fortran_FLAGS_RELEASE})
MESSAGE(STATUS "CMAKE_Fortran_FLAGS_DEBUG    : " ${CMAKE_Fortran_FLAGS_DEBUG})

file(GLOB SRCS *.f90)      # .f90/.f95 => free format | .f/.for => fixed
ADD_EXECUTABLE(skelfortran ${SRCS}) 