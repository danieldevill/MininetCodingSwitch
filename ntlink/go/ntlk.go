package main

import (		
	"fmt"
	"os"
	"./go-openvswitch/ovsnl"
)

func main() {
    fmt.Println("Testing go")

	// Dial a generic netlink connection and create a *ovsnl.Client.
	c, err := ovsnl.New()
	if err != nil {
	    // If OVS generic netlink families aren't available, do nothing.
	    if os.IsNotExist(err) {
	        fmt.Printf("generic netlink OVS families not found: %v", err)
	        return
	    }

		fmt.Printf("failed to create client %v", err)
	}
	// Be sure to close the generic netlink connection!
	defer c.Close()

	//List packets?
	pkts, err := c.Datapath.List()
	if err != nil {
		fmt.Printf("failed to list packets: %v", err)
	}

	for _, p := range pkts {
		fmt.Printf("packets: %q", p.Name)
	}


	fmt.Printf("\n")


	// List available OVS datapaths.
	dps, err := c.Datapath.List()
	if err != nil {
		fmt.Printf("failed to list datapaths: %v", err)
	}

	for _, d := range dps {
		fmt.Printf("datapath: %q, flows: %d", d.Name, d.Stats.Flows)
	}

    fmt.Println("\nStopping go")
}