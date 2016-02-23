#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

#include "../Common.h"

namespace tactical
{
	namespace utils
	{ namespace {
		bool read_file(const char* fileName, std::string& outFile)
		{
			std::ifstream f(fileName);

			bool ret = false;

			if (f.is_open()) {
				std::string line;
				while (std::getline(f, line)) {
					outFile.append(line);
					outFile.append("\n");
				}

				f.close();

				ret = true;
			}

			return ret;
		} }
	}
}

#endif