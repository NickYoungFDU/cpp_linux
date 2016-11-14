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
				BOOST_LOG(lg) << "Calling [MapFileShare]";
				boost::filesystem::path mountPointPath(mountPoint);
				try {
					if (!boost::filesystem::exists(mountPointPath)) {
						BOOST_LOG(lg) << "[MapFileShare] " << mountPoint << " does not exist, creating it..";
						boost::filesystem::create_directory(mountPointPath);
					}
					std::string xsmbVersion = "3.0";
					std::string mountCommand = "mount -t cifs " + smbShareAddress + " " + mountPoint + " -o iocharset=utf8,vers='" + xsmbVersion + "',username='" + username + "',password='" + password + "',dir_mode=0777,file_mode=0777 2>&1";
					BOOST_LOG(lg) << "[MapFileShare] Excecuting '" << mountCommand << "'";
					std::string ret = exec(mountCommand.c_str());
					BOOST_LOG(lg) << "[MapFileShare] Command result: " << ret;
					if (ret.find("error") != std::string::npos) {
						set_response(_return, false, "Failed to map " + mountPoint + ". Error Message: " + ret);
					}
					else {
						set_response(_return, true, "Successfully mapped " + mountPoint + " to " + smbShareAddress);
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::MapFileShare);
				}
			}
			void FileShareServiceHandler::UnmapFileShare(LinuxFileResponse& _return, const std::string& mountPoint) {				
				BOOST_LOG(lg) << "Calling [UnmapFileShare]";
				try {										
					bool isUmounted = false;
					std::string umountCmd = "umount " + mountPoint + " 2>&1";
					std::string detectCmd = "df -T " + mountPoint + " | tail -n +2";
					std::string ret;
					std::chrono::seconds timespan(1);
					int maxRetryCount = 5;
					int attempt = 0;
					while (attempt < maxRetryCount) {
						BOOST_LOG(lg) << "[UnmapFileShare] Attempt " << attempt << ":";
						BOOST_LOG(lg) << "[UnmapFileShare] Excecuting '" << umountCmd << "'";
						exec(umountCmd.c_str());						
						BOOST_LOG(lg) << "[UnmapFileShare] Detecting if " << mountPoint << " is unmounted";
						ret = exec(detectCmd.c_str());
						BOOST_LOG(lg) << "[UnmapFileShare] Detect result: " << ret;
						isUmounted = ret.find("cifs") == std::string::npos;
						BOOST_LOG(lg) << "[UnmapFileShare] " << mountPoint << " state: " << (isUmounted ? "Unmounted" : "Still mounted");
						if (isUmounted) break;
						attempt++;
						BOOST_LOG(lg) << "[UnmapFileShare] Sleeping...";
						std::this_thread::sleep_for(timespan);
						timespan *= 2;
					}					
					if (isUmounted) {
						BOOST_LOG(lg) << "[UnmapFileShare] Successfully unmounted " << mountPoint << ". Removing it...";						
						std::string removeCmd = "rm -rf " + mountPoint + " 2>&1";
						BOOST_LOG(lg) << "[UnmapFileShare] Excecuting '" << removeCmd << "'";
						ret = exec(removeCmd.c_str());
						BOOST_LOG(lg) << "[UnmapFileShare] Removal result: " << ret;
						set_response(_return, true, "Successfully unmapped " + mountPoint);
					}
					else {
						BOOST_LOG(lg) << "[UnmapFileShare] Failed to unmounted " << mountPoint << " after " << maxRetryCount << " attempts";
						set_response(_return, false, "Failed to unmap " + mountPoint);
					}
				}
				catch (const std::exception& ex) {
					throw set_exception(ex.what(), OperationType::UnmapFileShare);
				}
			}			
		}
	}
}
