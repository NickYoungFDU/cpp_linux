#include <FileShareServiceHandler.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <infrastructure.h>
#include <thread>
#include <chrono>

using boost::shared_ptr;

using namespace  ::azure::storage::cpp_linux;

namespace azure {
	namespace storage {
		namespace cpp_linux
		{
			void FileShareServiceHandler::MapFileShare(LinuxFileResponse& _return, const std::string& smbShareAddress, const std::string& username, const std::string& password, const std::string& mountPoint) {				
				printf("MapFileShare");
				boost::filesystem::path mountPointPath(mountPoint);
				try {
					if (!boost::filesystem::exists(mountPointPath)) {
						boost::filesystem::create_directory(mountPointPath);
					}

					std::string xsmbVersion = "3.0";
					std::string mountCommand = "mount -t cifs " + smbShareAddress + " " + mountPoint + " -o vers='" + xsmbVersion + "',username='" + username + "',password='" + password + "',dir_mode=0777,file_mode=0777";
					std::string ret = exec(mountCommand.c_str());
					std::cout << ret << std::endl;
					set_response(_return, true, "Successfully mapped " + mountPoint + " to " + smbShareAddress);
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::MapFileShare);
				}
			}
			void FileShareServiceHandler::UnmapFileShare(LinuxFileResponse& _return, const std::string& mountPoint) {				
				printf("UnmapFileShare");
				try {
					std::string cmd = "umount " + mountPoint + " 2>&1";
					std::string ret = exec(cmd.c_str());
					std::cout << "cmd output:" << ret << std::endl;
					/*
					bool isUmounted = false;
					std::string umountCmd = "umount " + mountPoint;
					std::string detectCmd = "df -T " + mountPoint + " | tail -n +2";
					std::string ret;
					std::chrono::seconds timespan(1);
					while (!isUmounted) {
						exec(umountCmd.c_str());						
						ret = exec(detectCmd.c_str());
						isUmounted = ret.find("cifs") == std::string::npos;
						if (isUmounted) break;
						std::this_thread::sleep_for(timespan);
						timespan *= 2;
					}					
					std::string removeCmd = "rm -rf " + mountPoint;										
					ret = exec(removeCmd.c_str());
					std::cout << ret << std::endl;
					set_response(_return, true, "Successfully unmapped " + mountPoint);
					*/
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::UnmapFileShare);
				}
			}			
		}
	}
}
