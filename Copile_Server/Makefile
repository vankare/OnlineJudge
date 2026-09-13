all: Compile Runner Compile_Run Compile_Server

Compile_Server: Compile_Server.cpp
	g++ -g $^ -o $@

Compile_Run: Compile_Run.cpp
	g++ -g $^ -o $@

Compile: Compile.cpp
	g++ -g $^ -o $@

Runner: Runner.cpp
	g++ -g $^ -o $@

.PHONY: clean
clean:
	rm -f Compile_Server Compile_Run Compile Runner