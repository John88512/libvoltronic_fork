#include <stdio.h>   /* printf */
#include <string.h>  /* strcmp(), strcpy () */
#include <stdbool.h> /* bool, true, false */
#include <errno.h>    /* return values */
#include <libserialport.h>

#include "findport.h" /* make get_port_name and defines public */

/* Test program for get_serial_port
 * get a list of serial ports on the system, use the port
 * to get the VID, PID and serial number of each port.
 *
 * Originally take from libserialports example list_ports.c
 * The original file was release to the public domain. */

/*** Defines ***/
#ifndef DEBUG
#define DEBUG false
#endif

/*** local prototypes ***/
int stringCompare(const char* string1, const char* string2, int maxSize);

/* get_port_name - returns the device name for a serial port using
 *                 the vendor ID, the product ID and serial number
 *                 of the device being searched for
 *
 * Arguments:
 * vid - vendor id of the device to search for
 * pid - product id of the device to search for
 * serial - serial number of the device to search for
 * port_name - returns the device name or NULL if device not found
 *
 * Returns - > 0 if device found - number of ports checked
 *             0 if port not found,
 *            -2 if duplicate devices found
 *
 * */
int get_port_name(int vid, int pid, const char* serial, char* port_name) {
  /* A pointer to a null-terminated array of pointers to
   * struct sp_port, which will contain the ports found.*/
  struct sp_port** port_list;
  int returnValue = 0;
  int numPorts = 0;

  if (DEBUG) {
    printf("Getting port list.\n");
    printf("Looking for VID: %04X  PID: %04X  Serial Number: %s\n", vid, pid, serial);
    printf("\n");
  }

  /* Call sp_list_ports() to get the ports. The port_list
   * pointer will be updated to refer to the array created. */
  enum sp_return result = sp_list_ports(&port_list);
  if (result != SP_OK) {
    if (DEBUG) {
      strcpy(port_name, "");
      printf("sp_list_ports() failed!\n");
    }
    return (int)result;
  }

  /**
   *  Iterate through the ports. When port_list[i] is NULL
   * this indicates the end of the list. 
   * 
   */
  int i;
  char* serialNumber;
  int usb_vid = 0;
  int usb_pid = 0;
  int match_count = 0;
  bool serial_same = false;
  bool vid_same = false;
  bool pid_same = false;

  for (i = 0; port_list[i] != NULL; i++) {
    struct sp_port* port = port_list[i];
    /* Get the name of the port. */
    char* portName = sp_get_port_name(port);
    if (DEBUG) {
      printf("List port %i: %s\n", i, portName);
    }

    /* get serial number from the USB descriptors. */
    serialNumber = sp_get_port_usb_serial(port);

    /* get USB vendor and product IDs. */
    sp_get_port_usb_vid_pid(port, &usb_vid, &usb_pid);

    if (DEBUG) {
      /* print port data */
      printf("Port %i      VID: %04X  PID: %04X  Serial number: %s\n", i, usb_vid, usb_pid, serialNumber);
    }
    /* return evaluation */
    if (usb_vid == vid) {
      vid_same = true;
      if (DEBUG) {
        printf("            vid same   ");
      }
    } else {
      vid_same = false;
    }
    if (usb_pid == pid) {
      pid_same = true;
      if (DEBUG) {
        printf("pid same  ");
      }
    } else {
      pid_same = false;
    }

    int sc_res = stringCompare(serialNumber, port_name, 20);
    if (DEBUG) {
      printf ("Serial number ");
    }
    switch (sc_res) {
      /* strings are the same */
      case 0:
        serial_same = true;
        if (DEBUG) {
          printf("same \n");
        }
        break;

      case -1:
        /* Error: maxSize reached without EOS */
        serial_same = false;
        if (DEBUG) {
          printf("Error: maxSize reached without EOS\n");
        }
        break;

      case -2:
        /* Argument error: NULL pointer string passed */
        serial_same = false;
        if (DEBUG) {
          printf("Argument error: NULL pointer string passed\n");
        }
        break;

      case -3:
        /* Argument error: maxSize less than 1 */
        serial_same = false;
        if (DEBUG) {
          printf("Argument error: maxSize less than 1\n");
        }
        break;

      default:
        /* strings are not the same */
        if (sc_res > 0) {
          serial_same = false;
          if (DEBUG) {
            printf("Strings not the same starting at %i\n", sc_res);
          }
        } else {
          if (DEBUG) {
            printf("Error: unknown %i\n", sc_res);
          }
        }
        break;
    }

    /* is this the desired port? */
    if (serial_same && vid_same && pid_same) {
      strcpy(port_name, portName);
      if (DEBUG) {
        printf("Device found with port name: %s \n", port_name);
      }
      match_count++;
    }
    serial_same = false;
    vid_same = false;
    pid_same = false;
    if (DEBUG) {
      printf("\n");
    }
  } /* for */
  /* number of ports found and checked */
  numPorts = i;

  /* handle results */
  if (match_count == 0) {
    returnValue = SP_OK; /* no match found */
    strcpy(port_name, "");
  } else if (match_count > 1) {
    returnValue = -2; /* Duplicate ports found */
    strcpy(port_name, "");
    fprintf(stderr, "Warning: %d ports match VID:%04X PID:%04X Serial:%s\n",
            match_count, vid, pid, serial);
  } else {  // Exactly 1
    returnValue = numPorts;
  }

  if (DEBUG) {
    printf("Found %d ports.\n", i);
    printf("Freeing port list.\n");
  }

  /* Free the array created by sp_list_ports(). */
  sp_free_port_list(port_list);
  /* Note that this will also free all the sp_port structures
   * it points to. If you want to keep one of them (e.g. to
   * use that port in the rest of your program), take a copy
   * of it first using sp_copy_port(). */
  return (returnValue);
}

/* stringCompare - compares string1 character by character with
 *                 with string2 until end of string (EOS) or maxSize.
 *                 Designed for opaque string pointers
 *
 * Arguments:
 * string1 - first string for comparison
 * string2 - second string for comparison
 * maxSize - >0, max number of characters to compare
 *           if EOS is not found
 *
 *
 * returns: 0 - if strings are the same
 *          >0 - position where strings are not the same
 *          <0 - error
 *          -1 - maxSize reached without EOS
 *          -2 - NULL pointer string passed
 *          -3 - maxSize <= 0
 *
 * */
int stringCompare(const char* string1, const char* string2, int maxSize) {
  /* check args */
  /* string arg NULL pointer */
  if (!string1 || !string2) {
    return -2; /* Error: Null pointer string */
  }
  /* maxSize arg <= 0 */
  if (maxSize <= 0) {
    return -3; /* Error: maxSize out of bounds */
  }

  int i;
  for (i = 0; i < maxSize; i++) {
    if (string1[i] != string2[i]) {
      return i; /* Mismatch position (0-based) */
    }
    if (string1[i] == '\0') {
      return 0; /* Equal up to EOS */
    }
  }
  return -1; /* Error: maxSize reached, no EOS */
} /* stringCompare */