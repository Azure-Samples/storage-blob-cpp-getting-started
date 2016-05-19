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

/// 
/// Azure Storage Blob Sample - Demonstrate how to use the Blob Storage service. 
/// Blob storage stores unstructured data such as text, binary data, documents or media files. 
/// Blobs can be accessed from anywhere in the world via HTTP or HTTPS.
///
/// Documentation References: 
/// - What is a Storage Account - http://azure.microsoft.com/en-us/documentation/articles/storage-whatis-account/
/// - Getting Started with Blobs - http://azure.microsoft.com/en-us/documentation/articles/storage-dotnet-how-to-use-blobs/
/// - Blob Service Concepts - http://msdn.microsoft.com/en-us/library/dd179376.aspx 
/// - Blob Service REST API - http://msdn.microsoft.com/en-us/library/dd135733.aspx
/// - Blob Service C++ API - https://azure.microsoft.com/en-us/documentation/articles/storage-c-plus-plus-how-to-use-blobs/
/// - Storage Emulator - http://msdn.microsoft.com/en-us/library/azure/hh403989.aspx
/// 

#include "stdafx.h"

using namespace azure::storage;

void basic_blob_operations(utility::string_t storage_connection_string);
void set_cors_rules(utility::string_t storage_connection_string);
void lease_blob(utility::string_t storage_connection_string);
void lease_container(utility::string_t storage_connection_string);
void copy_blob(utility::string_t storage_connection_string);
void upload_file_with_blocks(utility::string_t storage_connection_string);
void upload_file_with_page_blob(utility::string_t storage_connection_string);

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
	//      1. Change the storage_connection_string  for the emulator (UseDevelopmentStorage=True) and
	//         replace it with the storage_connection_string the storage service (AccountName=[]...)
	//      2. Create a Storage Account through the Azure Portal and provide your [AccountName] and [AccountKey] in 
	//         the App.Config file. See http://go.microsoft.com/fwlink/?LinkId=325277 for more information
	//      3. Set breakpoints and run the project using F10. 
	// 
	// *************************************************************************************************************************

	const utility::string_t storage_connection_string(U("UseDevelopmentStorage=true"));

	basic_blob_operations(storage_connection_string);

	set_cors_rules(storage_connection_string);

	lease_blob(storage_connection_string);

	lease_container(storage_connection_string);
	
	copy_blob(storage_connection_string);

	upload_file_with_blocks(storage_connection_string);

	upload_file_with_page_blob(storage_connection_string);

	upload_file_with_page_blob(storage_connection_string);

    return 0;
}

