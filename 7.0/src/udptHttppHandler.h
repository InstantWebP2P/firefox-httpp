/*
 * HttppHandler.h
 *
 *  Created on: 2010-11-28
 *      Author: yiqxiong
 */

#ifndef udptHttppHandler_h_
#define udptHttppHandler_h_

#include "nsStringAPI.h"
//#include "nsXPIDLString.h"
#include "nsCOMPtr.h"
#include "nsWeakReference.h"

#include "nsIHttpProtocolHandler.h"
#include "nsIProtocolProxyService.h"
#include "nsIIOService.h"
#include "nsIObserver.h"
#include "nsIObserverService.h"
#include "nsIProxyObjectManager.h"
#include "nsIStreamConverterService.h"
#include "nsICacheSession.h"
#include "nsICookieService.h"
#include "nsIIDNService.h"
#include "nsITimer.h"

//#include "nsHttpHandler.h"

// {1f443279-d03c-4810-96b7-660ed6693461}
#define NS_HTTPPHANDLER_CID     \
{ 0x1f443279, 0xd03c, 0x4810, \
    { 0x66, 0x0e, 0xd6, 0x69, 0x34, 0x61} }

class nsHttpHandler;
extern nsHttpHandler *gHttpHandler;

//-----------------------------------------------------------------------------
// nsHttpsHandler - thin wrapper to distinguish the HTTP handler from the
//                  HTTPS handler (even though they share the same impl).
//-----------------------------------------------------------------------------

class udptHttppHandler : public nsIHttpProtocolHandler
                       , public nsSupportsWeakReference
{
public:
    // we basically just want to override GetScheme and GetDefaultPort...
    // all other methods should be forwarded to the nsHttpHandler instance.

    NS_DECL_ISUPPORTS
    NS_DECL_NSIPROTOCOLHANDLER
    NS_FORWARD_NSIPROXIEDPROTOCOLHANDLER (_http_handler->)
    NS_FORWARD_NSIHTTPPROTOCOLHANDLER    (_http_handler->)

    udptHttppHandler() { }
    virtual ~udptHttppHandler() { }

    nsresult Init();

    // Define a Create method to be used with a factory:
    static NS_METHOD Create(nsISupports* aOuter, const nsIID& aIID, void* *aResult);
private:
    nsCOMPtr<nsIHttpProtocolHandler> _http_handler;
};

#endif /* udptHttppHandler_h_ */
