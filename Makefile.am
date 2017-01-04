SUBDIRS = docs config daemon/src client/src
dist_doc_DATA = README.md
#CXXFLAGS= -Wall -g -std=gnu++11 -I /usr/local/include
LDFLAGS = -L/usr/lib -L/usr/local/lib
#LIBS = libjsoncpp.a -lboost_regex -lboost_system -lboost_filesystem -lpthread
install-exec-hook:
	CONFIG_DIR_NAME=$(CONFIG_DIR_NAME) CONFIG_FILE_NAME=$(CONFIG_FILE_NAME) LOG_DIR=$(LOG_DIR) ./post-install.sh
