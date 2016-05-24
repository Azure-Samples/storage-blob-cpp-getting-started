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
#include "blob_advanced.h"

using namespace azure::storage;

blob_advanced::blob_advanced()
{
}

blob_advanced::~blob_advanced()
{
}

///
/// This sample shows how to list all the available containers in an storage account.
///
void blob_advanced::list_containers(utility::string_t connection_string)
{
	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(connection_string);

	cloud_blob_client blob_client = storage_account.create_cloud_blob_client();

	std::wcout << U("Listing all the available containers") << std::endl;
	container_result_iterator end_of_results;
	for (auto it = blob_client.list_containers(); it != end_of_results; ++it)
	{
		std::wcout << U("Container, Name = ") << it->name() << ", URI = " << it->uri().primary_uri().to_string() << std::endl;
	}
}

///
/// This sample shows how to set cors rules for the blob service so it can be accessed from a different domain in a web browser.
///
void blob_advanced::set_cors_rules(utility::string_t connection_string)
{
	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(connection_string);

	cloud_blob_client blob_client = storage_account.create_cloud_blob_client();

	std::wcout << U("Setting cors rules for account") << std::endl;
	try
	{
		service_properties service_properties = blob_client.download_service_properties();

		service_properties::cors_rule cors_rules = service_properties::cors_rule();
		cors_rules.allowed_origins().push_back(U("*"));
		cors_rules.allowed_methods().push_back(U("POST"));
		cors_rules.allowed_methods().push_back(U("GET"));
		cors_rules.allowed_headers().push_back(U("*"));
		cors_rules.set_max_age(std::chrono::seconds(3600));

		service_properties.cors().push_back(cors_rules);

		service_properties_includes includes;
		includes.set_cors(true);

		blob_client.upload_service_properties(service_properties, includes);
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The cors rules could not be set.") << std::endl;
	}

}

///
/// This sample shows how a lease can be acquired on a blob for exclusive access.
///
void blob_advanced::lease_blob(utility::string_t connection_string)
{
	// Generate unique container name
	utility::string_t container_name = U("sample-lease-container-") + utility::uuid_to_string(utility::new_uuid());

	std::wcout << U("Creating container") << std::endl;

	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(connection_string);

	cloud_blob_client blob_client = storage_account.create_cloud_blob_client();

	// Get a reference to the container
	cloud_blob_container container = blob_client.get_container_reference(container_name);
	try
	{
		// Create the container if it does not exist yet
		container.create_if_not_exists();
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".If you are running with the default configuration, make sure the storage emulator is started.") << std::endl;
	}

	try
	{
		std::wcout << U("Creating blob") << std::endl;

		cloud_block_blob block_blob = container.get_block_blob_reference(U("exclusive"));
		block_blob.upload_text(U("Blob created"));

		utility::string_t lease = block_blob.acquire_lease(lease_time(std::chrono::seconds(15)), U(""));

		access_condition access = access_condition::generate_lease_condition(lease);

		blob_request_options options;
		operation_context context;

		try
		{
			std::wcout << U("Trying to update blob without lease") << std::endl;

			// This fails, no access condition with lease is used
			block_blob.upload_text(U("Blob updated"));
		}
		catch (const std::exception& e)
		{
			std::wcout << U("Error:") << e.what() << U(". Tried to update blob without a lease ") << std::endl;
		}

		std::wcout << U("Trying to update blob with a lease ") << lease << std::endl;

		block_blob.upload_text(U("Blob updated"), access, options, context);

		std::wcout << U("Blob updated successfully") << std::endl;

		std::wcout << U("Releasing lease") << std::endl;

		block_blob.release_lease(access);
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The blob could not be updated.") << std::endl;
	}

}

