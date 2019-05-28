cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(RePlAce)

set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(THREADS_PREFER_PTHREAD_FLAG ON)

set( OPENDP_SRC
  src/assign.cpp
  src/check_legal.cpp
  src/main.cpp
  src/mymeasure.cpp
  src/parser.cpp
  src/parser_helper.cpp
  src/place.cpp
  src/utility.cpp
)

set ( OPENDP_HEADER
  src/circuit.h
  src/mymeasure.h
)

add_library( opendplib STATIC
  ${OPENDP_SRC}
)

set_target_properties( opendplib 
  PROPERTIES OUTPUT_NAME "opendp"  
)

add_executable( opendp
  ${OPENDP_SRC}
)

target_link_libraries( opendp
  -static-libstdc++  
)

############################################################
# Install  
############################################################

install(FILES ${OPENDP_HEADER} DESTINATION include)
install(TARGETS opendp DESTINATION bin)
install(TARGETS opendplib DESTINATION lib)
