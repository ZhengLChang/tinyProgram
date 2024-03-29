/*
 * ntpclient.c - NTP client
 *
 * Copyright 1997, 1999, 2000, 2003  Larry Doolittle  <larry@doolittle.boa.org>
 * Last hack: July 5, 2003
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (Version 2,
 *  June 1991) as published by the Free Software Foundation.  At the
 *  time of writing, that license was published by the FSF with the URL
 *  http://www.gnu.org/copyleft/gpl.html, and is incorporated herein by
 *  reference.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  Possible future improvements:
 *      - Double check that the originate timestamp in the received packet
 *        corresponds to what we sent.
 *      - Verify that the return packet came from the host we think
 *        we're talking to.  Not necessarily useful since UDP packets
 *        are so easy to forge.
 *      - Write more documentation  :-(
 *
 *  Compile with -D_PRECISION_SIOCGSTAMP if your machine really has it.
 *  There are patches floating around to add this to Linux, but
 *  usually you only get an answer to the nearest jiffy.
 *  Hint for Linux hacker wannabes: look at the usage of get_fast_time()
 *  in net/core/dev.c, and its definition in kernel/time.c .
 *
 *  If the compile gives you any flak, check below in the section
 *  labelled "XXXX fixme - non-automatic build configuration".
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>     /* gethostbyname */
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#ifdef _PRECISION_SIOCGSTAMP
#include <sys/ioctl.h>
#endif

#define ENABLE_DEBUG

extern char *optarg;

/* XXXX fixme - non-automatic build configuration */
#ifdef linux
#include <sys/utsname.h>
#include <sys/time.h>
typedef u_int32_t __u32;
#include <sys/timex.h>
#else
#define main ntpclient
extern struct hostent *gethostbyname(const char *name);
extern int h_errno;
#define herror(hostname) \
	fprintf(stderr,"Error %d looking up hostname %s\n", h_errno,hostname)
typedef uint32_t __u32;
#endif

#define JAN_1970        0x83aa7e80      /* 2208988800 1970 - 1900 in seconds */
#define NTP_PORT (123)

#define NET_MONITOR_PORT         30006
#define NET_MSG_NTP_SYNC_OK      0x000010
#define NET_MSG_NTP_SYNC_NOK     0x000011

/* How to multiply by 4294.967296 quickly (and not quite exactly)
 * without using floating point or greater than 32-bit integers.
 * If you want to fix the last 12 microseconds of error, add in
 * (2911*(x))>>28)
 */
#define NTPFRAC(x) ( 4294*(x) + ( (1981*(x))>>11 ) )

/* The reverse of the above, needed if we want to set our microsecond
 * clock (via settimeofday) based on the incoming time in NTP format.
 * Basically exact.
 */
#define USEC(x) ( ( (x) >> 12 ) - 759 * ( ( ( (x) >> 10 ) + 32768 ) >> 16 ) )

/* Converts NTP delay and dispersion, apparently in seconds scaled
 * by 65536, to microseconds.  RFC1305 states this time is in seconds,
 * doesn't mention the scaling.
 * Should somehow be the same as 1000000 * x / 65536
 */
#define sec2u(x) ( (x) * 15.2587890625 )

struct ntptime {
	unsigned int coarse;
	unsigned int fine;
};

typedef struct msgHeader
{
	u_int32_t type;
	u_int32_t datalength;
}msgHeader;

/* prototype for function defined in phaselock.c */
int contemplate_data(unsigned int absolute, double skew, double errorbar, int freq);

/* prototypes for some local routines */
void send_packet(int usd);
int rfc1305print(uint32_t *data, struct ntptime *arrival);
void udp_handle(int usd, char *data, int data_len, struct sockaddr *sa_source, int sa_len);

/* variables with file scope
 * (I know, bad form, but this is a short program) */
static uint32_t incoming_word[325];
#define incoming ((char *) incoming_word)
#define sizeof_incoming (sizeof(incoming_word)*sizeof(uint32_t))
static struct timeval time_of_send;
static int live=0;
static int set_clock=0;   /* non-zero presumably needs root privs */
static int time_offset=0;   
int ai_fam_templ=AF_UNSPEC;

