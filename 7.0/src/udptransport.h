/*
 * udptransport.h
 *
 *  Created on: Oct 26, 2010
 *      Author: yiqxiong
 */

#ifndef UDPTRANSPORT_H_
#define UDPTRANSPORT_H_

#define UDT_SOCKETPROVIDER_CID \
	{ 0x0c081258, 0xea25, 0x4970, \
	{ 0xae, 0xce, 0x7d, 0x71, 0xae, 0x7f, 0xa7, 0x8a } }

#define UDT_SOCKETPROVIDER_CLASSNAME	"UDPTransport Socket Provider Service"

#define UDT_SOCKETPROVIDER_CONTRACTID \
  NS_NETWORK_SOCKET_CONTRACTID_PREFIX "udptransport"

#define UDT_HTTPPSOCKETPROVIDER_CONTRACTID \
  NS_NETWORK_PROTOCOL_CONTRACTID_PREFIX "httpp"

#endif /* UDPTRANSPORT_H_ */
