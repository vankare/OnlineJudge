oj_server: OJ_Server.cc
	g++ -g -o $@ $^ -lpthread -lctemplate -lhttplib -ljsoncpp \
	   	-I../Comm -L../Comm/lib/ -DCPPHTTPLIB_COMPILE \
	   	-std=c++17

.PHONY: clean
clean:
	rm oj_server
