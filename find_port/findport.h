#ifndef FINDPORT_H
#define FINDPORT_H

#ifdef __cplusplus
extern "C" {
#endif

/*** Defines ***/
#define PI_DEV_VID 0x0403
#define PI_DEV_PID 0x6001
#define PI_DEV_SERIAL "B004507V"
#define MP_DEV_VID 0x067B
#define MP_DEV_PID 0x23A3
#define MP_DEV_SERIAL "ARAHb11A921"

/*** 
 * prototypes 
 ***/

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

int get_port_name (int vid, int pid, const char *serial, char *port_name);

#ifdef __cplusplus
}
#endif

#endif