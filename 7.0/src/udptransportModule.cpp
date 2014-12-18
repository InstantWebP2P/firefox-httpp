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

NS_DEFINE_NAMED_CID(UDT_SOCKETPROVIDER_CID);

NS_DEFINE_NAMED_CID(NS_HTTPPHANDLER_CID);

static const mozilla::Module::CIDEntry udptransport_cids[]={
    {&kNS_HTTPPHANDLER_CID, false, NULL, (mozilla::Module::ConstructorProcPtr)(udptHttppHandler::Create)},
    {&kUDT_SOCKETPROVIDER_CID, false, NULL, (mozilla::Module::ConstructorProcPtr)(udtSocketProvider::Create)},
    {NULL}
};

static const mozilla::Module::ContractIDEntry udptransport_contracts[] = {
    { UDT_HTTPPSOCKETPROVIDER_CONTRACTID, &kNS_HTTPPHANDLER_CID},
    { UDT_SOCKETPROVIDER_CONTRACTID, &kUDT_SOCKETPROVIDER_CID},
    { NULL }
};

static const mozilla::Module::CategoryEntry udptransport_categories [] ={
    { NULL }
};

static const mozilla::Module kHTTPP_Module = {
    mozilla::Module::kVersion,
    udptransport_cids,
    udptransport_contracts,
    udptransport_categories
};

//NS_IMPL_NSGETMODULE("httpp", gResComponents);
NSMODULE_DEFN(nsHTTPPModule) = &kHTTPP_Module;
