#ifndef _LOGGER_FILE_H_
#define _LOGGER_FILE_H_

#include "../Common.h"

#define LOG (*tactical::utils::Logger::GetInstance())
#define LOGTYPE tactical::utils::Logger::logType
#define LOG_INFO(str) LOG << LOGTYPE::LOG_INFO << str
#define LOG_WARNING(str) LOG << LOGTYPE::LOG_WARNING << str
#define LOG_ERROR(str) LOG << LOGTYPE::LOG_ERROR << str
#define LOG_OPENGL() logOpenGL()

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

	static void logOpenGL()
	{
		GLenum errorGL = glGetError();
		if (errorGL != GL_NO_ERROR) {
			while (errorGL != GL_NO_ERROR) {
				LOG << LOGTYPE::LOG_WARNING << "OpenGL error: " + std::to_string(errorGL);
				errorGL = glGetError();
			}
		}
	}
}


#endif