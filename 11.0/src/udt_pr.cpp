/*
 * udt_pr.cpp
 *
 *  Created on: Oct 30, 2010
 *      Author: yiqxiong
 */
/*Jun18,2012:fixed polling mechanism,tom zhou,zs68j2ee@gmail.com*/


#include <time.h>
#include <string.h>
#include "nspr.h"
#include "nspr/prio.h"
#include "udtc.h"
#include "udt_pr.h"
//#include "nsDebug.h"

struct PRUdtSocketDesc {
	void (PR_CALLBACK *dtor)(PRFileDesc *fd);
	int udtfd;
};

static PRDescIdentity _udpt_desc_identity = PR_INVALID_IO_LAYER;

static bool _check_init(){
	if (PR_INVALID_IO_LAYER == _udpt_desc_identity) {
		udt_startup();
		_udpt_desc_identity = PR_GetUniqueIdentity("udptransport");
	}
	return true;
}

inline UDTSOCKET get_socket_from_fd(PRFileDesc *fd){
	return ((PRUdtSocketDesc*)fd->secret)->udtfd;
}

static PRStatus _udt_invalid_status() {
	return PR_FAILURE;
}

static PRInt16 _udt_invalid_int16() {
	return -1;
}
static PRIntn _udt_invalid_intn() {
	return -1;
}
static PRInt32 _udt_invalid_int32() {
	return -1;
}
static PRInt64 _udt_invalid_int64() {
	return -1;
}

static PRStatus _udt_close(PRFileDesc *fd) {
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
	UDTSOCKET s = get_socket_from_fd(fd);
	udt_close(s);
	return PR_SUCCESS;
}

static PRInt32 _udt_read(PRFileDesc *fd, void *buf, PRInt32 amount) {
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
	UDTSOCKET s = get_socket_from_fd(fd);
	int rc = udt_recv(s, (char*)buf, amount, 0);

	if(rc < 0) {
		int udterrcode = udt_getlasterror_code();

		if (UDT_EASYNCRCV == udterrcode) {
			PR_SetError(PR_WOULD_BLOCK_ERROR, 0);
		} else {
			PR_SetError(PR_IO_ERROR, 0);
		}
		return -1;
	}
	return rc;
}

static PRInt32 _udt_write(PRFileDesc *fd, const void *buf, PRInt32 amount) {
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
	UDTSOCKET s = get_socket_from_fd(fd);
	int rc = udt_send(s, (const char*)buf, amount, 0);

	if (rc < 0) {
		int udterrcode = udt_getlasterror_code();

		if (UDT_EASYNCSND == udterrcode) {
			PR_SetError(PR_WOULD_BLOCK_ERROR, 0);
		} else {
			PR_SetError(PR_IO_ERROR, 0);
		}
		return -1;
	}
	return rc;
}

static PRInt32 _udt_available(PRFileDesc *fd)
{
	UDTSOCKET s = get_socket_from_fd(fd);
	int rcnt, optlen;

	(udt_getsockopt(s, SOL_SOCKET, UDT_UDT_RCVDATA, &rcnt, &optlen) == 0);
	return rcnt;
}

static PRInt64 _udt_available64(PRFileDesc *fd)
{
	UDTSOCKET s = get_socket_from_fd(fd);
	int rcnt, optlen;

	(udt_getsockopt(s, SOL_SOCKET, UDT_UDT_RCVDATA, &rcnt, &optlen) == 0);
	return rcnt;
}

static PRFileDesc* _udt_accept(
		PRFileDesc *fd, PRNetAddr *addr, PRIntervalTime timeout) {
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
	// TODO
	return NULL;
}

static PRStatus _udt_bind(PRFileDesc *fd, const PRNetAddr *addr) {
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
	UDTSOCKET s = get_socket_from_fd(fd);
	struct sockaddr_in sock_addr;
	sock_addr.sin_family      = AF_INET;
	sock_addr.sin_port        = addr->inet.port;
	sock_addr.sin_addr.s_addr = addr->inet.ip;

	if (0 == udt_bind(s, (struct sockaddr *)&sock_addr, sizeof(sock_addr)))
		return PR_SUCCESS;
	return PR_FAILURE;
}

