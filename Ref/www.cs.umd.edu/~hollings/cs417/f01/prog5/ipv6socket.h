
int const	IPV6_PROTO_ICMP = 1
int const	IPV6_PROTO_UDP = 2;
int const	IPV6_PROTO_TCP = 3;

/* Init the IPv6 networking  */
int IPv6init(int argc, char **argv);

/* create a socket */
int IPv6socket(int family);

/* send a message */
int IPv6sendto(int sock, const char *msg, int len, unsigned char *to);

/* recv a datagram on a v6 socket */
int IPv6recvfrom(int sock, const char *msg, int len, unsigned char *from);

/* recv a datagram on a v6 socket and timeout if nothing shows up in msec
 *  mili-seconds */
int IPv6recvfromTO(int sock, const char *msg, int len, unsigned char *from,
    int msec);

int IPv6connect(int sock, unsigned char *to, int port);

int IPv6listen(int sock, int port);

int IPv6accept(int sock);

int IPv6send(int sock, char *msg, int len);

int IPv6recv(int sock, char *msg, int len);

int IPv6close(int sock);
