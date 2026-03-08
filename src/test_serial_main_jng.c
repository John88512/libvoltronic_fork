#include <stdlib.h>
#include <stdio.h>
#include "voltronic_dev_serial.h"

/**
* This class is only here to link everything related to serial port to assert compilation
*/

int main() {
  // Create a serial port dev
  voltronic_dev_t dev = voltronic_serial_create(
    "/dev/ttyUSB0",
    2400,
    DATA_BITS_EIGHT,
    STOP_BITS_ONE,
    SERIAL_PARITY_NONE
  );

  if (dev == 0) {
    printf("Could not open serial port \n");
    exit(1);
  }

  char buffer[256];
  int result;

  // Write end of input
  result = voltronic_dev_write(
    dev,
    "\r",
    1,
    1000
  );
  printf("Send \r result: %i\n", result);

  // Read (NAK
  result = voltronic_dev_read(
    dev,
    buffer,
    sizeof(buffer),
    1000
  );
  printf("Receive NAK result: %i\n", result);
  if (result > 0) printf("buffer: %s \n", buffer);

  // Query the device a bunch of ways to cover all code branches
  result = voltronic_dev_execute(dev, DISABLE_WRITE_VOLTRONIC_CRC, "QPI", 3, buffer, sizeof(buffer), 1000);
  printf("1) DISABLE_WRITE_VOLTRONIC_CRC: %i\n", result);
  if (result >0) printf("buffer: %s\n", buffer);
  result = voltronic_dev_execute(dev, DISABLE_PARSE_VOLTRONIC_CRC, "QPI", 3, buffer, sizeof(buffer), 1000);
  printf("2) DISABLE_PARSE_VOLTRONIC_CRC: %i\n", result);
  if (result >0) printf("buffer: %s\n", buffer);
  result = voltronic_dev_execute(dev, DISABLE_VERIFY_VOLTRONIC_CRC, "QPI", 3, buffer, sizeof(buffer), 1000);
  printf("3) DISABLE_VERIFY_VOLTRONIC_CRC: %i\n", result);
  if (result >0) printf("buffer: %s\n", buffer);
  result = voltronic_dev_execute(dev, 0, "QPI", 3, buffer, sizeof(buffer), 1000);
  printf("4) 0: %i\n", result);
  if (result >0) printf("buffer: %s\n", buffer);

  // Close the connection to the device
  voltronic_dev_close(dev);

  if (result > 2) {
    exit(0);
  } else {
    exit(2);
  }
}