static PRStatus _udt_listen(PRFileDesc *fd, PRIntn backlog) {
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
	UDTSOCKET s = get_socket_from_fd(fd);

	if (0 == udt_listen(s, backlog))
		return PR_SUCCESS;
	return PR_FAILURE;
}

static PRStatus _udt_connect(PRFileDesc *fd, const PRNetAddr *addr,
		PRIntervalTime timeout){
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
	UDTSOCKET s = get_socket_from_fd(fd);
	struct sockaddr_in peer_addr;
	peer_addr.sin_family      = AF_INET;///(addr->raw.family == PR_AF_INET6)? AF_INET6:AF_INET;
	peer_addr.sin_port        = addr->inet.port;
	peer_addr.sin_addr.s_addr = addr->inet.ip;
	bool sync = true, old;
	int optlen;

	if (udt_getsockopt(s, SOL_SOCKET, UDT_UDT_RCVSYN, &old, &optlen) < 0)
		return PR_FAILURE;
	if (udt_setsockopt(s, SOL_SOCKET, UDT_UDT_RCVSYN, &sync, sizeof(sync)) < 0)
		return PR_FAILURE;

	if (udt_connect(s, (sockaddr*)&peer_addr, sizeof(peer_addr)) == 0) {
		// check for sync connect
		if (udt_getsockstate(s) == UDT_CONNECTED) {
			if (udt_setsockopt(s, SOL_SOCKET, UDT_UDT_RCVSYN, &old, sizeof(old)) < 0)
				return PR_FAILURE;
			return PR_SUCCESS;
		} else {
			return PR_FAILURE;
		}
	} else {
		return PR_FAILURE;
	}
}

static PRInt32 _udt_recv(PRFileDesc *fd, void *buf, PRInt32 amount,
		PRIntn flags, PRIntervalTime timeout) {
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
	UDTSOCKET s = get_socket_from_fd(fd);

	// peek is not supported in udt
	if (PR_MSG_PEEK & flags) {
		PR_SetError(PR_WOULD_BLOCK_ERROR, 0);
		return -1;
	}

	int rc = udt_recv(s, (char*)buf, amount, 0);

	if(rc < 0) {
		int udterrcode = udt_getlasterror_code();

		if (UDT_EASYNCRCV == udterrcode) {
			PR_SetError(PR_WOULD_BLOCK_ERROR, 0);
		} else {
			PR_SetError(PR_IO_ERROR, 0);
		}
		return -1;
	}
	return rc;
}

static PRInt32 _udt_send(PRFileDesc *fd, const void *buf, PRInt32 amount,
		PRIntn flags, PRIntervalTime timeout) {
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
	UDTSOCKET s = get_socket_from_fd(fd);
	int rc = udt_send(s, (char*)buf, amount, flags);

	// peek is not supported in udt
	if (PR_MSG_PEEK & flags) {
		PR_SetError(PR_WOULD_BLOCK_ERROR, 0);
		return -1;
	}

	if (rc < 0) {
		int udterrcode = udt_getlasterror_code();

		if (UDT_EASYNCSND == udterrcode) {
			PR_SetError(PR_WOULD_BLOCK_ERROR, 0);
		} else {
			PR_SetError(PR_IO_ERROR, 0);
		}
		return -1;
	}
	return rc;
}

static PRInt16 _udt_poll(PRFileDesc *fd, PRInt16 in_flags, PRInt16 *out_flags) {
	static int pcnt = 0;
	fprintf(stdout, "%s:%d, pool@%d\n", __func__, __LINE__, pcnt++);
	UDTSOCKET s = get_socket_from_fd(fd);
	*out_flags = 0;
	int udtev, optlen;
	static int rcnt = 0, wcnt = 0;

	if (udt_getsockopt(s, 0, UDT_UDT_EVENT, &udtev, &optlen) < 0) {
		// check error anyway
		*out_flags |= PR_POLL_WRITE | PR_POLL_READ;
	} else {
		if ((udtev & (UDT_UDT_EPOLL_IN | UDT_UDT_EPOLL_ERR)) && (in_flags & PR_POLL_READ)) {
			*out_flags |= PR_POLL_READ;
			fprintf(stdout, "%s:%d, read event at%d, %d\%\n", __func__, __LINE__, rcnt++, (rcnt * 100) / pcnt);
		}
		if ((udtev & (UDT_UDT_EPOLL_OUT | UDT_UDT_EPOLL_ERR)) && (in_flags & PR_POLL_WRITE)) {
			*out_flags |= PR_POLL_WRITE;
			fprintf(stdout, "%s:%d, write event at%d, %d\%\n", __func__, __LINE__, wcnt++, (wcnt * 100) / pcnt);
		}
	}

	return PR_POLL_READ | PR_POLL_WRITE;
}

