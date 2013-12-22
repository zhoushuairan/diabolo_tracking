#pragma once

#include "ofMain.h"
#include <bluetooth/bluetooth.h>
#include <cwiid.h>


class wiimoteController {

	cwiid_wiimote_t *wiimote;
	static cwiid_ir_mesg ir_msg;
	//int msg_count;
	//union cwiid_mesg *msg[];
	//struct timespec timestamp;
	//bdaddr_t bdaddr;

	static void messageCallback(cwiid_wiimote_t *wiiremote, int mesg_count, union cwiid_mesg mesg[], struct timespec *timestamp);


	public:
		void setup();
		cwiid_ir_mesg getIRData();
};