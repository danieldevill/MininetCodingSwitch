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

## Scapy packet generator:``
z
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
* VFIO driver is for turning a VM into a userspace driver. Cant use VFIO within a vm. 
* uio_pci_generic seems to be returning no errors? Still cant get packets to forward
* Add ifconfig after port creation??
* Testpmd:
	* sudo testpmd –l 0-3 –n 4 -d librte_pmd_virtio.so --file-prefix=daniel -- -i --rxq=1 --txq=1 --rxd=256 --txd=25
* Ethtool:
	* sudo $RTE_SDK/examples/ethtool/ethtool-app/x86_64-native-linuxapp-gcc/ethtool -l 1 -m 100 --file-prefix ethl
* Skeleton (basicfwd):
	* sudo $RTE_SDK/examples/skeleton1/build/basicfwd -l 1 -n 4 -m 100 --file-prefix ske
* l2fwd:
	* sudo $RTE_SDK/examples/l2fwd_daniel/build/l2fwd -l 1,2 -n 4 --file-prefix l2  -- -p 0x3

## PcapPlusPlus

* To monitor traffic on dpdk ports 0 and 1.
* sudo PcapPlusPlus/Examples/DpdkExample-FilterTraffic/Bin/DpdkTrafficFilter -d 0,1
* PcapPlusPlus is picking up packets from l2fwd!

## Git sync:

* sudo scp -P 10022 -r mininet@localhost:/home/mininet/* ~/Dropbox/Academics/MEng/Working/MininetCodingSwitch

## Notes on editing basicfwd.c

* Disabled pronuscuous mode, as all ports were collecting all of the packets. 
* Ive only enabled receiving till I monitor how the program works and do various tests.

## To do: May 16th:

* Force host traffic through one port, capture traffic through specific port on OVS switch side. 
* Do performance tests to confirm ovs-dpdk working together, vs traditional vs ovs, vs dpdk.
* Add networking coding to it.

## Kodo-c

* Ive been installing kodo-c library.
* Im using /kodo-c/shared_kodoc as the shared library location for kodo-c.
* Kodo-c basic example code is running in the basicfwd DPDK application.
* This indicates that a link is established for the shared kodo-c library.
* I had to modify the basicfwd Makefile to do so.
* I can now begin integrating dpdk and kodo-c and doing performance tests, testing different configurations, etc.

## l2fwd:

* Now that I know Kodo can be integrated into DPDK, I want to imporve the work of the exisiting l2fwd application to include network coding. 
* I want to use this as a base and make improvements (optimations if there are any) to better my case.
* BSD 3 license, so improvements are welcome.
* Prof. Engelbrecht says I should build on their implementation. 
* sudo $RTE_SDK/examples/l2fwd_daniel/build/l2fwd -l 1,2,3,4 -n 4 --file-prefix l2  -- -p F
* Ive removed a bunch of code from l2fwd_daniel that is relevent for stats or any error detection, as this is a single use case which I pre configure, so I do not need general use case error handling.
* I am editing the way MAC addresses are updating.
	* Follow the progress of dst_port
	* MAC address updating disabled allows for normal L2 switch operation.
* I am adding ARP function to deal with ARP requests sent by hosts.
* Initial tests are with static arp entries using the "arp -s" command:
	* sudo arp -s 192.168.1.254 de:ad:be:ef:00:02
	* And this works! No more arp requests all over the place.
	* I need to make it automatic in the sense that the switch responds to ARP requests. 