static PRStatus _udt_getsockname(PRFileDesc *fd, PRNetAddr *addr)
{
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
    UDTSOCKET s = get_socket_from_fd(fd);
    struct sockaddr_storage address_storage;
    int len = sizeof(struct sockaddr_storage);
    struct sockaddr_in  * sin  = (struct sockaddr_in  *)&address_storage;
    struct sockaddr_in6 * sin6 = (struct sockaddr_in6 *)&address_storage;

    if (udt_getsockname(s, (struct sockaddr *)&address_storage, &len) < 0) {
        return PR_FAILURE;
    }

    if (sin->sin_family == AF_INET) {
    	addr->raw.family = PR_AF_INET;
    	addr->inet.port  = sin->sin_port;
    	addr->inet.ip    = sin->sin_addr.s_addr;
    } else {
    	addr->raw.family = PR_AF_INET6;
    	addr->ipv6.port  = sin6->sin6_port;
    	memcpy(addr->ipv6.ip._S6_un._S6_u8, &sin6->sin6_addr.s6_addr, 16);
    }

	return PR_SUCCESS;
}

static PRStatus _udt_getpeername(PRFileDesc *fd, PRNetAddr *addr)
{
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
    UDTSOCKET s = get_socket_from_fd(fd);
    struct sockaddr_storage address_storage;
    int len = sizeof(struct sockaddr_storage);
    struct sockaddr_in  * sin  = (struct sockaddr_in  *)&address_storage;
    struct sockaddr_in6 * sin6 = (struct sockaddr_in6 *)&address_storage;

	if (udt_getpeername(s, (struct sockaddr *)&address_storage, &len) < 0) {
		return PR_FAILURE;
	}

    if (sin->sin_family == AF_INET) {
    	addr->raw.family = PR_AF_INET;
    	addr->inet.port  = sin->sin_port;
    	addr->inet.ip    = sin->sin_addr.s_addr;
    } else {
    	addr->raw.family = PR_AF_INET6;
    	addr->ipv6.port  = sin6->sin6_port;
    	memcpy(addr->ipv6.ip._S6_un._S6_u8, &sin6->sin6_addr.s6_addr, 16);
    }

	return PR_SUCCESS;
}

//PRSocketOptionData opt;
//opt.option = PR_SockOpt_Nonblocking;
//opt.value.non_blocking = PR_TRUE;
//status = PR_SetSocketOption(fd, &opt);
static PRStatus _udt_setsocketoption(
		PRFileDesc *fd, const PRSocketOptionData *data) {
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
	UDTSOCKET s = get_socket_from_fd(fd);
	bool sync = false;
	uint32 sz = 0;

	switch(data->option) {
	case PR_SockOpt_Nonblocking:
		sync = (data->value.non_blocking == PR_TRUE) ? false : true;
		udt_setsockopt(s, SOL_SOCKET, UDT_UDT_RCVSYN, &sync, sizeof(sync));
		udt_setsockopt(s, SOL_SOCKET, UDT_UDT_SNDSYN, &sync, sizeof(sync));
		return PR_SUCCESS;
	case PR_SockOpt_SendBufferSize:
		sz = data->value.send_buffer_size;
		udt_setsockopt(s, SOL_SOCKET, UDT_UDT_SNDBUF, &sz, sizeof(sz));
		return PR_SUCCESS;
	default:
		return PR_FAILURE;
	}
}

