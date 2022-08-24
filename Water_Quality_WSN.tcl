set val(chan)   Channel/WirelessChannel
set val(prop)   Propagation/TwoRayGround
set val(ant)    Antenna/OmniAntenna
set val(ll)     LL
set val(ifq)    Queue/DropTail/PriQueue
set val(ifqlen) 50
set val(netif)  Phy/WirelessPhy
set val(mac)    Mac/802_11
set val(rp)     EAMRP
set val(nn)     50
set val(x)      700
set val(y)      700
set val(stop)   6.5
set val(traffic)        cbr
set val(traffic)        tcp

Mac/802_15_4 wpanCmd verbose on
Mac/802_15_4 wpanNam namStatus on

set ns [new Simulator]
set tracefd       [open Water_Quality_WSN.tr w]
set windowVsTime2 [open win.tr w]
set namtrace      [open Water_Quality_WSN.nam w]

$ns trace-all $tracefd
$ns namtrace-all-wireless $namtrace $val(x) $val(y)
set topo       [new Topography]
$topo load_flatgrid $val(x) $val(y)
create-god (1)
source len

#create-god (3)
 $ns node-config -adhocRouting $val(adhocRouting) \
                  -llType $val(ll) \
                  -macType $val(mac) \
                  -ifqType $val(ifq) \
                  -ifqLen $val(ifqlen) \
                  -antType $val(ant) \
                  -propType $val(prop) \
                  -phyType $val(netif) \
                  -channelType $val(chan) \
                  -topoInstance $topo \
                  -agentTrace ON \
                  -routerTrace ON \
                  -macTrace OFF \
                  -movementTrace OFF\
                  -energyModel "EnergyModel" \
                  -initialEnergy 1000

# Energy model
     $ns node-config   -energyModel EnergyModel \
                       -initialEnergy 1000 \
                       -txPower 1.75 \
                       -rxPower 1.75 \
                       -idlePower 0.0 \
#sensePower 0.0
for {set i 0} {$i < $val(nn) } { incr i } {
           set node_($i) [$ns node]
     }

source test2
source test1

Phy/WirelessPhy set bandwidth 250mb

#$ns at 0.0 "$ns trace-annotate \"MOBILE NODE MOVEMENTS\""
for {set i 0} {$i < $val(nn) } {incr i} {
$node_($i) color blue
}
 
for {set i 0} {$i < $val(nn) } {incr i} {
$node_($i) color blue
$ns at 0.0 "$node_($i) color black" 
}

#$ns at 18 "$node_(0) color red" 

#setting initial position:
for {set i 0} {$i < $val(nn)} { incr i } {
$ns initial_node_pos $node_($i) 20
}
Mac/802_15_4 wpanNam FlowClr -p EAMRP -c tomato
Mac/802_15_4 wpanNam FlowClr -p TCP -c green
Mac/802_15_4 wpanNam FlowClr -p MAC -c blue

#$ns at $val(stop) "$ns nam-end-wireless $val(stop)"
$ns at $val(stop) "stop"
$ns at 130.01 "puts \"end simulation\" ; $ns halt"
proc stop {} {
   global ns tracefd namtrace
   $ns flush-trace
   close $tracefd
   close $namtrace
    
exec xgraph Energy -x nodes -y --------------------Energy &
exec xgraph scalability.tr -x nodes -y --------------------Scalability &
exec xgraph bandwidth.tr -x nodes -y --------------------Bandwidth &
exec xgraph Throughput.tr -x nodes -y --------------------Throughput &
  
exec nam Water_Quality_WSN.nam &
}

$ns run
