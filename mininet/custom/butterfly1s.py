""""
	Custom topology

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
from mininet.topo import Topo

class ButterflyTopo(Topo):

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

topos = { 'butterflytopo': ( lambda: ButterflyTopo() ) }