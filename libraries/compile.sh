#/bin/bash
lemon_dir=~/lemon-1.2
boost_dir=~/boost-1.43
leda_dir=~/leda-5.0
opt=-O3

g++ -o dijkstra_lemon -D USE_LEMON $opt -I $lemon_dir dijkstra_benchmark.cpp -L $lemon_dir/lemon/.libs -lemon
g++ -o dijkstra_boost -D USE_BOOST $opt -I $lemon_dir -I $boost_dir dijkstra_benchmark.cpp -L $lemon_dir/lemon/.libs -lemon
g++ -o dijkstra_leda  -D USE_LEDA  $opt -I $lemon_dir -I $leda_dir/incl dijkstra_benchmark.cpp -L $lemon_dir/lemon/.libs -lemon -L $leda_dir -lG -lL
#g++ -o dijkstra_leda  -D USE_LEDA  $opt -m32 -I $lemon_dir -I $leda_dir/incl dijkstra_benchmark.cpp -L $leda_dir -lG -lL

g++ -o maxflow_lemon -D USE_LEMON $opt -I $lemon_dir maxflow_benchmark.cpp -L $lemon_dir/lemon/.libs -lemon
g++ -o maxflow_boost -D USE_BOOST $opt -I $lemon_dir -I $boost_dir maxflow_benchmark.cpp -L $lemon_dir/lemon/.libs -lemon
g++ -o maxflow_leda  -D USE_LEDA  $opt -I $lemon_dir -I $leda_dir/incl maxflow_benchmark.cpp -L $lemon_dir/lemon/.libs -lemon -L $leda_dir -lG -lL
#g++ -o maxflow_leda  -D USE_LEDA  $opt -m32 -I $lemon_dir -I $leda_dir/incl maxflow_benchmark.cpp -L $leda_dir -lG -lL

g++ -o mincostflow_lemon -D USE_LEMON $opt -I $lemon_dir mincostflow_benchmark.cpp -L $lemon_dir/lemon/.libs -lemon
g++ -o mincostflow_boost -D USE_BOOST $opt -I $lemon_dir -I $boost_dir mincostflow_benchmark.cpp -L $lemon_dir/lemon/.libs -lemon
g++ -o mincostflow_leda  -D USE_LEDA  $opt -I $lemon_dir -I $leda_dir/incl mincostflow_benchmark.cpp -L $lemon_dir/lemon/.libs -lemon -L $leda_dir -lG -lL
#g++ -o mincostflow_leda  -D USE_LEDA  $opt -m32 -I $lemon_dir -I $leda_dir/incl mincostflow_benchmark.cpp -lemon -L $leda_dir -lG -lL

g++ -o dijkstra_b2l -D USE_BOOST $opt -I $lemon_dir -I $boost_dir dijkstra_benchmark_b2l.cpp -L $lemon_dir/lemon/.libs -lemon
g++ -o dijkstra_l2b -D USE_BOOST $opt -I $lemon_dir -I $boost_dir dijkstra_benchmark_l2b.cpp -L $lemon_dir/lemon/.libs -lemon

g++ -o maxflow_b2l -D USE_BOOST $opt -I $lemon_dir -I $boost_dir maxflow_benchmark_b2l.cpp -L $lemon_dir/lemon/.libs -lemon
g++ -o maxflow_l2b -D USE_BOOST $opt -I $lemon_dir -I $boost_dir maxflow_benchmark_l2b.cpp -L $lemon_dir/lemon/.libs -lemon
