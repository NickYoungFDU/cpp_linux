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
						set_response(_return, false, dirPath + ": directory already exists");						
					}
					else {
						boost::filesystem::create_directory(dir);
						set_response(_return, true, "Successfully created directory " + dirPath);
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::CreateDirectory);
				}
				return;
			}

			void XSMBServiceHandler::DeleteDirectory(LinuxFileResponse& _return, const std::string& dirPath, const bool isRecursive) {
				printf("DeleteDirectory\n");
				
				boost::filesystem::path dir(dirPath);
				try {
					if (!boost::filesystem::exists(dir) || !boost::filesystem::is_directory(dir)) {						
						set_response(_return, false, dirPath + " does not exist or is not a directory");
					}
					else {
						if (boost::filesystem::is_empty(dir) || isRecursive) {							
							boost::filesystem::remove_all(dir);																					
							set_response(_return, true, "Successfully deleted directory " + dirPath);
						}
						else {							
							set_response(_return, false, dirPath + " is not empty while isRecursive is false");
						}
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::DeleteDirectory);
				}
				return;
			}

			void XSMBServiceHandler::CreateFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t fileSize, const bool noBuffering) {
				printf("CreateFile\n");
				
				boost::filesystem::path file(filePath);
				try {
					if (boost::filesystem::exists(file) && boost::filesystem::is_regular_file(file)) {				
						set_response(_return, false, filePath + " already exists");
					}
					else {
						boost::filesystem::fstream fs;
						fs.open(file, boost::filesystem::fstream::out);
						fs.seekp(fileSize - 1);
						fs.write("", 1);
						fs.close();					
						set_response(_return, true, "Successfully created " + filePath);
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::CreateFile);
				}
				return;
			}

			void XSMBServiceHandler::DeleteFile(LinuxFileResponse& _return, const std::string& filePath) {
				printf("DeleteFile\n");
				
				boost::filesystem::path file(filePath);
				try {
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {
						set_response(_return, false, filePath + " does not exist or is not a file");
					}
					else {
						boost::filesystem::remove(file);
						set_response(_return, true, "Successfully deleted file " + filePath);
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::DeleteFile);
				}
				return;
			}

			void XSMBServiceHandler::ReadFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t offset, const int64_t count) {
				printf("ReadFile\n");

				boost::filesystem::path file(filePath);
				try {
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {
						set_response(_return, false, filePath + " does not exist or is not a file");
					}
					else {
						std::fstream fs;
						fs.open(filePath.c_str(), std::ios::in);

						char* buffer = new char[count];

						fs.seekg(offset, fs.beg);

						fs.read(buffer, count);

						int64_t bytes_read = fs.gcount();

						std::string bytes_read_string = int_to_string(bytes_read);

						std::string buffer_string = std::string(buffer, bytes_read < count ? bytes_read : count);												

						std::cout << "bytes_read: [" << bytes_read_string << "]" << std::endl;
						std::cout << "buffer_string: [" << buffer_string << "]" << std::endl;
						std::cout << "buffer_string.length(): [" << buffer_string.length() << "]" << std::endl;

						fs.close();

						set_response(_return, true, "Successfully read from file " + filePath);

						std::map<std::string, std::string> additional_info;
						_return.__set_AdditionalInfo(additional_info);												
						_return.AdditionalInfo.insert(std::pair<std::string, std::string>("BytesRead", bytes_read_string));
						_return.__set_Buffer(buffer_string);
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::ReadFile);
				}
				return;
			}

			void XSMBServiceHandler::WriteFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t offset, const std::string& buffer, const int64_t count) {
				printf("WriteFile\n");
				std::cout << "buffer.length() - [" << buffer.length() << "]" << std::endl;
				boost::filesystem::path file(filePath);
				try {
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {
						set_response(_return, false, filePath + " does not exist or is not a file");
					}
					else {
						std::fstream fs;
						fs.open(filePath.c_str());
						
						fs.seekp(offset);

						fs.write(buffer.c_str(), buffer.length());

						fs.close();

						set_response(_return, true, "Successfully write to " + filePath);
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::WriteFile);
				}
				return;
			}

			boost::filesystem::recursive_directory_iterator create_recursive_directory_iterator(const boost::filesystem::path& path) {
				try {
					return boost::filesystem::recursive_directory_iterator(path);
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					std::cout << ex.what() << std::endl;
					return boost::filesystem::recursive_directory_iterator();
				}
			}
			
			void dump(const boost::filesystem::path& path, int level) {
				try {
					std::cout << (boost::filesystem::is_directory(path) ? 'D' : ' ') << ' ';
					std::cout << (boost::filesystem::is_symlink(path) ? 'L' : ' ') << ' ';
					for (int i = 0; i < level; i++)
						std::cout << ' ';
					std::cout << level << ' ' << path.filename() << std::endl;
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					std::cout << ex.what() << std::endl;
				}
			}

			void list_directory_recursive(const boost::filesystem::path& path, std::map<std::string, MatchInformation::type> & files, std::map<std::string, MatchInformation::type> & dirs) {
				//dump(path, 0);
				boost::filesystem::recursive_directory_iterator it = create_recursive_directory_iterator(path);
				boost::filesystem::recursive_directory_iterator end;
				while (it != end) {					
					/* Take care of symbolic link later.
					if (boost::filesystem::is_directory(*it) && boost::filesystem::is_symlink(*it))
						it.no_push();
					*/					
					try {
						dump(*it, it.level());
						if (boost::filesystem::is_directory(*it))
							dirs.insert(std::pair<std::string, MatchInformation::type>(boost::filesystem::path(*it).string(), MatchInformation::OnlyOnServer));
						else if (boost::filesystem::is_regular_file(*it))
							files.insert(std::pair<std::string, MatchInformation::type>(boost::filesystem::path(*it).string(), MatchInformation::OnlyOnServer));
						it++;						
					}
					catch (const boost::filesystem::filesystem_error& ex) {
						std::cout << ex.what() << std::endl;
						it.no_push();
						try {
							it++;
						}
						catch (...) {
							std::cout << "!!" << std::endl;
						}
					}
				}
			}

			void XSMBServiceHandler::ListFiles(LinuxFileResponse& _return, const std::string& dirPath, const bool isRecursive) {
				printf("ListCloudFiles\n");
				boost::filesystem::path dir(dirPath);
				std::map<std::string, MatchInformation::type> files, dirs;
				try {
					list_directory_recursive(dir, files, dirs);
					set_response(_return, true, "Success");
					_return.__set_Directories(dirs);
					_return.__set_Files(files);
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
						set_response(_return, true, "Successfully get file length for " + filePath);

						int64_t file_size =  (int64_t)boost::filesystem::file_size(file);
						std::string file_size_string = int_to_string(file_size);

						std::map<std::string, std::string> additional_info;
						_return.__set_AdditionalInfo(additional_info);
						_return.AdditionalInfo.insert(std::pair<std::string, std::string>("FileLength", file_size_string));						
					}
					else {
						set_response(_return, false, filePath + " does not exist or is not a file");
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::GetFileLength);
				}				
				return;
			}

			void XSMBServiceHandler::SetFileLength(LinuxFileResponse& _return, const std::string& filePath, const int64_t fileLength) {
				printf("SetCloudFileLength\n");				
				boost::filesystem::path file(filePath);
				try {
					if (boost::filesystem::exists(file) && boost::filesystem::is_regular_file(file)) {						
						boost::filesystem::resize_file(file, (uintmax_t)fileLength);
						set_response(_return, true, "Successfully set file length!");
					}
					else {
						set_response(_return, false, filePath + " does not exist or is not a file");
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::SetFileLength);
				}
				return;
			}

			void XSMBServiceHandler::OpenFileHandle(LinuxFileResponse& _return, const std::string& filePath, const LinuxFileMode::type fileMode, const LinuxFileAccess::type fileAccess, const int32_t handleId) {
				std::cout << "OpenFileHandle" << std::endl;
				boost::filesystem::path file(filePath);
				try {	
					/*
					 * Cpp Version
					 * /					
					std::ofstream ofs(filePath.c_str());
					ofs.close();
					std::fstream* fs = new std::fstream();					
					fs->open(filePath.c_str());
					file_handles.insert(std::pair<std::string, std::fstream*>(handleId, fs));		
					std::cout << "[" + handleId + "].is_open():" << (fs->is_open() ? "true" : "false") << std::endl;
					fs->write("test message", 12);
					fs->flush();
					std::cout << "#handles: " << file_handles.size() << std::endl;
					*/

					/*
					 * Linux System Call
					 */
					int flag = set_file_flag(fileAccess, fileMode);
					int fd = open(filePath.c_str(), flag);
					if (fd == -1) {
						printf("open() failed with error[%s]\n", strerror(errno));
						set_response(_return, false, "open() failed with error [" + std::string(strerror(errno)) + "]");
						return;
					}
					FILE* file = fdopen(fd, "rb+");
					char test[] = { (char)97, (char)98, (char)0, (char)99 };
					fwrite(test, sizeof(char), 4, file);
					fflush(file);
					file_pointers.insert(std::pair<int, FILE*>(fd, file));
					std::cout << "#handles: " << file_pointers.size() << std::endl;
					set_response(_return, true, "Sucessfully opened file handler [" + int_to_string(fd) + "]");

					std::map<std::string, std::string> additional_info;
					_return.__set_AdditionalInfo(additional_info);
					_return.AdditionalInfo.insert(std::pair<std::string, std::string>("FileDescriptor", int_to_string(fd)));
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::WriteFile);
				}
				return;
			}

			void XSMBServiceHandler::CloseFileHandle(LinuxFileResponse& _return, const int32_t handleId) {				
				std::cout << "CloseFileHandle" << std::endl;
				try {
					/*
					 * Cpp Version
					 * /
					std::map<std::string, std::fstream*>::iterator it = file_handles.find(handleId);
					if (it != file_handles.end()) {
						std::fstream* fs = file_handles[handleId];
						fs->close();						
						file_handles.erase(it);
						delete fs;
						std::cout << "Successfully closed file handle [" + handleId + "]" << std::endl;
						set_response(_return, true, "Successfully closed file handle [" + handleId + "]");
					}
					else {
						std::cout << "File handle [" + handleId + "] does not exist. It may have been closed." << std::endl;
						set_response(_return, false, "File handle [" + handleId + "] does not exist. It may have been closed.");
					}
					*/

					std::map<int, FILE*>::iterator it = file_pointers.find(handleId);
					if (it != file_pointers.end()) {
						FILE* file = it->second;
						fclose(file);
						file_pointers.erase(it);
						close(it->first);
						//delete file;
						std::cout << "Successfully closed file handle [" << handleId << "]" << std::endl;
						set_response(_return, true, "Successfully closed file handle [" + int_to_string(handleId) + "]");
					}
					else {
						std::cout << "File handle [" << handleId << "] does not exist. It may have been closed." << std::endl;
						set_response(_return, false, "File handle [" + int_to_string(handleId) + "] does not exist. It may have been closed.");
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::WriteFile);
				}
				return;
			}

			void XSMBServiceHandler::ReadFileByHandle(LinuxFileResponse& _return, const int32_t handleId, const int64_t offset, const int64_t count) {
				std::cout << "ReadFileByHandle" << std::endl;
				try {
					/*
					 * Cpp Version
					 * /
					std::map<std::string, std::fstream*>::iterator it = file_handles.find(handleId);
					if (it != file_handles.end() && it->second->is_open()) {
						std::fstream* fs = it->second;
						char* buffer = new char[count];
						fs->seekg(offset, fs->beg);
						fs->read(buffer, count);
						int64_t bytes_read = fs->gcount();
						std::string bytes_read_string = int_to_string(bytes_read);
						std::string buffer_string = std::string(buffer, bytes_read < count ? bytes_read : count);

						std::cout << "bytes_read: [" << bytes_read_string << "]" << std::endl;
						std::cout << "buffer_string: [" << buffer_string << "]" << std::endl;
						std::cout << "buffer_string.length(): [" << buffer_string.length() << "]" << std::endl;
						set_response(_return, true, "Successfully read from file handle [" + handleId + "]");

						std::map<std::string, std::string> additional_info;
						_return.__set_AdditionalInfo(additional_info);
						_return.AdditionalInfo.insert(std::pair<std::string, std::string>("BytesRead", bytes_read_string));
						_return.__set_Buffer(buffer_string);
					}
					else {
						std::cout << "file handle [" << handleId << "] does not exist, or somehow it has been closed." << std::endl;
						set_response(_return, false, "file handle [" + handleId + "] does not exist, or somehow it has been closed.");
					}
					*/

					std::map<int, FILE*>::iterator it = file_pointers.find(handleId);
					if (it != file_pointers.end() && it->second != NULL) {
						FILE* file = it->second;
						char* buffer = new char[count];
						fseek(file, offset, SEEK_SET);
						int64_t bytes_read = fread(buffer, sizeof(char), count, file);

						std::string bytes_read_string = int_to_string(bytes_read);
						std::string buffer_string = std::string(buffer, bytes_read < count ? bytes_read : count);
						std::cout << "bytes_read: [" << bytes_read_string << "]" << std::endl;
						std::cout << "buffer_string: [" << buffer_string << "]" << std::endl;
						std::cout << "buffer_string.length(): [" << buffer_string.length() << "]" << std::endl;
						set_response(_return, true, "Successfully read from file handle [" + int_to_string(handleId) + "]");

						std::map<std::string, std::string> additional_info;
						_return.__set_AdditionalInfo(additional_info);
						_return.AdditionalInfo.insert(std::pair<std::string, std::string>("BytesRead", bytes_read_string));
						_return.__set_Buffer(buffer_string);
					}
					else {
						std::cout << "file handle [" << handleId << "] does not exist, or somehow it has been closed." << std::endl;
						set_response(_return, false, "file handle [" + int_to_string(handleId) + "] does not exist, or somehow it has been closed.");
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::WriteFile);
				}
				return;
			}

			void XSMBServiceHandler::WriteFileByHandle(LinuxFileResponse& _return, const int32_t handleId, const int64_t offset, const std::string& buffer, const int64_t count) {
				std::cout << "WriteFileByHandle" << std::endl;
				try {
					/*
					 * Cpp Version
					 * /
					std::map<std::string, std::fstream*>::iterator it = file_handles.find(handleId);
					if (it != file_handles.end() && it->second->is_open()) {
						std::cout << "Start writing..." << std::endl;
						std::fstream* fs = it->second;
						fs->seekp(offset);
						fs->write(buffer.c_str(), buffer.length());
						fs->flush();
						set_response(_return, true, "Successfully write to file handle [" + handleId + "]");
					}
					else {
						std::cout << "file handle [" + handleId + "] does not exist, or somehow it has been closed." << std::endl;
						set_response(_return, false, "file handle [" + handleId + "] does not exist, or somehow it has been closed.");
					}
					*/

					std::map<int, FILE*>::iterator it = file_pointers.find(handleId);
					if (it != file_pointers.end() && it->second != NULL) {
						std::cout << "Start writing..." << std::endl;
						FILE* file = it->second;
						fseek(file, offset, SEEK_SET);
						fwrite(buffer.c_str(), sizeof(char), buffer.length(), file);
						fflush(file);
						std::cout << "Should write: [" << buffer << "]" << std::endl;
						set_response(_return, true, "Successfully write to file handle [" + int_to_string(handleId) + "]");
					}
					else {
						std::cout << "file handle [" << handleId << "] does not exist, or somehow it has been closed." << std::endl;
						set_response(_return, false, "file handle [" + int_to_string(handleId) + "] does not exist, or somehow it has been closed.");
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::WriteFile);
				}
				return;
			}


			/*
			void XSMBServiceHandler::OpenFileHandle(LinuxFileResponse& _return, const std::string& filePath) {
				
				int fd = open(filePath.c_str(), O_CREAT | O_RDWR, S_IRWXU);
				set_response(_return, true, "Successfully opened " + filePath);

				std::string fd_string = int_to_string(fd);

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
					throw set_exception(ex.what(), OperationType::ListFile);
				}
				

			}
			void XSMBServiceHandler::CloseFileHandle(LinuxFileResponse& _return, const int32_t fileDescriptor) {
				
				int error_num = close(fileDescriptor);
				if (error_num == 0) {
					set_response(_return, true, "Successfully closed file descriptor");
				}
				else {
					std::cout << "error..." << std::endl;	
				}
				
			}
			*/ 
		}
	}
}
