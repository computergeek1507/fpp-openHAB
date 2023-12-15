include /opt/fpp/src/makefiles/common/setup.mk

all: libfpp-openhab.so
debug: all

OBJECTS_fpp_openhab_so += src/openHABPlugin.o src/openHABItem.o src/openHABLight.o src/openHABSwitch.o
LIBS_fpp_openhab_so += -L/opt/fpp/src -lfpp
CXXFLAGS_src/openHABPlugin.o += -I/opt/fpp/src

%.o: %.cpp Makefile
	$(CCACHE) $(CC) $(CFLAGS) $(CXXFLAGS) $(CXXFLAGS_$@) -c $< -o $@

libfpp-openhab.so: $(OBJECTS_fpp_openhab_so) /opt/fpp/src/libfpp.so
	$(CCACHE) $(CC) -shared $(CFLAGS_$@) $(OBJECTS_fpp_openhab_so) $(LIBS_fpp_openhab_so) $(LDFLAGS) -o $@

clean:
	rm -f libfpp-openhab.so $(OBJECTS_fpp_openhab_so)
