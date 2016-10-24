#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

int OpenFileHandle(std::string filePath);

int CreateFile(std::string filePath);

int LockFileRangeTest(std::string filePath);