from pox.core import core
import pox.openflow.discovery  # Enables link discovery (LLDP)
import pox.openflow.spanning_tree  # Enables STP
import pox.forwarding.l2_learning  # Learning switch behavior

log = core.getLogger()

def launch():
    # Start discovery to detect links between switches
    pox.openflow.discovery.launch()

    # Enable spanning tree with default priority (lowest priority wins)
    # You can adjust priorities to control the root switch
    pox.openflow.spanning_tree.launch(
        hold_down=True,  # Prevent STP until topology settles
        #forward_delay=1  # Speeds up topology convergence
    )

    # Launch learning switch behavior
    pox.forwarding.l2_learning.launch()

    log.info("STP-based L2 learning switch with loop prevention launched.")