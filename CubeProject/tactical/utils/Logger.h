#ifndef _LOGGER_FILE_H_
#define _LOGGER_FILE_H_

#include "../Common.h"

#define LOG (*tactical::utils::Logger::GetInstance())
#define LOGTYPE tactical::utils::Logger::logType

namespace tactical 
{
	namespace utils 
	{
		class Logger
		{
		public:
			enum class logType { LOG_ERROR, LOG_WARNING, LOG_INFO };

			static Logger* GetInstance() {
				if (m_instance == NULL) {
					m_instance = new Logger("log.txt", false);
				}
				return m_instance;
			}

			~Logger();

			friend Logger& operator<< (Logger &logger, const logType type);
			friend Logger& operator << (Logger &logger, const std::string text);
			friend Logger& operator << (Logger &logger, const char* text);

			Logger(const Logger &) = delete;
			Logger &operator= (const Logger &) = delete;

			void SetConsoleLogging(bool option) { m_console = option; }

		protected:
			Logger(const std::string & fileName, bool console = false);
			void UpdateTimeString();

			static Logger* m_instance;

			unsigned int m_numErrors;
			unsigned int m_numWarning;

			std::ofstream m_file;

			time_t m_currentTime;
			struct tm* m_now;
			std::string m_timeString;

			bool m_console;
		};
	}
}


#endif