///
/// This sample shows how a lease can be acquired on a container for exclusive access.
///
void blob_advanced::lease_container(utility::string_t connection_string)
{
	// Generate unique container name
	utility::string_t container_name = U("sample-lease-container-") + utility::uuid_to_string(utility::new_uuid());

	std::wcout << U("Creating container") << std::endl;

	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(connection_string);

	cloud_blob_client blob_client = storage_account.create_cloud_blob_client();

	// Get a reference to the container
	cloud_blob_container container = blob_client.get_container_reference(container_name);
	try
	{
		// Create the container if it does not exist yet
		container.create_if_not_exists();
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".If you are running with the default configuration, make sure the storage emulator is started.") << std::endl;
	}

	try
	{
		std::wcout << U("Acquiring lease for container") << std::endl;

		utility::string_t lease = container.acquire_lease(lease_time(std::chrono::seconds(15)), U(""));

		try
		{
			std::wcout << U("Trying to container without lease") << std::endl;

			// This fails, no access condition with lease is used
			container.delete_container();
		}
		catch (const std::exception& e)
		{
			std::wcout << U("Error:") << e.what() << U(". Tried to delete blob without a lease ") << std::endl;
		}

		access_condition access = access_condition::generate_lease_condition(lease);

		blob_request_options options;
		operation_context context;

		std::wcout << U("Trying to delete container with a lease ") << lease << std::endl;

		container.delete_container(access, options, context);

		std::wcout << U("Container deleted successfully") << std::endl;

		std::wcout << U("Releasing lease") << std::endl;
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The container could not be deleted.") << std::endl;
	}

}

///
/// This sample shows how to copy a blob from one location to another and how to cancel an existing copy operation.
///
void blob_advanced::copy_blob(utility::string_t connection_string)
{
	// Generate unique container name
	utility::string_t container_name = U("sample-copy-container-") + utility::uuid_to_string(utility::new_uuid());

	std::wcout << U("Creating container") << std::endl;

	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(connection_string);

	cloud_blob_client blob_client = storage_account.create_cloud_blob_client();

	// Get a reference to the container
	cloud_blob_container container = blob_client.get_container_reference(container_name);
	try
	{
		// Create the container if it does not exist yet
		container.create_if_not_exists();
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".If you are running with the default configuration, make sure the storage emulator is started.") << std::endl;
	}

	try
	{
		std::wcout << U("Creating blob") << std::endl;

		cloud_block_blob source_blob = container.get_block_blob_reference(U("source"));
		source_blob.upload_text(U("Blob created"));

		std::wcout << U("Copying blob") << std::endl;
		cloud_block_blob target_blob = container.get_block_blob_reference(U("target"));

		utility::string_t copy_id = target_blob.start_copy(source_blob);

		std::wcout << U("Checking copy state") << std::endl;
		copy_state state = target_blob.copy_state();

		utility::string_t status;

		switch (state.status())
		{
		case copy_status::aborted:
			status = U("aborted");
			break;
		case copy_status::failed:
			status = U("failed");
			break;
		case copy_status::invalid:
			status = U("invalid");
			break;
		case copy_status::pending:
			status = U("pending");
			break;
		case copy_status::success:
			status = U("success");
			break;
		}

		std::wcout << U("Status ") << status << std::endl;

		if (state.status() == copy_status::pending)
		{
			std::wcout << U("Aborting copy") << std::endl;

			target_blob.abort_copy(copy_id);
		}

		std::wcout << U("Deleting container") << std::endl;
		container.delete_container();

	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The blob could not be copied.") << std::endl;
	}

}

