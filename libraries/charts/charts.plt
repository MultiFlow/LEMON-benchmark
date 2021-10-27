set terminal postscript eps monochrome enhanced
set size 0.62,0.58
set logscale
set key left top Left reverse samplen 3 spacing 1.2 width -2 nobox
set format x "%7.0f"
set format y "%gs"
# set format y "10^{%L}s"

set mxtics 10
set mytics 2
# set grid

# set xlabel "Number of nodes"
# set ylabel "Running time"

set yrange [0.0003333:300]

set output "dijkstra_lg.eps"
plot [666:1500000] \
"../results/dijkstra_lg.txt" using 1:2 title "LEMON" with lp lt 1 pt 1 , \
"../results/dijkstra_lg.txt" using 1:3 title "BGL" with lp lt 2 pt 2 , \
"../results/dijkstra_lg.txt" using 1:4 title "LEDA" with lp lt 3 pt 3

set output "dijkstra_sq.eps"
plot [750:133333] \
"../results/dijkstra_sq.txt" using 1:2 title "LEMON" with lp lt 1 pt 1 , \
"../results/dijkstra_sq.txt" using 1:3 title "BGL" with lp lt 2 pt 2 , \
"../results/dijkstra_sq.txt" using 1:4 title "LEDA" with lp lt 3 pt 3

# set output "dijkstra_static_lg.eps"
# plot [666:1500000] \
# "../results/dijkstra_static_lg.txt" using 1:2 title "LEMON" with lp lt 1 pt 1 , \
# "../results/dijkstra_static_lg.txt" using 1:3 title "BGL" with lp lt 2 pt 2 , \
# "../results/dijkstra_static_lg.txt" using 1:4 title "LEDA" with lp lt 3 pt 3
# 
# set output "dijkstra_static_sq.eps"
# plot [750:133333] \
# "../results/dijkstra_static_sq.txt" using 1:2 title "LEMON" with lp lt 1 pt 1 , \
# "../results/dijkstra_static_sq.txt" using 1:3 title "BGL" with lp lt 2 pt 2 , \
# "../results/dijkstra_static_sq.txt" using 1:4 title "LEDA" with lp lt 3 pt 3

set output "maxflow_lg.eps"
plot [666:1500000] \
"../results/maxflow_lg.txt" using 1:2 title "LEMON" with lp lt 1 pt 1 , \
"../results/maxflow_lg.txt" using 1:3 title "BGL" with lp lt 2 pt 2 , \
"../results/maxflow_lg.txt" using 1:4 title "LEDA" with lp lt 3 pt 3

set output "maxflow_sq.eps"
plot [750:133333] \
"../results/maxflow_sq.txt" using 1:2 title "LEMON" with lp lt 1 pt 1 , \
"../results/maxflow_sq.txt" using 1:3 title "BGL" with lp lt 2 pt 2 , \
"../results/maxflow_sq.txt" using 1:4 title "LEDA" with lp lt 3 pt 3

# set output "maxflow_static_lg.eps"
# plot [666:1500000] \
# "../results/maxflow_static_lg.txt" using 1:2 title "LEMON" with lp lt 1 pt 1 , \
# "../results/maxflow_static_lg.txt" using 1:3 title "BGL" with lp lt 2 pt 2 , \
# "../results/maxflow_static_lg.txt" using 1:4 title "LEDA" with lp lt 3 pt 3
# 
# set output "maxflow_static_sq.eps"
# plot [750:133333] \
# "../results/maxflow_static_sq.txt" using 1:2 title "LEMON" with lp lt 1 pt 1 , \
# "../results/maxflow_static_sq.txt" using 1:3 title "BGL" with lp lt 2 pt 2 , \
# "../results/maxflow_static_sq.txt" using 1:4 title "LEDA" with lp lt 3 pt 3


set yrange [0.0003333:3000]

set output "mincostflow_lg.eps"
plot [666:1500000] \
"../results/mincostflow_lg.txt" using 1:2 title "LEMON" with lp lt 1 pt 1 , \
"../results/mincostflow_lg.txt" using 1:4 title "LEDA" with lp lt 3 pt 3
 
set output "mincostflow_sq.eps"
plot [750:133333] \
"../results/mincostflow_sq.txt" using 1:2 title "LEMON" with lp lt 1 pt 1 , \
"../results/mincostflow_sq.txt" using 1:4 title "LEDA" with lp lt 3 pt 3

# set output "mincostflow_lg.eps"
# plot [666:1500000] \
# "../results/mincostflow_lg.txt" using 1:2 title "LEMON CostScaling" with lp lt 1 pt 1 , \
# "../results/mincostflow_lg.txt" using 1:3 title "LEMON NetworkSimplex" with lp lt 2 pt 13 , \
# "../results/mincostflow_lg.txt" using 1:4 title "LEDA" with lp lt 3 pt 3
#  
# set output "mincostflow_sq.eps"
# plot [750:133333] \
# "../results/mincostflow_sq.txt" using 1:2 title "LEMON CostScaling" with lp lt 1 pt 1 , \
# "../results/mincostflow_sq.txt" using 1:3 title "LEMON NetworkSimplex" with lp lt 2 pt 13 , \
# "../results/mincostflow_sq.txt" using 1:4 title "LEDA" with lp lt 3 pt 3
