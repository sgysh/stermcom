CXX := g++
CC := g++
CXXFLAGS :=
CXXFLAGS += -g
CXXFLAGS += -Weffc++
CXXFLAGS += -std=c++11
TARGET := stermcom
OBJS :=
OBJS += $(patsubst %.cc,%.o,$(wildcard *.cc))

.PHONY : release debug clean

release : CXXFLAGS += -DNDEBUG
release : $(TARGET)

debug : CXXFLAGS += -g -O0
debug : CXXFLAGS += -DPRIVATE_DEBUG
debug : $(TARGET)

$(TARGET) : $(OBJS)

clean:
	$(RM) -v $(TARGET) $(OBJS)

