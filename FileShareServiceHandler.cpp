#include <FileShareServiceHandler.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <infrastructure.h>

using boost::shared_ptr;

using namespace  ::azure::storage::cpp_linux;

namespace azure {
	namespace storage {
		namespace cpp_linux
		{
			void FileShareServiceHandler::MapFileShare(LinuxFileResponse& _return, const std::string& smbShareAddress, const std::string& username, const std::string& password, const std::string& mountPoint) {
				// Your implementation goes here
				printf("MapFileShare");
				boost::filesystem::path mountPointPath(mountPoint);
				try {
					if (!boost::filesystem::exists(mountPointPath)) {
						boost::filesystem::create_directory(mountPointPath);
					}

					std::string xsmbVersion = "2.1";
					std::string mountCommand = "mount -t cifs " + smbShareAddress + " " + mountPoint + " -o vers='" + xsmbVersion + "',username='" + username + "',password='" + password + "',dir_mode=0777,file_mode=0777";
					std::string ret = exec(mountCommand.c_str());
					std::cout << ret << std::endl;
					SetResponse(_return, true, "Successfully mapped " + mountPoint + " to " + smbShareAddress);
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::MapFileShare);
				}
			}

			void FileShareServiceHandler::UnmapFileShare(LinuxFileResponse& _return, const std::string& mountPoint) {
				// Your implementation goes here
				printf("UnmapFileShare");
				try {
					std::string unmountCommand = "umount " + mountPoint;
					std::string ret = exec(unmountCommand.c_str());
					std::cout << ret << std::endl;
					SetResponse(_return, true, "Successfully unmapped " + mountPoint);
				}
				catch (const std::exception& ex) {
					throw GetException(ex.what(), OperationType::UnmapFileShare);
				}
			}

						
			std::string FileShareServiceHandler::exec(const char* cmd) {
				char buffer[128];
				std::string result = "";
				FILE* pipe = popen(cmd, "r");
				if (!pipe) throw std::runtime_error("popen() failed!");
				try {
					while (!feof(pipe)) {
						if (fgets(buffer, 128, pipe) != NULL)
							result += buffer;
					}
				}
				catch (...) {
					pclose(pipe);
					throw;
				}
				pclose(pipe);
				return result;
			}
			
		}
	}
}
