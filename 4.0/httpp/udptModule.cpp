/*
 * udptModule.cpp
 *
 *  Created on: 2010-11-30
 *      Author: yiqxiong
 */

#include "nsIGenericFactory.h"
#include "udptHttppHandler.h"

static const nsModuleComponentInfo gResComponents[] = {
    { "The HTTP P2P Protocol Handler",
      NS_HTTPPHANDLER_CID,
      NS_NETWORK_PROTOCOL_CONTRACTID_PREFIX "httpp",
      udptHttppHandler::Create
    }
};

NS_IMPL_NSGETMODULE("httpp", gResComponents);
