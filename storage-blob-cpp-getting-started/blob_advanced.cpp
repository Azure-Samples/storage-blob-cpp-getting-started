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
#include "blob_advanced.h"

using namespace azure::storage;

blob_advanced::blob_advanced()
{
}

blob_advanced::~blob_advanced()
{
}

///
/// Creates a container in the blob storage
///
cloud_blob_container blob_advanced::create_container(cloud_blob_client blob_client, utility::string_t container_name)
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
/// This sample shows how to list all the available containers in an storage account.
///
void blob_advanced::list_containers(cloud_blob_client blob_client)
{
  // Generate a few containers using a 'sample-list-container' prefix
  utility::string_t container_prefix = U("sample-list-container-");

  // Try to generate 5 containers with random name using the prefix
  for (int i = 0; i < 5; i++)
  {
    create_container(blob_client, container_prefix + string_util::random_string());
  }

  ucout << U("Listing all the available containers with prefix ") << container_prefix << std::endl;
  container_result_iterator end_of_results;
  // List the containers using the prefix. Passing in no prefix will list all the containers in the account.
  for (auto it = blob_client.list_containers(container_prefix); it != end_of_results; ++it)
  {
    ucout << U("Container, Name = ") << it->name() << ", URI = " << it->uri().primary_uri().to_string() << std::endl;
  }

  ucout << U("Deleting all the containers with prefix ") << container_prefix << std::endl;
  try
  {
    for (auto it = blob_client.list_containers(container_prefix); it != end_of_results; ++it)
    {
      cloud_blob_container container = blob_client.get_container_reference(it->name());
      container.delete_container_if_exists();
    }
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The containers could not be deleted.") << std::endl;
  }
}

///
/// This sample shows how to set cors rules for the blob service so it can be accessed from a different domain in a web browser.
///
void blob_advanced::set_cors_rules(cloud_blob_client blob_client)
{
  ucout << U("Setting cors rules for account") << std::endl;

  service_properties service_properties;
  try
  {
    service_properties = blob_client.download_service_properties();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The cors rules could not be downloaded.") << std::endl;
    return;
  }

  try
  {
    std::vector<service_properties::cors_rule> current_cors_rules = service_properties.cors();

    service_properties::cors_rule cors_rules = service_properties::cors_rule();
    cors_rules.allowed_origins().push_back(U("*"));
    cors_rules.allowed_methods().push_back(U("POST"));
    cors_rules.allowed_methods().push_back(U("GET"));
    cors_rules.allowed_headers().push_back(U("*"));
    cors_rules.set_max_age(std::chrono::seconds(3600));

    service_properties.cors().clear();
    service_properties.cors().push_back(cors_rules);

    service_properties_includes includes;
    includes.set_cors(true);

    blob_client.upload_service_properties(service_properties, includes);

    // reverts the CORS rules back to the original ones
    service_properties.cors().clear();
    service_properties.cors().insert(service_properties.cors().end(), current_cors_rules.begin(), current_cors_rules.end());

    blob_client.upload_service_properties(service_properties, includes);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The cors rules could not be set.") << std::endl;
  }
}

///
/// This sample shows how a lease can be acquired on a blob for exclusive access.
///
void blob_advanced::lease_blob(cloud_blob_client blob_client)
{
  // Generate unique container name
  utility::string_t container_name = U("sample-lease-container-") + string_util::random_string();

  ucout << U("Creating container") << std::endl;

  cloud_blob_container container = create_container(blob_client, container_name);

  ucout << U("Creating blob") << std::endl;

  cloud_block_blob block_blob = container.get_block_blob_reference(U("exclusive"));
  block_blob.upload_text(U("Blob created"));

  utility::string_t lease;

  try
  {
    ucout << U("Acquiring blob lease") << std::endl;

    lease = block_blob.acquire_lease(lease_time(std::chrono::seconds(15)), U(""));
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The blob lease could not be acquired.") << std::endl;
    return;
  }

  access_condition access = access_condition::generate_lease_condition(lease);

  blob_request_options options;
  operation_context context;

  try
  {
    ucout << U("Trying to update blob without lease") << std::endl;

    // This fails, no access condition with lease is used
    block_blob.upload_text(U("Blob updated"));
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Got expected error:") << e.what() << U(". Tried to update blob without a lease ") << std::endl;
  }

  try
  {
    ucout << U("Trying to update blob with a lease ") << lease << std::endl;

    block_blob.upload_text(U("Blob updated"), access, options, context);

    ucout << U("Blob updated successfully") << std::endl;
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The blob could not be updated.") << std::endl;
  }

  try
  {
    ucout << U("Releasing lease") << std::endl;

    block_blob.release_lease(access);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The blob lease could not be released.") << std::endl;
  }
}

