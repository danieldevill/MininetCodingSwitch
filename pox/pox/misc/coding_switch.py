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

		#Add default VLAN flow mod. All VLAN packets with a VLAN ID within a specific range are forwarded to the NC module. At first to userspace NC module (i.e a socket) then to?? Kernel??
		#Sends all packets with a VLAN to the localhost on a specific socket.
		#vlanmsg = of.ofp_flow_mod()
		#vlanmsg.match = of.ofp_match(dl_type = 0x8100) #Match flows with VLAN
		#action_pc = of.ofp_action_tp_port(type=OFPAT_SET_TP_DST,tp_port=666)  #Changes the destination socket to 666, which is Doom, but there should be no doom on here.
		#action_ec = of.ofp_action_dl_addr.set_dst(of.EthAddr("00:00:00:00:00:01"))
		#action_output = of.ofp_action_output(port=of.OFPP_IN_PORT)
		#vlanmsg.actions.append(action_ec)
		#self.connection.send(vlanmsg)

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

			if self.check_vlan(packet): #Send VLAN packets back to the same host socket
				msg.actions.append(of.ofp_action_output(port = of.OFPP_IN_PORT))
				msg.actions.append(of.ofp_action_tp_port(type=of.OFPAT_SET_TP_DST,tp_port=666))
				print("Has VLAN")
			else:
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
		msg.actions.append(of.ofp_action_vlan_vid(vlan_vid = 2135))
		self.connection.send(msg)

	def check_vlan(self,packet): #Checks if the packet contains a VLAN tag. Once Packet contains a VLAN, add flow to switch
		if packet.type == 0x8100:
			return True
		return False

def launch():
	"""
	Starts the component. This is automatically invoked when the application starts.
	"""
	def start_switch(event):
		log.debug("Controlling %s" % (event.connection,))
		CodingController(event.connection)
	core.openflow.addListenerByName("ConnectionUp", start_switch)

