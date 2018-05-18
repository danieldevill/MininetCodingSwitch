/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2010-2015 Intel Corporation. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*Edited by Daniel de Villiers */

//Incudes for Kodo-c library. DD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <kodoc/kodoc.h>

#include <stdint.h>
#include <inttypes.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>

#define RX_RING_SIZE 128
#define TX_RING_SIZE 512

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 32

static const struct rte_eth_conf port_conf_default = {
	.rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN }
};

/* basicfwd.c: Basic DPDK skeleton forwarding example. */

/*
 * Initializes a given port using global settings and with the RX buffers
 * coming from the mbuf_pool passed as a parameter.
 */
static inline int
port_init(uint16_t port, struct rte_mempool *mbuf_pool)
{
	struct rte_eth_conf port_conf = port_conf_default;
	const uint16_t rx_rings = 1, tx_rings = 1;
	uint16_t nb_rxd = RX_RING_SIZE;
	uint16_t nb_txd = TX_RING_SIZE;
	int retval;
	uint16_t q;

	if (port >= rte_eth_dev_count())
		return -1;

	/* Configure the Ethernet device. */
	retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
	if (retval != 0)
		return retval;

	retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);
	if (retval != 0)
		return retval;

	/* Allocate and set up 1 RX queue per Ethernet port. */
	for (q = 0; q < rx_rings; q++) {
		retval = rte_eth_rx_queue_setup(port, q, nb_rxd,
				rte_eth_dev_socket_id(port), NULL, mbuf_pool);
		if (retval < 0)
			return retval;
	}

	/* Allocate and set up 1 TX queue per Ethernet port. */
	for (q = 0; q < tx_rings; q++) {
		retval = rte_eth_tx_queue_setup(port, q, nb_txd,
				rte_eth_dev_socket_id(port), NULL);
		if (retval < 0)
			return retval;
	}

	/* Start the Ethernet port. */
	retval = rte_eth_dev_start(port);
	if (retval < 0)
		return retval;

	/* Display the port MAC address. */
	struct ether_addr addr;
	rte_eth_macaddr_get(port, &addr);
	printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
			   " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
			port,
			addr.addr_bytes[0], addr.addr_bytes[1],
			addr.addr_bytes[2], addr.addr_bytes[3],
			addr.addr_bytes[4], addr.addr_bytes[5]);

	/* Enable RX in promiscuous mode for the Ethernet device. */
	//Disabled by DD	
	//rte_eth_promiscuous_enable(port);

	return 0;
}

/*
 * The lcore main. This is the main thread that does the work, reading from
 * an input port and writing to an output port.
 */
