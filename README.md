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

* ssh -X -R 52698:localhost:52698 mininet@localhost -p 10022
* rsub filename

## To start POX controller with coding switch

* pox/pox.py log.level --DEBUG misc.coding_switch

## To start mininet

* With standard setup
	* sudo mn --topo single,3 --mac --switch ovsk --controller remote
* With custom topo:
	* sudo mn --custom ~/mininet/custom/butterfly.py --topo butterflytopo --mac --switch ovsk --controller remote

## Scapy packet generator:

* To start: sudo .local/bin/scapy
* Scapy is a packet generating python program.
* To send 10 VLAN ethernet packets from h1 to h2: Do this in xterm
	* sendp(Ether(type=0x8100,src="00:00:00:00:00:01",dst="00:00:00:00:00:02")/"Test",count=10)
	* OR use the sudo python l2_vlan.py 
	* the scapy folder contains all scapy scripts used.

## Netlink

* Worked on using Netlink to interface with the ovs kernel module to implement a userspace packet processor. 
* Looked at Generic netlink in C (libnl), Python (Facebook Netlink module) and Go (go-openvswitch osnl)
* However, there is little documentation (Besides the source code) that details how netlink can be used to write user specific applications by processing the packets from the ovs kernel module. 
* The Go package (go-openvswitch) provides the closest implementation, however it stops after Datapath operations. 
* I modified the package to add support for packet specific commands but receving a "Operation Not Permitted" error.
* I am going to Look at using DPDK with ovs instead, as it is better documented and suited for use in developing userspace applications with ovs, as well as the possibility of adding hardware offloading as DPDK supports some form of this. 
* Also DPDK is well documented!

## DPDK

* Datapath development kit is a packet processing layer that works with ovs.
* This should enable me to add a processing path.
* Added a script to setup ovs with dpdk at start. (Mininet does not have dpdk support at boot): setup_ovsdpdk
* To do: 
	* Make minnet ports type dpdk 
	* Run dpdk sample applications
* Hugepages must be configed before any application can run.
* To make interfaces use DPDK drivers instead of generic:
	* sudo modprobe uio
	* sudo instmod $RTE_SDK/build/kmod/igb_uio.ko
	* sudo $RTE_SDK/usertools/dpdk-devbind.py --bind=igb_uio eth1
	* And to view: ./usertools/dpdk-devbind.py --status
* setup_ovsdpdk now does all device config and binding with dpdk and ovs.
	* To add: 
		* Add ovs flows between ports?
		* They are bridged but no flows exist?
	* Use the --file-prefix EAL if getting .rte_config classes.
* Packets not sending in testpmd because:
	* There is a kernel error where:
		[  109.630804] igb_uio: loading out-of-tree module taints kernel.
		[  109.630835] igb_uio: module verification failed: signature and/or required key missing - tainting kernel
		[  109.631422] igb_uio: Use MSIX interrupt by default
		[  109.938565] igb_uio 0000:00:04.0: mapping 1K dma=0x1b5b5f000 host=ffff907fb5b5f000
		[  109.938568] igb_uio 0000:00:04.0: unmapping 1K dma=0x1b5b5f000 host=ffff907fb5b5f000
		[  109.941223] igb_uio 0000:00:04.0: uio device registered with irq a
		[  110.068260] igb_uio 0000:00:05.0: mapping 1K dma=0x1b491c000 host=ffff907fb491c000
		[  110.068262] igb_uio 0000:00:05.0: unmapping 1K dma=0x1b491c000 host=ffff907fb491c000
		[  112.634455] device br0 left promiscuous mode
		[  112.634486] device ovs-netdev left promiscuous mode
		[  113.178974] igb_uio 0000:00:04.0: uio device registered with irq a
		[  113.482514] igb_uio 0000:00:05.0: uio device registered with irq a
		[  113.798373] device ovs-netdev entered promiscuous mode
		[  113.799513] device br0 entered promiscuous mode
	* Trying to add this to grub kernel: iommu=pt intel_iommu=off
