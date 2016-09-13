#include "gen-cpp/XSMBService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <XSMBServiceHandler.h>
#include <infrastructure.h>

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
			// TODO: Add logging to these handler functions.

			void XSMBServiceHandler::CreateDirectory(LinuxFileResponse& _return, const std::string& dirPath) {
				printf("CreateDirectory\n");		

				boost::filesystem::path dir(dirPath);
				try {
					if (boost::filesystem::exists(dir) && boost::filesystem::is_directory(dir)) {						
						SetResponse(_return, false, dirPath + ": directory already exists");						
					}
					else {
						boost::filesystem::create_directory(dir);
						SetResponse(_return, true, "Successfully created directory " + dirPath);
					}
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::CreateDirectory);
				}
				return;
			}

			void XSMBServiceHandler::DeleteDirectory(LinuxFileResponse& _return, const std::string& dirPath, const bool isRecursive) {
				printf("DeleteDirectory\n");
				
				boost::filesystem::path dir(dirPath);
				try {
					if (!boost::filesystem::exists(dir) || !boost::filesystem::is_directory(dir)) {						
						SetResponse(_return, false, dirPath + " does not exist or is not a directory");
					}
					else {
						if (boost::filesystem::is_empty(dir) || isRecursive) {							
							boost::filesystem::remove_all(dir);																					
							SetResponse(_return, true, "Successfully deleted directory " + dirPath);
						}
						else {							
							SetResponse(_return, false, dirPath + " is not empty while isRecursive is false");
						}
					}
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::DeleteDirectory);
				}
				return;
			}

			void XSMBServiceHandler::CreateFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t fileSize, const bool noBuffering) {
				printf("CreateFile\n");
				
				boost::filesystem::path file(filePath);
				try {
					if (boost::filesystem::exists(file) && boost::filesystem::is_regular_file(file)) {				
						SetResponse(_return, false, filePath + " already exists");
					}
					else {
						boost::filesystem::fstream fs;
						fs.open(file, boost::filesystem::fstream::out);
						fs.seekp(fileSize - 1);
						fs.write("", 1);
						fs.close();					
						SetResponse(_return, true, "Successfully created " + filePath);
					}
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::CreateFile);
				}
				return;
			}

			void XSMBServiceHandler::DeleteFile(LinuxFileResponse& _return, const std::string& filePath) {
				printf("DeleteFile\n");
				
				boost::filesystem::path file(filePath);
				try {
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {
						SetResponse(_return, false, filePath + " does not exist or is not a file");
					}
					else {
						boost::filesystem::remove(file);
						SetResponse(_return, true, "Successfully deleted file " + filePath);
					}
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::DeleteFile);
				}
				return;
			}

			void XSMBServiceHandler::ReadFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t offset, const int64_t count) {
				printf("ReadFile\n");
				try {
					std::fstream fs;
					fs.open(filePath.c_str(), std::ios::in);

					char* buffer = new char[count];

					fs.seekg(offset, fs.beg);

					fs.read(buffer, count);					

					std::string buffer_string = std::string(buffer);

					std::string bytes_read = IntToString(fs.gcount());

					SetResponse(_return, true, "Successfully read from file " + filePath);

					std::map<std::string, std::string> additional_info;
					_return.__set_AdditionalInfo(additional_info);
					_return.AdditionalInfo.insert(std::pair<std::string, std::string>("bufferString", buffer_string));
					_return.AdditionalInfo.insert(std::pair<std::string, std::string>("bytesRead", bytes_read);
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::ReadFile);
				}
				return;
			}

			void XSMBServiceHandler::WriteFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t offset, const std::string& buf) {
				printf("WriteFile\n");
				//boost::filesystem::path file(filePath);
				try {
					/*
					std::ofstream fs;
					
					fs.open(filePath.c_str());
					fs.seekp(0, std::ios_base::end);
					fs.write(bufToSend.c_str(), bufToSend.length());
					
					fs.close();
					*/
					std::fstream fs;
					fs.open(filePath.c_str());					
					/* Debug information
					std::cout << "Opening " << filePath.c_str() << std::endl;
					std::cout << "Writing " << bufToSend.c_str() << std::endl;
					std::cout << "Length: " << bufToSend.size() << "(" << bufToSend.length() << ")" << std::endl;
					std::cout << "Current Position: " << outfile.tellp() << std::endl;
					*/
					fs.seekp(offset);

					fs.write(buf.c_str(), buf.length());					

					fs.close();

					SetResponse(_return, true, "Successfully write to " + filePath);
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::WriteFile);
				}
				return;
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

			void XSMBServiceHandler::ListFiles(LinuxFileResponse& _return, const std::string& dirPath, const bool isRecursive, const std::map<std::string, MatchInformation::type> & files, const std::map<std::string, MatchInformation::type> & dirs) {
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
				return;
			}

			void XSMBServiceHandler::GetFileLength(GetFileLengthResponse& _return, const std::string& filePath) {
				printf("GetFileLength\n");
				boost::filesystem::path file(filePath);				
				try {
					if (boost::filesystem::exists(file) && boost::filesystem::is_regular_file(file)) {						
						_return.FileLength = (int64_t)boost::filesystem::file_size(file);
						_return.Success = true;							
					}
					else {
						_return.FileLength = -1;
						_return.Success = false;
						_return.ErrorMessage = filePath + " does not exist or is not a file";
					}
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::GetFileLength);
				}				
				return;
			}

			void XSMBServiceHandler::SetFileLength(LinuxFileResponse& _return, const std::string& filePath, const int64_t fileLength) {
				printf("SetCloudFileLength\n");				
				boost::filesystem::path file(filePath);
				try {
					if (boost::filesystem::exists(file) && boost::filesystem::is_regular_file(file)) {						
						boost::filesystem::resize_file(file, (uintmax_t)fileLength);
						SetResponse(_return, true, "Successfully set file length!");
					}
					else {
						SetResponse(_return, false, filePath + " does not exist or is not a file");
					}
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::SetFileLength);
				}
				return;
			}
		}
	}
}
