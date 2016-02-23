#include "Logger.h"

namespace tactical
{
	namespace utils
	{
		Logger* Logger::m_instance = NULL;

		Logger::Logger(const std::string & fileName, bool console) : m_numErrors(0U), m_numWarning(0U), m_console(console)
		{
			m_file.open(fileName.c_str(), std::ios::app);

			if (m_file.is_open()) {
				UpdateTimeString();

				m_file << "<" + m_timeString + "> - Session Initiated." << std::endl;
				m_file << std::endl;

				if (m_console) {
					std::cout << "<" + m_timeString + "> - Session Initiated." << std::endl;
					std::cout << std::endl;
				}
			}
		}

		Logger::~Logger()
		{
			if (m_file.is_open()) {
				UpdateTimeString();
				m_file << std::endl;
				m_file << "<" + m_timeString + "> - There were " << m_numWarning << " warnings and " << m_numErrors << " errors." << std::endl;
				m_file << std::endl << "---------------------------------------" << std::endl;
				m_file << std::endl;

				if (m_console) {
					std::cout << std::endl;
					std::cout << "<" + m_timeString + "> - There were " << m_numWarning << " warnings and " << m_numErrors << " errors." << std::endl;
					m_file << std::endl << "---------------------------------------" << std::endl;
					std::cout << std::endl;
				}

				m_file.close();
			}
		}

		Logger& operator<< (Logger &logger, const Logger::logType type) {
			logger.UpdateTimeString();
			switch (type) {
			case Logger::logType::LOG_ERROR:
				logger.m_file << "<" + logger.m_timeString + "> - [ERROR]: ";

				if (logger.m_console) std::cout << "<" + logger.m_timeString + "> - [ERROR]: ";

				++logger.m_numErrors;
				break;

			case Logger::logType::LOG_WARNING:
				logger.m_file << "<" + logger.m_timeString + "> - [WARNING]: ";

				if (logger.m_console) std::cout << "<" + logger.m_timeString + "> - [WARNING]: ";

				++logger.m_numWarning;
				break;

			case Logger::logType::LOG_INFO:
				logger.m_file << "<" + logger.m_timeString + "> - [INFO]: ";
				if (logger.m_console) std::cout << "<" + logger.m_timeString + "> - [INFO]: ";
				break;
			}

			return logger;
		}

		Logger &operator << (Logger &logger, const std::string text) {
			logger.m_file << text << std::endl;
			if (logger.m_console) std::cout << text << std::endl;
			return logger;
		}

		Logger &operator << (Logger &logger, const char* text) {
			logger.m_file << text << std::endl;
			if (logger.m_console) std::cout << text << std::endl;
			return logger;
		}

		void Logger::UpdateTimeString() {
			m_currentTime = time(0);
			m_now = localtime(&m_currentTime);

			char buff[10];
			_snprintf(buff, sizeof(buff), "%02d:%02d:%02d", m_now->tm_hour, m_now->tm_min, m_now->tm_sec);

			m_timeString = buff;
		}
	}
}