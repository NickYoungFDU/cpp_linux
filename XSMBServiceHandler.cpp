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

			void XSMBServiceHandler::PathExists(LinuxFileResponse& _return, const std::string& path) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "Calling [PathExists]\n";
				boost::filesystem::path b_path(path);
				try {
					if (boost::filesystem::exists(b_path)) {
						set_response(_return, true, path + " exists");
						log << "[PathExists] - " << path << " exists.\n";
					}
					else{
						set_response(_return, false, path + " does not exist");
						log << "[PathExists] - " << path << " does not exist.\n";
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::ListFile);
				}
				log.close();
			}

			void XSMBServiceHandler::CreateDirectory(LinuxFileResponse& _return, const std::string& dirPath) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "Calling [CreateDirectory]\n";

				boost::filesystem::path dir(dirPath);
				try {
					if (boost::filesystem::exists(dir) && boost::filesystem::is_directory(dir)) {						
						set_response(_return, false, dirPath + ": directory already exists");						
						log << "[CreateDirectory] - " << dirPath << ": directory already exists.\n";
					}
					else {
						boost::filesystem::create_directory(dir);
						set_response(_return, true, "Successfully created directory " + dirPath);
						log << "[CreateDirectory] - Successfully created directory " << dirPath << "\n" ;
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::CreateDirectory);
				}
				return;
			}

			void XSMBServiceHandler::DeleteDirectory(LinuxFileResponse& _return, const std::string& dirPath, const bool isRecursive) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "Calling [DeleteDirectory]\n";
				
				boost::filesystem::path dir(dirPath);
				try {
					if (!boost::filesystem::exists(dir) || !boost::filesystem::is_directory(dir)) {						
						set_response(_return, false, dirPath + " does not exist or is not a directory");
						log << "[DeleteDirectory] - " << dirPath << "does not exist or is not a directory\n";
					}
					else {
						if (boost::filesystem::is_empty(dir) || isRecursive) {							
							boost::filesystem::remove_all(dir);																					
							set_response(_return, true, "Successfully deleted directory " + dirPath);
							log << "[DeleteDirectory] - Successfully deleted directory " << dirPath << "\n" ;
						}
						else {							
							set_response(_return, false, dirPath + " is not empty while isRecursive is false");
							log << "[DeleteDirectory] - " << dirPath << " is not empty while isRecursive is false\n";
						}
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::DeleteDirectory);
				}
				log.close();
				return;
			}

			void XSMBServiceHandler::CreateFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t fileSize, const bool noBuffering) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "Calling [CreateFile]\n"; 
				
				boost::filesystem::path file(filePath);
				try {
					if (boost::filesystem::exists(file) && boost::filesystem::is_regular_file(file)) {				
						set_response(_return, false, filePath + " already exists");
						log << "[CreateFile] - " << filePath << " already exists\n";
					}
					else {
						/*
						boost::filesystem::fstream fs;
						fs.open(file, boost::filesystem::fstream::out);
						fs.seekp(fileSize - 1);
						fs.write("", 1);
						fs.close();					
						*/
						int fd = creat(filePath.c_str(), 0777);
						boost::filesystem::resize_file(file, (uintmax_t)fileSize);
						close(fd);
						set_response(_return, true, "Successfully created " + filePath);
						log << "[CreateFile] - Successfully created " << filePath << "\n" ;
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::CreateFile);
				}
				log.close();
				return;
			}

			void XSMBServiceHandler::DeleteFile(LinuxFileResponse& _return, const std::string& filePath) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "Calling [DeleteFile]\n";
				
				boost::filesystem::path file(filePath);
				try {
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {
						set_response(_return, false, filePath + " does not exist or is not a file");
						log << "[DeleteFile] - " << filePath << " does not exist or is not a file\n";
					}
					else {
						boost::filesystem::remove(file);
						set_response(_return, true, "Successfully deleted file " + filePath);
						log << "[DeleteFile] - Successfully deleted file " << filePath << "\n" ;
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::DeleteFile);
				}
				log.close();
				return;
			}

			void XSMBServiceHandler::ReadFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t offset, const int64_t count) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "Calling [ReadFile]\n";

				boost::filesystem::path file(filePath);
				try {
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {
						set_response(_return, false, filePath + " does not exist or is not a file");
						log << "[ReadFile] - " << filePath << " does not exist or is not a file\n";
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
						log << "[ReadFile] - Successfully read from file " << filePath << "\n" ;
						std::map<std::string, std::string> additional_info;
						_return.__set_AdditionalInfo(additional_info);												
						_return.AdditionalInfo.insert(std::pair<std::string, std::string>("BytesRead", bytes_read_string));
						_return.__set_Buffer(buffer_string);
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::ReadFile);
				}
				log.close();
				return;
			}

			void XSMBServiceHandler::WriteFile(LinuxFileResponse& _return, const std::string& filePath, const int64_t offset, const std::string& buffer, const int64_t count) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "Calling [WriteFile]\n";
				boost::filesystem::path file(filePath);
				try {
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {
						set_response(_return, false, filePath + " does not exist or is not a file");
						log << "[WriteFile] - " << filePath << " does not exist or is not a file\n";
					}
					else {
						std::fstream fs;
						fs.open(filePath.c_str());
						
						fs.seekp(offset);

						/* fix a bug here, should use 'count' rather than 'buffer.length()' in the following line of code */
						fs.write(buffer.c_str(), /* buffer.length() */ count);
						log << "buffer dump:\n";
						for (int i = 0; i < count; i++) {
							log << (((int)(buffer[i])) & 0xff) << " ";
						}
						log << "\n";
						fs.close();

						set_response(_return, true, "Successfully write to " + filePath);
						log << "[WriteFile] - Successfully write to " << filePath << "\n" ;
						log << "[WriteFile] - Dump - offset " << offset << " - count " << count << "\n";
						for (int i = 0; i < count; i++) {
							log << (int)buffer[i] << " ";
						}
						log << "\n";
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::WriteFile);
				}
				log.close();
				return;
			}

			boost::filesystem::recursive_directory_iterator create_recursive_directory_iterator(const boost::filesystem::path& path) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				try {
					return boost::filesystem::recursive_directory_iterator(path);
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					log << ex.what() << "\n" ;
					return boost::filesystem::recursive_directory_iterator();
				}
			}
			
			void dump(const boost::filesystem::path& path, int level) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				try {
					log << (boost::filesystem::is_directory(path) ? 'D' : ' ') << ' ';
					log << (boost::filesystem::is_symlink(path) ? 'L' : ' ') << ' ';
					for (int i = 0; i < level; i++)
						log << ' ';
					log << level << ' ' << path.filename() << "\n" ;
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					log << ex.what() << "\n" ;
				}
			}

			void list_directory_recursive(const boost::filesystem::path& path, std::map<std::string, MatchInformation::type> & files, std::map<std::string, MatchInformation::type> & dirs) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
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
						log << ex.what() << "\n" ;
						it.no_push();
						try {
							it++;
						}
						catch (...) {
							log << "!!\n";
						}
					}
				}
			}

			void XSMBServiceHandler::ListFiles(LinuxFileResponse& _return, const std::string& dirPath, const bool isRecursive) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				printf("Calling [ListCloudFiles]\n");
				boost::filesystem::path dir(dirPath);
				std::map<std::string, MatchInformation::type> files, dirs;
				try {
					list_directory_recursive(dir, files, dirs);
					set_response(_return, true, "Success");
					_return.__set_Directories(dirs);
					_return.__set_Files(files);
				}
				catch (const boost::filesystem::filesystem_error& ex) {
					log << ex.what() << "\n" ;
				}
				return;
			}

			void XSMBServiceHandler::GetFileLength(LinuxFileResponse& _return, const std::string& filePath) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "Calling [GetFileLength]\n";
				boost::filesystem::path file(filePath);				
				try {
					if (boost::filesystem::exists(file) && boost::filesystem::is_regular_file(file)) {			
						set_response(_return, true, "Successfully get file length for " + filePath);
						log << "[GetFileLength] - Successfully get file length for " << filePath << "\n" ;
						
						int64_t file_size =  (int64_t)boost::filesystem::file_size(file);
						std::string file_size_string = int_to_string(file_size);

						std::map<std::string, std::string> additional_info;
						_return.__set_AdditionalInfo(additional_info);
						_return.AdditionalInfo.insert(std::pair<std::string, std::string>("FileLength", file_size_string));						
					}
					else {
						set_response(_return, false, filePath + " does not exist or is not a file");
						log << "[GetFileLength] - " << filePath << " does not exist or is not a file\n";
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::GetFileLength);
				}			
				log.close();
				return;
			}

			void XSMBServiceHandler::SetFileLength(LinuxFileResponse& _return, const std::string& filePath, const int64_t fileLength) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "SetCloudFileLength\n";
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
				log.close();
				return;
			}

			void XSMBServiceHandler::OpenFileHandle(LinuxFileResponse& _return, const std::string& filePath, const LinuxFileMode::type fileMode, const LinuxFileAccess::type fileAccess, const int32_t handleId) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "OpenFileHandle\n";
				boost::filesystem::path file(filePath);
				try {	
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
					file_pointers.insert(std::pair<int, FILE*>(fd, file));

					log << "Open file " << filePath << ", descriptor " << fd << "\n";
					set_response(_return, true, "Sucessfully opened file handler [" + int_to_string(fd) + "]");

					std::map<std::string, std::string> additional_info;
					_return.__set_AdditionalInfo(additional_info);
					_return.AdditionalInfo.insert(std::pair<std::string, std::string>("FileDescriptor", int_to_string(fd)));
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::WriteFile);
				}
				log.close();
				return;
			}

			void XSMBServiceHandler::CloseFileHandle(LinuxFileResponse& _return, const int32_t handleId) {		
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "CloseFileHandle\n";
				try {
					std::map<int, FILE*>::iterator it = file_pointers.find(handleId);
					if (it != file_pointers.end()) {
						FILE* file = it->second;
						fclose(file);
						file_pointers.erase(it);
						close(it->first);						
						log << "Successfully closed file handle [" << handleId << "]\n";
						set_response(_return, true, "Successfully closed file handle [" + int_to_string(handleId) + "]");
					}
					else {
						log << "File handle [" << handleId << "] does not exist. It may have been closed.\n";
						set_response(_return, false, "File handle [" + int_to_string(handleId) + "] does not exist. It may have been closed.");
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::WriteFile);
				}
				log.close();
				return;
			}

			void XSMBServiceHandler::ReadFileByHandle(LinuxFileResponse& _return, const int32_t handleId, const int64_t offset, const int64_t count) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "ReadFileByHandle\n";
				try {
					std::map<int, FILE*>::iterator it = file_pointers.find(handleId);
					if (it != file_pointers.end() && it->second != NULL) {
						FILE* file = it->second;
						char* buffer = new char[count];
						fseek(file, offset, SEEK_SET);
						int64_t bytes_read = fread(buffer, sizeof(char), count, file);
						log << "handleId" << handleId << " - offset " << offset << " - count " << count << "\n";
						std::string bytes_read_string = int_to_string(bytes_read);
						std::string buffer_string = std::string(buffer, bytes_read < count ? bytes_read : count);
						log << "bytes_read: [" << bytes_read_string << "]\n";
						log << "buffer dump:\n";
						for (int i = 0; i < count; i++) {
							log << (((int)(buffer[i])) & 0xff)  << " ";
						}
						log << "\n";
						log << "buffer_string.length(): [" << buffer_string.length() << "]\n";
						set_response(_return, true, "Successfully read from file handle [" + int_to_string(handleId) + "]");

						std::map<std::string, std::string> additional_info;
						_return.__set_AdditionalInfo(additional_info);
						_return.AdditionalInfo.insert(std::pair<std::string, std::string>("BytesRead", bytes_read_string));
						_return.__set_Buffer(buffer_string);
					}
					else {
						log << "file handle [" << handleId << "] does not exist, or somehow it has been closed.\n";
						set_response(_return, false, "file handle [" + int_to_string(handleId) + "] does not exist, or somehow it has been closed.");
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::WriteFile);
				}
				log.close();
				return;
			}

			void XSMBServiceHandler::WriteFileByHandle(LinuxFileResponse& _return, const int32_t handleId, const int64_t offset, const std::string& buffer, const int64_t count) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "WriteFileByHandle\n";
				try {								
					std::map<int, FILE*>::iterator it = file_pointers.find(handleId);
					if (it != file_pointers.end() && it->second != NULL) {
						log << "Start writing...\n";
						log << "handleId" << handleId << " - offset " << offset << " - count " << count << "\n";
						FILE* file = it->second;
						fseek(file, offset, SEEK_SET);
						fwrite(buffer.c_str(), sizeof(char), /* buffer.length() */ count, file);
						fflush(file);
						log << "buffer dump:\n";
						for (int i = 0; i < count; i++) {
							log << (((int)(buffer[i])) & 0xff) << " ";
						}
						log << "\n";
						set_response(_return, true, "Successfully write to file handle [" + int_to_string(handleId) + "]");
					}
					else {
						log << "file handle [" << handleId << "] does not exist, or somehow it has been closed.\n";
						set_response(_return, false, "file handle [" + int_to_string(handleId) + "] does not exist, or somehow it has been closed.");
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::WriteFile);
				}
				log.close();
				return;
			}

			void XSMBServiceHandler::CopyFile(LinuxFileResponse& _return, const std::string& sourcePath, const std::string& destinationPath, const bool overwriteIfExists) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "Calling [CopyFile]\n";
				boost::filesystem::path source(sourcePath), destination(destinationPath);
				try {
					if (!overwriteIfExists && boost::filesystem::exists(destination)) {
						set_response(_return, false, destinationPath + " exists, copy failed.");
						log << destinationPath << " exists, copy failed.\n";
					}
					else {
						/*
						boost::filesystem::copy_file(source, destination, boost::filesystem::copy_option::overwrite_if_exists);
						*/
						std::string cmd = "cp ";
						if (overwriteIfExists)
							cmd += "-f ";
						else
							cmd += "-n ";
						cmd += sourcePath + " " + destinationPath;
						exec(cmd.c_str());
						log << "[CopyFile] - Running command - " << cmd << "\n" ;
						set_response(_return, true, "Succesfully copied " + sourcePath + " to" + destinationPath);
						log << "[CopyFile] - Source - " << sourcePath << " - Destination - " << destinationPath << "\n" ;
					}				
				} 
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::CreateDirectory);
				}
				log.close();
				return;
			}

			void XSMBServiceHandler::MoveFile(LinuxFileResponse& _return, const std::string& sourcePath, const std::string& destinationPath, const bool overwriteIfExists, const bool fileCopyAllowed) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "Calling [MoveFile]\n";
				boost::filesystem::path source(sourcePath), destination(destinationPath);
				try {
					if (fileCopyAllowed) {
						if (!overwriteIfExists && boost::filesystem::exists(destination)) {
							set_response(_return, false, "overwriteIfExists: " + (overwriteIfExists ? std::string("[true]. ") : std::string("[false]. ")) +  destinationPath + " exists, move failed.");
							log << "[MoveFile] - overwriteIfExists: " << (overwriteIfExists ? "[true] - " : "[false] - ") << destinationPath << " exists, move failed.\n";
						}
						else {
							//boost::filesystem::copy_file(source, destination);
							std::string cp_cmd = "cp ";
							cp_cmd += "-f " + sourcePath + " " + destinationPath;
							exec(cp_cmd.c_str());
							boost::filesystem::remove(source);
							set_response(_return, true, "Successfully moved " + sourcePath + " to " + destinationPath);
							log << "[MoveFile] - Successfully moved " << sourcePath << " to " << destinationPath << "\n" ;
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
						log << "[MoveFile] - Running command - " << cmd << "\n" ;
						set_response(_return, true, "Successfully moved " + sourcePath + " to " + destinationPath);
						log << "[MoveFile] - Successfully moved " << sourcePath << " to " << destinationPath << "\n" ;
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::CreateDirectory);
				}
				log.close();
				return;
			}

			void XSMBServiceHandler::TruncateFile(LinuxFileResponse& _return, const std::string& filePath) {
				std::ofstream log("/home/xufyan/cpp_linux/logfile.txt", std::ios_base::app | std::ios_base::out);
				log << "Calling [TruncateFile]\n";
				boost::filesystem::path file(filePath);
				try {
					if (!boost::filesystem::exists(file) || !boost::filesystem::is_regular_file(file)) {
						set_response(_return, false, filePath + " does not exist or is not a file");
						log << "[TruncateFile] - " << filePath << " does not exist or is not a file\n";
					}
					else {
						boost::filesystem::fstream fs;
						fs.open(file, boost::filesystem::fstream::out);						
						fs.close();
						set_response(_return, true, "Successfully truncated " + filePath);
						log << "[TruncateFile] - Successfully truncated " << filePath << "\n" ;
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::CreateDirectory);
				}
				log.close();
				return;
			}


		}
	}
}
