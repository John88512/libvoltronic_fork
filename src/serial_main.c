#include <stdio.h>
#include <stdlib.h>
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
    printf("dev == 0 -> exit\n");
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
  printf("write result %i\n", result);

  // Read (NAK
  result = voltronic_dev_read(
    dev,
    buffer,
    sizeof(buffer),
    1000
  );
  printf("read NAK result %i\n", result);


  // Query the device a bunch of ways to cover all code branches
  result = voltronic_dev_execute(dev, DISABLE_WRITE_VOLTRONIC_CRC, "QPI", 3, buffer, sizeof(buffer), 1000);
  printf("execute 1 result %i, %s\n", result, buffer);
  result = voltronic_dev_execute(dev, DISABLE_PARSE_VOLTRONIC_CRC, "QPI", 3, buffer, sizeof(buffer), 1000);
  printf("execute 2 result %i\n", result);
  result = voltronic_dev_execute(dev, DISABLE_VERIFY_VOLTRONIC_CRC, "QPI", 3, buffer, sizeof(buffer), 1000);
  printf("execute 3 result %i\n", result);
  result = voltronic_dev_execute(dev, 0, "QPI", 3, buffer, sizeof(buffer), 1000);
  printf("execute 4 result %i\n", result);
 
  // Close the connection to the device
  voltronic_dev_close(dev);
  printf("dev closed\n");

  if (result > 2) {
    printf("result > 2\n");
    exit(0);
  } else {
    printf("result <= 2");
    exit(2);
  }
}
