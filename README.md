# SGT-simulation
## Compilation
```bash
g++ graph_construction.cpp -O3 -o graph_construction --std=c++11
g++ tarjan.cpp --std=c++11 -O3 -g3 -o tarjan
```
## Examples
```bash
`echo 0.8` | ./graph_construction # create a graph input file with 0.8 skew factor among 1000 transactions, produces a file named 0.800000_1000.txt

./tarjan < 0.800000_1000.txt     # run cycle detection and breaking algorithm on input file
aborted 109
Number of SCCs found at iter(1): 255, # size>1 SSCs this round: 1, # size-1 SSCs this round: 254, # vertices: 884, Total # size-1 SCCs now: 254, aborted nodes 109
Number of SCCs found at iter(2): 34, # size>1 SSCs this round: 1, # size-1 SSCs this round: 33, # vertices: 884, Total # size-1 SCCs now: 287, aborted nodes 201
Number of SCCs found at iter(3): 26, # size>1 SSCs this round: 2, # size-1 SSCs this round: 24, # vertices: 884, Total # size-1 SCCs now: 311, aborted nodes 264
Number of SCCs found at iter(4): 12, # size>1 SSCs this round: 1, # size-1 SSCs this round: 11, # vertices: 884, Total # size-1 SCCs now: 322, aborted nodes 303
Number of SCCs found at iter(5): 8, # size>1 SSCs this round: 1, # size-1 SSCs this round: 7, # vertices: 884, Total # size-1 SCCs now: 329, aborted nodes 338
Number of SCCs found at iter(6): 22, # size>1 SSCs this round: 1, # size-1 SSCs this round: 21, # vertices: 884, Total # size-1 SCCs now: 350, aborted nodes 367
Number of SCCs found at iter(7): 24, # size>1 SSCs this round: 1, # size-1 SSCs this round: 23, # vertices: 884, Total # size-1 SCCs now: 373, aborted nodes 393
Number of SCCs found at iter(8): 9, # size>1 SSCs this round: 1, # size-1 SSCs this round: 8, # vertices: 884, Total # size-1 SCCs now: 381, aborted nodes 410
Number of SCCs found at iter(9): 30, # size>1 SSCs this round: 1, # size-1 SSCs this round: 29, # vertices: 884, Total # size-1 SCCs now: 410, aborted nodes 420
Number of SCCs found at iter(10): 28, # size>1 SSCs this round: 2, # size-1 SSCs this round: 26, # vertices: 884, Total # size-1 SCCs now: 436, aborted nodes 436
Number of SCCs found at iter(11): 22, # size>1 SSCs this round: 1, # size-1 SSCs this round: 21, # vertices: 884, Total # size-1 SCCs now: 457, aborted nodes 443
Number of SCCs found at iter(12): 6, # size>1 SSCs this round: 0, # size-1 SSCs this round: 6, # vertices: 884, Total # size-1 SCCs now: 463, aborted nodes 444
Tarjan: 841us, committed 444 aborted 444 cascading_aborts 165 abort rate 0.444
```