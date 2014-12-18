/*
 * udt_pr.h
 *
 *  Created on: Oct 30, 2010
 *      Author: yiqxiong
 */

#ifndef UDT_PR_H_
#define UDT_PR_H_

#include "prio.h"

PRFileDesc* PR_OpenUDPTransportSocket(PRIntn af,
        const char* host, PRInt32 port);

#endif /* UDT_PR_H_ */
