#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

int OpenFileHandle(std::string filePath) {
	int fd = open(filePath.c_str(), O_RDWR);
	if (fd == -1) {
		printf("open() failed with error[%s]\n", strerror(errno));				
	}
	return fd;
}

void CreateFile(std::string filePath) {
	boost::filesystem::path file(filePath);
	int fd = creat(filePath.c_str(), 0777);
	int fileSize = 64;
	boost::filesystem::resize_file(file, (uintmax_t)fileSize);
	close(fd);
}

void LockFileRangeTest(std::string filePath) {
	if (!boost::filesystem::exists(boost::filesystem::path(filePath)))
		CreateFile(filePath);
	int fd = OpenFileHandle(filePath);

	/* Lock file range [0, 4). Exclusive. */
	std::cout << "Lock file range [0, 4). Exclusive." << std::endl;
	struct flock exLock;
	memset(&exLock, 0, sizeof(exLock));
	exLock.l_type = F_WRLCK;
	exLock.l_whence = SEEK_SET;
	exLock.l_start = 0;
	exLock.l_len = 4;
	int result = fcntl(fd, F_SETLK, &exLock);
	std::cout << result << std::endl;
	try {
		FILE* file = fopen(filePath.c_str(), "rb+");
		fseek(file, 0, SEEK_SET);
		fwrite("abcd", sizeof(char), 4, file);
	}
	catch (const std::exception &ex) {
		std::cout << ex.what() << std::endl;
	}
}