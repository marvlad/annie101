CC = g++
CFLAGS = -I ./include  -I ../../Tools  -I /opt/homebrew/Cellar/boost/1.89.0/include -I ../../Tools/zmq/zeromq-4.0.7/include
LDFLAGS = -L /opt/homebrew/Cellar/boost/1.89.0/lib -lboost_date_time -lboost_serialization -lboost_iostreams -L ../../Tools/zmq/zeromq-4.0.7/lib -lzmq

TARGET = BoostStore

SOURCES = BoostStore101.cpp ../../Tools/BoostStore.cpp src/CardData.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) *.bin *.data *.txt src/*.o

.PHONY: all clean
