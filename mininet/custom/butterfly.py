""""
	Custom topology

	Butterfly network:

	h1                   h2
	|                    |
	v                    v
	|                    |
	s1--->----s3-----<---s2
	|          |         |
	|          |         |
	v          v         v 
	|          |         |
	s5---<--- s4---->----s6 
	|                    |
	v                    v
	|                    |
	h3                   h4
"""
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
from mininet.cli import CLI

class ButterflyTopo(Topo):

	def __init__( self ):
		
		Topo.__init__( self )

		#Add host and switches
		senderHost1 = self.addHost('h1')
		senderHost2 = self.addHost('h2')
		receiverHost3 = self.addHost('h3')
		receiverHost4 = self.addHost('h4')      
		top_leftSwitch = self.addSwitch('s1')
		top_middleSwitch = self.addSwitch('s3')
		top_rightSwitch = self.addSwitch('s2')
		bottem_leftSwitch = self.addSwitch('s5')
		bottem_middleSwitch = self.addSwitch('s4')
		bottem_rightSwitch = self.addSwitch('s6')

		# Add links
		self.addLink(senderHost1,top_leftSwitch)
		self.addLink(senderHost2,top_rightSwitch)
		self.addLink(top_leftSwitch,top_middleSwitch)
		self.addLink(top_middleSwitch,top_rightSwitch)
		self.addLink(top_leftSwitch,bottem_leftSwitch)
		self.addLink(top_middleSwitch,bottem_middleSwitch)
		self.addLink(top_rightSwitch,bottem_rightSwitch)
		self.addLink(bottem_leftSwitch,bottem_middleSwitch)
		self.addLink(bottem_middleSwitch,bottem_rightSwitch)
		self.addLink(bottem_leftSwitch,receiverHost3)
		self.addLink(bottem_rightSwitch,receiverHost4)

""""
	Butterfly network: (With one switch)

	h1                   h2
	|                    |
	v                    v
	----------------------
	           |         
	           |     
	           s1       
	           | 
	           |       
    ----------------------
	|                    |
	v                    v
	|                    |
	h3                   h4
"""

class ButterflyTopo_s1(Topo):

	def __init__( self ):
		
		Topo.__init__( self )

		#Add host and switches
		senderHost1 = self.addHost('h1')
		senderHost2 = self.addHost('h2')
		receiverHost3 = self.addHost('h3')
		receiverHost4 = self.addHost('h4')      
		codingsswitch = self.addSwitch('s1')


		# Add links
		self.addLink(senderHost1,codingsswitch)
		self.addLink(senderHost2,codingsswitch)
		self.addLink(codingsswitch,receiverHost3)
		self.addLink(codingsswitch,receiverHost4)

def multicastTest(net):
    h1 = net.get('h1')
    h2 = net.get('h2')	
    h3 = net.get('h3')	
    h4 = net.get('h4')

    h1.cmd('route add -host 244.0.67.67 h1-eth0')
    #h2.cmd('route add -host 244.0.67.68 h2-eth0')
    h3.cmd('route add -host 244.0.67.67 h3-eth0')
    h4.cmd('route add -host 244.0.67.67 h4-eth0')
    #h3.cmd('route add -host 244.0.67.68 h3-eth0')
    #h4.cmd('route add -host 244.0.67.68 h4-eth0')

    #Simulatnously send multicast h1 to h3,h4 and h2 to h3,h4 via two multicast groups?

    popens = {}

    #popens[h1] = h1.popen('iperf -c 244.0.67.67 -u -t 5 -P 2') #Multicast for 5 seconds, with 2 parallel connections to server
    #popens[h3] = h3.popen('iperf -s -u -B 224.0.67.67 -i 1') #Bind to multicast address
    #popens[h4] = h4.popen('iperf -s -u -B 224.0.67.67 -i 1') #Bind to multicast address

    #h2.cmdPrint('iperf -c 244.0.67.68 -u -t 5 -P 2') #Multicast for 5 seconds, with 2 parallel connections to server
    #h3.cmdPrint('iperf -s -u -B 224.0.67.68 -i 1') #Bind to multicast address
    #h4.cmdPrint('iperf -s -u -B 224.0.67.68 -i 1') #Bind to multicast address

    #Other
    # h1.cmdPrint('iperf -c 244.0.67.67 -u -t 5 -P 2') #Multicast for 5 seconds, with 2 parallel connections to server
    # h3.cmdPrint('iperf -s -u -B 224.0.67.67 -i 1') #Bind to multicast address
    # h4.cmdPrint('iperf -s -u -B 224.0.67.67 -i 1') #Bind to multicast address

    # h2.cmdPrint('iperf -c 244.0.67.68 -u -t 5 -P 2') #Multicast for 5 seconds, with 2 parallel connections to server
    # h3.cmdPrint('iperf -s -u -B 224.0.67.68 -i 1') #Bind to multicast address
    # h4.cmdPrint('iperf -s -u -B 224.0.67.68 -i 1') #Bind to multicast address

    #Start MN CLI.
    CLI(net)

topos = { 'butterflytopo': ( lambda: ButterflyTopo() ), 'butterflytopo_s1': ( lambda: ButterflyTopo_s1() ) }
tests = { 'multicasttest' : multicastTest}