/* when present, debug is a true global, shared with phaselock.c */
#ifdef ENABLE_DEBUG
int debug=0;
#define DEBUG_OPTION "d"
#else
#define debug 0
#define DEBUG_OPTION
#endif

typedef union {
        struct sockaddr         sa;
        struct sockaddr_in      sa4;
        struct sockaddr_in6     sa6;
} sockaddr_u;

#define AF(psau)                ((psau)->sa.sa_family)

#define IS_IPV4(psau)   (AF_INET == AF(psau))
#define IS_IPV6(psau)   (AF_INET6 == AF(psau))

#define SOCKLEN(psau)                                           \
        (IS_IPV4(psau)                                          \
            ? sizeof((psau)->sa4)                               \
            : sizeof((psau)->sa6))


static int readIp(const char* ip)
{
   int n = 0;
   int res = 0;
   
   while (n < 4 && *ip)
   {
      if (isdigit(*ip)) 
      {
         res = (res << 8) | atoi(ip);
         n++;
         while (isdigit(*ip)) 
         {
            ip++;
         }
      } 
      else 
      {
         ip++;
		}
   }
   return res;
}

static int udpSend(short port, void *data, int len)
{
  int sockfd;
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;

  /* fill in server address */
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(readIp("127.0.0.1"));
  serv_addr.sin_port        = htons(port);

  /* open udp socket */
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    return -1; /* could not open socket */
  }

  /* bind any local address for us */ 
  memset(&cli_addr, 0, sizeof(cli_addr));
  cli_addr.sin_family      = AF_INET;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  cli_addr.sin_port        = htons(0);

  if (bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
    return -2; /* could not bind client socket */
  }
 
  /* send the data */
  if (sendto(sockfd, data, len, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != len) {
    return -3; /* could not sendto */
  }
  close(sockfd);
  return 0;
}


int sendNtpClientMonitorMsg(int code)
{
    char buf[sizeof(msgHeader)] = {0};
    msgHeader *msg = (msgHeader*)buf;

    msg->type = code;
    return udpSend(NET_MONITOR_PORT, (void*)msg, sizeof(msgHeader));
		
}

int get_current_freq(void)
{
	/* OS dependent routine to get the current value of clock frequency.
	 */
#ifdef linux
	struct timex txc;
	txc.modes=0;
	if (adjtimex(&txc) < 0) {
		perror("adjtimex"); exit(1);
	}
	return txc.freq;
#else
	return 0;
#endif
}

int set_freq(int new_freq)
{
	/* OS dependent routine to set a new value of clock frequency.
	 */
#ifdef linux
	struct timex txc;
	txc.modes = ADJ_FREQUENCY;
	txc.freq = new_freq;
	if (adjtimex(&txc) < 0) {
		perror("adjtimex"); exit(1);
	}
	return txc.freq;
#else
	return 0;
#endif
}

void send_packet(int usd)
{
	__u32 data[12];
	struct timeval now;
#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 4 
#define PREC -6

	if (debug) fprintf(stderr,"Sending ...\n");
	if (sizeof(data) != 48) {
		fprintf(stderr,"size error\n");
		return;
	}
	bzero((char *) data,sizeof(data));
	data[0] = htonl (
		( LI << 30 ) | ( VN << 27 ) | ( MODE << 24 ) |
		( STRATUM << 16) | ( POLL << 8 ) | ( PREC & 0xff ) );
	data[1] = htonl(1<<16);  /* Root Delay (seconds) */
	data[2] = htonl(1<<16);  /* Root Dispersion (seconds) */
	gettimeofday(&now,NULL);
	data[10] = htonl(now.tv_sec + JAN_1970); /* Transmit Timestamp coarse */
	data[11] = htonl(NTPFRAC(now.tv_usec));  /* Transmit Timestamp fine   */
	send(usd,data,48,0);
	time_of_send=now;
}

