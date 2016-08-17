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

using namespace azure::storage;

#pragma once
class blob_advanced
{
public:
  blob_advanced();
  ~blob_advanced();

  static void list_containers(cloud_blob_client blob_client);
  static void set_cors_rules(cloud_blob_client blob_client);
  static void lease_blob(cloud_blob_client blob_client);
  static void lease_container(cloud_blob_client blob_client);
  static void copy_blob(cloud_blob_client blob_client);
  static void file_upload_with_blocks(cloud_blob_client blob_client);
  static void page_blob_operations(cloud_blob_client blob_client);
  static void set_service_properties(cloud_blob_client blob_client);
  static void set_metadata_and_properties(cloud_blob_client blob_client);
  static void set_container_acl(cloud_blob_client blob_client);
private:
  static cloud_blob_container create_container(cloud_blob_client blob_client, utility::string_t container_name);
};

