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
#include "queue_basic.h"

using namespace azure::storage;

queue_basic::queue_basic()
{
}


queue_basic::~queue_basic()
{
}

///
/// This shows basic operations for queues such as queuing, dequeing or peeking messages.
///
void queue_basic::queue_operations(cloud_queue_client queue_client)
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

  ucout << U("Pushing messsages to the queue") << std::endl;

  // Create a message and add it to the queue.
  cloud_queue_message hello_message(U("Hello, World"));
  cloud_queue_message bye_message(U("Bye, World"));
  try
  {
    queue.add_message(hello_message);

    // Add a second message to the queue
    queue.add_message(bye_message);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The messages could not be pushed to the queue") << std::endl;
  }

  // Storage queues are not strictly ordered so while you'll almost always get the first message 'Hello, World', 
  // you may occasionally get the message'Bye, World' first.
  ucout << U("Peeking first message from queue") << std::endl;

  cloud_queue_message peek_message;
  try
  {
    // Peek at the next message.
    peek_message = queue.peek_message();

    // Output the message content.
    ucout << U("Peeked message content: ") << peek_message.content_as_string() << std::endl;
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The message could not be retrieved from the queue") << std::endl;
  }

  ucout << U("Getting message from queue") << std::endl;

  cloud_queue_message update_message;
  try
  {
    update_message = queue.get_message();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The message could not be retrieved from the queue") << std::endl;
  }

  ucout << U("Changing message content for message ") << update_message.id() << std::endl;

  try
  {
    update_message.set_content(U("Changed message"));
    queue.update_message(update_message, std::chrono::seconds(60), true);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The message could not be updated") << std::endl;
  }

  ucout << U("Deleting message ") << update_message.id() << std::endl;
  try
  {
    // Delete the message.
    queue.delete_message(update_message);
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The message could not be deleted") << std::endl;
  }

  ucout << U("Getting messages with visibility timeout ") << std::endl;

  try
  {
    // Dequeue some queue messages (maximum 32 at a time) and set their visibility timeout to
    // 5 minutes (300 seconds).
    queue_request_options options;
    operation_context context;

    // Retrieve 20 messages from the queue with a visibility timeout of 300 seconds.
    std::vector<azure::storage::cloud_queue_message> messages = queue.get_messages(20, std::chrono::seconds(300), options, context);

    for (auto it = messages.cbegin(); it != messages.cend(); ++it)
    {
      // Display the contents of the message.
      ucout << U("Got: ") << it->content_as_string() << std::endl;
    }
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The messages could not be retrieved from the queue") << std::endl;
  }

  ucout << U("Getting queue attributes ") << std::endl;

  ucout << U('Add more messages in the queue') << std::endl;

  try
  {
    for (int i = 0; i < 5; i++)
    {
      // Create a message and add it to the queue.
      cloud_queue_message message(U("message"));

      queue.add_message(message);
    }
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The messages could not be pushed to the queue") << std::endl;
  }

  try
  {
    // Fetch the queue attributes.
    queue.download_attributes();

    // Retrieve the cached approximate message count.
    int cachedMessageCount = queue.approximate_message_count();

    // Display number of messages.
    ucout << U("Number of messages in queue: ") << cachedMessageCount << std::endl;
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The queue attributes could not retrieved") << std::endl;
  }

  ucout << U("Clearing all messages from queue") << std::endl;

  try
  {
    // Clear all the messages in queue
    queue.clear();
  }
  catch (const azure::storage::storage_exception& e)
  {
    ucout << U("Error:") << e.what() << std::endl << U("The queue could not be cleared") << std::endl;
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
