//----------------------------------------------------------------------------------
// Microsoft Developer & Platform Evangelism
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
// EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
// OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//----------------------------------------------------------------------------------
// The example companies, organizations, products, domain names,
// e-mail addresses, logos, people, places, and events depicted
// herein are fictitious.  No association with any real company,
// organization, product, domain name, email address, logo, person,
// places, or events is intended or should be inferred.
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "blob_basic.h"
#include "blob_advanced.h"

using namespace azure::storage;

void run_storage_blob_samples(utility::string_t storage_connection_string);

int main()
{
  // *************************************************************************************************************************
  // Instructions: This sample can be run using either the Azure Storage Emulator that installs as part of the Windows Azure SDK (in Windows only) - or by
  // updating the storage_connection_string with your AccountName and Key. 
  // 
  // To run the sample using the Storage Emulator (Windows Azure SDK)
  //      1. Start the Azure Storage Emulator (once only) by pressing the Start button or the Windows key and searching for it
  //         by typing "Azure Storage Emulator". Select it from the list of applications to start it.
  //      2. Set breakpoints and run the project using F10. 
  // 
  // To run the sample using the Storage Service
  //      1. Update the storage_connection_string variable for the emulator (UseDevelopmentStorage=True) and
  //         replace it with the storage_connection_string for the storage service (AccountName=[]...)
  //      2. Create a Storage Account through the Azure Portal and provide your [AccountName] and [AccountKey] in 
  //         the App.Config file. See http://go.microsoft.com/fwlink/?LinkId=325277 for more information
  //      3. Set breakpoints and run the project using F10. 
  // 
  // *************************************************************************************************************************

  const utility::string_t storage_connection_string(U("UseDevelopmentStorage=true"));

  try
  {
    run_storage_blob_samples(storage_connection_string);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("Unexpected exception while running the sample.") << std::endl;
  }

  return 0;
}

///
/// Azure Storage Blob Sample - Demonstrate how to use the Blob Storage service.
/// Blob storage stores unstructured data such as text, binary data, documents or media files.
/// Blobs can be accessed from anywhere in the world via HTTP or HTTPS.
///
void run_storage_blob_samples(utility::string_t storage_connection_string)
{
  // Parse the connection string
  cloud_storage_account storage_account = cloud_storage_account::parse(storage_connection_string);

  cloud_blob_client blob_client = storage_account.create_cloud_blob_client();

  // basic operations with block blobs
  blob_basic::block_blob_operations(blob_client);

  //basic operations with append blobs
  blob_basic::append_blob_operations(blob_client);

  // list containers
  blob_advanced::list_containers(blob_client);

  // copy blobs
  blob_advanced::copy_blob(blob_client);

  // file upload with blocks
  blob_advanced::file_upload_with_blocks(blob_client);

  // lease blob for exclusive access
  blob_advanced::lease_blob(blob_client);

  // lease container for exclusive access
  blob_advanced::lease_container(blob_client);

  // page blob operations
  blob_advanced::page_blob_operations(blob_client);

  // set cors rules for the blob service
  blob_advanced::set_cors_rules(blob_client);

  // set service properties for the blob service
  blob_advanced::set_service_properties(blob_client);

  // set container and blob metadata and properties
  blob_advanced::set_metadata_and_properties(blob_client);

  // set container permissions
  blob_advanced::set_container_acl(blob_client);
}