// Basic operations to work with block blobs
void basic_blob_operations(utility::string_t storage_connection_string)
{
	const utility::string_t image_file(U("HelloWorld.png"));

	// Generate unique containepagr name
	utility::string_t container_name = U("blobdemocontainer-") + utility::uuid_to_string(utility::new_uuid());

	std::wcout << U("Creating container") << std::endl;

	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(storage_connection_string);

	cloud_blob_client blob_client = storage_account.create_cloud_blob_client();

	// Get a reference to the container
	cloud_blob_container container = blob_client.get_container_reference(container_name);
	try
	{
		// Create the container if it does not exist yet
		container.create_if_not_exists();

		// Set the public permissions for the container
		blob_container_permissions* permissions = new blob_container_permissions();
		permissions->set_public_access(blob_container_public_access_type::blob);
		container.upload_permissions(*permissions);
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".If you are running with the default configuration, make sure the storage emulator is started.") << std::endl;
	}

	std::wcout << U("Listing all the available containers") << std::endl;
	container_result_iterator end_of_results;
	for (auto it = blob_client.list_containers(); it != end_of_results; ++it)
	{
		std::wcout << U("Container, Name = ") << it->name() << ", URI = " << it->uri().primary_uri().to_string() << std::endl;
	}

	std::wcout << U("Uploading BlockBlob") << std::endl;

	// Get a reference to a cloud block blob
	cloud_block_blob block_blob = container.get_block_blob_reference(image_file);
	try
	{
		//Push a file from disk to the cloud block blob
		concurrency::streams::istream image_stream = concurrency::streams::file_stream<uint8_t>::open_istream(image_file).get();
		block_blob.upload_from_stream(image_stream);
		image_stream.close().wait();
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The file could not uploaded.") << std::endl;
	}

	std::wcout << U("Listing all blobs and directories in container ") << std::endl;
	try
	{
		//Enumerate all the blobs in the container
		list_blob_item_iterator end_of_results;
		for (auto it = container.list_blobs(); it != end_of_results; ++it)
		{
			if (it->is_blob())
			{
				std::wcout << U("Blob: ") << it->as_blob().uri().primary_uri().to_string() << std::endl;
			}
			else
			{
				std::wcout << U("Directory: ") << it->as_directory().uri().primary_uri().to_string() << std::endl;
			}
		}
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The blobs can not be listed.") << std::endl;
	}

	std::wcout << U("Downloading blob from ") << block_blob.uri().primary_uri().to_string() << std::endl;
	try
	{
		// Pull the data from the block blob into a file on disk
		concurrency::streams::container_buffer<std::vector<uint8_t>> buffer;
		concurrency::streams::ostream output_stream(buffer);
		block_blob.download_to_stream(output_stream);

		std::ofstream outfile("copy of hello_world.png", std::ofstream::binary);
		std::vector<unsigned char>& data = buffer.collection();

		outfile.write((char *)&data[0], buffer.size());
		outfile.close();
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The file could not be downloaded.") << std::endl;
	}

	std::wcout << U("Creating a read-only snapshot of the blob") << std::endl;
	try
	{
		// Create a read-only replica or snapshot of the block blob content
		cloud_blob snapshot = block_blob.create_snapshot();
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The snapshot could not be created.") << std::endl;
	}

	std::wcout << U("Deleting block Blob and all of its snapshots") << std::endl;
	try
	{
		// Delete the block blob and all the associated snapshots
		block_blob.delete_blob(delete_snapshots_option::include_snapshots,
			access_condition::generate_empty_condition(), *new blob_request_options(), *new operation_context());

	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The snapshot could not be deleted.") << std::endl;
	}

	std::wcout << U("Uploading AppendBlob") << std::endl;
	cloud_append_blob append_blob = container.get_append_blob_reference(U("my-append-blob"));
	try
	{
		append_blob.properties().set_content_type(U("text/plain; charset=utf-8"));
		append_blob.create_or_replace();

		// Append blocks in different ways:
		// Append data from one block
		concurrency::streams::istream append_input_stream1 = concurrency::streams::bytestream::open_istream(utility::conversions::to_utf8string(U("block text.")));
		append_blob.append_block(append_input_stream1, utility::string_t());
		append_input_stream1.close().wait();

		// Append data from stream
		concurrency::streams::istream append_input_stream2 = concurrency::streams::bytestream::open_istream(utility::conversions::to_utf8string(U("stream text.")));
		append_blob.append_from_stream(append_input_stream2);
		append_input_stream2.close().wait();

		// Append data from text
		append_blob.append_text(U("more text."));
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The appendblob could not be created.") << std::endl;
	}

	std::wcout << U("Downloading AppendBlob") << std::endl;
	try
	{
		// Download all the data in append blob as text
		utility::string_t append_text = append_blob.download_text();
		ucout << U("Append Text: ") << append_text << std::endl;
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The appendblob could not be downloaded.") << std::endl;
	}

	std::wcout << U("Deleting AppendBlob") << std::endl;
	try
	{
		// Delete the blob
		append_blob.delete_blob();
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The appendblob could not be deleted.") << std::endl;
	}

	std::wcout << U("Deleting container") << std::endl;
	try
	{
		//Delete the container
		container.delete_container();
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error:") << e.what() << U(".The container could not be deleted.") << std::endl;
	}
}

// set cors rules for enabling preflight blob requests from a web browser
void set_cors_rules(utility::string_t storage_connection_string)
{
	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(storage_connection_string);

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

// creates lease for exclusive write operations in a blob
void lease_blob(utility::string_t storage_connection_string)
{
	// Generate unique container name
	utility::string_t container_name = U("blobleasedemocontainer-") + utility::uuid_to_string(utility::new_uuid());

	std::wcout << U("Creating container") << std::endl;

	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(storage_connection_string);

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

// creates lease for exclusive write operations in a blob container
void lease_container(utility::string_t storage_connection_string)
{
	// Generate unique container name
	utility::string_t container_name = U("leasedemocontainer-") + utility::uuid_to_string(utility::new_uuid());

	std::wcout << U("Creating container") << std::endl;

	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(storage_connection_string);

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

// copy a blob from one location to another
void copy_blob(utility::string_t storage_connection_string)
{
	// Generate unique container name
	utility::string_t container_name = U("copyblobdemocontainer-") + utility::uuid_to_string(utility::new_uuid());

	std::wcout << U("Creating container") << std::endl;

	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(storage_connection_string);

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

// uploads a file in a block blob using several blocks
void upload_file_with_blocks(utility::string_t storage_connection_string)
{
	const utility::string_t image_file(U("HelloWorld.png"));

	// Generate unique container name
	utility::string_t container_name = U("blobblockdemocontainer-") + utility::uuid_to_string(utility::new_uuid());

	std::wcout << U("Creating container") << std::endl;

	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(storage_connection_string);

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

// write several pages of a page blob
void upload_file_with_page_blob(utility::string_t storage_connection_string)
{
	const utility::string_t image_file(U("HelloWorld.png"));
	const int page_size = 1024;

	// Generate unique container name
	utility::string_t container_name = U("blobpagedemocontainer") + utility::uuid_to_string(utility::new_uuid());

	std::wcout << U("Creating container") << std::endl;

	// Parse the connection string
	cloud_storage_account storage_account = cloud_storage_account::parse(storage_connection_string);

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