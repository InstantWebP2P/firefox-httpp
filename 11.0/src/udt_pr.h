/*
 * udt_pr.h
 *
 *  Created on: Oct 30, 2010
 *      Author: yiqxiong
 */

#ifndef UDT_PR_H_
#define UDT_PR_H_

#include "prio.h"

PRFileDesc* PR_OpenUDPTransportSocket(
		PRIntn af,
		const char* host = "127.0.0.1", // local host
		PRInt32 port = 51686);

#endif /* UDT_PR_H_ */
