#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>     /* uint16_t */
#include <string.h>     /* strlen(), */
#include "voltronic_dev_serial.h"
#include "findport.h"

#include "voltronic_crc.h" /* test crc calculation */

/* Defines */
#define PVPI
#define MP_N
#define CRC_INIT 0x000

/* local functions */
int checkStringPrefix(const char *str);
uint8_t checksumAppend(const char *input, char *sendData);
int printString(const char *string);
uint16_t crc16_xmodem(const char *data, size_t length, uint16_t crcInit);


/* mimics the masterpower - receive message, send answer */
int main() {
  char input[20];
  char sendData[50];
  char receiveData[50];
  char portName[20] = "";
  int fp_result = 0;

  /* get serial port name for adapters used */
#ifdef PVPI
  /* blue adapter */
  fp_result = get_port_name (PI_DEV_VID, PI_DEV_PID, PI_DEV_SERIAL, portName);
  if (fp_result == 0) {
    printf ("Blue adapter not found\n");
  }
#elif
  /* grey adapter -  simulates MasterPower */
  fp_result = get_port_name (MP_DEV_VID, MP_DEV_PID, MP_DEV_SERIAL, portName);
  if (fp_result == 0) {
    printf ("Grey adapter not found\n");
  }
#endif

  /* Create a serial port */
  voltronic_dev_t dev = voltronic_serial_create(
      portName, 2400, DATA_BITS_EIGHT, STOP_BITS_ONE, SERIAL_PARITY_NONE);

  if (dev == 0) {
    printf("Could not open serial communication -> exiting!\n");
    exit(1);
  }

  //char buffer[256];
  int result = 0;
  int i = 0;
  size_t lengthsendDataBuffer;
  size_t lengthreceiveDataBuffer;
  int numberOfCharactersIn;
  int numberOfCharactersOut;
  int numberOfCharactersReceived;
  int readResult = 0;
  uint16_t rtCRC1 = 0;
  uint16_t rtCRC2 = 0;
  size_t lenInputString = 0;

  char buffer[256];
  /* Write end of input
   *  number of bytes written
   *  0 on timeout
   *  -1 on error
   */
  result = voltronic_dev_write(dev, "\r", 1, 1000);
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

    /* get number of character in string */
    lenInputString = strlen(input);

    /* print entered input */
    numberOfCharactersIn = printString (input);
    
    /* calculate CRC of input */
    rtCRC1 = crc16_xmodem(input, lenInputString, CRC_INIT);
    printf ("CRC1 = 0x%x for input string %s\n", rtCRC1, input);
    rtCRC2 = (uint16_t)calculate_voltronic_crc(input, lenInputString);
    printf ("CRC2 = 0x%x for input string %s\n", rtCRC2, input);

    /* add CRC to end of input string */
    input[numberOfCharactersIn] = (char) (((rtCRC1 >> 8) & 0xFF));
    input[(numberOfCharactersIn+1)] = (char) ((rtCRC1 & 0xFF));

    /* end with CR add NULL for end of string */
    input[(numberOfCharactersIn+2)] = *"\r";
    input[(numberOfCharactersIn +3)] = *"\0";
    printf("add <CR> and NULL to end of string\n");
    numberOfCharactersIn = printString (input);
    printf("numberOfCharactersIn %i \n", numberOfCharactersIn);

    // /* check if command needs a checksum */
    // if (checkStringPrefix(input)) {
    //   /* Compute checksum and append */
    //   chk = checksumAppend(input, sendData);
    //   if (!chk) {
    //     printf("Error in checkStringPrefis\n");
    //   }
    /* otherwise copy input into sendData */
    printf("strlen(input) %lu \n", strlen(input));
    printf("copy input string to sendData string\n");
    for (i = 0; input[i] != '\0'; i++) {
      sendData[i] = input[i];
    }
    /* add the EOL to sendData */
    sendData[i] = *"\0";
    printf("strlen(sendData) %lu \n", strlen(sendData));
    numberOfCharactersOut = printString (sendData);
    printf("Postision sendData of EOL from left: %i\n", numberOfCharactersOut);
    lengthsendDataBuffer = strlen(sendData);
    printf("Nr bytes to send %i\n", (int)lengthsendDataBuffer);
    printf("\n");

    // result = voltronic_dev_execute(dev,
    // VOLTRONIC_EXECUTE_DEFAULT_OPTIONS, sendData, lengthsendDataBuffer, buffer,
    // sizeof(buffer), 1000); 
   
    /* send command */
    result = voltronic_dev_write(dev, sendData, lengthsendDataBuffer, 1000);

    /* result of serial send
     *  number of bytes written
     *  0 on timeout
     *  -1 on error
     */
    printf("result of send %i\n", result);
    printf("\n");

    /* pick up response */
    printf("Waiting for response . . .\n");
    lengthreceiveDataBuffer = sizeof(receiveData);
    printf("strlen of receiveData: %i\n", (int)lengthreceiveDataBuffer);
    readResult = voltronic_dev_read(dev, receiveData, lengthreceiveDataBuffer, 1000);
    printf("result received\n");

    if (readResult > 0) {
      /* print received dats */
      printf("Received data:\n");
      numberOfCharactersReceived = printString(receiveData);
      printf("Number of characters received %i \n", numberOfCharactersReceived);
    }
    else if (readResult == 0) {
      printf("Received timout\n");
    }
    else
    {
      /* print error code received */
      printf("Receive error %i \n", readResult);
    }
    printf("\n");
    printf("deleting receive data\n");
    for (i=0; i < (int)lengthreceiveDataBuffer; i++){
      receiveData[i] = *"\0";
    }
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
 * checksum_append_to - Computes 8-bit checksum and appends decimal string to sendData
 * @input: Input null-terminated string (max 12 characters)
 * @sendData: sendData buffer for result (minimum 18 bytes capacity)
 *
 * Computes the 8-bit checksum (sum of input bytes modulo 256) of the input string,
 * then appends the decimal representation as ASCII characters to the sendData buffer.
 * The appended checksum excludes itself from the computation.
 * Input is truncated if longer than 12 characters to ensure buffer safety.
 *
 * Return:
 *   uint8_t - Checksum value (0-255) on success
 *   0 - On error (null input/sendData pointers or input >12 chars)
 */
uint8_t checksumAppend(const char *input, char *sendData) {
    /* Validate inputs: check for null pointers or excessive length */
    if (!input || !sendData || strlen(input) > 12) {  /* Reduced to leave room for up to 3 decimal digits + null */
        return 0;  /* Error case: invalid arguments */
    }
    
    /* Determine input length and safely copy to sendData buffer */
    size_t lenghtInputString = strlen(input);
    strcpy(sendData, input);  /* Copies null terminator */
    size_t lenghtsendDataString = strlen(sendData);
    
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
        sendData[lenghtsendDataString++] = '0' + (sum / 100);
        sum %= 100;
    }
    if (sum >= 10) {
        /* Tens digit */
        sendData[lenghtsendDataString++] = '0' + (sum / 10);
        sum %= 10;
    }
    /* Units digit */
    sendData[lenghtsendDataString++] = '0' + sum;
    
    /* Null-terminate the resulting string */
    sendData[lenghtsendDataString] = '\0';
    
    /* Return computed checksum value for verification */
    return checksum8;
} /* checksumAppend */

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
}

/* Calculate CRC-16/XMODEM over a data buffer. */
/*  - data: pointer to input bytes                 */
/*  - length: number of bytes in data              */
/*  - crc: initial CRC value (usually 0x0000)      */
/* Returns: 16-bit CRC value.                      */
uint16_t crc16_xmodem(const char *data, size_t length, uint16_t crcInit)
{
    size_t i;
    /* Process each byte in the buffer. */
    for (i = 0; i < length; ++i) {
        /* XOR byte into the high-order CRC byte. */
        crcInit ^= (uint16_t)data[i] << 8;

        /* Process each of the 8 bits. */
        for (int bit = 0; bit < 8; ++bit) {
            /* If the MSB is set, shift and XOR with polynomial 0x1021. */
            if (crcInit & 0x8000) {
                crcInit = (crcInit << 1) ^ 0x1021;
            } else {
                /* Otherwise just shift left. */
                crcInit <<= 1;
            }

            /* Mask to 16 bits to avoid overflow. */
            crcInit &= 0xFFFF;
        }
    }

    /* Final CRC value. */
    return crcInit;
}
