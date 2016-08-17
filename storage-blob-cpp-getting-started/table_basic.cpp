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
#include "table_basic.h"

using namespace azure::storage;

table_basic::table_basic()
{
}


table_basic::~table_basic()
{
}

///
/// This sample shows how to perform basic operations with the table storage
///
void table_basic::table_operations(cloud_table_client table_client)
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

  ucout << U("Inserting a row into table") << std::endl;

  try
  {
    table_entity customer(U("argentina"), U("carlos@contoso.com"));

    table_entity::properties_type& properties = customer.properties();
    properties.reserve(2);
    properties[U("FullName")] = azure::storage::entity_property(U("Carlos Lopez"));
    properties[U("Phone")] = azure::storage::entity_property(U("425-555-0101"));

    // Create the table operation that inserts the customer entity.
    table_operation insert_operation = table_operation::insert_entity(customer);

    // Execute the insert operation.
    table_result insert_result = table.execute(insert_operation);

    ucout << U("Row inserted. Http Status code =") << insert_result.http_status_code() << std::endl;
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("Data could not be inserted in the table") << std::endl;
  }

  ucout << U("Inserting multiple rows in a batch into table") << std::endl;

  try
  {
    table_entity customer1(U("argentina"), U("federico@contoso.com"));

    customer1.properties().reserve(2);
    customer1.properties()[U("FullName")] = azure::storage::entity_property(U("Federico Lopez"));
    customer1.properties()[U("Phone")] = azure::storage::entity_property(U("425-888-0101"));

    table_entity customer2(U("argentina"), U("jorge@contoso.com"));
    customer2.properties().reserve(2);
    customer2.properties()[U("FullName")] = azure::storage::entity_property(U("Jorge Reinoso"));
    customer2.properties()[U("Phone")] = azure::storage::entity_property(U("425-666-0202"));

    table_entity customer3(U("argentina"), U("diego@contoso.com"));
    customer3.properties().reserve(2);
    customer3.properties()[U("FullName")] = azure::storage::entity_property(U("Diego Perez"));
    customer3.properties()[U("Phone")] = azure::storage::entity_property(U("425-777-0303"));

    // Define a batch operation.
    azure::storage::table_batch_operation batch_operation;

    // Add customer entities to the batch insert operation.
    batch_operation.insert_or_replace_entity(customer1);
    batch_operation.insert_or_replace_entity(customer2);
    batch_operation.insert_or_replace_entity(customer3);

    // Execute the batch operation.
    std::vector<azure::storage::table_result> results = table.execute_batch(batch_operation);
    for (std::vector<azure::storage::table_result>::iterator it = results.begin(); it != results.end(); ++it)
    {
      ucout << U("Inserted row. Http Status Code = ") << it->http_status_code() << std::endl;
    }
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << U(".Data could not be inserted in the table") << std::endl;
  }

  ucout << U("Querying data by partition key 'argentina' and row key 'federico@contoso.com'") << std::endl;

  try
  {
    // Construct the query operation for all customer entities where PartitionKey="Argentina".
    table_query query;

    query.set_filter_string(table_query::generate_filter_condition(U("PartitionKey"),
      query_comparison_operator::equal,
      U("argentina")));

    // Execute the query.
    table_query_iterator it = table.execute_query(query);

    // Print the fields for each customer.
    table_query_iterator end_of_results;
    for (; it != end_of_results; ++it)
    {
      ucout << U("PartitionKey: ") << it->partition_key() << U(", RowKey: ") << it->row_key()
        << U(", Property1: ") << it->properties().at(U("FullName")).string_value()
        << U(", Property2: ") << it->properties().at(U("Phone")).string_value() << std::endl;
    }
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("Data could not be retrieved") << std::endl;
  }

  ucout << U("Retrieving single row by partition key 'argentina' and row key 'federico@contoso.com'") << std::endl;
  try
  {
    table_entity entity;
    // Retrieve the entity with partition key of "argentina" and row key of "federico@contoso.com".
    table_operation retrieve_operation = table_operation::retrieve_entity(U("argentina"), U("federico@contoso.com"));
    table_result retrieve_result = table.execute(retrieve_operation);

    entity = retrieve_result.entity();
    const azure::storage::table_entity::properties_type& properties = entity.properties();

    ucout << U("PartitionKey: ") << entity.partition_key() << U(", RowKey: ") << entity.row_key()
      << U(", Property1: ") << properties.at(U("FullName")).string_value()
      << U(", Property2: ") << properties.at(U("Phone")).string_value() << std::endl;
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("Data could not be retrieved") << std::endl;
  }

  ucout << U("Replacing single row by partition key 'argentina' and row key 'federico@contoso.com'") << std::endl;

  try
  {
    table_entity entity_to_replace(U("argentina"), U("federico@contoso.com"));
    azure::storage::table_entity::properties_type& properties_to_replace = entity_to_replace.properties();
    properties_to_replace.reserve(2);

    // Specify a new phone number.
    properties_to_replace[U("FullName")] = entity_property(U("Federico M Gonzalez"));

    // Specify a new email address.
    properties_to_replace[U("Phone")] = entity_property(U("425-111-0234"));

    // Create an operation to replace the entity.
    table_operation replace_operation = table_operation::replace_entity(entity_to_replace);

    // Submit the operation to the Table service.
    table_result replace_result = table.execute(replace_operation);

    ucout << U("Row replaced. Http Status code =") << replace_result.http_status_code() << std::endl;
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("Row could not be retrieved") << std::endl;
  }

  ucout << U("Deleting row by partition key 'argentina' and row key 'carlos@contoso.com'") << std::endl;
  try
  {
    // Create an operation to retrieve the entity with partition key of "Smith" and row key of "Jeff".
    table_operation retrieve_operation = azure::storage::table_operation::retrieve_entity(U("argentina"), U("carlos@contoso.com"));
    table_result retrieve_result = table.execute(retrieve_operation);

    // Create an operation to delete the entity.
    table_operation delete_operation = table_operation::delete_entity(retrieve_result.entity());

    // Submit the delete operation to the Table service.
    table_result delete_result = table.execute(delete_operation);

    ucout << U("Row deleted. Http Status code =") << delete_result.http_status_code() << std::endl;
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("Row could not be deleted") << std::endl;
  }

  ucout << U("Deleting table") << std::endl;
  try
  {
    table.delete_table();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("Table could not be deleted") << std::endl;
  }

}
