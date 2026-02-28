CC = g++
CFLAGS = -I ./include -I ../../boost/Tools -I /ToolAnalysis/ToolDAQ/boost_1_66_0/install/include `root-config --cflags`
LDFLAGS = -L /ToolAnalysis/ToolDAQ/boost_1_66_0/install/lib -lboost_date_time -lboost_serialization -lboost_iostreams `root-config --libs`
TARGET = BoostStore
SOURCES = BoostStore101.cpp ../../boost/Tools/BoostStore.cpp src/CardData.cpp
OBJECTS = $(SOURCES:.cpp=.o)
all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJECTS) $(TARGET) *.bin *.data *.txt *.root src/*.o
.PHONY: all clean
