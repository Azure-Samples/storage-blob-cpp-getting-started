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
#include "queue_advanced.h"

using namespace azure::storage;

queue_advanced::queue_advanced()
{
}


queue_advanced::~queue_advanced()
{
}

///
/// This sample shows how to list all the queues in the storage account.
///
void queue_advanced::list_queues(utility::string_t connection_string)
{
	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(connection_string);

	cloud_queue_client queue_client = storage_account.create_cloud_queue_client();

	std::wcout << U("Listing all the available queues") << std::endl;
	queue_result_iterator end_of_results;
	for (auto it = queue_client.list_queues(); it != end_of_results; ++it)
	{
		std::wcout << U("Queue ") << it->name() << ", URI = " << it->uri().primary_uri().to_string() << std::endl;
	}
}

///
/// This sample shows how to set cors rules for the queue service so it can be accessed from a different domain in a web browser.
///
void queue_advanced::set_cors_rules(utility::string_t connection_string)
{
	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(connection_string);

	cloud_queue_client queue_client = storage_account.create_cloud_queue_client();

	std::wcout << U("Setting cors rules for account") << std::endl;
	try
	{
		service_properties service_properties = queue_client.download_service_properties();

		service_properties::cors_rule cors_rules = service_properties::cors_rule();
		cors_rules.allowed_origins().push_back(U("*"));
		cors_rules.allowed_methods().push_back(U("POST"));
		cors_rules.allowed_methods().push_back(U("GET"));
		cors_rules.allowed_headers().push_back(U("*"));
		cors_rules.set_max_age(std::chrono::seconds(3600));

		service_properties.cors().push_back(cors_rules);

		service_properties_includes includes;
		includes.set_cors(true);

		queue_client.upload_service_properties(service_properties, includes);
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The cors rules could not be set.") << std::endl;
	}
}