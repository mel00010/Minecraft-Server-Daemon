#ifndef MAINLOOP_H
#define MAINLOOP_H
#include "log4cpp/Category.hh"
#include <event2/event.h>

void mainLoop (std::vector<Server*>* servers, log4cpp::Category& root, evutil_socket_t controlSocket, struct event_base *base);
void writeToSocket(std::string message, int controlSocket, log4cpp::Category& root);
std::string readFromSocket(int controlSocket, log4cpp::Category& root);
void cb(int sock, short what, void *arg);
void recieveCommand(int controlSocket, short what, void *arg);
#endif /* MAINLOOP_H */