void get_packet_timestamp(int usd, struct ntptime *udp_arrival_ntp)
{
	struct timeval udp_arrival;
#ifdef _PRECISION_SIOCGSTAMP
	if ( ioctl(usd, SIOCGSTAMP, &udp_arrival) < 0 ) {
		perror("ioctl-SIOCGSTAMP");
		gettimeofday(&udp_arrival,NULL);
	}
#else
	gettimeofday(&udp_arrival,NULL);
#endif
	udp_arrival_ntp->coarse = udp_arrival.tv_sec + JAN_1970;
	udp_arrival_ntp->fine   = NTPFRAC(udp_arrival.tv_usec);
}

void check_source(int data_len, struct sockaddr *sa_source, int sa_len)
{
	/* This is where one could check that the source is the server we expect */
	char addr[INET6_ADDRSTRLEN] = {0};
	
	if (debug) {
		struct sockaddr_in *sa_in=(struct sockaddr_in *)sa_source;
		printf("packet of length %d received\n",data_len);
		if (sa_source->sa_family==AF_INET) {
			printf("Source: INET Port %d host %s\n",
				ntohs(sa_in->sin_port),inet_ntoa(sa_in->sin_addr));
		} else {
			struct sockaddr_in6 *sa_in6 = (struct sockaddr_in6*)sa_source;
			printf("Source: Address family %d\n",sa_source->sa_family);
			printf("Source: INET Port %d host %s\n",
				ntohs(sa_in6->sin6_port),inet_ntop(AF_INET6,&sa_in6->sin6_addr,addr,INET6_ADDRSTRLEN));
		}
	}
}

double ntpdiff( struct ntptime *start, struct ntptime *stop)
{
	int a;
	unsigned int b;
	a = stop->coarse - start->coarse;
	if (stop->fine >= start->fine) {
		b = stop->fine - start->fine;
	} else {
		b = start->fine - stop->fine;
		b = ~b;
		a -= 1;
	}
	
	return a*1.e6 + b * (1.e6/4294967296.0);
}

/* Does more than print, so this name is bogus.
 * It also makes time adjustments, both sudden (-s)
 * and phase-locking (-l).  */
