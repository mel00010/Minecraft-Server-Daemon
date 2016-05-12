#ifndef MAINLOOP_H
#define MAINLOOP_H
void mainLoop (std::vector<Server*>* servers, log4cpp::Category& root);
void writeToPipe(std::string message);
void cb(int sock, short what, void *arg);
#endif /* MAINLOOP_H */