///
/// This sample shows how a lease can be acquired on a container for exclusive access.
///
void blob_advanced::lease_container(cloud_blob_client blob_client)
{
  // Generate unique container name
  utility::string_t container_name = U("sample-lease-container-") + string_util::random_string();

  ucout << U("Creating container") << std::endl;

  cloud_blob_container container = create_container(blob_client, container_name);

  utility::string_t lease;
  try
  {
    ucout << U("Acquiring lease for container") << std::endl;

    lease = container.acquire_lease(lease_time(std::chrono::seconds(15)), U(""));
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The container lease could not be acquired.") << std::endl;
    return;
  }

  try
  {
    ucout << U("Trying to delete container without lease") << std::endl;

    // This fails, no access condition with lease is used
    container.delete_container();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Got expected error:") << e.what() << std::endl << U("Tried to delete blob without a lease") << std::endl;
  }

  access_condition access = access_condition::generate_lease_condition(lease);

  blob_request_options options;
  operation_context context;

  try
  {
    ucout << U("Trying to delete container with a lease ") << lease << std::endl;

    container.delete_container(access, options, context);

    ucout << U("Container deleted successfully") << std::endl;
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The container could not be deleted.") << std::endl;
  }
}

///
/// This sample shows how to copy a blob from one location to another and how to cancel an existing copy operation.
///
void blob_advanced::copy_blob(cloud_blob_client blob_client)
{
  // Generate unique container name
  utility::string_t container_name = U("sample-copy-container-") + string_util::random_string();

  ucout << U("Creating container") << std::endl;

  cloud_blob_container container = create_container(blob_client, container_name);

  cloud_block_blob source_blob = container.get_block_blob_reference(U("source"));
  try
  {
    ucout << U("Creating blob") << std::endl;
    source_blob.upload_text(U("Blob created"));
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The blob could not be created.") << std::endl;
    return;
  }

  cloud_block_blob target_blob = container.get_block_blob_reference(U("target"));
  utility::string_t copy_id;
  try
  {
    ucout << U("Copying blob") << std::endl;

    copy_id = target_blob.start_copy(source_blob);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The blob could not be copied.") << std::endl;
    return;
  }

  copy_state state;
  try
  {
    ucout << U("Fetching attributes to get latest copy state") << std::endl;
    target_blob.download_attributes();

    ucout << U("Checking copy state") << std::endl;
    state = target_blob.copy_state();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The copy state could not be retrieved.") << std::endl;
    return;
  }

  utility::string_t state_description;
  switch (state.status())
  {
  case copy_status::aborted:
    state_description = U("aborted");
    break;
  case copy_status::failed:
    state_description = U("failed");
    break;
  case copy_status::invalid:
    state_description = U("invalid");
    break;
  case copy_status::pending:
    state_description = U("pending");
    break;
  case copy_status::success:
    state_description = U("success");
    break;
  }

  ucout << U("Copy state ") << state_description << std::endl;

  if (state.status() == copy_status::pending)
  {
    try
    {
      ucout << U("Aborting copy") << std::endl;

      target_blob.abort_copy(copy_id);
    }
    catch (const azure::storage::storage_exception& e)
    {
      ucout << U("Error:") << e.what() << std::endl << U("The copy could not be aborted.") << std::endl;
    }
  }

  try
  {
    ucout << U("Deleting container") << std::endl;
    container.delete_container_if_exists();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The containter could not be deleted.") << std::endl;
    return;
  }
}

