import scapy.all as scapy
import sys

#Send 10 VLAN paclkets. With data = "Test"

eth_src = "00:00:00:00:00:01" #Host 1
eth_dst = "00:00:00:00:00:02" #Host 2
eth_type = 0x8100 #VLAN
data = "Test" #Data to send
total_packets = 10 #Number of packets to send

l2packet = scapy.Ether(type=eth_type,src=eth_src,dst=eth_dst)/data #Creates a L2 ethernet packet, and adds data to the header.

scapy.sendp(l2packet,count=total_packets)