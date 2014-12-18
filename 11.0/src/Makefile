MODULE		= httpp

CXX	        = g++
CPPSRCS		= \
    udt_pr.cpp \
    udptransportModule.cpp \
    udtSocketProvider.cpp \
    udptHttppHandler.cpp \
    udptProxyInfo.cpp \
    $(NULL)

UDTSRCS 	= \
	$(UDT_PATH)/md5.cpp \
	$(UDT_PATH)/common.cpp \
	$(UDT_PATH)/window.cpp \
	$(UDT_PATH)/list.cpp \
	$(UDT_PATH)/buffer.cpp \
	$(UDT_PATH)/packet.cpp \
	$(UDT_PATH)/channel.cpp \
	$(UDT_PATH)/queue.cpp \
	$(UDT_PATH)/ccc.cpp \
	$(UDT_PATH)/cache.cpp \
	$(UDT_PATH)/core.cpp \
	$(UDT_PATH)/epoll.cpp \
	$(UDT_PATH)/api.cpp \
	$(UDT_PATH)/udtc.cpp \
	$(NULL) 

CXXFLAGS += \
    -fno-rtti -fno-exceptions -fPIC	\
    -fomit-frame-pointer -std=gnu++0x \
    $(NULL)

UDT_INCLUDES = -I$(UDT_PATH)

###GECKO_INCLUDES = -I/usr/lib/firefox-devel-11.0/include/ 
GECKO_INCLUDES = -I/usr/lib/firefox-devel-7.0.1/include/ 

###GECKO_LDFLAGS =  \
	-L/usr/lib/firefox-devel-11.0/lib/ \
	-lxpcomglue_s -lnspr4 -lplds4 -lxul \
	$(NULL)

GECKO_LDFLAGS =  \
	-L/usr/lib/firefox-devel-7.0.1/lib/ \
	-lxpcomglue_s -lnspr4 -lplds4 -lxul \
	$(NULL)

%.o: %.cpp Makefile
	$(CXX) -c $(CXXFLAGS) $(GECKO_CONFIG_INCLUDE) $(GECKO_INCLUDES) $(UDT_INCLUDES) $<

$(MODULE).so: $(CPPSRCS:%.cpp=%.o)
	make -C $(UDT_PATH) -f Makefile all ### build UDT .o
	$(CXX) -shared -o $@ -Wl,-soname=$(MODULE).so $(CPPSRCS:%.cpp=%.o) $(UDTSRCS:%.cpp=%.o) $(GECKO_LDFLAGS)
	chmod +x $@

build: $(MODULE).so

clean:
	make -C $(UDT_PATH) -f Makefile clean ### clean UDT .o
	rm -rf $(MODULE).so *.o

