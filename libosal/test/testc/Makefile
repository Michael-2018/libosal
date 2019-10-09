#
# Bin
#
BIN_NAME   = osal_testcpp
VERSION    = 1.0.0

#
# Target
#
TARGET = $(BIN_NAME)-$(VERSION)

#
# Message: Install ok
#
INSTALL_OK_MSG = "Install $(TARGET) OK!"

#
# path: source code
#
DIR_OSAL_APP_INC    = $(ROOTDIR)/inc
DIR_OSAL_APP_SRC    = $(ROOTDIR)/src
DIR_OSAL_CORE       = $(DIR_OSAL_APP_SRC)/core
DIR_OSAL_UTIL       = $(DIR_OSAL_APP_SRC)/util
DIR_OSAL_UTIL_TOPIC = $(DIR_OSAL_UTIL)/topic

#
# path: test
#
DIR_TEST = $(ROOTDIR)/test/testcpp

#
# build custom flags
#
FLAGS += -I$(DIR_OSAL_APP_INC)
FLAGS += -I$(DIR_OSAL_APP_SRC)
FLAGS += -I$(DIR_OSAL_CORE)
FLAGS += -I$(DIR_OSAL_UTIL)
FLAGS += -I$(DIR_OSAL_UTIL_TOPIC)
FLAGS += -I$(DIR_TEST)

FLAGS += -fPIC -O2

#
# build compile and link flags
#
CFLAGS   += $(FLAGS)
CXXFLAGS += -std=c++11 $(FLAGS)
LDFLAGS  += -lpthread

SRC_FILES  = $(shell find $(DIR_OSAL_APP_SRC) -name "*.c" -o -name "*.cpp")
SRC_FILES  = $(shell find $(DIR_OSAL_CORE) -name "*.c" -o -name "*.cpp")
SRC_FILES  = $(shell find $(DIR_OSAL_UTIL) -name "*.c" -o -name "*.cpp")
SRC_FILES  = $(shell find $(DIR_OSAL_UTIL_TOPIC) -name "*.c" -o -name "*.cpp")
SRC_FILES += $(shell find $(DIR_TEST) -name "*.c" -o -name "*.cpp")

OBJS      += $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SRC_FILES)))

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

clean:
	-rm -f $(OBJS)
	-rm -f $(TARGET)

install:
	cp $(TARGET) $(DESTDIR)
	echo $(INSTALL_OK_MSG) 
#end
