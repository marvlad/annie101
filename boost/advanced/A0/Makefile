CC = g++ 
CFLAGS = -I ./include -I ../../Tools  -I /opt/homebrew/Cellar/boost/1.89.0/include
LDFLAGS = -L /opt/homebrew/Cellar/boost/1.89.0/lib -lboost_date_time -lboost_serialization -lboost_iostreams

TARGET = BoostStore

SOURCES = BoostStore101.cpp ../../Tools/BoostStore.cpp src/TriggerData.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
        $(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
        $(CC) $(CFLAGS) -c $< -o $@

clean:
        rm -f $(OBJECTS) $(TARGET) *.bin *.data *.txt

.PHONY: all clean
