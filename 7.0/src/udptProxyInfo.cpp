/*
 * udptProxyInfo.cpp
 *
 *  Created on: 2010-12-2
 *      Author: yiqxiong
 */

#include "udptProxyInfo.h"

NS_IMPL_THREADSAFE_ISUPPORTS2(udptProxyInfo, udptProxyInfo, nsIProxyInfo)

udptProxyInfo::udptProxyInfo(const char* type,
		  const nsACString& host,
		  PRInt32 port,
		  PRUint32 flags,
		  PRUint32 timeout) {
	mType = type;
	mHost = host;
	mPort = port;
	mFlags = flags;
	mTimeout = timeout;
	// TODO Auto-generated constructor stub

}

udptProxyInfo::~udptProxyInfo() {
	// TODO Auto-generated destructor stub
}

NS_IMETHODIMP
udptProxyInfo::GetHost(nsACString &result)
{
  result = mHost;
  return NS_OK;
}

NS_IMETHODIMP
udptProxyInfo::GetPort(PRInt32 *result)
{
  *result = mPort;
  return NS_OK;
}

NS_IMETHODIMP
udptProxyInfo::GetType(nsACString &result)
{
  result = mType;
  return NS_OK;
}

NS_IMETHODIMP
udptProxyInfo::GetFlags(PRUint32 *result)
{
  *result = 0;
  return NS_OK;
}

NS_IMETHODIMP
udptProxyInfo::GetFailoverTimeout(PRUint32 *result)
{
  *result = mTimeout;
  return NS_OK;
}

NS_IMETHODIMP
udptProxyInfo::GetFailoverProxy(nsIProxyInfo **result)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
udptProxyInfo::SetFailoverProxy(nsIProxyInfo *proxy)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
udptProxyInfo::GetResolveFlags(PRUint32 *aResolveFlags)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}
