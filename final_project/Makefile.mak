CXX = g++
CXXFLAGS = -Wall

SRCS = cdcl.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = cdcl

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(TARGET)
