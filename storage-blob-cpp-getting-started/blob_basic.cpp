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
#include "string_util.h"
#include "blob_basic.h"

using namespace azure::storage;

blob_basic::blob_basic()
{
}


blob_basic::~blob_basic()
{
}

///
/// Creates a container in the blob storage
///
cloud_blob_container blob_basic::create_container(cloud_blob_client blob_client, utility::string_t container_name)
{
  // Get a reference to the container
  cloud_blob_container container = blob_client.get_container_reference(container_name);
  try
  {
    // Create the container if it does not exist yet
    container.create_if_not_exists();

    return container;
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("If you are running with the default configuration, make sure the storage emulator is started.") << std::endl;
    throw;
  }
}

///
/// This sample shows how to perform basic operations on block blobs.
///
void blob_basic::block_blob_operations(cloud_blob_client blob_client)
{
  const utility::string_t image_file(U("HelloWorld.png"));

  // Generate unique container name
  utility::string_t container_name = U("sample-block-container-") + string_util::random_string();

  ucout << U("Creating container") << std::endl;

  cloud_blob_container container = create_container(blob_client, container_name);

  ucout << U("Uploading BlockBlob") << std::endl;

  // Get a reference to a cloud block blob
  cloud_block_blob block_blob = container.get_block_blob_reference(image_file);
  try
  {
    //Push a file from disk to the cloud block blob
    concurrency::streams::istream image_stream = concurrency::streams::file_stream<uint8_t>::open_istream(image_file).get();
    block_blob.upload_from_stream(image_stream);
    image_stream.close().wait();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The file could not uploaded.") << std::endl;
  }

  ucout << U("Listing all blobs and directories in container ") << std::endl;
  try
  {
    //Enumerate all the blobs in the container
    list_blob_item_iterator end_of_results;
    for (auto it = container.list_blobs(); it != end_of_results; ++it)
    {
      if (it->is_blob())
      {
        ucout << U("Blob: ") << it->as_blob().uri().primary_uri().to_string() << std::endl;
      }
    }
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The blobs can not be listed.") << std::endl;
  }

  ucout << U("Downloading blob from ") << block_blob.uri().primary_uri().to_string() << std::endl;
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
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The file could not be downloaded.") << std::endl;
  }

  ucout << U("Creating a read-only snapshot of the blob") << std::endl;
  try
  {
    // Create a read-only replica or snapshot of the block blob content
    cloud_blob snapshot = block_blob.create_snapshot();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The snapshot could not be created.") << std::endl;
  }

  ucout << U("Deleting block Blob and all of its snapshots") << std::endl;
  try
  {
    // Delete the block blob and all the associated snapshots
    block_blob.delete_blob(delete_snapshots_option::include_snapshots,
      access_condition::generate_empty_condition(), *new blob_request_options(), *new operation_context());

  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The snapshot could not be deleted.") << std::endl;
  }

  ucout << U("Deleting container") << std::endl;
  try
  {
    //Delete the container
    container.delete_container_if_exists();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The container could not be deleted.") << std::endl;
  }

}

///
/// This sample shows how to perform basic operations on append blobs.
///
void blob_basic::append_blob_operations(cloud_blob_client blob_client)
{
  // Generate unique container name
  utility::string_t container_name = U("sample-append-container-") + string_util::random_string();

  ucout << U("Creating container") << std::endl;

  cloud_blob_container container = create_container(blob_client, container_name);

  ucout << U("Uploading AppendBlob") << std::endl;
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
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The appendblob could not be created.") << std::endl;
  }

  ucout << U("Downloading AppendBlob") << std::endl;
  try
  {
    // Download all the data in append blob as text
    utility::string_t append_text = append_blob.download_text();
    ucout << U("Append Text: ") << append_text << std::endl;
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The appendblob could not be downloaded.") << std::endl;
  }

  ucout << U("Deleting AppendBlob") << std::endl;
  try
  {
    // Delete the blob
    append_blob.delete_blob();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The appendblob could not be deleted.") << std::endl;
  }

  ucout << U("Deleting container") << std::endl;
  try
  {
    //Delete the container
    container.delete_container_if_exists();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The container could not be deleted.") << std::endl;
  }
}
