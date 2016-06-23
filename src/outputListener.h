#ifndef OUTPUT_LISTENER_H
#define OUTPUT_LISTENER_H
#include <sstream>
#include <functional>
#include <memory>
#include "log4cpp/Category.hh"
class OutputListener {
	public:
		explicit OutputListener(size_t linesRequested, std::string listenerName, bool persistent, std::function<void(size_t, std::stringstream*, log4cpp::Category*)> callback) : linesRequested{linesRequested}, listenerName{listenerName}, persistent{persistent}, callback{callback} {};
		virtual ~OutputListener()
		{
			delete lines;
		};
		std::string listenerName;
		size_t linesRequested;
		size_t currentLine = 0;
		bool persistent;
		std::stringstream* lines = new std::stringstream;
		std::function<void(size_t, std::stringstream*, log4cpp::Category*)> callback;
};
#endif /* OUTPUT_LISTENER_H */
