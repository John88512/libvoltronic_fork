#include <stdlib.h>
#include <stdio.h>     /* printf() srtlen() */
#include <string.h>    /* strlen() */
#include "voltronic_dev_serial.h"

/* local prototypes */
int printString(const char* string);

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

  char input[20];
  char buffer[256];
  int result;
  size_t lenInputString;
  int numCharsReceived;

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

  /* loop for input until "0", then quit */
  printf("Enter strings (max 12 chars, '0' to quit):\n");

  while (1) {
    printf("Input command > ");
    scanf("%s", input);

    /* Exit loop on quit condition */
    if (input[0] == *"0") {
      printf("Goodbye!\n");
      break;
    }

    lenInputString = strlen(input);
    printf("strlen input: %i\n", (int)lenInputString);
    printf("\n");

    /* Query the device a bunch of ways to cover all code branches */
    // result = voltronic_dev_execute(dev, DISABLE_WRITE_VOLTRONIC_CRC, input, 3, buffer, sizeof(buffer), 1000);
    // result = voltronic_dev_execute(dev, DISABLE_PARSE_VOLTRONIC_CRC, input, 3, buffer, sizeof(buffer), 1000);
    // result = voltronic_dev_execute(dev, DISABLE_VERIFY_VOLTRONIC_CRC, input, 3, buffer, sizeof(buffer), 1000);
    result = voltronic_dev_execute(dev, 0, input, lenInputString, buffer, sizeof(buffer), 1000);

    printf("Returned:\n");
    numCharsReceived = printString(buffer);
    printf("\n");
    if (result >0) {
      printf("Characters received: %i\n", numCharsReceived);
    } else {
      printf("no characters received\n");
    }  

  } /* while */
  // Close the connection to the device
  voltronic_dev_close(dev);

  if (result > 2) {
    exit(0);
  } else {
    exit(2);
  }
} /* main */

/** printString - prints inputed string in ascii and hex characters
 *                until '\0' found
 *
 * Arguements:  String - pointer to string to be printed
 * 
 * Returns:     on success the number of characters until \0 (end of string)
 *              on empty string, 0
 *              on error -1
 * 
 * 25-Jan-26 JnG created
 * 
 */
int printString(const char* string) {
  int i;

  printf("\n");
  for (i = 0; string[i] != '\0'; i++) {
    printf("%x ", (int)string[i]);  // Cast to int for clarity
  }
  printf("\n");
  for (i = 0; string[i] != '\0'; i++) {
    if ((int)(string[i] > 31) & (int)(string[i] < 127)) {
      printf("%c ", (char)string[i]);  // Cast to char for clarity
    } else {
      printf("*");
    }
  }
  printf("\n");

  if (i > 0) {
    return (i);
  } else {
    return (-1);
  }
} /* printString */