/* return value is number of microseconds uncertainty in answer */
int rfc1305print(uint32_t *data, struct ntptime *arrival)
{
/* straight out of RFC-1305 Appendix A */
	int li, vn, mode, stratum, poll, prec;
	int delay, disp, refid;
	struct ntptime reftime, orgtime, rectime, xmttime;
	double el_time,st_time,skew1,skew2;
	int freq;

#define Data(i) ntohl(((uint32_t *)data)[i])
	li      = Data(0) >> 30 & 0x03;
	vn      = Data(0) >> 27 & 0x07;
	mode    = Data(0) >> 24 & 0x07;
	stratum = Data(0) >> 16 & 0xff;
	poll    = Data(0) >>  8 & 0xff;
	prec    = Data(0)       & 0xff;
	if (prec & 0x80) prec|=0xffffff00;
	delay   = Data(1);
	disp    = Data(2);
	refid   = Data(3);
	reftime.coarse = Data(4);
	reftime.fine   = Data(5);
	orgtime.coarse = Data(6);
	orgtime.fine   = Data(7);
	rectime.coarse = Data(8);
	rectime.fine   = Data(9);
	xmttime.coarse = Data(10);
	xmttime.fine   = Data(11);
#undef Data

	if (set_clock) {   /* you'd better be root, or ntpclient will crash! */
		struct timeval tv_set;
		/* it would be even better to subtract half the slop */
		tv_set.tv_sec  = xmttime.coarse - JAN_1970;
		/* offset time */
		tv_set.tv_sec  += time_offset*60;
		
		/* divide xmttime.fine by 4294.967296 */
		tv_set.tv_usec = USEC(xmttime.fine);
		if (settimeofday(&tv_set,NULL)<0) {
			sendNtpClientMonitorMsg(NET_MSG_NTP_SYNC_NOK);
			perror("settimeofday");
			exit(1);
		}
		else
		{
    		sendNtpClientMonitorMsg(NET_MSG_NTP_SYNC_OK);
			system("touch /usr/local/data/tmp/ntpsync");
		}
		if (debug) {
			printf("set time to %lu.%.6lu\n", tv_set.tv_sec, tv_set.tv_usec);
		}
	}

	if (debug) {
	printf("LI=%d  VN=%d  Mode=%d  Stratum=%d  Poll=%d  Precision=%d\n",
		li, vn, mode, stratum, poll, prec);
	printf("Delay=%.1f  Dispersion=%.1f  Refid=%u.%u.%u.%u\n",
		sec2u(delay),sec2u(disp),
		refid>>24&0xff, refid>>16&0xff, refid>>8&0xff, refid&0xff);
	printf("Reference %u.%.10u\n", reftime.coarse, reftime.fine);
	printf("Originate %u.%.10u\n", orgtime.coarse, orgtime.fine);
	printf("Receive   %u.%.10u\n", rectime.coarse, rectime.fine);
	printf("Transmit  %u.%.10u\n", xmttime.coarse, xmttime.fine);
	printf("Our recv  %u.%.10u\n", arrival->coarse, arrival->fine);
	}
	el_time=ntpdiff(&orgtime,arrival);   /* elapsed */
	st_time=ntpdiff(&rectime,&xmttime);  /* stall */
	skew1=ntpdiff(&orgtime,&rectime);
	skew2=ntpdiff(&xmttime,arrival);
	freq=get_current_freq();
	if (debug) {
	printf("Total elapsed: %9.2f\n"
	       "Server stall:  %9.2f\n"
	       "Slop:          %9.2f\n",
		el_time, st_time, el_time-st_time);
	printf("Skew:          %9.2f\n"
	       "Frequency:     %9d\n"
	       " day   second     elapsed    stall     skew  dispersion  freq\n",
		(skew1-skew2)/2, freq);
	}
	/* Not the ideal order for printing, but we want to be sure
	 * to do all the time-sensitive thinking (and time setting)
	 * before we start the output, especially fflush() (which
	 * could be slow).  Of course, if debug is turned on, speed
	 * has gone down the drain anyway. */
	if (live) {
		int new_freq;
		new_freq = contemplate_data(arrival->coarse, (skew1-skew2)/2,
			el_time+sec2u(disp), freq);
		if (!debug && new_freq != freq) set_freq(new_freq);
	}
	printf("%d %.5d.%.3d  %8.1f %8.1f  %8.1f %8.1f %9d\n",
		arrival->coarse/86400, arrival->coarse%86400,
		arrival->fine/4294967, el_time, st_time,
		(skew1-skew2)/2, sec2u(disp), freq);
	fflush(stdout);
	return(el_time-st_time);
}

void stuff_net_addr(struct in_addr *p, char *hostname)
{
	struct hostent *ntpserver;
	ntpserver=gethostbyname(hostname);
TRYAGAIN:
	while (ntpserver == NULL){
		sendNtpClientMonitorMsg(NET_MSG_NTP_SYNC_NOK);	
		//herror(hostname);
		sleep(3);
		ntpserver=gethostbyname(hostname);
	}	
	if (ntpserver->h_length != 4) {
		fprintf(stderr,"oops %d\n",ntpserver->h_length);
		goto TRYAGAIN;
	}
        else {
	memcpy(&(p->s_addr),ntpserver->h_addr_list[0],4);
	}
}

void setup_receive(int usd, unsigned int interface, short port)
{
    int rc = 0;
	sockaddr_u sa_rcvr;
	struct addrinfo hints;
	struct addrinfo *res;
	char str_port[64] = {0};
	bzero((char *) &sa_rcvr, sizeof(sa_rcvr));
	bzero((char *) &hints, sizeof(hints));
	hints.ai_family = ai_fam_templ;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_DGRAM;
	sprintf(str_port, "%d", port);
	if (getaddrinfo(NULL, str_port, &hints, &res) != 0){
		fprintf(stderr, "%s:%d getaddrinfo() failed\n", __FUNCTION__, __LINE__);
		exit(1);
	}
	for (; res; res = res->ai_next) {
		memcpy(&sa_rcvr, res->ai_addr, res->ai_addrlen);
		rc = bind(usd, &sa_rcvr.sa, SOCKLEN(&sa_rcvr));
		if (rc < 0) {
		    fprintf(stderr, "bind() fails\n");
		    exit(1);
		}
	}
//	listen(usd,3); not tcp
}

