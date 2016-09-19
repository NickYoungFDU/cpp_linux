#include "gen-cpp/XSMBService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
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

				char test[] = {(char)97, (char)98, (char)0, (char)99};
				std::string test_str = std::string(test);
				std::cout << test_str.length() << std::endl;

				boost::filesystem::path file(filePath);
				try {
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {
						SetResponse(_return, false, filePath + " does not exist or is not a file");
					}
					else {
						std::fstream fs;
						fs.open(filePath.c_str(), std::ios::in);

						char* buffer = new char[count];

						fs.seekg(offset, fs.beg);

						fs.read(buffer, count);

						int64_t bytes_read = fs.gcount();

						std::string bytes_read_string = IntToString(bytes_read);

						std::string buffer_string = std::string(buffer, bytes_read < count ? bytes_read : count);												

						std::cout << "bytes_read: [" << bytes_read_string << "]" << std::endl;
						std::cout << "buffer_string: [" << buffer_string << "]" << std::endl;
						std::cout << "buffer_string.length(): [" << buffer_string.length() << "]" << std::endl;

						fs.close();

						SetResponse(_return, true, "Successfully read from file " + filePath);

						std::map<std::string, std::string> additional_info;
						_return.__set_AdditionalInfo(additional_info);												
						_return.AdditionalInfo.insert(std::pair<std::string, std::string>("BytesRead", bytes_read_string));
						_return.__set_Buffer(buffer_string);
					}
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::ReadFile);
				}
				return;
			}

			void XSMBServiceHandler::WriteFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t offset, const std::string& buffer, const int64_t count) {
				printf("WriteFile\n");
				std::cout << "buffer.length() - [" << buffer.length() << "]" << std::endl;
				boost::filesystem::path file(filePath);
				try {
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {
						SetResponse(_return, false, filePath + " does not exist or is not a file");
					}
					else {
						std::fstream fs;
						fs.open(filePath.c_str());
						
						fs.seekp(offset);

						fs.write(buffer.c_str(), buffer.length());

						fs.close();

						SetResponse(_return, true, "Successfully write to " + filePath);
					}
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

			void XSMBServiceHandler::GetFileLength(LinuxFileResponse& _return, const std::string& filePath) {
				printf("GetFileLength\n");
				boost::filesystem::path file(filePath);				
				try {
					if (boost::filesystem::exists(file) && boost::filesystem::is_regular_file(file)) {			
						SetResponse(_return, true, "Successfully get file length for " + filePath);

						int64_t file_size =  (int64_t)boost::filesystem::file_size(file);
						std::string file_size_string = IntToString(file_size);

						std::map<std::string, std::string> additional_info;
						_return.__set_AdditionalInfo(additional_info);
						_return.AdditionalInfo.insert(std::pair<std::string, std::string>("FileLength", file_size_string));						
					}
					else {
						SetResponse(_return, false, filePath + " does not exist or is not a file");
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

			void XSMBServiceHandler::OpenFileHandle(LinuxFileResponse& _return, const std::string& filePath, const std::string& fileMode, const std::string& fileAccess, const std::string& handleId) {
				std::cout << "OpenFileHandle" << std::endl;
				boost::filesystem::path file(filePath);
				try {															
					std::ofstream ofs(filePath.c_str());
					ofs.close();
					std::fstream fs(filePath.c_str());
					file_handles.insert(std::pair<std::string, std::fstream>(handleId, fs));					
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::WriteFile);
				}
				return;
			}
			void XSMBServiceHandler::CloseFileHandle(LinuxFileResponse& _return, const std::string& handleId) {
				std::cout << "CloseFileHandle" << std::endl;
				try {
					std::map<std::string, std::fstream>::iterator it = file_handles.find(handleId);
					if (it != file_handles.end()) {
						file_handles[handleId].close();
						file_handles.erase(handleId);
						std::cout << "Successfully closed file handle [" + handleId + "]" << std::endl;
						SetResponse(_return, true, "Successfully closed file handle [" + handleId + "]");
					}
					else {
						std::cout << "File handle [" + handleId + "] does not exist. It may have been closed." << std::endl;
						SetResponse(_return, false, "File handle [" + handleId + "] does not exist. It may have been closed.");
					}
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::WriteFile);
				}
				return;
			}
			void XSMBServiceHandler::ReadFileByHandle(LinuxFileResponse& _return, const std::string& handleId, const int64_t offset, const int64_t count) {

			}
			void XSMBServiceHandler::WriteFileByHandle(LinuxFileResponse& _return, const std::string& handleId, const int64_t offset, const std::string& buffer, const int64_t count) {

			}


			/*
			void XSMBServiceHandler::OpenFileHandle(LinuxFileResponse& _return, const std::string& filePath) {
				
				int fd = open(filePath.c_str(), O_CREAT | O_RDWR, S_IRWXU);
				SetResponse(_return, true, "Successfully opened " + filePath);

				std::string fd_string = IntToString(fd);

				std::map<std::string, std::string> additional_info;
				_return.__set_AdditionalInfo(additional_info);
				_return.AdditionalInfo.insert(std::pair<std::string, std::string>("FileDescriptor", fd_string));				
				
				
				try {
					boost::iostreams::file_descriptor fd(filePath, std::ios_base::out);
					std::cout << "file descriptor: " << fd.handle() << std::endl;
					std::cout << "file is open: " << (fd.is_open() ? "true" : "false") << std::endl;
					fd.close();
					std::cout << "file is open: " << (fd.is_open() ? "true" : "false") << std::endl;
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::ListFile);
				}
				

			}
			void XSMBServiceHandler::CloseFileHandle(LinuxFileResponse& _return, const int32_t fileDescriptor) {
				
				int error_num = close(fileDescriptor);
				if (error_num == 0) {
					SetResponse(_return, true, "Successfully closed file descriptor");
				}
				else {
					std::cout << "error..." << std::endl;	
				}
				
			}
			*/
		}
	}
}
