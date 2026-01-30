#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "voltronic_dev_serial.h"

/* Defines */
#define USB_PORT "/dev/ttyUSB2"

/* local functions */
int checkStringPrefix(const char *str);
uint8_t checksumAppend(const char *input, char *output);
int printString(const char *string);

/* mimics the masterpower - receive message, send answer */
int main() {
  char input[16];
  char output[20];
  //uint8_t chk;

  /* Create a serial port dev */
  voltronic_dev_t dev = voltronic_serial_create(
      USB_PORT, 2400, DATA_BITS_EIGHT, STOP_BITS_ONE, SERIAL_PARITY_NONE);

  if (dev == 0) {
    printf("Could not open serial communication -> exiting!\n");
    exit(1);
  }

  char buffer[256];
  int result;
  int i = 0;
  size_t lengthOutputBuffer;
  int numberOfCharacters;

  /* Write end of input
   *  number of bytes written
   *  0 on timeout
   *  -1 on error
   */
  result = voltronic_dev_write(dev, "(NAK\r", 4, 1000);
  printf("Number of bytes written to serial: %i\n", result);
  printf("\n");

  /* Read (NAK
   *  number of bytes read
   *  0 on timeout
   *  -1 on error
   */
  result = voltronic_dev_read(dev, buffer, sizeof(buffer), 1000);
  printf("read in result %i\n", result);
  if (result > 0) {
    printString(buffer);
  } else {
    printf("Empty string, timeout or error\n");
  }
  printf("\n");

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

    /* print command entered */
    numberOfCharacters = printString (input);
    if (numberOfCharacters > 0) {
      printf("Postision of EOL from left: %i\n", numberOfCharacters);
    } else {
      printf("error in printing\n");
    }
    

    /* replace newline, if present, with CR */
    input[numberOfCharacters] = *"\r";
    printf("add <CR> to end of string\n");

    // /* check if command needs a checksum */
    // if (checkStringPrefix(input)) {
    //   /* Compute checksum and append */
    //   chk = checksumAppend(input, output);
    //   if (!chk) {
    //     printf("Error in checkStringPrefis\n");
    //   }
    // } else {
      /* otherwise copy input into output */
      printf("copy input string to output string\n");
      for (i = 0; input[i] != '\0'; i++) {
        output[i] = input[i];
      }
      printf("Postision now of EOL from left: %i\n", i);
//    }
    printf("output string to send %s\n", output);
    printf("\n");
    /* print characters send */
    numberOfCharacters = printString (input);
    if (numberOfCharacters > 0) {
      printf("Postision of EOL from left: %i\n", i);
    } else {
      printf("error in printing\n");
    }
    lengthOutputBuffer = strlen(output);
    printf("Nr bytes to send %i\n", (int)lengthOutputBuffer);
    printf("\n");

    // result = voltronic_dev_execute(dev,
    // VOLTRONIC_EXECUTE_DEFAULT_OPTIONS, output, lengthOutputBuffer, buffer,
    // sizeof(buffer), 1000); printf("Sent %s\n", output);
    // /* result of serial send
    //  *  number of bytes written
    //  *  0 on timeout
    //  *  -1 on error
    //  */
    // printf("result of send %i\n", result);

    result = voltronic_dev_write(dev, output, lengthOutputBuffer, 1000);

    /* result of serial send
     *  number of bytes written
     *  0 on timeout
     *  -1 on error
     */
    printf("result of send %i\n", result);

  } /* while */

  // Close the connection to the device
  voltronic_dev_close(dev);
  printf("dev closed\n");

  if (result > 2) {
    printf("result > 2\n");
    exit(0);
  } else {
    printf("result <= 2\n");
    exit(2);
  }
} /* main */

int checkStringPrefix(const char *str) {
    /* Step 1: Validate input pointer - NULL input is error condition */
    if (!str) {
        return -1;
    }

    /* Step 2: Check first 12 characters for non-printable chars (32-126 ASCII range required) */
    /* Non-printable characters indicate corrupted/invalid input */
    for (int i = 0; i < 12 && str[i] != '\0'; i++) {
        if (str[i] < 32 || str[i] > 126) {
            return -1;  /* Error: Invalid character found */
        }
    }

    /* Step 3: Define allowed prefix patterns in array for easy maintenance */
    const char *prefixes[] = {"QEY", "QEM", "QED", "QEH"};
    size_t num_prefixes = sizeof(prefixes) / sizeof(prefixes[0]);  /* Calculate array size */

    /* Step 4: Compare input string prefix against each allowed pattern */
    for (size_t i = 0; i < num_prefixes; i++) {
        size_t len = strlen(prefixes[i]);  /* Get length of current prefix pattern */
        
        /* Use strncmp to compare exact prefix length - allows longer strings with matching prefix */
        if (strncmp(str, prefixes[i], len) == 0) {
            return 1;  /* Match found - success */
        }
    }

    /* Step 5: No matches found */
    return 0;
} /* checkStringPrefix */

/*
 * checksum_append_to - Computes 8-bit checksum and appends decimal string to output
 * @input: Input null-terminated string (max 12 characters)
 * @output: Output buffer for result (minimum 18 bytes capacity)
 *
 * Computes the 8-bit checksum (sum of input bytes modulo 256) of the input string,
 * then appends the decimal representation as ASCII characters to the output buffer.
 * The appended checksum excludes itself from the computation.
 * Input is truncated if longer than 12 characters to ensure buffer safety.
 *
 * Return:
 *   uint8_t - Checksum value (0-255) on success
 *   0 - On error (null input/output pointers or input >12 chars)
 */
uint8_t checksumAppend(const char *input, char *output) {
    /* Validate inputs: check for null pointers or excessive length */
    if (!input || !output || strlen(input) > 12) {  /* Reduced to leave room for up to 3 decimal digits + null */
        return 0;  /* Error case: invalid arguments */
    }
    
    /* Determine input length and safely copy to output buffer */
    size_t lenghtInputString = strlen(input);
    strcpy(output, input);  /* Copies null terminator */
    size_t lenghtOutputString = strlen(output);
    
    /* Accumulate 8-bit sum: add each byte value, overflow wraps mod 256 */
    uint8_t checksum8 = 0;
    for (size_t i = 0; i < lenghtInputString; i++) {
        checksum8 += (uint8_t)input[i];  /* Cast ensures byte treatment */
    }
    
    uint8_t sum;
    sum = checksum8;

    /* Convert uint8_t checksum to decimal ASCII digits using division */
    if (sum >= 100) {
        /* Hundreds digit (100-255) */
        output[lenghtOutputString++] = '0' + (sum / 100);
        sum %= 100;
    }
    if (sum >= 10) {
        /* Tens digit */
        output[lenghtOutputString++] = '0' + (sum / 10);
        sum %= 10;
    }
    /* Units digit */
    output[lenghtOutputString++] = '0' + sum;
    
    /* Null-terminate the resulting string */
    output[lenghtOutputString] = '\0';
    
    /* Return computed checksum value for verification */
    return checksum8;
} /* checksumAppend */

/** printString - prints inputed string in ascii and hex characters
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

  for (i = 0; string[i] != '\0'; i++) {
    printf("%x ", (int)string[i]);  // Cast to int for clarity
  }
  printf("\n");
  for (i = 0; string[i] != '\0'; i++) {
    if ((int)(string[i] > 32) & (int)(string[i] < 127)) {
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
}