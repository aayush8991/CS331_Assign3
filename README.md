# CS331_Assign3

## Aayush Parmar 22110181
## Bhoumik Patidar 22110049

**Task 1**

To run task 1, run the following:
```
sudo python3 topoa.py
```

To run updated Remote Controller with Spanning Tree Protocol:
First run 
```
./pox.py stp_config.py
```
on another terminal, then run

```
sudo python3 topob.py
```


**Task 2**

To run NAT implemented topology, 
```
sudo python3 toponat.py
```

To use ping or iperf,
```
h1 ping h3 -c 8
```
and 
```
xterm -title "H1 Client (Cubic)" -e "iperf -c', h6.IP(), '-p 5001 -b 1M -P 1 -t 120 -i 1 ; bash" &
```
on mininet CLI

**Task 3**

To compile all file,
```
cc distance_vector.c node0.c node1.c node2.c node3.c
```
then run 
```
./a.out
```