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


## Already Working:

* OpenFlow switch, adds flow tables and only calls controller when flow not recognised. 

## TO do:

* Use VLAN tag to forward packets to kodo NC program.
* Check for VLAN tags,:
	* Generate Vlan tags with a packet generator, and add flow table rule to check for them.
* NC module must do encoding and remove VLAN tags.

## To connect to Mininet from host with rsub support:

* ssh -X -R 52698:localhost:52698 daniel@localhost -p 10022
* rsub filename

## To start POX controller with coding switch

* ./pox.py log.level --DEBUG misc.coding_switch

## Scapy packet generator:

* To start: sudo .local/bin/scapy
* Scapy is a packet generating python program.
* To send 10 VLAN ethernet packets from h1 to h2: Do this in xterm
	* sendp(Ether(type=0x8100,src="00:00:00:00:00:01",dst="00:00:00:00:00:02")/"Test",count=10)