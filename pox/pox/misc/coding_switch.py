""" @auther: Daniel de Villiers: Stellenbosch University.
	Pox controller implementation that supports network coding rules.
	Progress:
	1. Create a basic learning switch
	2. Modify header to flag network coding
	3. Add custom actions with flow mod
"""

from pox.core import core
import pox.openflow.libopenflow_01 as of

log = core.getLogger()

class CodingController(object):
	def __init__(self,connection):
		#Keeps track of connection to switch
		self.connection = connection
		#Binds PacketIn event listener
		connection.addListeners(self)
		#Ethernet forwarding table dictionary, mac to port.
		self.fwd_table = {}

	def _handle_PacketIn(self,event):
		#Handles packet_in messages
		packet = event.parsed
		if not packet.parsed:
			log.warning("Ignoring incomplete packet")
			return
		#Actual openflow packet_in message
		packet_in = event.ofp
		#start switch
		self.coding_switch(packet,packet_in)

	def coding_switch(self,packet,packet_in):

		#Assign source mac to forwarding table.
		src_mac = packet.src
		dst_mac = packet.dst
		self.fwd_table[src_mac] = packet_in.in_port

		#log.debug("%s \n" % self.fwd_table)

		if dst_mac in self.fwd_table: 

			#Add flow mod
			msg = of.ofp_flow_mod()
			msg.match = of.ofp_match.from_packet(packet) #Contruct exact match based on incoming packet.
			msg.data = packet_in
			msg.actions.append(of.ofp_action_output(port = self.fwd_table[dst_mac]))

			#Normal learning switch (No Flow Mods). Used for comparison
			#msg = of.ofp_packet_out() #Instructs switch to send packet out.
			#msg.data = packet_in
			#msg.actions.append(of.ofp_action_output(port = self.fwd_table[dst_mac]))

		else: #Act like a hub and forward to all output ports. 
			msg = of.ofp_packet_out() #Instructs switch to send packet out.
			msg.data = packet_in
			msg.actions.append(of.ofp_action_output(port = of.OFPP_ALL))

		#Send message to switch
		self.connection.send(msg)

	def check_vlan(self,packet,packet_in): #Checks if the packet contains a VLAN tag. Once Packet contains a VLAN, add flow to switch
		if packet.type == 0x8100:
			print("Packet type %s has VLAN" % (packet.type,) )

def launch():
	"""
	Starts the component. This is automatically invoked when the application starts.
	"""
	def start_switch(event):
		log.debug("ConTROLLing %s" % (event.connection,))
		CodingController(event.connection)
	core.openflow.addListenerByName("ConnectionUp", start_switch)

