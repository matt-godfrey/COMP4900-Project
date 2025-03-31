# Compiler and flags
CXX = q++
CXXFLAGS = -g -std=c++17 -Iinclude -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lc -lm -lsocket
# -L/home/matt/qnx710/target/qnx7/x86_64/lib maybe add this above
# -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L enable certain POSIX features; might not be necessary

# Executable
EXECUTABLES = PacketProcessor

# Source files
SOURCES = src/main.cpp src/PacketReceiver.cpp src/FlowQueue.cpp \
          src/PacketProcessor.cpp src/RateLimiter.cpp src/DriverManager.cpp src/NetworkTask.cpp
#SOURCES = src/main.cpp src/PacketReceiver.cpp

all: $(EXECUTABLES)

PacketProcessor: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(EXECUTABLES)

