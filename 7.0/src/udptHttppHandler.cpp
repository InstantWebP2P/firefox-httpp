/*
 * HttppHandler.h
 *
 *  Created on: 2010-11-28
 *      Author: yiqxiong
 */

#include "nsNetCID.h"
#include "nsNetUtil.h"
#include "nsIURI.h"
#include "udptHttppHandler.h"
#include "nsIStandardURL.h"
#include "nsServiceManagerUtils.h"
#include "udptProxyInfo.h"

NS_IMPL_THREADSAFE_ISUPPORTS4(udptHttppHandler,
                              nsIHttpProtocolHandler,
                              nsIProxiedProtocolHandler,
                              nsIProtocolHandler,
                              nsISupportsWeakReference)


nsresult
udptHttppHandler::Init()
{
	nsCOMPtr<nsIHttpProtocolHandler> httpHandler(
            do_GetService(NS_NETWORK_PROTOCOL_CONTRACTID_PREFIX "http"));
    NS_ASSERTION(httpHandler.get() != nsnull, "no http handler?");
    _http_handler = httpHandler;
    return NS_OK;
}

NS_METHOD
udptHttppHandler::Create(nsISupports* aOuter, const nsIID& aIID, void** aResult) {

	udptHttppHandler* ph = new udptHttppHandler();
    if (ph == nsnull)
        return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(ph);
    nsresult rv = ph->QueryInterface(aIID, aResult);
    ph->Init();
    NS_RELEASE(ph);
    return rv;
}

NS_IMETHODIMP
udptHttppHandler::GetScheme(nsACString &aScheme)
{
    aScheme.AssignLiteral("httpp");
    return NS_OK;
}

NS_IMETHODIMP
udptHttppHandler::GetDefaultPort(PRInt32 *aPort)
{
    *aPort = 80;
    return NS_OK;
}

NS_IMETHODIMP
udptHttppHandler::GetProtocolFlags(PRUint32 *aProtocolFlags)
{
    *aProtocolFlags = (URI_STD | ALLOWS_PROXY | ALLOWS_PROXY_HTTP | URI_LOADABLE_BY_ANYONE);
    return NS_OK;
}

NS_IMETHODIMP
udptHttppHandler::NewURI(const nsACString &aSpec,
                       const char *aOriginCharset,
                       nsIURI *aBaseURI,
                       nsIURI **_retval)
{
	_http_handler->NewURI(aSpec, aOriginCharset, aBaseURI, _retval);
    return NS_OK;
}

NS_IMETHODIMP
udptHttppHandler::NewChannel(nsIURI *uri, nsIChannel **_retval)
{
    if (!_http_handler)
      return NS_ERROR_UNEXPECTED;

    nsCString host;
    PRInt32 port;
    uri->GetHost(host);
    uri->GetPort(&port);

    udptProxyInfo *proxyInfo = new udptProxyInfo("udptransport",
    		host, port, 0, 0);
    if (!proxyInfo)
        return NS_ERROR_OUT_OF_MEMORY;

    NS_ADDREF(proxyInfo);

//    nsCString scheme;
//    scheme.AssignLiteral("http");
//    uri->SetScheme(scheme);
    return NewProxiedChannel(uri, proxyInfo, _retval);
}

NS_IMETHODIMP
udptHttppHandler::AllowPort(PRInt32 aPort, const char *aScheme, PRBool *_retval)
{
    // don't override anything.
    *_retval = PR_FALSE;
    return NS_OK;
}
