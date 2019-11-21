cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(OpenDP)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(SWIG REQUIRED)
find_package(TCL REQUIRED)

set(OPENDP_WRAP ${CMAKE_CURRENT_BINARY_DIR}/opendp_wrap.cc)
set(OPENDP_TCL_INIT ${CMAKE_CURRENT_BINARY_DIR}/OpendpTclInitVar.cc)

add_custom_command(OUTPUT ${OPENDP_WRAP}
  COMMAND ${SWIG_EXECUTABLE} -tcl8 -c++ -I${OPENDP_HOME}/include -o ${OPENDP_WRAP} ${OPENDP_HOME}/src/opendp.i
  WORKING_DIRECTORY ${OPENDP_HOME}/src
  DEPENDS ${OPENDP_HOME}/src/opendp.i
)

add_custom_command(OUTPUT ${OPENDP_TCL_INIT}
  COMMAND ${OPENSTA_HOME}/etc/TclEncode.tcl ${OPENDP_TCL_INIT} opendp_tcl_inits ${OPENDP_HOME}/src/opendp.tcl
  WORKING_DIRECTORY ${OPENDP_HOME}/src
  DEPENDS ${OPENDP_HOME}/src/opendp.tcl ${OPENSTA_HOME}/etc/TclEncode.tcl
  )

############################################################
# Libraries setting 
############################################################

add_library( opendp
  src/assign.cpp
  src/check_legal.cpp
  src/circuit.cpp
  src/mymeasure.cpp
  src/parser.cpp
  src/parser_helper.cpp
  src/place.cpp
  src/utility.cpp
  src/dbToCircuit.cpp

  src/opendp_external.cpp
  src/MakeOpendp.cpp

  ${OPENDP_WRAP}
  ${OPENDP_TCL_INIT}
)

target_include_directories(opendp
  PUBLIC ${OPENDP_HOME}/include
  PRIVATE
  ${OPENDP_HOME}/src
  ${OPENROAD_HOME}/include
  ${OPENDB_HOME}/include
  ${OPENSTA_HOME}/app
 )
