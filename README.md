# MininetCodingSwitch

# This is a Mininet Implementation of a network coding capable switch

## Notes:

Which Flow Match Fields will work best:
  * VLAN id?
  * Metadata?
  * Aim to use as much existing infrastructure as possible, these will ease with future integration and reduce complexities. 

Notes on running iPerf in Multicast mode:

* route add -host 244.0.67.67 ethX : This assigns the multicast address to the host. 

* Two parts: Client and server.
	* Server: The server needs to bind to the multicast group. The client streams to all the servers.
		-cdm: iperf -s -B 244.0.67.67 -u , this binds the server to the multicast group under UDP.

	* Client: iperf -c 244.0.67.67 -u -t 5 -P 2 --ttl 5, this sends 2 streams to the multicast group.