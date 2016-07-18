SUBDIRS = src/daemon src/client
dist_doc_DATA = README.md
#CXXFLAGS= -Wall -g -std=gnu++11 -I /usr/local/include
LDFLAGS = -L/usr/lib -L/usr/local/lib
#LIBS = libjsoncpp.a -lboost_regex -lboost_system -lboost_filesystem -lpthread
install-exec-hook:
	./post-install.sh
