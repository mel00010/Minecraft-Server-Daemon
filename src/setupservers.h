#ifndef SETUPSERVERS_H
#define SETUPSERVERS_H
#include "server.h"
#include <vector>
#include <json/json.h>
std::vector<Server*>* setupServers(Json::Value* _config);
#endif /* SETUPSERVERS_H */
