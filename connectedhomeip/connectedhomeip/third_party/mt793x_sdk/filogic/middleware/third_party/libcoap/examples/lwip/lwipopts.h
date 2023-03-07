#define NO_SYS                          1

/* they'd require NO_SYS=0, but are enabled by default */
#define LWIP_SOCKET                     0
#define LWIP_NETCONN                    0

#define MEMP_USE_CUSTOM_POOLS 1


/* +1 for a missing timeout in the default list i could not yet track down */
#define MEMP_NUM_SYS_TIMEOUT            (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2*LWIP_DHCP) + LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + PPP_SUPPORT + 1)
