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
#include "table_advanced.h"

using namespace azure::storage;

table_advanced::table_advanced()
{
}


table_advanced::~table_advanced()
{
}

///
/// This sample shows how to list all the tables in the storage account
///
void table_advanced::list_tables(utility::string_t connection_string)
{
	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(connection_string);

	cloud_table_client table_client = storage_account.create_cloud_table_client();

	std::wcout << U("Listing all the available tables") << std::endl;
	table_result_iterator end_of_results;
	for (auto it = table_client.list_tables(); it != end_of_results; ++it)
	{
		std::wcout << U("Queue ") << it->name() << ", URI = " << it->uri().primary_uri().to_string() << std::endl;
	}

}