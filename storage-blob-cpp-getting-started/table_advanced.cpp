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
void table_advanced::list_tables(cloud_table_client table_client)
{
  ucout << U("Creating tables") << std::endl;

  utility::string_t table_prefix = U("sampletable");

  try
  {
    // Try to generate 5 tables with random name using the prefix
    for (int i = 0; i < 5; i++)
    {
      // Retrieve a reference to a table.
      cloud_table table = table_client.get_table_reference(table_prefix.append(std::to_wstring(i)));

      // Create the table if it doesn't already exist.
      table.create_if_not_exists();
    }
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The tables could not be created.") << std::endl;
    return;
  }

  ucout << U("Listing all the available tables") << std::endl;
  table_result_iterator end_of_results;
  for (auto it = table_client.list_tables(); it != end_of_results; ++it)
  {
    ucout << U("Table ") << it->name() << ", URI = " << it->uri().primary_uri().to_string() << std::endl;
  }

  ucout << U("Deleting tables") << std::endl;

  try
  {
    // Try to generate 5 tables with random name using the prefix
    for (int i = 0; i < 5; i++)
    {
      // Retrieve a reference to a table.
      cloud_table table = table_client.get_table_reference(table_prefix.append(std::to_wstring(i)));

      // Delete the table if it exists.
      table.delete_table_if_exists();
    }
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The tables could not be deleted.") << std::endl;
    return;
  }
}

///
/// This sample shows how to set the service properties (logging and metrics) for the blob service.
///
void table_advanced::set_service_properties(cloud_table_client table_client)
{
  ucout << U("Setting service properties for the account") << std::endl;
  service_properties service_properties;
  try
  {
    service_properties = table_client.download_service_properties();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The service properties could not be downloaded.") << std::endl;
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

    table_client.upload_service_properties(service_properties, includes);
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

    table_client.upload_service_properties(service_properties, includes);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The original service properties could not be restored.") << std::endl;
  }
}

///
/// This sample shows how to set the a named permissions on a table.
///
void table_advanced::set_table_acl(cloud_table_client table_client)
{
  // Generate unique table name
  utility::string_t table_name = U("Customers") + string_util::random_string();

  ucout << U("Creating table") << std::endl;

  // Get a reference to the table
  cloud_table table = table_client.get_table_reference(table_name);
  try
  {
    // Create the table if it does not exist yet
    table.create_if_not_exists();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("If you are running with the default configuration, make sure the storage emulator is started.") << std::endl;
  }

  // Permission expires in 1 hour
  utility::datetime expiry = utility::datetime::utc_now() + utility::datetime::from_hours(1);
  table_shared_access_policy policy = table_shared_access_policy(expiry, table_shared_access_policy::permissions::read);

  shared_access_policies<table_shared_access_policy> policies = shared_access_policies<table_shared_access_policy>();
  policies.insert(std::pair<const utility::string_t, table_shared_access_policy>(U("read_policy"), policy));

  table_permissions permissions = table_permissions();
  permissions.set_policies(policies);

  ucout << U("Uploading table permissions") << std::endl;
  try
  {
    table.upload_permissions(permissions);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The permissions could not be uploaded.") << std::endl;
  }

  ucout << U("Deleting table") << std::endl;

  try
  {
    // Delete table
    table.delete_table_if_exists();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The table could not be deleted") << std::endl;
  }
}


