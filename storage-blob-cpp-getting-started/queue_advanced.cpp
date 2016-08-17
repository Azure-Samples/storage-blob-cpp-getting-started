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
void queue_advanced::list_queues(cloud_queue_client queue_client)
{
  ucout << U("Creating queues") << std::endl;

  utility::string_t queue_prefix = U("my-sample-queue");

  try
  {
    // Try to generate 5 queues with random name using the prefix
    for (int i = 0; i < 5; i++)
    {
      // Retrieve a reference to a queue.
      cloud_queue queue = queue_client.get_queue_reference(queue_prefix.append(std::to_wstring(i)));
    
      // Create the queue if it doesn't already exist.
      queue.create_if_not_exists();
    }
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("Queues could not be created.") << std::endl;
    return;
  }

  ucout << U("Listing all the available queues") << std::endl;
  queue_result_iterator end_of_results;
  for (auto it = queue_client.list_queues(); it != end_of_results; ++it)
  {
    ucout << U("Queue ") << it->name() << ", URI = " << it->uri().primary_uri().to_string() << std::endl;
  }

  ucout << U("Deleting queues") << std::endl;
  try
  {
    // Try to generate 5 containers with random name using the prefix
    for (int i = 0; i < 5; i++)
    {
      // Retrieve a reference to a queue.
      cloud_queue queue = queue_client.get_queue_reference(queue_prefix.append(std::to_wstring(i)));

      // Delete the queue if it exists.
      queue.delete_queue_if_exists();
    }
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("Queues could not be deleted.") << std::endl;
    return;
  }
}

///
/// This sample shows how to set cors rules for the queue service so it can be accessed from a different domain in a web browser.
///
void queue_advanced::set_cors_rules(cloud_queue_client queue_client)
{
  ucout << U("Setting cors rules for account") << std::endl;
  
  service_properties service_properties;
  try
  {
    service_properties = queue_client.download_service_properties();
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

    queue_client.upload_service_properties(service_properties, includes);

    // reverts the CORS rules back to the original ones
    service_properties.cors().clear();
    service_properties.cors().insert(service_properties.cors().end(), current_cors_rules.begin(), current_cors_rules.end());

    queue_client.upload_service_properties(service_properties, includes);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The cors rules could not be set.") << std::endl;
  }
}

///
/// This sample shows how to set the service properties (logging and metrics) for the queue service.
///
void queue_advanced::set_service_properties(cloud_queue_client queue_client)
{
  ucout << U("Setting service properties for the account") << std::endl;
  service_properties service_properties;
  try
  {
    service_properties = queue_client.download_service_properties();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The service properties could not be downloaded.") << std::endl;
    return;
  }

  service_properties::logging_properties current_logging = service_properties.logging();
  service_properties::metrics_properties current_minute_metrics = service_properties.minute_metrics();
  service_properties::metrics_properties current_hour_metrics = service_properties.hour_metrics();

  service_properties_includes includes;
  includes.set_logging(true);
  includes.set_minute_metrics(true);
  includes.set_hour_metrics(true);

  try
  {
    ucout << U("Setting logging properties for the account") << std::endl;

    service_properties::logging_properties logging = service_properties::logging_properties();
    logging.set_delete_enabled(true);
    logging.set_read_enabled(true);
    logging.set_write_enabled(true);
    logging.set_retention_policy_enabled(true);
    logging.set_retention_days(30);

    service_properties.set_logging(logging);

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

    queue_client.upload_service_properties(service_properties, includes);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The service properties could not be set.") << std::endl;
  }

  try
  {
    // reverts the properties back to the original ones
    service_properties.set_logging(current_logging);
    service_properties.set_hour_metrics(current_hour_metrics);
    service_properties.set_minute_metrics(current_minute_metrics);

    queue_client.upload_service_properties(service_properties, includes);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The original service properties could not be restored.") << std::endl;
  }
}

///
/// This sample shows how to set additional properties and metadata on a queue.
///
void queue_advanced::set_metadata_and_properties(cloud_queue_client queue_client)
{
  ucout << U("Creating queue") << std::endl;

  // Retrieve a reference to a queue.
  cloud_queue queue = queue_client.get_queue_reference(U("my-sample-queue"));

  try
  {
    // Create the queue if it doesn't already exist.
    queue.create_if_not_exists();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("If you are running with the default configuration, make sure the storage emulator is started.") << std::endl;
  }

  ucout << U("Uploading queue metadata") << std::endl;

  queue.metadata().reserve(1);
  queue.metadata()[U("Category")] = U("Sample");
  queue.upload_metadata();

  ucout << U("Deleting queue") << std::endl;

  try
  {
    // Delete queue
    queue.delete_queue_if_exists();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The queue could not be deleted") << std::endl;
  }
}

///
/// This sample shows how to set the a named permissions on a queue.
///
void queue_advanced::set_queue_acl(cloud_queue_client queue_client)
{
  ucout << U("Creating queue") << std::endl;

  // Retrieve a reference to a queue.
  cloud_queue queue = queue_client.get_queue_reference(U("my-sample-queue"));

  try
  {
    // Create the queue if it doesn't already exist.
    queue.create_if_not_exists();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("If you are running with the default configuration, make sure the storage emulator is started.") << std::endl;
  }

  // Permission expires in 1 hour
  utility::datetime expiry = utility::datetime::utc_now() + utility::datetime::from_hours(1);
  queue_shared_access_policy policy = queue_shared_access_policy(expiry, queue_shared_access_policy::permissions::process);

  shared_access_policies<queue_shared_access_policy> policies = shared_access_policies<queue_shared_access_policy>();
  policies.insert(std::pair<const utility::string_t, queue_shared_access_policy>(U("process_policy"), policy));

  queue_permissions permissions = queue_permissions();
  permissions.set_policies(policies);

  ucout << U("Uploading queue permissions") << std::endl;
  try
  {
    queue.upload_permissions(permissions);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The permissions could not be uploaded.") << std::endl;
  }

  ucout << U("Deleting queue") << std::endl;

  try
  {
    // Delete queue
    queue.delete_queue_if_exists();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The queue could not be deleted") << std::endl;
  }
}
