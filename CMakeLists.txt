cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(OpenDP)

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(THREADS_PREFER_PTHREAD_FLAG ON)


set(OPENDP_HOME ${PROJECT_SOURCE_DIR} )
set(DEFLIB_HOME
  ${PROJECT_SOURCE_DIR}/module/def/5.8-p027
)
set(LEFLIB_HOME
  ${PROJECT_SOURCE_DIR}/module/lef/5.8-p027
)

add_subdirectory(${DEFLIB_HOME})
add_subdirectory(${LEFLIB_HOME})

set( OPENDP_SRC
  src/assign.cpp
  src/check_legal.cpp
  src/main.cpp
  src/mymeasure.cpp
  src/parser.cpp
  src/parser_helper.cpp
  src/place.cpp
  src/utility.cpp

  src/defParser.cpp
  src/lefParser.cpp

  src/circuitParser.cpp
)

set ( OPENDP_HEADER
  src/circuit.h
  src/mymeasure.h
)

############################################################
# Include directories
############################################################

include_directories (
  ${LEFLIB_HOME}/lef
  ${LEFLIB_HOME}/lefzlib
  ${LEFLIB_HOME}/clef
  ${LEFLIB_HOME}/clefzlib

  ${DEFLIB_HOME}/def
  ${DEFLIB_HOME}/defzlib
  ${DEFLIB_HOME}/cdef
  ${DEFLIB_HOME}/cdefzlib
)

############################################################
# Libraries setting 
############################################################

add_library( opendplib STATIC
  ${OPENDP_SRC}
)

set_target_properties( opendplib 
  PROPERTIES OUTPUT_NAME "opendp"  
)

############################################################
# Executable setting 
############################################################

add_executable( opendp
  ${OPENDP_SRC}
)

target_link_libraries( opendp
  PUBLIC
  def
  defzlib
  cdef
  cdefzlib
  lef
  lefzlib
  clef
  clefzlib
)

add_dependencies( opendp def )
add_dependencies( opendp lef )

add_dependencies( opendplib def )
add_dependencies( opendplib lef )
############################################################
# Install  
############################################################

install(FILES ${OPENDP_HEADER} DESTINATION include)
install(TARGETS opendp DESTINATION bin)
install(TARGETS opendplib DESTINATION lib)
