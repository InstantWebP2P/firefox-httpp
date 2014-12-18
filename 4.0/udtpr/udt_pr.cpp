/*
 * udt_pr.cpp
 *
 *  Created on: Oct 30, 2010
 *      Author: yiqxiong
 */

#include <time.h>
#include "nspr.h"
#include "nspr/prio.h"
#include "nsDebug.h"
#include "udt.h"
#include "udt_pr.h"

struct PRUdtSocketDesc {
    void (PR_CALLBACK *dtor)(PRFileDesc *fd);
    PRFileDesc* sock_pair0;
    PRFileDesc* sock_pair1;
    PRInt32 sign_cnt;
    PRInt32 rd_cnt;
    UDTSOCKET udtfd;
};

static PRDescIdentity _udpt_desc_identity = PR_INVALID_IO_LAYER;

static bool _check_init(){
    if(PR_INVALID_IO_LAYER == _udpt_desc_identity) {
    	UDT::startup();
        _udpt_desc_identity = PR_GetUniqueIdentity("udptransport");
    }
    return true;
}

inline UDTSOCKET get_socket_from_fd(PRFileDesc *fd){
    return ((PRUdtSocketDesc*)fd->secret)->udtfd;
}
inline PRFileDesc* get_pair_sock_from_fd(PRFileDesc *fd){
    return ((PRUdtSocketDesc*)fd->secret)->sock_pair0;
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

static PRInt32 _udt_read(PRFileDesc *fd, void *buf, PRInt32 amount) {
    UDTSOCKET s = get_socket_from_fd(fd);
    uint32_t dummy_data;
    int rc = 0;
    PRUdtSocketDesc* desc = (PRUdtSocketDesc*)(fd->secret);
    if (desc->sign_cnt != desc->rd_cnt) {
        PR_Recv(desc->sock_pair0, (char*)&dummy_data, sizeof(dummy_data), 0, 0);
        PR_AtomicIncrement(&desc->rd_cnt);
    }
    rc = UDT::recv(s, (char*)buf, amount, 0);
	if(rc < 0) {
	    int udterrcode = UDT::getlasterror().getErrorCode();
	    if (CUDTException::ECONNLOST == udterrcode) {
	        rc = 0;
	        PR_Close(desc->sock_pair0);
	        PR_Close(desc->sock_pair1);
	    } else if (CUDTException::ENOCONN == udterrcode) {
	        rc = 0;
            PR_Close(desc->sock_pair0);
            PR_Close(desc->sock_pair1);
	    }
	}
    return rc;
}

static PRInt32 _udt_write(PRFileDesc *fd, const void *buf, PRInt32 amount) {
    UDTSOCKET s = get_socket_from_fd(fd);
    int rc = UDT::send(s, (const char*)buf, amount, 0);
    if (rc < 0) {
        PRUdtSocketDesc* desc = (PRUdtSocketDesc*)(fd->secret);
        PR_Close(desc->sock_pair0);
        PR_Close(desc->sock_pair1);
    }
    return rc;
}

static PRStatus _udt_close(PRFileDesc *fd) {
    PRUdtSocketDesc* desc = (PRUdtSocketDesc*)(fd->secret);
    UDT::close(desc->udtfd);
    PR_Close(desc->sock_pair0);
    PR_Close(desc->sock_pair1);
	return PR_SUCCESS;
}

static PRFileDesc* _udt_accept(
    PRFileDesc *fd, PRNetAddr *addr, PRIntervalTime timeout) {
    // TODO
    return NULL;
}

static PRStatus _udt_bind(PRFileDesc *fd, const PRNetAddr *addr) {
    UDTSOCKET s = get_socket_from_fd(fd);
    sockaddr sock_addr;
    if (0 == UDT::bind(s, &sock_addr, sizeof(sock_addr)))
        return PR_SUCCESS;
    return PR_FAILURE;
}

static PRStatus _udt_listen(PRFileDesc *fd, PRIntn backlog) {
    UDTSOCKET s = get_socket_from_fd(fd);
    if (0 == UDT::listen(s, backlog))
        return PR_SUCCESS;
    return PR_FAILURE;
}

static PRStatus _udt_connect(PRFileDesc *fd, const PRNetAddr *addr,
        PRIntervalTime timeout){
    UDTSOCKET s = get_socket_from_fd(fd);
    struct sockaddr_in peer_addr;
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = addr->inet.port;
    peer_addr.sin_addr.s_addr = addr->inet.ip;

    if (UDT::ERROR == UDT::connect(s, (sockaddr*)&peer_addr, sizeof(peer_addr)))
        return PR_FAILURE;

	return PR_SUCCESS;
}

static PRInt32 _udt_recv(PRFileDesc *fd, void *buf, PRInt32 amount,
        PRIntn flags, PRIntervalTime timeout) {
    uint32_t dummy_data;
    PRUdtSocketDesc* desc = (PRUdtSocketDesc*)(fd->secret);
    if (desc->sign_cnt != desc->rd_cnt) {
        PR_Recv(desc->sock_pair0, (char*)&dummy_data, sizeof(dummy_data), 0, 0);
        PR_AtomicIncrement(&desc->rd_cnt);
    }
	// peek is not supported in udt
	if(PR_MSG_PEEK & flags)
		return PR_WOULD_BLOCK_ERROR;
    int rc = UDT::recv(desc->udtfd, (char*)buf, amount, 0);
    if(rc < 0) {
        int udterrcode = UDT::getlasterror().getErrorCode();
        if ((CUDTException::ECONNLOST == udterrcode) ||
           (CUDTException::ENOCONN == udterrcode)) {
            PR_Close(desc->sock_pair1);
            //rc = 0;
        }
    }
	return rc;
}

static PRInt32 _udt_send(PRFileDesc *fd, const void *buf, PRInt32 amount,
        PRIntn flags, PRIntervalTime timeout) {
    PRUdtSocketDesc* desc = (PRUdtSocketDesc*)(fd->secret);
    UDTSOCKET s = get_socket_from_fd(fd);
    int ret = UDT::send(s, (char*)buf, amount, flags);
    if (ret < 0) {
        int udterrcode = UDT::getlasterror().getErrorCode();
        if ((CUDTException::ECONNLOST == udterrcode) ||
           (CUDTException::ENOCONN == udterrcode)) {
            PR_Close(desc->sock_pair1);
            //ret = 0;
        }
    }
	return ret;
}

static PRInt16 _udt_poll(PRFileDesc *fd, PRInt16 in_flags, PRInt16 *out_flags) {
    PRUdtSocketDesc* desc = (PRUdtSocketDesc*)(fd->secret);
    *out_flags = 0;

	if (desc->sign_cnt > desc->rd_cnt) {
		*out_flags |= (in_flags & PR_POLL_READ);
	} else {
	    if(in_flags & PR_POLL_WRITE)
	        *out_flags |= PR_POLL_WRITE;
	}

	return PR_POLL_READ | PR_POLL_WRITE;
}

//PRSocketOptionData opt;
//opt.option = PR_SockOpt_Nonblocking;
//opt.value.non_blocking = PR_TRUE;
//status = PR_SetSocketOption(fd, &opt);
static PRStatus _udt_setsocketoption(
    PRFileDesc *fd, const PRSocketOptionData *data) {
    UDTSOCKET s = get_socket_from_fd(fd);
    uint32 val = 0;
    switch(data->option) {
    case PR_SockOpt_Nonblocking:
    	val = data->value.non_blocking;
        UDT::setsockopt(s, SOL_SOCKET, UDT_SNDSYN, &val, sizeof(val));
        UDT::setsockopt(s, SOL_SOCKET, UDT_RCVSYN, &val, sizeof(val));
        return PR_SUCCESS;
	case PR_SockOpt_SendBufferSize:
		val = data->value.send_buffer_size;
		UDT::setsockopt(s, SOL_SOCKET, UDT_SNDBUF, &val, sizeof(val));
		return PR_SUCCESS;
	default:
        return PR_FAILURE;
    }
}

//PR_DESC_LAYERED
PRIOMethods udptMethods ={
        PR_DESC_LAYERED,                       // file_type
        (PRCloseFN)_udt_close,                 // close
        _udt_read,                             // read
        _udt_write,                            // write
        (PRAvailableFN) _udt_invalid_int32,    // available
        (PRAvailable64FN) _udt_invalid_int64,  // available64
        (PRFsyncFN) _udt_invalid_status,       // fsync
        (PRSeekFN) _udt_invalid_int32,         // seek
        (PRSeek64FN) _udt_invalid_int64,       // seek64
        (PRFileInfoFN) _udt_invalid_status,    // fileInfo
        (PRFileInfo64FN) _udt_invalid_status,  // fileInfo64
        (PRWritevFN) _udt_invalid_int32,       // writev
        (PRConnectFN) _udt_connect,            // connect
        (PRAcceptFN) _udt_accept,              // accept
        (PRBindFN) _udt_bind,                  // bind
        (PRListenFN) _udt_listen,              // listen
        (PRShutdownFN) _udt_invalid_status,    // shutdown
        (PRRecvFN) _udt_recv,                  // recv
        (PRSendFN) _udt_send,                  // send
        (PRRecvfromFN) _udt_invalid_int32,     // recvfrom
        (PRSendtoFN) _udt_invalid_int32,       // sendto
        (PRPollFN) _udt_poll,                  // poll
        (PRAcceptreadFN) _udt_invalid_int32,   // acceptread
        (PRTransmitfileFN) _udt_invalid_int32, // transmitfile
        (PRGetsocknameFN) _udt_invalid_status, // getsockname
        (PRGetpeernameFN) _udt_invalid_status, // getpeername
        (PRReservedFN) _udt_invalid_intn,      // reserved_fn_6
        (PRReservedFN) _udt_invalid_intn,      // reserved_fn_5
        (PRGetsocketoptionFN) _udt_invalid_status,// getsocketoption
        (PRSetsocketoptionFN) _udt_setsocketoption,// setsocketoption
        (PRSendfileFN) _udt_invalid_int32,     // sendfile
        (PRConnectcontinueFN) _udt_invalid_status,// connectcontinue
        (PRReservedFN) _udt_invalid_intn,      // reserved_fn_3
        (PRReservedFN) _udt_invalid_intn,      // reserved_fn_2
        (PRReservedFN) _udt_invalid_intn,      // reserved_fn_1
        (PRReservedFN) _udt_invalid_intn,      // reserved_fn_0
};

static void _udtp_detor(PRFileDesc *fd) {
    PRUdtSocketDesc desc = *(PRUdtSocketDesc*)fd->secret;
    PR_Free(fd->secret);
    PR_Close(desc.sock_pair0);
    PR_Close(desc.sock_pair1);
    UDT::close(desc.udtfd);
    desc.dtor(fd);
}

static void _udp_event_cb(UDTSOCKET u, void* parm, int events){
    uint32_t data = 0;
    PRUdtSocketDesc* desc = (PRUdtSocketDesc*)parm;
    PR_AtomicIncrement(&desc->sign_cnt);
    int rc = PR_Send(desc->sock_pair1, (const char*)&data, sizeof(data), 0, 0);
}

PRFileDesc* PR_OpenUDPTransportSocket(PRIntn af, const char* host,
        PRInt32 port) {
    _check_init();
    PRFileDesc* socks[2] = {0};
    UDTSOCKET udt_socket = UDT::INVALID_SOCK;
    PRUdtSocketDesc* p_desc = NULL;

    if (UDT::INVALID_SOCK == (udt_socket = UDT::socket(af, SOCK_STREAM, 0)))
        goto cleanup;

    if (PR_SUCCESS != PR_NewTCPSocketPair(socks))
        goto cleanup;

    PRFileDesc* udpt_fd = PR_CreateIOLayerStub(_udpt_desc_identity,
            &udptMethods);

    p_desc = (PRUdtSocketDesc*)PR_Malloc(sizeof(PRUdtSocketDesc));
    if (UDT::bind_events(udt_socket, UDT_EPOLL_IN, _udp_event_cb, p_desc) < 0)
        goto cleanup;

    if ((NULL != udpt_fd) && (NULL != p_desc)) {
        PRFileDesc * sockpair_fd = socks[0];
        memset(p_desc, 0, sizeof(PRUdtSocketDesc));
    	p_desc->sock_pair0 = socks[0];
    	p_desc->sock_pair1 = socks[1];
    	p_desc->dtor       = udpt_fd->dtor;
    	p_desc->udtfd      = udt_socket;
        udpt_fd->secret = (PRFilePrivate*)p_desc;
        udpt_fd->lower = sockpair_fd;
        sockpair_fd->higher = udpt_fd;
        udpt_fd->dtor = _udtp_detor;
        return udpt_fd;
    }

cleanup:
    if (NULL != socks[0])
        PR_Close(socks[0]);
    if (NULL != socks[1])
        PR_Close(socks[1]);
    if (UDT::INVALID_SOCK == udt_socket)
        UDT::close(udt_socket);
    if (NULL != p_desc)
        PR_Free(p_desc);
    if (NULL != udpt_fd)
        PR_Close(udpt_fd);

    return NULL;
}
