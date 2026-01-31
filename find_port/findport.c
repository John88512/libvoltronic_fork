#include <libserialport.h>
#include <stdio.h>    /* printf */
#include <string.h>   /* strcmp(), strcpy () */
#include <stdbool.h>  /* bool, true, false */
#include <errno.h>    /* return values */

/* Test porgram for get_serial_port
 * get a list of serial ports on the system, use the port
 * to get the VID, PID and serial number of each port.
 *
 * Originally take from libserialports example list_ports.c
 * The original file was release to the public domain. */

/*** Defines ***/
#define PI_DEV_VID 0x0403
#define PI_DEV_PID 0x6001
#define PI_DEV_SERIAL "B004507V"
#define MP_DEV_VID 0x067B
#define MP_DEV_PID 0x23A3
#define MP_DEV_SERIAL "ARAHb11A921"

/*** local prototypes ***/
int get_port_name (int vid, int pid, const char *serial, char *port_name);

int main(int argc, char** argv) {
  char piPortName[20] = "";
  char mpPortName[20] = "";

  /*  Store return values (SP_OK=0 on success) */
  int pi_res = get_port_name(PI_DEV_VID, PI_DEV_PID, PI_DEV_SERIAL, piPortName);
  printf("PI port: %s (res=%d)\n", piPortName, pi_res);

  /* leave space */
  printf("\n");

  int mp_res = get_port_name(MP_DEV_VID, MP_DEV_PID, MP_DEV_SERIAL, mpPortName);
  printf("MP port: %s (res=%d)\n", mpPortName, mp_res);  
  return 0;
}

int get_port_name(int vid, int pid, const char *serial, char *port_name) {
  /* A pointer to a null-terminated array of pointers to
   * struct sp_port, which will contain the ports found.*/
  struct sp_port** port_list;
  int returnValue;

  printf("Getting port list.\n");
  /* Call sp_list_ports() to get the ports. The port_list
   * pointer will be updated to refer to the array created. */
  enum sp_return result = sp_list_ports(&port_list);
  if (result != SP_OK) {
    strcpy(port_name, "");
    printf("sp_list_ports() failed!\n");
    return (int)result;
  }

  /* Iterate through the ports. When port_list[i] is NULL
   * this indicates the end of the list. */
  int i;
  char *serialNumber;
  int usb_vid = 0;
  int usb_pid = 0;
  int match_count =0;
  bool serial_same = false;
  bool vid_same = false;
  bool pid_same = false;
  
  for (i = 0; port_list[i] != NULL; i++) {
    struct sp_port* port = port_list[i];
    /* Get the name of the port. */
    char* portName = sp_get_port_name(port);
    printf("Found port %i: %s\n", i, portName);

    /* get serial number from the USB descriptors. */
    serialNumber = sp_get_port_usb_serial(port);
    printf("Serial port %i: %s\n", i, serialNumber);
    
    // if (strcmp(serialNumber, serial) == 0) {
    //   serial_same = true;
    //   printf("serial same \n");
    // } else {
    //   serial_same = false;
    // }

    /* get USB vendor and product IDs. */
    sp_get_port_usb_vid_pid(port, &usb_vid, &usb_pid);
    printf("Port %i VID: %04X\n", i, usb_vid);
    if (usb_vid == vid) {
      vid_same = true;
      printf("vid same\n");
    } else {
      vid_same = false;
    }
    printf("Port %i PID: %04X\n", i, usb_pid);
    if (usb_pid == pid) {
      pid_same = true;
      printf("pid same\n");
    } else {
      pid_same = false;
    }
    /* is this the desired port? */
    if ( serial_same && vid_same && pid_same ) {
      strcpy(port_name, portName);
      printf("Port found: %s \n", port_name);
      match_count++;
    }
  } /* for */

  /* handle results */
  if (match_count == 0) {
    returnValue = SP_ERR_SUPP;
    strcpy(port_name, "");
  } else if (match_count > 1) {
    returnValue = -2;  // Duplicates
    strcpy(port_name, "");
    fprintf(stderr, "Warning: %d ports match VID:%04X PID:%04X Serial:%s\n",
            match_count, vid, pid, serial);
  } else {  // Exactly 1
    returnValue = SP_OK;
  }

  printf("Found %d ports.\n", i);
  printf("Freeing port list.\n");
  /* Free the array created by sp_list_ports(). */
  sp_free_port_list(port_list);
  /* Note that this will also free all the sp_port structures
   * it points to. If you want to keep one of them (e.g. to
   * use that port in the rest of your program), take a copy
   * of it first using sp_copy_port(). */
  return (returnValue);
}

