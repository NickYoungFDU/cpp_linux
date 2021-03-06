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
			void XSMBServiceHandler::IsMounted(LinuxFileResponse& _return, const std::string& dirPath) {
				BOOST_LOG(lg) << "Calling [IsMounted] ";
				boost::filesystem::path directory(dirPath);
				try {
					refresh_client_cache(dirPath);
					if (boost::filesystem::exists(directory) && boost::filesystem::is_directory(directory)) {
						std::string cmd = "df -T " + dirPath + " | tail -n +2";
						std::string ret = exec(cmd.c_str());
						bool isMounted = ret.find("cifs") != std::string::npos;
						std::map<std::string, std::string> additional_info;
						_return.__set_AdditionalInfo(additional_info);
						_return.AdditionalInfo.insert(std::pair<std::string, std::string>("IsMounted", isMounted ? "true" : "false"));
						set_response(_return, true, dirPath + " isMounted:" + (isMounted ? "true" : "false"));
					}
					else {
						set_response(_return, false, dirPath + " does not exist or is not a directory");
						BOOST_LOG(lg) << "[IsMounted] - " << dirPath << " does not exist or is not a directory ";
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::IsMounted);
				}
				return;
			}

			void XSMBServiceHandler::PathExists(LinuxFileResponse& _return, const std::string& path) {				 
				BOOST_LOG(lg) << "Calling [PathExists] ";
				boost::filesystem::path b_path(path);
				try {
					refresh_client_cache(path);
					if (boost::filesystem::exists(b_path)) {
						set_response(_return, true, path + " exists");
						BOOST_LOG(lg) << "[PathExists] - " << path << " exists. ";
					}
					else{
						set_response(_return, false, path + " does not exist");
						BOOST_LOG(lg) << "[PathExists] - " << path << " does not exist. ";
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::ListFile);
				}				 
			}

			void XSMBServiceHandler::CreateDirectory(LinuxFileResponse& _return, const std::string& dirPath) {				 
				BOOST_LOG(lg) << "Calling [CreateDirectory] ";
				boost::filesystem::path dir(dirPath);
				try {
					refresh_client_cache(dirPath);
					if (boost::filesystem::exists(dir) && boost::filesystem::is_directory(dir)) {						
						set_response(_return, false, dirPath + ": directory already exists");						
						BOOST_LOG(lg) << "[CreateDirectory] - " << dirPath << ": directory already exists. ";
					}
					else {
						boost::filesystem::create_directory(dir);
						set_response(_return, true, "Successfully created directory " + dirPath);
						BOOST_LOG(lg) << "[CreateDirectory] - Successfully created directory " << dirPath << " " ;
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::CreateDirectory);
				}
				return;
			}

			void XSMBServiceHandler::DeleteDirectory(LinuxFileResponse& _return, const std::string& dirPath, const bool isRecursive) {				 
				BOOST_LOG(lg) << "Calling [DeleteDirectory] ";				
				boost::filesystem::path dir(dirPath);
				try {
					refresh_client_cache(dirPath);
					if (!boost::filesystem::exists(dir) || !boost::filesystem::is_directory(dir)) {						
						set_response(_return, false, dirPath + " does not exist or is not a directory");
						BOOST_LOG(lg) << "[DeleteDirectory] - " << dirPath << "does not exist or is not a directory ";
					}
					else {
						if (boost::filesystem::is_empty(dir) || isRecursive) {							
							boost::filesystem::remove_all(dir);																					
							set_response(_return, true, "Successfully deleted directory " + dirPath);
							BOOST_LOG(lg) << "[DeleteDirectory] - Successfully deleted directory " << dirPath << " " ;
						}
						else {							
							set_response(_return, false, dirPath + " is not empty while isRecursive is false");
							BOOST_LOG(lg) << "[DeleteDirectory] - " << dirPath << " is not empty while isRecursive is false ";
						}
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::DeleteDirectory);
				}				 
				return;
			}

			void XSMBServiceHandler::CreateFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t fileSize, const bool noBuffering) {				 
				BOOST_LOG(lg) << "Calling [CreateFile] "; 			
				boost::filesystem::path file(filePath);				
				try {
					refresh_client_cache(filePath);
					if (boost::filesystem::exists(file) && boost::filesystem::is_regular_file(file)) {				
						set_response(_return, false, filePath + " already exists");
						BOOST_LOG(lg) << "[CreateFile] - " << filePath << " already exists ";
					}
					else {
						int fd = creat(filePath.c_str(), 0777);
						boost::filesystem::resize_file(file, (uintmax_t)fileSize);
						close(fd);						
						set_response(_return, true, "Successfully created " + filePath);
						BOOST_LOG(lg) << "[CreateFile] - Successfully created " << filePath << " " ;
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::CreateFile);
				}				 
				return;
			}

			void XSMBServiceHandler::DeleteFile(LinuxFileResponse& _return, const std::string& filePath) {				
				BOOST_LOG(lg) << "Calling [DeleteFile] ";						
				boost::filesystem::path file(filePath);				
				try {
					refresh_client_cache(filePath);
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {
						set_response(_return, false, filePath + " does not exist or is not a file");
						BOOST_LOG(lg) << "[DeleteFile] - " << filePath << " does not exist or is not a file ";
						if (!boost::filesystem::exists(file)) {
							BOOST_LOG(lg) << "Exist test failed.";
						}
						else {
							BOOST_LOG(lg) << "Regular file test failed";
						}
					}
					else {
						//boost::filesystem::remove(file);							
						std::string cmd = "rm -f " + filePath;
						std::string ret = exec(cmd.c_str());
						BOOST_LOG(lg) << "Executing '" + cmd + "'";
						BOOST_LOG(lg) << "Result: " << ret;
						set_response(_return, true, "Successfully deleted file " + filePath);
						BOOST_LOG(lg) << "[DeleteFile] - Successfully deleted file " << filePath << " " ;
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::DeleteFile);
				}				 
				return;
			}

			void XSMBServiceHandler::ReadFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t offset, const int64_t count) {				 
				BOOST_LOG(lg) << "Calling [ReadFile] ";
				boost::filesystem::path file(filePath);				
				try {
					refresh_client_cache(filePath);
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {						
						set_response(_return, false, filePath + " does not exist or is not a file");
						BOOST_LOG(lg) << "[ReadFile] - " << filePath << " does not exist or is not a file ";
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
						fs.close();						
						set_response(_return, true, "Successfully read from file " + filePath);
						BOOST_LOG(lg) << "[ReadFile] - Successfully read from file " << filePath << " " ;
						std::map<std::string, std::string> additional_info;
						_return.__set_AdditionalInfo(additional_info);												
						_return.AdditionalInfo.insert(std::pair<std::string, std::string>("BytesRead", bytes_read_string));
						_return.__set_Buffer(buffer_string);
						delete[] buffer;
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::ReadFile);
				}				 
				return;
			}

			void XSMBServiceHandler::WriteFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t offset, const std::string& buffer, const int64_t count) {				 
				BOOST_LOG(lg) << "Calling [WriteFile] ";
				boost::filesystem::path file(filePath);
				try {
					refresh_client_cache(filePath);
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {
						set_response(_return, false, filePath + " does not exist or is not a file");
						BOOST_LOG(lg) << "[WriteFile] - " << filePath << " does not exist or is not a file ";
					}
					else {
						std::fstream fs;
						fs.open(filePath.c_str());					
						fs.seekp(offset);
						fs.write(buffer.c_str(), count);
						fs.close();
						set_response(_return, true, "Successfully write to " + filePath);
						BOOST_LOG(lg) << "[WriteFile] - Successfully write to " << filePath << " " ;
						BOOST_LOG(lg) << "[WriteFile] - Dump - offset " << offset << " - count " << count << " ";
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
					std::cout << ex.what() << " " ;
					return boost::filesystem::recursive_directory_iterator();
				}
			}
			
			void dump(const boost::filesystem::path& path, int level) {
				 
				try {
					std::cout << (boost::filesystem::is_directory(path) ? 'D' : ' ') << ' ';
					std::cout << (boost::filesystem::is_symlink(path) ? 'L' : ' ') << ' ';
					for (int i = 0; i < level; i++)
						std::cout << ' ';
					std::cout << level << ' ' << path.filename() << " " ;
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					std::cout << ex.what() << " " ;
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
						std::cout << ex.what() << " " ;
						it.no_push();
						try {
							it++;
						}
						catch (...) {
							std::cout << "!! ";
						}
					}
				}
			}

			void XSMBServiceHandler::ListFiles(LinuxFileResponse& _return, const std::string& dirPath, const bool isRecursive) {				 
				printf("Calling [ListCloudFiles] ");
				boost::filesystem::path dir(dirPath);
				std::map<std::string, MatchInformation::type> files, dirs;
				try {
					list_directory_recursive(dir, files, dirs);
					set_response(_return, true, "Success");
					_return.__set_Directories(dirs);
					_return.__set_Files(files);
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					BOOST_LOG(lg) << ex.what() << " " ;
				}
				return;
			}

			void XSMBServiceHandler::GetFileLength(LinuxFileResponse& _return, const std::string& filePath) {				 
				BOOST_LOG(lg) << "Calling [GetFileLength] ";
				boost::filesystem::path file(filePath);				
				try {
					refresh_client_cache(filePath);
					if (boost::filesystem::exists(file) && boost::filesystem::is_regular_file(file)) {			
						set_response(_return, true, "Successfully get file length for " + filePath);
						BOOST_LOG(lg) << "[GetFileLength] - Successfully get file length for " << filePath << " " ;						
						int64_t file_size =  (int64_t)boost::filesystem::file_size(file);
						std::string file_size_string = int_to_string(file_size);
						std::map<std::string, std::string> additional_info;
						_return.__set_AdditionalInfo(additional_info);
						_return.AdditionalInfo.insert(std::pair<std::string, std::string>("FileLength", file_size_string));						
					}
					else {
						set_response(_return, false, filePath + " does not exist or is not a file");
						BOOST_LOG(lg) << "[GetFileLength] - " << filePath << " does not exist or is not a file ";
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::GetFileLength);
				}							 
				return;
			}

			void XSMBServiceHandler::SetFileLength(LinuxFileResponse& _return, const std::string& filePath, const int64_t fileLength) {				 
				BOOST_LOG(lg) << "SetCloudFileLength ";
				boost::filesystem::path file(filePath);
				try {
					refresh_client_cache(filePath);
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
				BOOST_LOG(lg) << "OpenFileHandle ";
				boost::filesystem::path file(filePath);
				try {	
					refresh_client_cache(filePath);
					int flag = set_file_flag(fileAccess, fileMode);
					int fd = open(filePath.c_str(), flag);
					if (fd == -1) {
						std::string error_message = strerror(errno);
						BOOST_LOG(lg) << "open() failed with error [" << error_message << "]";
						set_response(_return, false, "open() failed with error [" + error_message + "]");
						return;
					}
					FILE* file = fdopen(fd, "rb+");
					if (file == NULL) {
						std::string error_message = strerror(errno);
						BOOST_LOG(lg) << "fdopen() failed with error [" << error_message << "]" << ". FD = " << fd;
						set_response(_return, false, "fdopen() failed with error [" + error_message + "]");
						return;
					}
					mtx.lock();
					BOOST_LOG(lg) << "[Before insert] file_pointer.count() = " << file_pointers.size() << ". FD = " << handleId;
					file_pointers.insert(std::pair<int, FILE*>(fd, file));
					BOOST_LOG(lg) << "[After insert] file_pointer.count() = " << file_pointers.size() << ". FD = " << handleId;
					mtx.unlock();
					BOOST_LOG(lg) << "Open file " << filePath << ", descriptor " << fd << " ";
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
				BOOST_LOG(lg) << "CloseFileHandle ";
				try {
					std::map<int, FILE*>::iterator it = file_pointers.find(handleId);
					if (it != file_pointers.end()) {
						FILE* file = it->second;
						int n = fclose(file);
						if (n != 0) {
							std::string error_message = strerror(errno);
							BOOST_LOG(lg) << "fclose() failed with error [" << error_message << "]. FD = " << handleId;
							set_response(_return, false, "fclose() failed with error [" + error_message + "]. FD = " + int_to_string(handleId));
							BOOST_LOG(lg) << "[Before erase] file_pointer.count() = " << file_pointers.size() << ". FD = " << handleId;
							mtx.lock();
							file_pointers.erase(it);
							mtx.unlock();
							BOOST_LOG(lg) << "[After erase] file_pointer.count() = " << file_pointers.size() << ". FD = " << handleId;
							return;
						}
						BOOST_LOG(lg) << "[Before erase] file_pointer.count() = " << file_pointers.size() << ". FD = " << handleId;
						mtx.lock();
						file_pointers.erase(it);
						mtx.unlock();
						BOOST_LOG(lg) << "[After erase] file_pointer.count() = " << file_pointers.size() << ". FD = " << handleId;
						BOOST_LOG(lg) << "Successfully closed file handle [" << handleId << "] ";
						set_response(_return, true, "Successfully closed file handle [" + int_to_string(handleId) + "]");
					}
					else {
						BOOST_LOG(lg) << "File handle [" << handleId << "] does not exist. It may have been closed. ";
						set_response(_return, false, "File handle [" + int_to_string(handleId) + "] does not exist. It may have been closed.");
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::WriteFile);
				}				 
				return;
			}

			void XSMBServiceHandler::ReadFileByHandle(LinuxFileResponse& _return, const int32_t handleId, const int64_t offset, const int64_t count) {				 
				BOOST_LOG(lg) << "ReadFileByHandle ";
				try {
					std::map<int, FILE*>::iterator it = file_pointers.find(handleId);
					if (it != file_pointers.end() && it->second != NULL) {
						FILE* file = it->second;
						char* buffer = new char[count];
						fseek(file, offset, SEEK_SET);
						int64_t bytes_read = fread(buffer, sizeof(char), count, file);
						BOOST_LOG(lg) << "handleId" << handleId << " - offset " << offset << " - count " << count << " ";
						std::string bytes_read_string = int_to_string(bytes_read);
						std::string buffer_string = std::string(buffer, bytes_read < count ? bytes_read : count);
						BOOST_LOG(lg) << "bytes_read: [" << bytes_read_string << "] ";
						BOOST_LOG(lg) << "buffer_string.length(): [" << buffer_string.length() << "] ";
						set_response(_return, true, "Successfully read from file handle [" + int_to_string(handleId) + "]");
						std::map<std::string, std::string> additional_info;
						_return.__set_AdditionalInfo(additional_info);
						_return.AdditionalInfo.insert(std::pair<std::string, std::string>("BytesRead", bytes_read_string));
						_return.__set_Buffer(buffer_string);
						delete[] buffer;
					}
					else if (it == file_pointers.end()){
						BOOST_LOG(lg) << "file handle [" << handleId << "] does not exist";
						set_response(_return, false, "file handle [" + int_to_string(handleId) + "] does not exist");
					}
					else {
						BOOST_LOG(lg) << "file handle [" << handleId << "] exists, but corresponding FILE* is null";
						set_response(_return, false, "file handle [" + int_to_string(handleId) + "] exists, but corresponding FILE* is null");
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::WriteFile);
				}				 
				return;
			}

			void XSMBServiceHandler::WriteFileByHandle(LinuxFileResponse& _return, const int32_t handleId, const int64_t offset, const std::string& buffer, const int64_t count) {				 
				BOOST_LOG(lg) << "WriteFileByHandle ";
				try {								
					std::map<int, FILE*>::iterator it = file_pointers.find(handleId);
					if (it != file_pointers.end() && it->second != NULL) {
						BOOST_LOG(lg) << "Start writing... ";
						BOOST_LOG(lg) << "handleId" << handleId << " - offset " << offset << " - count " << count << " ";
						FILE* file = it->second;
						fseek(file, offset, SEEK_SET);
						fwrite(buffer.c_str(), sizeof(char), count, file);
						fflush(file);
						set_response(_return, true, "Successfully write to file handle [" + int_to_string(handleId) + "]");
					}
					else if (it == file_pointers.end()){
						BOOST_LOG(lg) << "file handle [" << handleId << "] does not exist";
						set_response(_return, false, "file handle [" + int_to_string(handleId) + "] does not exist");
					}
					else {
						BOOST_LOG(lg) << "file handle [" << handleId << "] exists, but corresponding FILE* is null";
						set_response(_return, false, "file handle [" + int_to_string(handleId) + "] exists, but corresponding FILE* is null");
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::WriteFile);
				}				 
				return;
			}

			void XSMBServiceHandler::CopyFile(LinuxFileResponse& _return, const std::string& sourcePath, const std::string& destinationPath, const bool overwriteIfExists) {				 
				BOOST_LOG(lg) << "Calling [CopyFile] ";
				boost::filesystem::path source(sourcePath), destination(destinationPath);
				try {
					refresh_client_cache(sourcePath);
					refresh_client_cache(destinationPath);
					if (!overwriteIfExists && boost::filesystem::exists(destination)) {
						set_response(_return, false, destinationPath + " exists, copy failed.");
						BOOST_LOG(lg) << destinationPath << " exists, copy failed. ";
					}
					else {
						std::string cmd = "cp ";
						if (overwriteIfExists)
							cmd += "-f ";
						else
							cmd += "-n ";
						cmd += sourcePath + " " + destinationPath;
						exec(cmd.c_str());
						BOOST_LOG(lg) << "[CopyFile] - Running command - " << cmd << " " ;
						set_response(_return, true, "Succesfully copied " + sourcePath + " to" + destinationPath);
						BOOST_LOG(lg) << "[CopyFile] - Source - " << sourcePath << " - Destination - " << destinationPath << " " ;
					}				
				} 
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::CreateDirectory);
				}				
				return;
			}

			void XSMBServiceHandler::MoveFile(LinuxFileResponse& _return, const std::string& sourcePath, const std::string& destinationPath, const bool overwriteIfExists, const bool fileCopyAllowed) {				
				BOOST_LOG(lg) << "Calling [MoveFile] ";
				boost::filesystem::path source(sourcePath), destination(destinationPath);
				refresh_client_cache(sourcePath);
				refresh_client_cache(destinationPath);
				try {
					if (fileCopyAllowed) {
						if (!overwriteIfExists && boost::filesystem::exists(destination)) {
							set_response(_return, false, "overwriteIfExists: " + (overwriteIfExists ? std::string("[true]. ") : std::string("[false]. ")) +  destinationPath + " exists, move failed.");
							BOOST_LOG(lg) << "[MoveFile] - overwriteIfExists: " << (overwriteIfExists ? "[true] - " : "[false] - ") << destinationPath << " exists, move failed. ";
						}
						else {
							std::string cp_cmd = "cp ";
							cp_cmd += "-f " + sourcePath + " " + destinationPath;
							exec(cp_cmd.c_str());
							boost::filesystem::remove(source);
							set_response(_return, true, "Successfully moved " + sourcePath + " to " + destinationPath);
							BOOST_LOG(lg) << "[MoveFile] - Successfully moved " << sourcePath << " to " << destinationPath << " " ;
						}
					} 
					else {
						std::string cmd = "mv ";
						if (overwriteIfExists)
							cmd += "-f ";
						else
							cmd += "-n ";
						cmd += sourcePath + " " + destinationPath;
						exec(cmd.c_str());
						BOOST_LOG(lg) << "[MoveFile] - Running command - " << cmd << " " ;
						set_response(_return, true, "Successfully moved " + sourcePath + " to " + destinationPath);
						BOOST_LOG(lg) << "[MoveFile] - Successfully moved " << sourcePath << " to " << destinationPath << " " ;
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::CreateDirectory);
				}				
				return;
			}

			void XSMBServiceHandler::TruncateFile(LinuxFileResponse& _return, const std::string& filePath) {				
				BOOST_LOG(lg) << "Calling [TruncateFile] ";
				boost::filesystem::path file(filePath);
				try {
					refresh_client_cache(filePath);
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {
						set_response(_return, false, filePath + " does not exist or is not a file");
						BOOST_LOG(lg) << "[TruncateFile] - " << filePath << " does not exist or is not a file ";
					}
					else {
						boost::filesystem::fstream fs;
						fs.open(file, boost::filesystem::fstream::out);						
						fs.close();
						set_response(_return, true, "Successfully truncated " + filePath);
						BOOST_LOG(lg) << "[TruncateFile] - Successfully truncated " << filePath << " " ;
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::CreateDirectory);
				}				 
				return;
			}
		}
	}
}
