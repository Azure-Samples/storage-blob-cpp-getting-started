# FindCasablanca package
#
# Tries to find the Azure Storage library
#

find_package(PkgConfig)

include(LibFindMacros)

# Include dir
find_path(AZURESTORAGE_INCLUDE_DIR
  NAMES
    was/storage_account.h
  PATHS 
    ${AZURESTORAGE_PKGCONF_INCLUDE_DIRS}
    ${AZURESTORAGE_DIR}
    $ENV{AZURESTORAGE_DIR}
    /usr/local/include
    /usr/include
    ../../azure-storage-cpp
  PATH_SUFFIXES 
    Microsoft.WindowsAzure.Storage/includes
    includes
)

# Library
find_library(AZURESTORAGE_LIBRARY
  NAMES 
    azurestorage
  PATHS 
    ${AZURESTORAGE_PKGCONF_LIBRARY_DIRS}
    ${AZURESTORAGE_DIR}
    ${AZURESTORAGE_DIR}
    $ENV{AZURESTORAGE_DIR}
    /usr/local
    /usr
    ../../azure-storage-cpp
  PATH_SUFFIXES
    lib
    Microsoft.WindowsAzure.Storage/build.release/Binaries/
    build.release/Binaries/
)

set(AZURESTORAGE_PROCESS_LIBS AZURESTORAGE_LIBRARY)
set(CASABLANCA_PROCESS_INCLUDES AZURESTORAGE_INCLUDE_DIR)

libfind_process(AZURESTORAGE)