///
/// This sample shows how to upload and commit a batch of blocks with data in a block blob.
///
void blob_advanced::file_upload_with_blocks(cloud_blob_client blob_client)
{
  const utility::string_t image_file(U("HelloWorld.png"));

  // Generate unique container name
  utility::string_t container_name = U("blobblockdemocontainer-") + string_util::random_string();

  ucout << U("Creating container") << std::endl;

  cloud_blob_container container = create_container(blob_client, container_name);

  ucout << U("Creating blob") << std::endl;

  cloud_block_blob block_blob = container.get_block_blob_reference(image_file);

  std::vector<block_list_item> blocks;

  ucout << U("Creating block id") << std::endl;

  utility::string_t block_id = utility::uuid_to_string(utility::new_uuid());
  std::vector<unsigned char> block_id_as_array(block_id.cbegin(), block_id.cend());
  utility::string_t encoded_block_id(utility::conversions::to_base64(block_id_as_array));

  concurrency::streams::istream file_stream = concurrency::streams::file_stream<uint8_t>::open_istream(image_file).get();

  try
  {
    ucout << U("Pushing file content in block with id ") << encoded_block_id << std::endl;

    block_blob.upload_block(encoded_block_id, file_stream, utility::string_t(U("")));
    blocks.push_back(block_list_item(encoded_block_id));

    ucout << U("Commiting block with id ") << encoded_block_id << std::endl;
    block_blob.upload_block_list(blocks);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << "The file could not be uploaded" << std::endl;
  }

  file_stream.close();

  ucout << U("Enumerating block list") << std::endl;

  blocks = block_blob.download_block_list();

  std::vector<block_list_item>::iterator it;

  for (it = blocks.begin(); it < blocks.end(); it++)
  {
    ucout << U("Block id: ") << it->id() << std::endl;
  }

  ucout << U("Deleting container") << std::endl;

  try
  {
    container.delete_container_if_exists();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << "The container could not be deleted" << std::endl;
  }
}

///
/// This sample shows the usage of a page blob. 
/// A file in disk is splitted in several pages and uploaded to the storage using a page blob.
///
void blob_advanced::page_blob_operations(cloud_blob_client blob_client)
{
  const utility::string_t image_file(U("HelloWorld.png"));
  const int page_size = 1024;

  // Generate unique container name
  utility::string_t container_name = U("blobpagedemocontainer") + string_util::random_string();

  ucout << U("Creating container") << std::endl;

  cloud_blob_container container = create_container(blob_client, container_name);

  ucout << U("Creating blob") << std::endl;

  cloud_page_blob page_blob = container.get_page_blob_reference(image_file);
  page_blob.create(1024 * 1024);

  std::ifstream file;
  std::vector<char> vector_buffer;
  int read_buffer[page_size];
  int index = 0;

  vector_buffer.reserve(page_size);

  ucout << U("Uploading file content in pages. Every page must be a multiple of 512 bytes") << std::endl;

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
    catch (const azure::storage::storage_exception& e)
    {
      ucout << U("Error:") << e.what() << std::endl;
    }

    vector_buffer.clear();
    index++;
  }

  file.close();

  ucout << U("Listing pages") << std::endl;

  std::vector<page_range>::iterator it;

  std::vector<page_range> pages = page_blob.download_page_ranges();

  for (it = pages.begin(); it < pages.end(); it++)
  {
    ucout << U("Page ") << it->start_offset() << " - " << it->end_offset() << std::endl;
  }

  ucout << U("Deleting container") << std::endl;

  try
  {
    container.delete_container_if_exists();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << "The container could not be deleted" << std::endl;
  }
}

