/*
 * udptProxyInfo.h
 *
 *  Created on: 2010-12-2
 *      Author: yiqxiong
 */

#ifndef UDPTPROXYINFO_H_
#define UDPTPROXYINFO_H_

#include "nsIProxyInfo.h"
#include "nsStringAPI.h"

//d3662e72-ee1c-48b3-8de6-d0d7a25c4e70
#define UDPT_PROXYINFO_IID \
{ \
	0xd3662e72, \
    0xee1c, \
    0x48b3, \
    {0x8d, 0xe6, 0xd0, 0xd7, 0xa2, 0x5c, 0x4e, 0x70} \
}

#define NS_PROXYINFO_IID \
{ /* ed42f751-825e-4cc2-abeb-3670711a8b85 */         \
    0xed42f751,                                      \
    0x825e,                                          \
    0x4cc2,                                          \
    {0xab, 0xeb, 0x36, 0x70, 0x71, 0x1a, 0x8b, 0x85} \
}

class udptProxyInfo  : public nsIProxyInfo
{
public:
  NS_DECLARE_STATIC_IID_ACCESSOR(NS_PROXYINFO_IID)

  NS_DECL_ISUPPORTS
  NS_DECL_NSIPROXYINFO

  //udptProxyInfo();
  udptProxyInfo(const char* type,
		  const nsACString& host,
		  PRInt32 port,
		  PRUint32 flags,
		  PRUint32 timeout);
private:
  ~udptProxyInfo();

protected:
  const char  *mType;  // pointer to statically allocated value
  nsCString    mHost;
  PRInt32      mPort;
  PRUint32     mFlags;
  PRUint32     mTimeout;
};

NS_DEFINE_STATIC_IID_ACCESSOR(udptProxyInfo, NS_PROXYINFO_IID)

#endif /* UDPTPROXYINFO_H_ */
