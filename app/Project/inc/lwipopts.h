/*!
    \file  lwipopts.h
    \brief LwIP options configuration 
*/

/*
    Copyright (C) 2016 GigaDevice

    2016-08-15, V1.0.0, firmware for GD32F4xx
*/

#ifndef LWIPOPTS_H
#define LWIPOPTS_H


#define SYS_LIGHTWEIGHT_PROT    0                        /* SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection 
                                                            for certain critical regions during buffer allocation,
                                                            deallocation and memory allocation and deallocation */                                                            

#define NO_SYS                  1                        /* NO_SYS==1: provides VERY minimal functionality. 
                                                            Otherwise, use lwIP facilities */

/*  memory options  */
#define MEM_ALIGNMENT           4                        /* should be set to the alignment of the CPU for which lwIP
                                                            is compiled. 4 byte alignment -> define MEM_ALIGNMENT 
                                                            to 4, 2 byte alignment -> define MEM_ALIGNMENT to 2 */

#define MEM_SIZE                (50*1024)                /* the size of the heap memory, if the application will 
                                                            send a lot of data that needs to be copied, this should
                                                            be set high */

#define MEMP_NUM_PBUF           100                       /* the number of memp struct pbufs. If the application
                                                            sends a lot of data out of ROM (or other static memory),
                                                            this should be set high */

#define MEMP_NUM_UDP_PCB        6                        /* the number of UDP protocol control blocks, one
                                                            per active UDP "connection" */

#define MEMP_NUM_TCP_PCB        20                       /* the number of simulatenously active TCP connections */

#define MEMP_NUM_TCP_PCB_LISTEN 6                        /* the number of listening TCP connections */

#define MEMP_NUM_TCP_SEG        ((30* TCP_SND_BUF)/TCP_MSS)  /* the number of simultaneously queued TCP segments */

#define MEMP_NUM_SYS_TIMEOUT    10                        /* the number of simulateously active timeouts */


/* Pbuf options */
#define PBUF_POOL_SIZE          35                       /* the number of buffers in the pbuf pool */
#define PBUF_POOL_BUFSIZE       1500                     /* the size of each pbuf in the pbuf pool */

/* TCP options */
#define LWIP_TCP                1
#define TCP_TTL                 255

#define TCP_QUEUE_OOSEQ         0                        /* controls if TCP should queue segments that arrive out of
                                                            order, Define to 0 if your device is low on memory. */

#define TCP_MSS                 (1500 - 40)              /* TCP Maximum segment size, 
                                                            TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

#define TCP_SND_BUF             (10*TCP_MSS)              /* TCP sender buffer space (bytes) */

#define TCP_SND_QUEUELEN        ((20* TCP_SND_BUF)/TCP_MSS)   /* TCP sender buffer space (pbufs), this must be at least
                                                            as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work */

#define TCP_WND                 (10*TCP_MSS)              /* TCP receive window */
                                                   

/* ICMP options */
#define LWIP_ICMP               1


/* DHCP options */
#define LWIP_DHCP               1                        /* define to 1 if you want DHCP configuration of interfaces,
                                                            DHCP is not implemented in lwIP 0.5.1, however, so
                                                            turning this on does currently not work. */

/* UDP options */
#define LWIP_UDP                1
#define UDP_TTL                 255


/* statistics options */
#define LWIP_STATS              0
#define LWIP_PROVIDE_ERRNO      1

/* checksum options */
#define CHECKSUM_BY_HARDWARE                             /* computing and verifying the IP, UDP, TCP and ICMP
                                                            checksums by hardware */

/* sequential layer options */
#define LWIP_NETCONN            0                        /* set to 1 to enable netconn API (require to use api_lib.c) */

/* socket options */
#define LWIP_SOCKET             0                        /* set to 1 to enable socket API (require to use sockets.c) */

/* Lwip debug options */
//#define LWIP_DEBUG            1

#define LWIP_TCP_KEEPALIVE			1
#define TCP_KEEPIDLE_DEFAULT    6000 									// 60秒内连接双方都无数据，则发起保活探测（该值默认为2小时）
#define TCP_KEEPINTVL_DEFAULT   1000									// 每1秒发送一次保活探测
#define TCP_KEEPCNT_DEFAULT     3 										// 一共发送9次保活探测包，如果这9个包对方均无回应，则表示连接异常，内核关闭连接，并发送err回调到用户程序
#define  TCP_MAXIDLE  					TCP_KEEPCNT_DEFAULT * TCP_KEEPINTVL_DEFAULT


