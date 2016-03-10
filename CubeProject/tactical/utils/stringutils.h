#include "../Common.h"

namespace tactical
{
	namespace utils
	{
		namespace
		{
#ifdef _WIN32
			struct MatchPathSeparator
			{
				bool operator()(char ch) const
				{
					return ch == '/';
				}
			};

#else
			struct MatchPathSeparator
			{
				bool operator()(char ch) const
				{
					return ch == '\\' || ch == '/';
				}
			};
#endif

			struct Filename
			{
				std::string filename;
				std::string extension;

				Filename() {}
				Filename(std::string f, std::string ext) : filename(f), extension(ext) {}
			};

			std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
				std::stringstream ss(s);
				std::string item;
				while (std::getline(ss, item, delim)) {
					elems.push_back(item);
				}
				return elems;
			}


			std::vector<std::string> split(const std::string &s, char delim) {
				std::vector<std::string> elems;
				split(s, delim, elems);
				return elems;
			}

			Filename split_name_and_ext(const std::string& s) {
				std::string filename = std::string(
					std::find_if(s.rbegin(), s.rend(),
						MatchPathSeparator()).base(),
					s.end());

				std::size_t extensionPosition = filename.find_last_of(".");
				return Filename(filename.substr(0, extensionPosition), filename.substr(extensionPosition + 1));

			}
		}
	}
}