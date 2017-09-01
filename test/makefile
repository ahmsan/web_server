#$(CXX) -static  -o $@ $(OBJS) $(LIBPATH) $(LIB)
CXX		= g++
CFLAGS	= -g -Wall -O2

VPATH := ./../src/:$(VPATH)

LINK_INC :=  -I./../src/ -I./../src/common/ $(LINK_INC)

.cpp.o:
	$(CXX) $(CFLAGS) ${LINK_INC} -c $< -o $@

TARGET = ws_unit_test

all: $(TARGET)
$(TARGET): ws_unit_test.o ws_conf_parser.o ws_http_request.o ws_parser.o
	$(CXX) -o $@ .o $(patsubst %.o


.PHONY: clean
clean:
	rm -rf *.o $(TARGET) 