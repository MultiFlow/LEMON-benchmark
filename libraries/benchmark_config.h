#ifndef BENCHMARK_CONFIG
#define BENCHMARK_CONFIG

#define USE_LEMON
//#define USE_BOOST
// #define USE_LEDA

#define PLOT_OUTPUT

#define MAX_FLOW_FACTOR 1.5

#define LEMON_GRAPH SmartDigraph
//#define LEMON_GRAPH ListDigraph
//#define LEMON_GRAPH StaticDigraph
#define LEMON_MCF_SOLVER CostScaling
//#define LEMON_MCF_SOLVER NetworkSimplex

// #define BOOST_GRAPH vecS, vecS
//#define BOOST_GRAPH listS, listS
//#define BOOST_GRAPH_CSR
// #define BOOST_HEAP_ARITY 2
//#define BOOST_HEAP_ARITY 4

//#define LEDA_STATIC_GRAPH
#define LEDA_STANDARD_BINARY_HEAP
#define LEDA_CHECKING_OFF

#endif