///
/// This sample shows how to upload and commit a batch of blocks with data in a block blob.
///
void blob_advanced::file_upload_with_blocks(utility::string_t connection_string) 
{
	const utility::string_t image_file(U("HelloWorld.png"));

	// Generate unique container name
	utility::string_t container_name = U("blobblockdemocontainer-") + utility::uuid_to_string(utility::new_uuid());

	std::wcout << U("Creating container") << std::endl;

	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(connection_string);

	cloud_blob_client blob_client = storage_account.create_cloud_blob_client();

	// Get a reference to the container
	cloud_blob_container container = blob_client.get_container_reference(container_name);
	try
	{
		// Create the container if it does not exist yet
		container.create_if_not_exists();
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".If you are running with the default configuration, make sure the storage emulator is started.") << std::endl;
	}

	std::wcout << U("Creating blob") << std::endl;

	cloud_block_blob block_blob = container.get_block_blob_reference(image_file);

	std::vector<block_list_item> blocks;

	std::wcout << U("Creating block id") << std::endl;

	utility::string_t block_id = utility::uuid_to_string(utility::new_uuid());
	std::vector<unsigned char> block_id_as_array(block_id.cbegin(), block_id.cend());
	utility::string_t encoded_block_id(utility::conversions::to_base64(block_id_as_array));

	concurrency::streams::istream file_stream = concurrency::streams::file_stream<uint8_t>::open_istream(image_file).get();

	try
	{
		std::wcout << U("Pushing file content in block with id ") << encoded_block_id << std::endl;

		block_blob.upload_block(encoded_block_id, file_stream, utility::string_t(U("")));
		blocks.push_back(block_list_item(encoded_block_id));

		std::wcout << U("Commiting block with id ") << encoded_block_id << std::endl;
		block_blob.upload_block_list(blocks);
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << ". The file could not be uploaded" << std::endl;
	}

	file_stream.close();

	std::wcout << U("Enumerating block list") << std::endl;

	blocks = block_blob.download_block_list();

	std::vector<block_list_item>::iterator it;

	for (it = blocks.begin(); it < blocks.end(); it++)
	{
		std::wcout << U("Block id: ") << it->id() << std::endl;
	}

	std::wcout << U("Deleting container") << std::endl;

	try
	{
		container.delete_container();
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << ". The container could not be deleted" << std::endl;
	}

}

///
/// This sample shows the usage of a page blob. 
/// A file in disk is splitted in several pages and uploaded to the storage using a page blob.
///
void blob_advanced::page_blob_operations(utility::string_t connection_string)
{
	const utility::string_t image_file(U("HelloWorld.png"));
	const int page_size = 1024;

	// Generate unique container name
	utility::string_t container_name = U("blobpagedemocontainer") + utility::uuid_to_string(utility::new_uuid());

	std::wcout << U("Creating container") << std::endl;

	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(connection_string);

	cloud_blob_client blob_client = storage_account.create_cloud_blob_client();

	// Get a reference to the container
	cloud_blob_container container = blob_client.get_container_reference(container_name);
	try
	{
		// Create the container if it does not exist yet
		container.create_if_not_exists();
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".If you are running with the default configuration, make sure the storage emulator is started.") << std::endl;
	}

	std::wcout << U("Creating blob") << std::endl;

	cloud_page_blob page_blob = container.get_page_blob_reference(image_file);
	page_blob.create(1024 * 1024);

	std::ifstream file;
	std::vector<char> vector_buffer;
	int read_buffer[page_size];
	int index = 0;

	vector_buffer.reserve(page_size);

	std::wcout << U("Uploading file content in pages. Every page must be a multiple of 512 bytes") << std::endl;

	file.open(image_file, std::ios::binary);

	while (file.read((char *)&read_buffer, sizeof(read_buffer)))
	{
		vector_buffer.insert(vector_buffer.end(), &read_buffer[0], &read_buffer[page_size]);

		concurrency::streams::istream page_stream = concurrency::streams::bytestream::open_istream(vector_buffer);
		azure::storage::page_range range(index * page_size, index * page_size + page_size - 1);

		try
		{
			page_blob.upload_pages(page_stream, range.start_offset(), utility::string_t(U("")));

		}

		catch (const std::exception& e)
		{
			std::wcout << U("Error:") << e.what() << std::endl;
		}

		vector_buffer.clear();
		index++;
	}

	file.close();

	std::wcout << U("Listing pages") << std::endl;

	std::vector<page_range>::iterator it;

	std::vector<page_range> pages = page_blob.download_page_ranges();

	for (it = pages.begin(); it < pages.end(); it++)
	{
		std::wcout << U("Page ") << it->start_offset() << " - " << it->end_offset() << std::endl;
	}

	std::wcout << U("Deleting container") << std::endl;

	try
	{
		container.delete_container();
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << ". The container could not be deleted" << std::endl;
	}
}