# define the C compiler to use
CC := gcc

#directories
IDIR = include
LDIR = lib
SDIR = src
ODIR = obj

# define any compile-time flags
CFLAGS = -std=c99 -Werror -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wshadow -O3 -flto -fomit-frame-pointer

# add includes
CFLAGS += -I$(IDIR) -I$(LDIR)/libserialport -I$(LDIR)/hidapi/hidapi -I$(LDIR)/libusb/libusb

# define the C source files
SRCS = $(wildcard $(SDIR)/*.c)

SHARED_LIBS :=
SHARED_LIBS_SERIAL :=
SHARED_LIBS_HID :=
SERIAL_BINARY := $(LDIR)/libserialport/.libs/libserialport.a
HID_BINARY := $(LDIR)/hidapi/libusb/.libs/libhidapi.a

# Object files shared by all directives
SHARED_OBJS = $(ODIR)/voltronic_crc.o $(ODIR)/voltronic_dev.o

# Directives
default: $(SHARED_OBJS) $(ODIR)/serial_main.o $(ODIR)/voltronic_dev_serial_libserialport.o $(SERIAL_BINARY)
	$(CC) -o $@ $^ $(CFLAGS) $(SHARED_LIBS) $(SHARED_LIBS_SERIAL)

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	$(RM) $(ODIR)/*.o *~ libserialport libserialport.exe hidapi hidapi.exe $(INCDIR)/*~ 
