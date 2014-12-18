/*
 * udptModule.cpp
 *
 *  Created on: 2010-11-30
 *      Author: yiqxiong
 */

#include "nsXPCOM.h"
#include "nsIModule.h"
#include "mozilla/Module.h"
#include "udptHttppHandler.h"
#include "udptransport.h"
#include "udtSocketProvider.h"
//#include "WebSocketPPChannel.h"
//#include "nsWebSocketPP.h"


NS_DEFINE_NAMED_CID(IWEBPP_SOCKETPROVIDER_CID);

NS_DEFINE_NAMED_CID(IWEBPP_HTTPPHANDLER_CID);

//NS_DEFINE_NAMED_CID(IWEBPP_WSPPHANDLER_CID);

//NS_DEFINE_NAMED_CID(IWEBPP_WEBSOCKETPP_CID);

//NS_GENERIC_FACTORY_CONSTRUCTOR(nsWebSocket)

static const mozilla::Module::CIDEntry iwebpp_cids[]={
    ///{ &kIWEBPP_WEBSOCKET_CID, false, NULL, nsWebSocketConstructor },
    ///{ &kIWEBPP_WSPPHANDLER_CID, false, NULL, WebSocketPPChannel::Create },
    { &kIWEBPP_HTTPPHANDLER_CID, false, NULL, udptHttppHandler::Create },
    { &kIWEBPP_SOCKETPROVIDER_CID, false, NULL, udtSocketProvider::Create },
    { NULL }
};

static const mozilla::Module::ContractIDEntry iwebpp_contracts[] = {
    ///{ IWEBPP_WEBSOCKETPP_CONTRACTID, &kIWEBPP_WEBSOCKETPP_CID },
    ///{ IWEBPP_WEBSOCKETPPPROVIDER_CONTRACTID, &kIWEBPP_WSPPHANDLER_CID },
    { IWEBPP_HTTPPSOCKETPROVIDER_CONTRACTID, &kIWEBPP_HTTPPHANDLER_CID },
    { IWEBPP_SOCKETPROVIDER_CONTRACTID, &kIWEBPP_SOCKETPROVIDER_CID },
    { NULL }
};

static const mozilla::Module::CategoryEntry iwebpp_categories [] ={
    { NULL }
};

// iWebPP module startup hook
static nsresult iWebPPStartup()
{
    return NS_OK;
}

// iWebPP module shutdown hook
static void iWebPPShutdown()
{
    // Release the Websocket Admission Manager
    ///WebSocketPPChannel::Shutdown();
}

static const mozilla::Module iWebPP_Module = {
    mozilla::Module::kVersion,
    iwebpp_cids,
    iwebpp_contracts,
    iwebpp_categories,
    NULL,
    iWebPPStartup,
    iWebPPShutdown
};

//NS_IMPL_NSGETMODULE("iwebpp", gResComponents);
NSMODULE_DEFN(iWebPPModule) = &iWebPP_Module;
