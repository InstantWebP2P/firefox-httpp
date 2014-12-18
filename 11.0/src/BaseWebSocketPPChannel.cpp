/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set sw=2 ts=8 et tw=80 : */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla.
 *
 * The Initial Developer of the Original Code is
 * Mozilla Foundation.
 * Portions created by the Initial Developer are Copyright (C) 2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Josh Matthews <josh@joshmatthews.net>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "BaseWebSocketPPChannel.h"
#include "nsILoadGroup.h"
#include "nsIInterfaceRequestor.h"
#include "nsAutoPtr.h"
#include "nsIURI.h"
#include "nsIStandardURL.h"
#include "nsServiceManagerUtils.h"
#include "nsNetCID.h"
#include "nsNetUtil.h"


#define LOG(x) 

using namespace mozilla;

BaseWebSocketPPChannel::BaseWebSocketPPChannel()
  : mEncrypted(false)
{}

//-----------------------------------------------------------------------------
// BaseWebSocketPPChannel::nsIWebSocketPPChannel
//-----------------------------------------------------------------------------

NS_IMETHODIMP
BaseWebSocketPPChannel::GetOriginalURI(nsIURI **aOriginalURI)
{
  LOG(("BaseWebSocketPPChannel::GetOriginalURI() %p\n", this));

  if (!mOriginalURI)
    return NS_ERROR_NOT_INITIALIZED;
  NS_ADDREF(*aOriginalURI = mOriginalURI);
  return NS_OK;
}

NS_IMETHODIMP
BaseWebSocketPPChannel::GetURI(nsIURI **aURI)
{
  LOG(("BaseWebSocketPPChannel::GetURI() %p\n", this));

  if (!mOriginalURI)
    return NS_ERROR_NOT_INITIALIZED;
  if (mURI)
    NS_ADDREF(*aURI = mURI);
  else
    NS_ADDREF(*aURI = mOriginalURI);
  return NS_OK;
}

NS_IMETHODIMP
BaseWebSocketPPChannel::
GetNotificationCallbacks(nsIInterfaceRequestor **aNotificationCallbacks)
{
  LOG(("BaseWebSocketPPChannel::GetNotificationCallbacks() %p\n", this));
  NS_IF_ADDREF(*aNotificationCallbacks = mCallbacks);
  return NS_OK;
}

NS_IMETHODIMP
BaseWebSocketPPChannel::
SetNotificationCallbacks(nsIInterfaceRequestor *aNotificationCallbacks)
{
  LOG(("BaseWebSocketPPChannel::SetNotificationCallbacks() %p\n", this));
  mCallbacks = aNotificationCallbacks;
  return NS_OK;
}

NS_IMETHODIMP
BaseWebSocketPPChannel::GetLoadGroup(nsILoadGroup **aLoadGroup)
{
  LOG(("BaseWebSocketPPChannel::GetLoadGroup() %p\n", this));
  NS_IF_ADDREF(*aLoadGroup = mLoadGroup);
  return NS_OK;
}

NS_IMETHODIMP
BaseWebSocketPPChannel::SetLoadGroup(nsILoadGroup *aLoadGroup)
{
  LOG(("BaseWebSocketPPChannel::SetLoadGroup() %p\n", this));
  mLoadGroup = aLoadGroup;
  return NS_OK;
}

NS_IMETHODIMP
BaseWebSocketPPChannel::GetExtensions(nsACString &aExtensions)
{
 /// LOG(("BaseWebSocketPPChannel::GetExtensions() %p\n", this));
  aExtensions = mNegotiatedExtensions;
  return NS_OK;
}

NS_IMETHODIMP
BaseWebSocketPPChannel::GetProtocol(nsACString &aProtocol)
{
  LOG(("BaseWebSocketPPChannel::GetProtocol() %p\n", this));
  aProtocol = mProtocol;
  return NS_OK;
}

NS_IMETHODIMP
BaseWebSocketPPChannel::SetProtocol(const nsACString &aProtocol)
{
  LOG(("BaseWebSocketPPChannel::SetProtocol() %p\n", this));
  mProtocol = aProtocol;                        /* the sub protocol */
  return NS_OK;
}

//-----------------------------------------------------------------------------
// BaseWebSocketPPChannel::nsIProtocolHandler
//-----------------------------------------------------------------------------


NS_IMETHODIMP
BaseWebSocketPPChannel::GetScheme(nsACString &aScheme)
{
  LOG(("BaseWebSocketPPChannel::GetScheme() %p\n", this));

  if (mEncrypted)
    aScheme.AssignLiteral("wsspp");
  else
    aScheme.AssignLiteral("wspp");
  return NS_OK;
}

NS_IMETHODIMP
BaseWebSocketPPChannel::GetDefaultPort(PRInt32 *aDefaultPort)
{
  LOG(("BaseWebSocketPPChannel::GetDefaultPort() %p\n", this));

  if (mEncrypted)
    *aDefaultPort = kDefaultWSSPort;
  else
    *aDefaultPort = kDefaultWSPort;
  return NS_OK;
}

NS_IMETHODIMP
BaseWebSocketPPChannel::GetProtocolFlags(PRUint32 *aProtocolFlags)
{
  LOG(("BaseWebSocketPPChannel::GetProtocolFlags() %p\n", this));

  *aProtocolFlags = URI_NORELATIVE | URI_NON_PERSISTABLE | ALLOWS_PROXY | 
      ALLOWS_PROXY_HTTP | URI_DOES_NOT_RETURN_DATA | URI_DANGEROUS_TO_LOAD;
  return NS_OK;
}

NS_IMETHODIMP
BaseWebSocketPPChannel::NewURI(const nsACString & aSpec, const char *aOriginCharset,
                             nsIURI *aBaseURI, nsIURI **_retval NS_OUTPARAM)
{
	LOG(("BaseWebSocketPPChannel::NewURI() %p\n", this));

	PRInt32 port;
	nsresult rv = GetDefaultPort(&port);
	if (NS_FAILED(rv))
		return rv;

#if 0
	nsRefPtr<nsStandardURL> url = new nsStandardURL();
#else
	nsCOMPtr<nsIStandardURL> url = do_CreateInstance(NS_STANDARDURL_CONTRACTID, &rv);
	if (NS_FAILED(rv))
		return rv;
#endif

	rv = url->Init(nsIStandardURL::URLTYPE_AUTHORITY, port, aSpec,
			aOriginCharset, aBaseURI);
	if (NS_FAILED(rv))
		return rv;
	NS_ADDREF(*_retval = dynamic_cast<nsIURI *>(url.get()));
	return NS_OK;
}

NS_IMETHODIMP
BaseWebSocketPPChannel::NewChannel(nsIURI *aURI, nsIChannel **_retval NS_OUTPARAM)
{
  LOG(("BaseWebSocketPPChannel::NewChannel() %p\n", this));
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
BaseWebSocketPPChannel::AllowPort(PRInt32 port, const char *scheme,
                                bool *_retval NS_OUTPARAM)
{
  LOG(("BaseWebSocketPPChannel::AllowPort() %p\n", this));

  // do not override any blacklisted ports
  *_retval = false;
  return NS_OK;
}
