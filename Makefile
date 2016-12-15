CXX := g++
CC := g++
CXXFLAGS :=
CXXFLAGS += -g
CXXFLAGS += -Weffc++
CXXFLAGS += -std=c++11
TARGET := stermcom
OBJS :=
OBJS += $(patsubst %.cc,%.o,$(wildcard *.cc))
INSTALLROOTPATH := /usr/local
INSTALLBINPATH  := $(INSTALLROOTPATH)/bin
INSTALLMAN1PATH  := $(INSTALLROOTPATH)/man/man1

.PHONY : release debug clean install uninstall

release : CXXFLAGS += -DNDEBUG
release : $(TARGET)

debug : CXXFLAGS += -g -O0
debug : CXXFLAGS += -DPRIVATE_DEBUG
debug : $(TARGET)

$(TARGET) : $(OBJS)

clean:
	$(RM) -v $(TARGET) $(OBJS)

install:
	mkdir -p $(INSTALLBINPATH)
	cp $(TARGET) $(INSTALLBINPATH)
	mkdir -p $(INSTALLMAN1PATH)
	cp $(TARGET).1 $(INSTALLMAN1PATH)

uninstall:
	$(RM) -v $(INSTALLBINPATH)/$(TARGET)
	$(RM) -v  $(INSTALLMAN1PATH)/$(TARGET).1

