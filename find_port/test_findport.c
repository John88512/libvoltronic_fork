#include <stdio.h>   /* printf */
#include <string.h>  /* strcmp(), strcpy () */
#include <stdbool.h> /* bool, true, false */
#include <errno.h>    /* return values */

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

int main(int argc, char** argv) {
  char piPortName[20] = "";
  char mpPortName[20] = "";

  /*  Store return values (SP_OK=0 on success) */
  int pi_res = get_port_name(PI_DEV_VID, PI_DEV_PID, PI_DEV_SERIAL, piPortName);
  printf("PI port: %s (res=%d)\n", piPortName, pi_res);

  /* leave space */
  printf("\n");
  printf("\n");

  int mp_res = get_port_name(MP_DEV_VID, MP_DEV_PID, MP_DEV_SERIAL, mpPortName);
  printf("MP port: %s (res=%d)\n", mpPortName, mp_res);
  return 0;
}

