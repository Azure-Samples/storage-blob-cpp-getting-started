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
#include "queue_basic.h"
#include "queue_advanced.h"
#include "table_basic.h"
#include "table_advanced.h"

void run_storage_blob_samples(utility::string_t storage_connection_string);
void run_storage_queue_samples(utility::string_t storage_connection_string);
void run_storage_table_samples(utility::string_t storage_connection_string);

int main()
{
	// *************************************************************************************************************************
	// Instructions: This sample can be run using either the Azure Storage Emulator that installs as part of this SDK - or by
	// updating the storage_connection_string with your AccountName and Key. 
	// 
	// To run the sample using the Storage Emulator (default option)
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

	run_storage_blob_samples(storage_connection_string);

	run_storage_queue_samples(storage_connection_string);

	run_storage_table_samples(storage_connection_string);

    return 0;
}

///
/// Azure Storage Blob Sample - Demonstrate how to use the Blob Storage service.
/// Blob storage stores unstructured data such as text, binary data, documents or media files.
/// Blobs can be accessed from anywhere in the world via HTTP or HTTPS.
///
void run_storage_blob_samples(utility::string_t storage_connection_string)
{
	// basic operations with block blobs
	blob_basic::block_blob_operations(storage_connection_string);

	//basic operations with append blobs
	blob_basic::append_blob_operations(storage_connection_string);

	// list containers
	blob_advanced::list_containers(storage_connection_string);

	// copy blobs
	blob_advanced::copy_blob(storage_connection_string);

	// file upload with blocks
	blob_advanced::file_upload_with_blocks(storage_connection_string);

	// lease blob for exclusive access
	blob_advanced::lease_blob(storage_connection_string);

	// lease container for exclusive access
	blob_advanced::lease_container(storage_connection_string);

	// page blob operations
	blob_advanced::page_blob_operations(storage_connection_string);

	// set cors rules for the blob service
	blob_advanced::set_cors_rules(storage_connection_string);
	
}

/// 
/// The Queue Service provides reliable messaging for workflow processing and for communication
/// between loosely coupled components of cloud services.This sample demonstrates how to perform common tasks including
/// inserting, peeking, getting and deleting queue messages, as well as creating and deleting queues.
///
void run_storage_queue_samples(utility::string_t storage_connection_string)
{
	// basic operations with queues
	queue_basic::queue_operations(storage_connection_string);

	//list queues in account
	queue_advanced::list_queues(storage_connection_string);

	// set cors rules
	queue_advanced::set_cors_rules(storage_connection_string);
}

/// 
/// Demonstrates how to perform common tasks using the Microsoft Azure Table storage including creating a table, 
/// CRUD operations, batch operations and different querying techniques.
///
void run_storage_table_samples(utility::string_t storage_connection_string)
{
	// basic operations with tables
	table_basic::table_operations(storage_connection_string);

	// list tables in account
	table_advanced::list_tables(storage_connection_string);
}

