set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 2.6)
project(azurestoragesamples)

set(WARNINGS)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Platform (not compiler) specific settings
if(UNIX)
  find_package(Boost REQUIRED COMPONENTS log log_setup random system thread locale regex filesystem chrono date_time)
  find_package(Threads REQUIRED)
  find_package(OpenSSL REQUIRED)
  find_package(Glibmm REQUIRED)
  find_package(LibXML++ REQUIRED)
  find_package(UUID REQUIRED)
  find_package(Casablanca REQUIRED)
  find_package(AzureStorage REQUIRED)
else()
  message("-- Unsupported Build Platform.")
endif()

# Compiler (not platform) specific settings
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  message("-- Setting gcc options")

  set(WARNINGS "-Wall -Wextra -Wunused-parameter -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winit-self -Winvalid-pch -Wmissing-format-attribute -Wmissing-include-dirs -Wpacked -Wredundant-decls -Wunreachable-code")
  set(LINUX_SUPPRESSIONS "-Wno-deprecated -Wno-unknown-pragmas -Wno-reorder -Wno-unused-function -Wno-char-subscripts -Wno-switch -Wno-unused-but-set-parameter -Wno-unused-value -Wno-unused-local-typedefs -Wno-unused-parameter")
  set(WARNINGS "${WARNINGS} ${LINUX_SUPPRESSIONS}")

  set(LD_FLAGS "${LD_FLAGS} -Wl,-z,defs")
 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-strict-aliasing")
 
  set(STRICT_CXX_FLAGS ${WARNINGS} "-Werror -pedantic")
 
  add_definitions(-DBOOST_LOG_DYN_LINK)
else()
  message("-- Unknown compiler, success is doubtful.")
endif()

# Reconfigure final output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Binaries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Binaries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Binaries)

set(AZURESTORAGESAMPLES_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${AZURESTORAGE_INCLUDE_DIR} ${CASABLANCA_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIRS} ${LibXML++_INCLUDE_DIRS} ${UUID_INCLUDE_DIRS} ${Glibmm_INCLUDE_DIRS})

set(AZURESTORAGESAMPLES_LIBRARIES ${AZURESTORAGE_LIBRARIES} ${CASABLANCA_LIBRARIES} ${Boost_LIBRARIES} ${Boost_FRAMEWORK} ${OPENSSL_LIBRARIES} ${LibXML++_LIBRARIES} ${UUID_LIBRARIES} ${Glibmm_LIBRARIES})

include_directories(. ${AZURESTORAGESAMPLES_INCLUDE_DIRS})

add_executable(azurestoragesamples storage-getting-started.cpp
     stdafx.cpp
     string_util.cpp
     blob_basic.cpp
     blob_advanced.cpp)
target_link_libraries(azurestoragesamples ${AZURESTORAGESAMPLES_LIBRARIES})

file(COPY HelloWorld.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