///
/// This sample shows how to set the service properties (logging and metrics) for the blob service.
///
void blob_advanced::set_service_properties(cloud_blob_client blob_client)
{
  ucout << U("Setting service properties for the account") << std::endl;

  service_properties service_properties;
  try
  {
    service_properties = blob_client.download_service_properties();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << "The service properties could not be downloaded" << std::endl;
    return;
  }

  service_properties::logging_properties current_logging = service_properties.logging();

  ucout << U("Setting logging properties for the account") << std::endl;

  service_properties::logging_properties logging = service_properties::logging_properties();
  logging.set_delete_enabled(true);
  logging.set_read_enabled(true);
  logging.set_write_enabled(true);
  logging.set_retention_policy_enabled(true);
  logging.set_retention_days(30);

  service_properties.set_logging(logging);

  service_properties::metrics_properties current_minute_metrics = service_properties.minute_metrics();
  service_properties::metrics_properties current_hour_metrics = service_properties.hour_metrics();

  ucout << U("Setting metrics properties for the account") << std::endl;

  service_properties::metrics_properties minute_metrics = service_properties::metrics_properties();
  minute_metrics.set_enabled(true);
  minute_metrics.set_include_apis(true);
  minute_metrics.set_retention_days(30);
  minute_metrics.set_retention_policy_enabled(true);

  service_properties::metrics_properties hour_metrics = service_properties::metrics_properties();
  minute_metrics.set_enabled(true);
  minute_metrics.set_include_apis(true);
  minute_metrics.set_retention_days(30);
  minute_metrics.set_retention_policy_enabled(true);

  service_properties.set_minute_metrics(minute_metrics);
  service_properties.set_hour_metrics(hour_metrics);

  service_properties_includes includes;
  includes.set_logging(true);
  includes.set_minute_metrics(true);
  includes.set_hour_metrics(true);

  try
  {
    blob_client.upload_service_properties(service_properties, includes);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << "The service properties could not be set" << std::endl;
  }

  // reverts the properties back to the original ones
  service_properties.set_logging(current_logging);
  service_properties.set_hour_metrics(current_hour_metrics);
  service_properties.set_minute_metrics(current_minute_metrics);

  try
  {
    blob_client.upload_service_properties(service_properties, includes);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << "The original service properties could not be restored" << std::endl;
  }
}

///
/// This sample shows how to set additional properties and metadata on a container and blob.
///
void blob_advanced::set_metadata_and_properties(cloud_blob_client blob_client)
{
  const utility::string_t image_file(U("HelloWorld.png"));

  // Generate unique container name
  utility::string_t container_name = U("sample-block-container-") + string_util::random_string();

  ucout << U("Creating container") << std::endl;

  cloud_blob_container container = create_container(blob_client, container_name);

  ucout << U("Uploading container metadata") << std::endl;

  container.metadata().reserve(1);
  container.metadata()[U("Category")] = U("Sample");
  container.upload_metadata();

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

  ucout << U("Uploading block blob properties") << std::endl;

  try
  {
    block_blob.properties().set_content_type(U("application/png"));
    block_blob.upload_properties();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The properties could not be uploaded.") << std::endl;
  }

  ucout << U("Uploading block blob metadata") << std::endl;

  try
  {
    block_blob.metadata().reserve(2);
    block_blob.metadata()[U("origin")] = U("usa");
    block_blob.metadata()[U("title")] = U("image file");
    block_blob.upload_metadata();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The metadata could not be uploaded.") << std::endl;
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
/// This sample shows how to set the a named permissions on a container.
///
void blob_advanced::set_container_acl(cloud_blob_client blob_client)
{
  // Generate unique container name
  utility::string_t container_name = U("sample-block-container-") + string_util::random_string();

  ucout << U("Creating container") << std::endl;

  cloud_blob_container container = create_container(blob_client, container_name);

  // Permission expires in 1 hour
  utility::datetime expiry = utility::datetime::utc_now() + utility::datetime::from_hours(1);
  blob_shared_access_policy policy = blob_shared_access_policy(expiry, blob_shared_access_policy::permissions::read);

  shared_access_policies<blob_shared_access_policy> policies = shared_access_policies<blob_shared_access_policy>();
  policies.insert(std::pair<const utility::string_t, blob_shared_access_policy>(U("read_policy"), policy));

  blob_container_permissions permissions = blob_container_permissions();
  permissions.set_public_access(blob_container_public_access_type::blob);
  permissions.set_policies(policies);

  ucout << U("Uploading container permissions") << std::endl;
  try
  {
    container.upload_permissions(permissions);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The permissions could not be uploaded.") << std::endl;
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