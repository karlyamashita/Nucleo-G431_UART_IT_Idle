/*
 * ErrorStatus.h
 *
 *  Created on: Nov 24, 2021
 *      Author: karl.yamashita
 */

#ifndef ERRORSTATUS_H_
#define ERRORSTATUS_H_


// error status
enum {
    NO_ACK = -1,
    NO_ERROR, // not used
    COMMAND_UNKNOWN,
	VALUE_OUT_OF_RANGE,

    // for debug
    ERR_1 = 200,
    ERR_2,
    ERR_3,
    ERR_4
};





void GetErrorString(int errorCode, char *str);

#endif /* ERRORSTATUS_H_ */
