/*
 * upModule.cpp
 *
 *  Created on: Oct 26, 2010
 *      Author: yiqxiong
 */

#include "nsXPCOM.h"

#include "nsIGenericFactory.h"

/**
 * Components to be registered
 */
#include "udptransport.h"
#include "udtSocketProvider.h"

//NS_GENERIC_FACTORY_CONSTRUCTOR(udptransport)

//----------------------------------------------------------

static const nsModuleComponentInfo components[] =
{
	{
		UDT_SOCKETPROVIDER_CLASSNAME,
		UDT_SOCKETPROVIDER_CID,
		UDT_SOCKETPROVIDER_CONTRACTID,
		udtSocketProvider::Create,
	},
};

NS_IMPL_NSGETMODULE(udptransport, components)
