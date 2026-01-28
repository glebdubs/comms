CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
LDFLAGS = -lcrypto

TARGET = server

SRCS = main.cpp parts/encryption.cpp parts/utils.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean