#include <unistd.h>
#include <limits.h>
#include <string>
#include <iostream>

std::string getExecutablePath() {
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return std::string(result, (count > 0) ? count : 0);
}

int main() {
	std::cout << "Executable Path: " << getExecutablePath() << std::endl;
	return 0;
}
