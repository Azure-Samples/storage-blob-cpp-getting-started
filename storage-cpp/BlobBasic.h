#pragma once
class BlobBasic
{
public:
	BlobBasic(utility::string_t connection_string);
	~BlobBasic();
	void block_blob_operations(utility::string_t connection_string);
private:
	utility::string_t connection_string;
};

