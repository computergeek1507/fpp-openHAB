include /opt/fpp/src/makefiles/common/setup.mk

all: libfpp-openHAB.so
debug: all

OBJECTS_fpp_openHAB_so += src/openHABPlugin.o src/openHABItem.o src/openHABLight.o src/openHABSwitch.o
LIBS_fpp_openHAB_so += -L/opt/fpp/src -lfpp
CXXFLAGS_src/openHABPlugin.o += -I/opt/fpp/src

%.o: %.cpp Makefile
	$(CCACHE) $(CC) $(CFLAGS) $(CXXFLAGS) $(CXXFLAGS_$@) -c $< -o $@

libfpp-openHAB.so: $(OBJECTS_fpp_openHAB_so) /opt/fpp/src/libfpp.so
	$(CCACHE) $(CC) -shared $(CFLAGS_$@) $(OBJECTS_fpp_openHAB_so) $(LIBS_fpp_openHAB_so) $(LDFLAGS) -o $@

clean:
	rm -f libfpp-openHAB.so $(OBJECTS_fpp_openHAB_so)