#ifdef CHECKSUM_BY_HARDWARE
    /* CHECKSUM_GEN_IP==0: generate checksums by hardware for outgoing IP packets.*/
    #define CHECKSUM_GEN_IP                 0
    /* CHECKSUM_GEN_UDP==0: generate checksums by hardware for outgoing UDP packets.*/
    #define CHECKSUM_GEN_UDP                0
    /* CHECKSUM_GEN_TCP==0: generate checksums by hardware for outgoing TCP packets.*/
    #define CHECKSUM_GEN_TCP                0 
    /* CHECKSUM_CHECK_IP==0: check checksums by hardware for incoming IP packets.*/
    #define CHECKSUM_CHECK_IP               0
    /* CHECKSUM_CHECK_UDP==0: check checksums by hardware for incoming UDP packets.*/
    #define CHECKSUM_CHECK_UDP              0
    /* CHECKSUM_CHECK_TCP==0: check checksums by hardware for incoming TCP packets.*/
    #define CHECKSUM_CHECK_TCP              0
//    #define CHECKSUM_GEN_ICMP               0
#else
    /* CHECKSUM_GEN_IP==1: generate checksums in software for outgoing IP packets.*/
    #define CHECKSUM_GEN_IP                 1
    /* CHECKSUM_GEN_UDP==1: generate checksums in software for outgoing UDP packets.*/
    #define CHECKSUM_GEN_UDP                1
    /* CHECKSUM_GEN_TCP==1: generate checksums in software for outgoing TCP packets.*/
    #define CHECKSUM_GEN_TCP                1
    /* CHECKSUM_CHECK_IP==1: check checksums in software for incoming IP packets.*/
    #define CHECKSUM_CHECK_IP               1
    /* CHECKSUM_CHECK_UDP==1: check checksums in software for incoming UDP packets.*/
    #define CHECKSUM_CHECK_UDP              1
    /* CHECKSUM_CHECK_TCP==1: check checksums in software for incoming TCP packets.*/
    #define CHECKSUM_CHECK_TCP              1
//    #define CHECKSUM_GEN_ICMP               1
#endif



#if 0
#define U8_F "c"
#define S8_F "c"
#define X8_F "x"
#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

#define LWIP_PLATFORM_DIAG(x) {printf x;}
#define LWIP_DEBUG
#endif


//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_OFF
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_WARNING
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_SERIOUS
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_SEVERE

//#define LWIP_DBG_TYPES_ON               LWIP_DBG_ON
//#define LWIP_DBG_TYPES_ON               (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH)

//#define ETHARP_DEBUG                    LWIP_DBG_ON     
//#define NETIF_DEBUG                     LWIP_DBG_ON     
//#define PBUF_DEBUG                      LWIP_DBG_ON
//#define API_LIB_DEBUG                   LWIP_DBG_ON
//#define API_MSG_DEBUG                   LWIP_DBG_ON
//#define SOCKETS_DEBUG                   LWIP_DBG_ON
//#define ICMP_DEBUG                      LWIP_DBG_ON
//#define IGMP_DEBUG                      LWIP_DBG_ON
//#define INET_DEBUG                      LWIP_DBG_ON
//#define IP_DEBUG                        LWIP_DBG_ON     
//#define IP_REASS_DEBUG                  LWIP_DBG_ON
//#define RAW_DEBUG                       LWIP_DBG_ON
//#define MEM_DEBUG                       LWIP_DBG_ON
//#define MEMP_DEBUG                      LWIP_DBG_ON
//#define SYS_DEBUG                       LWIP_DBG_ON
//#define TCP_DEBUG                       LWIP_DBG_ON
//#define TCP_INPUT_DEBUG                 LWIP_DBG_ON
//#define TCP_FR_DEBUG                    LWIP_DBG_ON
//#define TCP_RTO_DEBUG                   LWIP_DBG_ON
//#define TCP_CWND_DEBUG                  LWIP_DBG_ON
//#define TCP_WND_DEBUG                   LWIP_DBG_ON
//#define TCP_OUTPUT_DEBUG                LWIP_DBG_ON
//#define TCP_RST_DEBUG                   LWIP_DBG_ON
//#define TCP_QLEN_DEBUG                  LWIP_DBG_ON
//#define UDP_DEBUG                       LWIP_DBG_ON     
//#define TCPIP_DEBUG                     LWIP_DBG_ON
//#define PPP_DEBUG                       LWIP_DBG_ON
//#define SLIP_DEBUG                      LWIP_DBG_ON
//#define DHCP_DEBUG                      LWIP_DBG_ON     
//#define AUTOIP_DEBUG                    LWIP_DBG_ON
//#define SNMP_MSG_DEBUG                  LWIP_DBG_ON
//#define SNMP_MIB_DEBUG                  LWIP_DBG_ON
//#define DNS_DEBUG                       LWIP_DBG_ON


#endif /* LWIPOPTS_H */
