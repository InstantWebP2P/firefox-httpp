MODULE		= udptransport

CXX	        = g++
CPPSRCS		= \
    udt_pr.cpp \
    udptransportModule.cpp \
    udtSocketProvider.cpp \
    udptHttppHandler.cpp \
    udptProxyInfo.cpp \
    $(NULL)

CXXFLAGS += \
    -fno-rtti -fno-exceptions -fPIC	\
    -fomit-frame-pointer -std=gnu++0x \
    $(NULL)

REPO_PATH = ../../../node-httpp/joyent-node-950a048
UDT_INCLUDES = -I$(REPO_PATH)/deps/UDT/src/

GECKO_INCLUDES = -I/usr/include/firefox-7.0.1 

GECKO_LDFLAGS =  \
	-L$(REPO_PATH)/deps/UDT/src \
	-L/usr/lib/firefox-devel-7.0.1/lib/ \
	-lxpcomglue_s -lnspr4 -lplds4 -lxul -ludt_s \
	$(NULL)

%.o: %.cpp Makefile
	$(CXX) -c $(CXXFLAGS) $(GECKO_CONFIG_INCLUDE) $(GECKO_INCLUDES) $(UDT_INCLUDES) $<

$(MODULE).so: $(CPPSRCS:%.cpp=%.o)
	$(CXX) -shared -o $@ -Wl,-soname=$(MODULE).so $(CPPSRCS:%.cpp=%.o) $(GECKO_LDFLAGS)
	chmod +x $@

build: $(MODULE).so

clean:
	rm $(MODULE).so *.o
