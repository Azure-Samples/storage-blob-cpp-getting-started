---
services: storage
platforms: cpp
author: pcibraro
---

# Azure Storage Service Samples in C++

Demonstrates how to use the Blob, Queue and Table Storage Services.

- Blob storage stores unstructured data such as text, binary data, documents or media files. Blobs can be accessed from anywhere in the world via HTTP or HTTPS.

- The Queue Service provides reliable messaging for workflow processing and for communication
between loosely coupled components of cloud services. This sample demonstrates how to perform common tasks including
inserting, peeking, getting and deleting queue messages, as well as creating and deleting queues.

- Demonstrates how to perform common tasks using the Microsoft Azure Table storage including creating a table, CRUD operations, batch operations and different querying techniques.

Note: This sample uses the Azure SDK for C++ available through the Nuget package "wascore". If you don't have a Microsoft Azure subscription you can
get a FREE trial account [here](http://go.microsoft.com/fwlink/?LinkId=330212)

## Running this sample

This sample can be run using either the Azure Storage Emulator that installs as part of this SDK - or by
updating the storage_connection_string variable defined at the top of the program.
To run the sample using the Storage Emulator (default option):

1. Download and Install the Azure Storage Emulator [here](http://azure.microsoft.com/en-us/downloads/).
2. Start the Azure Storage Emulator (once only) by pressing the Start button or the Windows key and searching for it by typing "Azure Storage Emulator". Select it from the list of applications to start it.
3. Set breakpoints and run the project using F10.

To run the sample using the Storage Service

1. Open the storage-cpp.cpp file and find the variable storage_connection_string. Replace the value (UseDevelopmentStorage=True) with the connection string for the storage service (AccountName=[]...)
2. Create a Storage Account through the Azure Portal and provide your [AccountName] and [AccountKey] in the storage_connection_string variable.
3. Set breakpoints and run the project using F10.

## More information
- [What is a Storage Account](http://azure.microsoft.com/en-us/documentation/articles/storage-whatis-account/)
- [Getting Started with Blobs](http://azure.microsoft.com/en-us/documentation/articles/storage-dotnet-how-to-use-blobs/)
- [Blob Service Concepts](http://msdn.microsoft.com/en-us/library/dd179376.aspx)
- [Blob Service REST API](http://msdn.microsoft.com/en-us/library/dd135733.aspx)
- [Blob Service C++ API](https://azure.microsoft.com/en-us/documentation/articles/storage-c-plus-plus-how-to-use-blobs/)
- [Queue Service Concepts](http://msdn.microsoft.com/en-us/library/dd179353.aspx)
- [Queue Service REST API](http://msdn.microsoft.com/en-us/library/dd179363.aspx)
- [Queue Service C++ API](https://azure.microsoft.com/en-us/documentation/articles/storage-c-plus-plus-how-to-use-queues/)
- [Getting Started with Tables](http://azure.microsoft.com/en-us/documentation/articles/storage-dotnet-how-to-use-tables/)
- [Table Service Concepts](http://msdn.microsoft.com/en-us/library/dd179463.aspx)
- [Table Service REST API](http://msdn.microsoft.com/en-us/library/dd179423.aspx)
- [Table Service C++ API](https://azure.microsoft.com/en-us/documentation/articles/storage-c-plus-plus-how-to-use-tables/)
- [Storage Emulator](http://msdn.microsoft.com/en-us/library/azure/hh403989.aspx)
