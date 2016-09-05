#include "gen-cpp/XSMBService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <XSMBServiceHandler.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::azure::storage::cpp_linux;

namespace azure {
	namespace storage {
		namespace cpp_linux 
		{
			std::string& XSMBServiceHandler::GetMountPoint(const std::string& fullPath) {
				return std::string("\\home\\xufyan\\linuxsmbtest2");
			}

			bool XSMBServiceHandler::CreateDirectory(const std::string& dirPath) {
				printf("CreateDirectory\n");
				bool success = false;
				boost::filesystem::path dir(GetMountPoint(dirPath));
				try {
					if (boost::filesystem::exists(dir)) {
						success = false;
					}
					else {
						success = boost::filesystem::create_directory(dir);
					}
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					std::cout << ex.what() << std::endl;
				}
				return success;

			}

			bool XSMBServiceHandler::DeleteDirectory(const std::string& dirPath, const bool isRecursive) {
				printf("DeleteDirectory\n");
				bool success = false;
				boost::filesystem::path dir(dirPath);
				try {
					if (!(!boost::filesystem::is_empty(dir) && !isRecursive)) {
						if (boost::filesystem::exists(dir)) {
							success = boost::filesystem::remove_all(dir);
						}
						else {
							success = true;
						}
					}
					else {
						std::cout << "Error: " << dirPath << " is not empty while isRecursive is false" << std::endl;
					}
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					std::cout << ex.what() << std::endl;
				}
				return success;
			}

			bool XSMBServiceHandler::CreateFile(const std::string& filePath, const int64_t fileSize, const bool noBuffering) {
				printf("CreateFile\n");
				bool success = false;
				boost::filesystem::path file(filePath);
				try {
					if (boost::filesystem::exists(file)) {
						success = false;
					}
					else {
						boost::filesystem::fstream fs;
						fs.open(file, boost::filesystem::fstream::out);
						fs.seekp(fileSize - 1);
						fs.write("", 1);
						fs.close();
						success = true;
					}
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					std::cout << ex.what() << std::endl;
				}
				return success;
			}

			bool XSMBServiceHandler::DeleteFile(const std::string& filePath) {
				printf("DeleteFile\n");
				bool success = false;
				boost::filesystem::path file(filePath);
				try {
					if (boost::filesystem::exists(file)) {
						success = boost::filesystem::remove(file);
					}
					else {
						success = true;
					}
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					std::cout << ex.what() << std::endl;
				}
				return success;
			}

			bool XSMBServiceHandler::ReadFile(const std::string& filePath, const StreamDataLayout& data, const bool noBuffering, const int8_t fileVersion, const bool useVersionInData, const std::string& keyName) {
				return true;
			}

			bool XSMBServiceHandler::WriteFile(const std::string& filePath, const StreamDataLayout& data, const bool noBuffering, const int8_t fileVersion, const bool useVersionInData, const std::string& keyName) {
				printf("WriteFile\n");
				return true;
			}

			bool XSMBServiceHandler::ListFiles(const std::string& dirPath) {
				printf("ListCloudFiles\n");
				boost::filesystem::path dir(dirPath);
				try {
					std::copy(boost::filesystem::directory_iterator(dir),
						boost::filesystem::directory_iterator(),
						std::ostream_iterator<boost::filesystem::directory_entry>(std::cout, "\n"));
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					std::cout << ex.what() << std::endl;
				}
				return true;
			}

			bool XSMBServiceHandler::ListCloudFiles(const std::string& dirPath, const bool isRecursive, const std::map<std::string, MatchInformation::type> & files, const std::map<std::string, MatchInformation::type> & dirs) {
				printf("ListCloudFiles\n");
				boost::filesystem::path dir(dirPath);
				try {
					std::copy(boost::filesystem::directory_iterator(dir),
						boost::filesystem::directory_iterator(),
						std::ostream_iterator<boost::filesystem::directory_entry>(std::cout, "\n"));
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					std::cout << ex.what() << std::endl;
				}
				return true;
			}

			int64_t XSMBServiceHandler::GetCloudFileLength(const std::string& filePath) {
				boost::filesystem::path file(filePath);
				int64_t fileSize = 0;
				try {
					if (boost::filesystem::exists(file)) {
						if (boost::filesystem::is_regular_file(file)) {
							fileSize = (int64_t)boost::filesystem::file_size(file);
							std::cout << file << ": " << fileSize << "bytes" << std::endl;
						}
						else if (boost::filesystem::is_directory(file)) {
							std::cout << file << " is a directory" << std::endl;
						}
						else {
							std::cout << file << " exists, but is neither a file or directory" << std::endl;
						}
					}
					else {
						std::cout << file << " does not exist" << std::endl;
					}
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					std::cout << ex.what() << std::endl;
				}
				printf("GetCloudFileLength\n");
				return 0;
			}

			bool XSMBServiceHandler::SetCloudFileLength(const std::string& filePath, const int64_t fileLength) {
				printf("SetCloudFileLength\n");
				bool success = false;
				boost::filesystem::path file(filePath);
				try {
					if (!boost::filesystem::exists(file)) {
						success = false;
						std::cout << file << " does not exist" << std::endl;
					}
					else {
						boost::filesystem::resize_file(file, (uintmax_t)fileLength);
						success = true;
					}
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					std::cout << ex.what() << std::endl;
				}
				return success;
			}
		}
	}
}