// PR_DESC_LAYERED
PRIOMethods udptMethods = {
	PR_DESC_LAYERED,                            // file_type
	(PRCloseFN)_udt_close,                      // close
	_udt_read,                                  // read
	_udt_write,                                 // write
	_udt_available,                             // available
	_udt_available64,                           // available64
	(PRFsyncFN) _udt_invalid_status,            // fsync
	(PRSeekFN) _udt_invalid_int32,              // seek
	(PRSeek64FN) _udt_invalid_int64,            // seek64
	(PRFileInfoFN) _udt_invalid_status,         // fileInfo
	(PRFileInfo64FN) _udt_invalid_status,       // fileInfo64
	(PRWritevFN) _udt_invalid_int32,            // writev
	(PRConnectFN) _udt_connect,                 // connect
	(PRAcceptFN) _udt_accept,                   // accept
	(PRBindFN) _udt_bind,                       // bind
	(PRListenFN) _udt_listen,                   // listen
	(PRShutdownFN) _udt_invalid_status,         // shutdown
	(PRRecvFN) _udt_recv,                       // recv
	(PRSendFN) _udt_send,                       // send
	(PRRecvfromFN) _udt_invalid_int32,          // recvfrom
	(PRSendtoFN) _udt_invalid_int32,            // sendto
	(PRPollFN) _udt_poll,                       // poll
	(PRAcceptreadFN) _udt_invalid_int32,        // acceptread
	(PRTransmitfileFN) _udt_invalid_int32,      // transmitfile
	(PRGetsocknameFN) _udt_getsockname,         // getsockname
	(PRGetpeernameFN) _udt_getpeername,         // getpeername
	(PRReservedFN) _udt_invalid_intn,           // reserved_fn_6
	(PRReservedFN) _udt_invalid_intn,           // reserved_fn_5
	(PRGetsocketoptionFN) _udt_invalid_status,  // getsocketoption
	(PRSetsocketoptionFN) _udt_setsocketoption, // setsocketoption
	(PRSendfileFN) _udt_invalid_int32,          // sendfile
	(PRConnectcontinueFN) _udt_invalid_status,  // connectcontinue
	(PRReservedFN) _udt_invalid_intn,           // reserved_fn_3
	(PRReservedFN) _udt_invalid_intn,           // reserved_fn_2
	(PRReservedFN) _udt_invalid_intn,           // reserved_fn_1
	(PRReservedFN) _udt_invalid_intn,           // reserved_fn_0
};

static void _udtp_detor(PRFileDesc *fd) {
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
	PRUdtSocketDesc desc = *(PRUdtSocketDesc*)fd->secret;
	PR_Free(fd->secret);
	udt_close(desc.udtfd);
	desc.dtor(fd);
}

// In case host is not null, this socket will bind on host:port to punch hole.
PRFileDesc* PR_OpenUDPTransportSocket(PRIntn af, const char* host, PRInt32 port) {
	///fprintf(stdout, "%s:%d\n", __func__, __LINE__);
	_check_init();
	UDTSOCKET u_socket = UDT_INVALID_SOCK;
	PRUdtSocketDesc* p_desc = NULL;

	u_socket = udt_socket(af, SOCK_STREAM, 0);

	PRFileDesc* udpt_fd = PR_CreateIOLayerStub(_udpt_desc_identity, &udptMethods);

	p_desc = (PRUdtSocketDesc*)PR_Malloc(sizeof(PRUdtSocketDesc));

	if ((NULL != udpt_fd) && (NULL != p_desc) && (u_socket != UDT_INVALID_SOCK)) {
		memset(p_desc, 0, sizeof(PRUdtSocketDesc));
		p_desc->dtor    = udpt_fd->dtor;
		p_desc->udtfd   = u_socket;
		udpt_fd->secret = (PRFilePrivate*)p_desc;
		udpt_fd->dtor   = _udtp_detor;
		return udpt_fd;
	}

	if (UDT_INVALID_SOCK != u_socket)
		udt_close(u_socket);
	if (NULL != p_desc)
		PR_Free(p_desc);
	if (NULL != udpt_fd)
		PR_Close(udpt_fd);

	return NULL;
}
