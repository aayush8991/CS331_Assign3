from mininet.topo import Topo
from mininet.net import Mininet
from mininet.cli import CLI
from mininet.log import setLogLevel
from mininet.link import TCLink
from mininet.node import RemoteController

class MyTopo(Topo):
    def __init__(self):
        Topo.__init__(self)

        # Hosts
        h1 = self.addHost('h1', ip='10.1.1.2/24', defaultRoute='via 10.1.1.1')
        h2 = self.addHost('h2', ip='10.1.1.3/24', defaultRoute='via 10.1.1.1')
        h3 = self.addHost('h3', ip='10.0.0.4/24', defaultRoute='via 10.0.0.1')  # Added default route
        h4 = self.addHost('h4', ip='10.0.0.5/24', defaultRoute='via 10.0.0.1')
        h5 = self.addHost('h5', ip='10.0.0.6/24', defaultRoute='via 10.0.0.1')
        h6 = self.addHost('h6', ip='10.0.0.7/24', defaultRoute='via 10.0.0.1')
        h7 = self.addHost('h7', ip='10.0.0.8/24', defaultRoute='via 10.0.0.1')
        h8 = self.addHost('h8', ip='10.0.0.9/24', defaultRoute='via 10.0.0.1')
        h9 = self.addHost('h9', ip=None)  # NAT gateway

        # Switches
        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2')
        s3 = self.addSwitch('s3')
        s4 = self.addSwitch('s4')

        # Links for h9
        self.addLink(h1, h9, intfName2='h9-eth1', params2={'ip': '10.1.1.1/24'})
        self.addLink(h2, h9, intfName2='h9-eth2', params2={'ip': '10.1.1.1/24'})
        self.addLink(h9, s1, intfName1='h9-eth0', params1={'ip': '172.16.10.10/24'})
        # Add a new interface on h9 for the 10.0.0.0/24 subnet
        self.addLink(h9, s1, intfName1='h9-eth3', params1={'ip': '10.0.0.1/24'})

        # Remaining links
        self.addLink(s1, s2, delay='7ms')
        self.addLink(h3, s2, delay='5ms')
        self.addLink(h4, s2, delay='5ms')
        self.addLink(s2, s3, delay='7ms')
        self.addLink(h5, s3, delay='5ms')
        self.addLink(h6, s3, delay='5ms')
        self.addLink(s3, s4, delay='7ms')
        self.addLink(h7, s4, delay='5ms')
        self.addLink(h8, s4, delay='5ms')
        self.addLink(s4, s1, delay='7ms')
        self.addLink(s1, s3, delay='7ms')

def run():
    topo = MyTopo()
    c0 = RemoteController('c0', ip='127.0.0.1', port=6633)
    net = Mininet(topo=topo, link=TCLink, controller=c0)
    net.start()

    # Configure h9 as NAT gateway
    h9 = net.get('h9')
    h9.cmd("sysctl -w net.ipv4.ip_forward=1")

    # Add routes for external hosts (h3-h8) to reach 10.1.1.0/24 via h9's 172.10.10.16 interface
    for host in ['h3', 'h4', 'h5', 'h6', 'h7', 'h8']:
        net.get(host).cmd("ip route add 10.1.1.0/24 via 172.10.10.16")

    # SNAT (Outbound: Translate private IPs to h9's public IP)
    h9.cmd("iptables -t nat -A POSTROUTING -s 10.1.1.0/24 -o h9-eth0 -j MASQUERADE")

    # DNAT (Inbound: Forward traffic to h9's public IP to h1/h2)
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -d 172.16.10.10 -j DNAT --to-destination 10.1.1.2")
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -d 172.16.10.10 -j DNAT --to-destination 10.1.1.3")

    # Allow forwarding between all interfaces
    h9.cmd("iptables -A FORWARD -i h9-eth0 -o h9-eth1 -j ACCEPT")
    h9.cmd("iptables -A FORWARD -i h9-eth1 -o h9-eth0 -j ACCEPT")
    h9.cmd("iptables -A FORWARD -i h9-eth3 -o h9-eth0 -j ACCEPT")
    h9.cmd("iptables -A FORWARD -i h9-eth0 -o h9-eth3 -j ACCEPT")

    CLI(net)
    net.stop()

if __name__ == '__main__':
    setLogLevel('info')
    run()