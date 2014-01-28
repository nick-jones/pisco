THRIFT_HEADERS := src/gen-cpp/pisco_constants.h src/gen-cpp/pisco_types.h src/gen-cpp/Search.h
THRIFT_SOURCE := src/gen-cpp/Search.cpp src/gen-cpp/pisco_constants.cpp src/gen-cpp/pisco_types.cpp
THRIFT_FILES := $(THRIFT_HEADERS) $(THRIFT_SOURCE)

PISCO_SOURCE := src/Search_server.cpp src/Match.cpp src/Database.cpp

SOURCE := $(PISCO_SOURCE) $(THRIFT_SOURCE)
OBJECTS := $(patsubst %.cpp,%.o, $(SOURCE))
DEPENDENCIES := $(OBJECTS:.o=.d)

THRIFT_DIR := /usr/local/include/thrift
BOOST_DIR := /usr/local/include

INC := -I$(THRIFT_DIR) -I$(BOOST_DIR)
CXX_FLAGS := -std=c++11 -Wall -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -MMD -MP $(INC)
LD_FLAGS := -L/usr/local/lib -L/usr/include/boost -lthrift -lthriftnb -levent -lpthread

all: pisco

tar:
	tar cvf pisco.tar def/pisco.thrift src/*.cpp src/*.h Makefile README.md

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

pisco: $(OBJECTS)
	$(CXX) $(LD_FLAGS) $^ -o $@

clean:
	$(RM) -r src/*.o src/*.d src/gen-cpp pisco pisco.tar

$(PISCO_SOURCE): $(THRIFT_FILES)

$(THRIFT_FILES): def/pisco.thrift
	thrift --gen cpp -o src/ $<

-include $(DEPENDENCIES)