static __attribute__((noreturn)) void
lcore_main(void)
{

	//Kodo-c init:
	//Define num symbols and size.
	//Values are just selected from examples for now.
	uint32_t max_symbols = 1;
	uint32_t max_symbol_size = 10;

	//Select codec
	uint32_t codec = kodoc_full_vector;

	//Finite field to use
	uint32_t finite_field = kodoc_binary;

	//Encoded and decoder factory
	kodoc_factory_t encoder_factory = kodoc_new_encoder_factory(
		codec,finite_field,max_symbols,max_symbol_size);
	kodoc_factory_t decoder_factory = kodoc_new_decoder_factory(
		codec,finite_field,max_symbols,max_symbol_size);

	//Create encoder and decoder
	kodoc_coder_t encoder = kodoc_factory_build_coder(encoder_factory);
	kodoc_coder_t decoder = kodoc_factory_build_coder(decoder_factory);

	//Create data buffers
	uint32_t bytes_used;
	uint32_t payload_size = kodoc_payload_size(encoder);
	uint8_t* payload = (uint8_t*) malloc(payload_size);

	uint32_t block_size = kodoc_block_size(encoder);
	uint8_t* data_in = (uint8_t*) malloc(block_size);
	uint8_t* data_out = (uint8_t*) malloc(block_size);

	const uint16_t nb_ports = rte_eth_dev_count();
	uint16_t port;

	/*
	 * Check that the port is on the same NUMA node as the polling thread
	 * for best performance.
	 */
	printf("Number of ports: %u \n",nb_ports); //DD
	for (port = 0; port < nb_ports; port++)
	{
		printf("%u",port);
		if (rte_eth_dev_socket_id(port) > 0 &&
				rte_eth_dev_socket_id(port) !=
						(int)rte_socket_id())
			printf("WARNING, port %u is on remote NUMA node to "
					"polling thread.\n\tPerformance will "
					"not be optimal.\n", port);
	}

	printf("\nCore %u forwarding packets. [Ctrl+C to quit]\n",
			rte_lcore_id());

	/* Run until the application is quit or killed. */
	for (;;) 
	{
		/*
		 * Receive packets on a port and forward them on the paired
		 * port. The mapping is 0 -> 1, 1 -> 0, 2 -> 3, 3 -> 2, etc.
		 */
		for (port = 0; port < nb_ports; port++) 
		{

			/* Get burst of RX packets, from first port of pair. */
			//Loops, parsing the RX ring of the receive queue, up to nb_pkts.
			struct rte_mbuf *bufs[BURST_SIZE];
			const uint16_t nb_rx = rte_eth_rx_burst(port, 0,
					bufs, BURST_SIZE);

			if (unlikely(nb_rx == 0))
			{
				continue;
			}

			//DD
			//Get dst of recieved packet
			//Exclude broadcast packets (Such as ARP)
			const unsigned char* data = rte_pktmbuf_mtod(bufs[0], void *); //Convert data to char.
			if(data[0]!=255)
			{
				//Dump packets into a file
				FILE *mbuf_file;
				mbuf_file = fopen("mbuf_dump.txt","a");
				fprintf(mbuf_file, "\n ------------------ \n Port:%d ----",port);
				rte_pktmbuf_dump(mbuf_file,bufs[0],1000);
				fprintf(mbuf_file,"------Decoded------\n"); //Decode raw frame.
				fprintf(mbuf_file, "DST MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",data[0],data[1],data[2],data[3],data[4],data[5]);
				fprintf(mbuf_file, "SRC MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",data[6],data[7],data[8],data[9],data[10],data[11]);
				fclose(mbuf_file);

				//DD
				//Create forwarding table and forward data according to received DST mac to a specifc port
				// de ad be ef 00 06 -> port 0
				// de ad be ef 00 07 -> port 1
				// de ad be ef 00 08 -> port 2		
				// de ad be ef 00 09 -> port 3

				//Kodo-c assign data to payload, encode and decode and test.`
				//Fill payload with data.
				uint32_t j;
				for(j=0;j<block_size;j++)
				{
					if(j<sizeof(data))
					{
						data_in[j] = data[j+14]; //Payload data starts at 14th byte position, after src and dst.
					}
					else
					{
						data_in[j] = 0; //Pad with zeros after payload
					}
				}

				//Assign data buffers to encoder and decoder.
				kodoc_set_const_symbols(encoder,data_in,block_size);
				kodoc_set_mutable_symbols(decoder,data_out,block_size);

				//Start encoding and decoding process
				while(!kodoc_is_complete(decoder))
				{
					//Generate encoded payload
					bytes_used = kodoc_write_payload(encoder, payload);
					printf("Encoded payload generated, rank = %d, bytes used = %d\n", 
						kodoc_rank(encoder),bytes_used);

					//Pass payload to decoder
					kodoc_read_payload(decoder,payload);
					printf("Payload decoded, current rank = %d\n",kodoc_rank(decoder));
				}

				//Confirm if data is decoded correctly
				if(memcmp(data_in,data_out,block_size) == 0)
				{
					printf("Data decoded correctly\n");
					uint32_t t;
					for(t=0;t<block_size;t++)
					{
						printf("%c\n",data_out[t]);
					}
				}
				else
				{
					printf("Data not decoded correctly\n");
				}

				//Get the last octet of the dst mac address.
				uint16_t out_port;
				//Array to store forwarding info, value is last mac octet, and index is the port.
				uint16_t dst_macs[] = {6,7,8,9};
				//Send packet to approriate port
				uint16_t i;
				for(i=0;i<sizeof(dst_macs);i++)
				{
					if(dst_macs[i] == data[5]) //Get index of dst addr to be the port.
					{
						out_port = i;
						printf("Out Port: %u\n", out_port);
						
						//Modify packet.
						//data[14] = 'D';

						//Send recieved packet to correct dst port:
						const uint16_t nb_tx = rte_eth_tx_burst(out_port, 0,bufs, nb_rx);

						/* Free any unsent packets. */
						if (unlikely(nb_tx < nb_rx)) {
							uint16_t buf;
							for (buf = nb_tx; buf < nb_rx; buf++)
								rte_pktmbuf_free(bufs[buf]);
						}
						
						printf("%u %u\n", port,i);
						
						//Break all loops
						i = sizeof(dst_macs);
					}
				}
			}
		}
	}

	//Kodo. DD
	//Free all memory buffers
	free(data_in);
	free(data_out);
	free(payload);

	//Delete coders and coder factories
	kodoc_delete_coder(encoder);
	kodoc_delete_coder(decoder);
	kodoc_delete_factory(encoder_factory);
	kodoc_delete_factory(decoder_factory);
}

/*
 * The main function, which does initialization and calls the per-lcore
 * functions.
 */
int
main(int argc, char *argv[])
{
	struct rte_mempool *mbuf_pool;
	unsigned nb_ports;
	uint16_t portid;

	/* Initialize the Environment Abstraction Layer (EAL). */
	int ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");

	argc -= ret;
	argv += ret;

	/* Check that there is an even number of ports to send/receive on. */
	nb_ports = rte_eth_dev_count();
	if (nb_ports < 2 || (nb_ports & 1))
		rte_exit(EXIT_FAILURE, "Error: number of ports must be even\n");

	/* Creates a new mempool in memory to hold the mbufs. */
	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * nb_ports,
		MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

	if (mbuf_pool == NULL)
		rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");

	/* Initialize all ports. */
	for (portid = 0; portid < nb_ports; portid++)
		if (port_init(portid, mbuf_pool) != 0)
			rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu16 "\n",
					portid);

	if (rte_lcore_count() > 1)
		printf("\nWARNING: Too many lcores enabled. Only 1 used.\n");

	/* Call lcore_main on the master core only. */
	lcore_main();

	return 0;
}
