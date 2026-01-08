#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "voltronic_dev_serial.h"


/* local functions */
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
}


int main() {
    char input[16];
    char output[20];
    uint8_t chk;

  /* Create a serial port dev */
  voltronic_dev_t dev = voltronic_serial_create(
    "/dev/ttyUSB0",
    2400,
    DATA_BITS_EIGHT,
    STOP_BITS_ONE,
    SERIAL_PARITY_NONE
  );

  if (dev == 0) {
    printf("Could not open serial communication -> exiting!\n");
    exit(1);
  }

  char buffer[256];
  int result;
  int i, j;

  // Write end of input
  result = voltronic_dev_write(
    dev,
    "\r",
    1,
    1000
  );
  printf("write end of input result %i\n", result);

  // Read (NAK
  result = voltronic_dev_read(
    dev,
    buffer,
    sizeof(buffer),
    1000
  );
  printf("read in expected (NAK result %i\n", result);
  for (i = 0; buffer[i] != '\0'; i++) {
        printf("%i ", (int)buffer[i]);  // Cast to int for clarity
    }
    printf("\n");
  for (i = 0; buffer[i] != '\0'; i++) {
        printf("%c ", (char)buffer[i]);  // Cast to char for clarity
    }
    printf("\n");

  /* loop for input until "0", then quit */
     printf("Enter strings (max 12 chars, '0' to quit):\n");
 
  while (1) {
       printf("> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            /* Handle input read error */
            continue;
        }
        
        /* Remove newline if present */
        input[strcspn(input, "\n")] = 0;
        
        /* Check for quit condition */
        if (strcmp(input, "0") == 0) {
            printf("Goodbye!\n");
            break;
        }
        
        if 
        /* Compute checksum and append */
        chk = checksumAppend(input, output);






    result = 0;
    result = voltronic_dev_execute(dev, VOLTRONIC_EXECUTE_DEFAULT_OPTIONS, "QPI", 3, buffer, sizeof(buffer), 1000);
    printf("read in expected (NAK result %i\n", result);
    for (i = 0; buffer[i] != '\0'; i++) {
      printf("%i ", (int)buffer[i]);  // Cast to int for clarity
    }
    printf("\n");
    for (i = 0; buffer[i] != '\0'; i++) {
      printf("%c ", (char)buffer[i]);  // Cast to char for clarity
    }
    printf("\n");
  }
  
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