char *print_string_addr(sockaddr_u sa_rcvr, char *str_buf)
{
	
	if (AF(&sa_rcvr) == AF_INET)
	{
		inet_ntop(AF_INET, &(sa_rcvr.sa4.sin_addr), str_buf, 64);
	}
	else if (AF(&sa_rcvr) == AF_INET6)
	{
		inet_ntop(AF_INET6, &(sa_rcvr.sa6.sin6_addr), str_buf, 64);
	}
	return str_buf;
}

void setup_transmit(int usd, char *host, short port)
{
	int rc = -1;
	sockaddr_u sa_rcvr;
	struct addrinfo hints;
	struct addrinfo *res;
	char str_port[64] = {0};
	
    bzero((char *)&hints, sizeof(hints));
	hints.ai_family = ai_fam_templ;
	hints.ai_socktype = SOCK_DGRAM;
	
	sprintf(str_port, "%d", port);
	bzero((char *) &sa_rcvr, sizeof(sa_rcvr));
	if (getaddrinfo(host, "ntp", &hints, &res) != 0){
		fprintf(stderr, "%s:%d getaddrinfo() failed\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	for (; res; res = res->ai_next) {
		memcpy(&sa_rcvr, res->ai_addr, res->ai_addrlen);
		rc = connect(usd, &sa_rcvr.sa, SOCKLEN(&sa_rcvr));
		if (rc < 0) {
		    fprintf(stderr, "connect() fails\n");
		}
		else
		{
			char str_buf[64] = {0};
			fprintf(stderr, "connect() %s ok\n", print_string_addr(sa_rcvr, str_buf));
			break;
		}
	}
}

void primary_loop(int usd, int num_probes, int interval, int goodness)
{
	fd_set fds;
	struct sockaddr sa_xmit;
	int i, pack_len, probes_sent, error;
	socklen_t sa_xmit_len;
	struct timeval to;
	struct ntptime udp_arrival_ntp;

	if (debug) printf("Listening...\n");

	probes_sent=0;
	sa_xmit_len=sizeof(sa_xmit);
	to.tv_sec=0;
	to.tv_usec=0;
	for (;;) {
		FD_ZERO(&fds);
		FD_SET(usd,&fds);
		i=select(usd+1,&fds,NULL,NULL,&to);  /* Wait on read or error */
		if ((i!=1)||(!FD_ISSET(usd,&fds))) {
			if (i==EINTR) continue;
			if (i<0) perror("select");
			if ((to.tv_sec == 0)|| (to.tv_sec == interval)) {
				if (probes_sent >= num_probes &&
					num_probes != 0) break;
				send_packet(usd);
				++probes_sent;
				to.tv_sec=interval;
				to.tv_usec=0;
			}	
			continue;
		}
		pack_len=recvfrom(usd,incoming,sizeof_incoming/4,0,
		                  &sa_xmit,&sa_xmit_len);
		error = goodness+1;
		if (pack_len<0) {
			perror("recvfrom");
		} else if (pack_len>0 && (unsigned)pack_len<sizeof_incoming){
			get_packet_timestamp(usd, &udp_arrival_ntp);
			check_source(pack_len, &sa_xmit, sa_xmit_len);
			error = rfc1305print(incoming_word, &udp_arrival_ntp);
			/* udp_handle(usd,incoming,pack_len,&sa_xmit,sa_xmit_len); */
		} else {
			printf("Ooops.  pack_len=%d\n",pack_len);
			fflush(stdout);
		}
		if ( error < goodness && goodness != 0) break;
		if (probes_sent >= num_probes && num_probes != 0) break;
	}
}

void do_replay(void)
{
	char line[100];
	int n, day, freq, absolute;
	float sec, el_time, st_time, disp;
	double skew, errorbar;
	int simulated_freq = 0;
	unsigned int last_fake_time = 0;
	double fake_delta_time = 0.0;

	while (fgets(line,sizeof(line),stdin)) {
		n=sscanf(line,"%d %f %f %f %lf %f %d",
			&day, &sec, &el_time, &st_time, &skew, &disp, &freq);
		if (n==7) {
			fputs(line,stdout);
			absolute=day*86400+(int)sec;
			errorbar=el_time+disp;
			if (debug) printf("contemplate %u %.1f %.1f %d\n",
				absolute,skew,errorbar,freq);
			if (last_fake_time==0) simulated_freq=freq;
			fake_delta_time += (absolute-last_fake_time)*((double)(freq-simulated_freq))/65536;
			if (debug) printf("fake %f %d \n", fake_delta_time, simulated_freq);
			skew += fake_delta_time;
			freq = simulated_freq;
			last_fake_time=absolute;
			simulated_freq = contemplate_data(absolute, skew, errorbar, freq);
		} else {
			fprintf(stderr,"Replay input error\n");
			exit(2);
		}
	}
}

void usage(char *argv0)
{
	fprintf(stderr,
	"Usage: %s [-4] [ -6] [-c count] [-d] [-g goodness] -h hostname [-i interval]\n"
	"\t[-l] [-p port] [-r] [-s] [-o timeoffset]\n",
	argv0);
}


int main(int argc, char *argv[]) {
	int usd;  /* socket */
	int c;
	/* These parameters are settable from the command line
	   the initializations here provide default behavior */
	short int udp_local_port=0;   /* default of 0 means kernel chooses */
	int cycle_time=3600;          /* seconds */
	int probe_count=0;            /* default of 0 means loop forever */
	/* int debug=0; is a global above */
	int goodness=0;
	char *hostname=NULL;          /* must be set */
	int replay=0;                 /* replay mode overrides everything */

	for (;;) {
		c = getopt( argc, argv, "c:" DEBUG_OPTION "46g:h:i:lp:rso:");
		if (c == EOF) break;
		switch (c) {
			case '4':
				ai_fam_templ = AF_INET;
				break;
			case '6':
				ai_fam_templ = AF_INET6;
				break;
			case 'c':
				probe_count = atoi(optarg);
				break;
#ifdef ENABLE_DEBUG
			case 'd':
				++debug;
				break;
#endif
			case 'g':
				goodness = atoi(optarg);
				break;
			case 'h':
				hostname = optarg;
				break;
			case 'i':
				cycle_time = atoi(optarg);
				break;
			case 'l':
				live++;
				break;
			case 'p':
				udp_local_port = atoi(optarg);
				break;
			case 'r':
				replay++;
				break;
			case 's':
				set_clock++;
				probe_count = 1;
				break;
			case 'o':
				time_offset = atoi(optarg);
				break;
			default:
				usage(argv[0]);
				exit(1);
		}
	}

#ifdef vxworks
	hostname = argv[0];
	set_clock++;
#endif

	if (replay) {
		do_replay();
		exit(0);
	}
	if (hostname == NULL) {
		usage(argv[0]);
		exit(1);
	}
	if (debug) {
		printf("Configuration:\n"
		"  -c probe_count %d\n"
		"  -d (debug)     %d\n"
		"  -g goodness    %d\n"
		"  -h hostname    %s\n"
		"  -i interval    %d\n"
		"  -l live        %d\n"
		"  -p local_port  %d\n"
		"  -s set_clock   %d\n",
		"  -o time_offset   %d\n",
		probe_count, debug, goodness, hostname, cycle_time,
		live, udp_local_port, set_clock, time_offset );
	}

	/* Startup sequence */
	if ((usd=socket(ai_fam_templ,SOCK_DGRAM,IPPROTO_UDP))==-1)
		{perror ("socket");exit(1);}

	setup_receive(usd, INADDR_ANY, udp_local_port);

	setup_transmit(usd, hostname, NTP_PORT);

	primary_loop(usd, probe_count, cycle_time, goodness);

	close(usd);
	return 0;
}
