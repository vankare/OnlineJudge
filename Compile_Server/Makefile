compile_server: Compile_Server.cc
	g++ $^ -o $@ -lhttplib -ljsoncpp -lpthread -std=c++17 \
		-I../Comm -L../Comm/lib/


.PHONY: clean
clean:
	rm -f compile_server
