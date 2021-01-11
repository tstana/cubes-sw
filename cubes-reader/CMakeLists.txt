# File for configuring build of CubesReader
#-------------------------------------------------
# User settings
#
set(name CubesReader)
set(useROOT true)
set(buildDOC true)

#---------------------------------------------------
# Setup project

cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)
project(${name})

#---------------------------------------------------------------------------

# Find ROOT package
#
if(useROOT)
  set (CMAKE_CXX_STANDARD 11)  
  find_package(ROOT)
  if (NOT ROOT_FOUND)
      message(FATAL_ERROR "ROOT was not found but is needed")
  endif ()
  include_directories(${ROOT_INCLUDE_DIRS})
endif(useROOT)

#----------------------------------------------------------------------------
# Enabling build of doxygen
#
if (buildDOC)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        add_custom_target(docs
            COMMAND mkdir -p ${CMAKE_BINARY_DIR}/docs/
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen. See ./docs/index.html"
            VERBATIM)
    else (DOXYGEN_FOUND)
        message("Doxygen needs to be installed to generate the documentation")
    endif (DOXYGEN_FOUND)
endif(buildDOC)

#----------------------------------------------------------------------------
# Find BOOST package 
#

if(useBOOST)
    set(BOOST_LIBS filesystem system program_options)
    set(Boost_USE_MULTITHREADED ON)
    find_package(Boost 1.68.0 COMPONENTS ${BOOST_LIBS})
    if (NOT Boost_FOUND)
       message(FATAL_ERROR "Boost was not found but is needed")
    endif ()
    include_directories(${Boost_INCLUDE_DIRS})
endif(useBOOST)


#------------------------------------------------------------------------------
# Add verbosity macro to a debug build
#
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDBGLEV=3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDBGLEV=3")

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/src/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(${name} ${sources} ${headers})
target_link_libraries(${name} ${ROOT_LIBRARIES})

