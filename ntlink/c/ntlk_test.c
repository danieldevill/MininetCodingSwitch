/*
*	author: Daniel de Villiers
*
*
*/
#include <stdio.h>
#include <sys/socket.h> 
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <linux/genetlink.h>

static struct nl_sock *sk;

int main(int argc, char *argv[])
{
	sk = nl_socket_alloc(); // Allocate new netlink socket

	genl_connect(sk); // Create the local socket file descriptor and binds socket to NETLINK_GENERIC.

	//res = genl_send_simple(sk,GENL_ID_CTRL,CTRL_CMD_GETFAMILY,CTRL_VERSION,NLM_F_DUMP);
	//unsigned int res = genl_send_simple(sk,0x001d,0x01,0x01,NLM_F_DUMP);

	//printf("%d",res);

	int i;
	for(i=0x0010;i<=0x001f;i+=1)
	{
		int j;
		for(j=0x01;j<=0x04;j+=1)
		{
			unsigned int res = genl_send_simple(sk,i,j,1,NLM_F_DUMP);
			printf("%d",res);
			printf("\n");
		}
		
	}

	nl_socket_free(sk); // Free a netlink socket
}