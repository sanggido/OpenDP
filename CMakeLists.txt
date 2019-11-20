cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(OpenDP)

set(THREADS_PREFER_PTHREAD_FLAG ON)

set(OPENDP_HOME ${PROJECT_SOURCE_DIR})

find_package(SWIG REQUIRED)
find_package(TCL REQUIRED)

set(OPENDP_WRAP ${CMAKE_CURRENT_BINARY_DIR}/opendp_wrap.cc)

set( OPENDP_SRC
  src/assign.cpp
  src/check_legal.cpp
  src/circuit.cpp
  src/mymeasure.cpp
  src/parser.cpp
  src/parser_helper.cpp
  src/place.cpp
  src/utility.cpp

  src/defParser.cpp
  src/lefParser.cpp

  src/circuitParser.cpp
  src/opendp_external.cpp
  
  ${OPENDP_WRAP}
)

set ( OPENDP_HEADER
  src/circuit.h
  src/mymeasure.h
  src/opendp_external.h
)

add_custom_command(OUTPUT ${OPENDP_WRAP}
  COMMAND ${SWIG_EXECUTABLE} -tcl8 -c++ -o ${OPENDP_WRAP} ${OPENDP_HOME}/src/opendp.i
  WORKING_DIRECTORY ${OPENDP_HOME}/src
  DEPENDS ${OPENDP_HOME}/src/opendp.i
)

############################################################
# Libraries setting 
############################################################

add_library( opendplib
  ${OPENDP_SRC}
)

set_target_properties( opendplib 
  PROPERTIES OUTPUT_NAME "opendp"  
)

target_include_directories(opendplib 
  PUBLIC ${OPENDP_HOME}/src
  PRIVATE ${OPENROAD_HOME}/include
  )
