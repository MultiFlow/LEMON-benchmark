#include <lemon/dimacs.h>
#include <lemon/list_graph.h>
#include <lemon/smart_graph.h>
#include <lemon/static_graph.h>
#include <lemon/time_measure.h>

#include <iostream>
#include <limits>

#include "benchmark_config.h"

#ifdef USE_LEMON
#include <lemon/dijkstra.h>
#endif

#ifdef USE_BOOST
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/iteration_macros.hpp>

#include "boost_dijkstra.hpp"
#endif

#ifdef USE_LEDA
#include <LEDA/graph/graph.h>
#include <LEDA/graph/shortest_path.h>
#include <LEDA/graph/static_graph.h>
#ifdef LEDA_STANDARD_BINARY_HEAP
#include "leda_dijkstra_std.h"
#else
#include "leda_dijkstra.h"
#endif
#endif

using namespace lemon;

typedef SmartDigraph    GR;
typedef LEMON_GRAPH     TGR;
typedef GR::ArcMap<int> LengthMap;

#ifdef USE_LEMON
void lemonBenchmark(GR* cgr, LengthMap* clen, GR::Node cs, GR::Node ct) {
  // Copy *cgr to gr (static memory)
  Timer            timer_copy;
  TGR              gr;
  TGR::ArcMap<int> len(gr);
  TGR::Node        source, target;
  digraphCopy(*cgr, gr)
      .arcMap(*clen, len)
      .node(cs, source)
      .node(ct, target)
      .run();
  timer_copy.stop();

  delete clen;
  delete cgr;

  typedef NullMap<TGR::Node, TGR::Arc> PredMap;
  PredMap                              predMap;

  // Run Dijkstra
  Timer                                                        timer;
  Dijkstra<TGR, TGR::ArcMap<int>>::SetPredMap<PredMap>::Create dijkstra(
      gr, len);
  dijkstra.predMap(predMap);
  dijkstra.run(source);
  timer.stop();

#ifndef PLOT_OUTPUT
  std::cout << "LEMON->LEMON copy: ";
  std::cout << timer_copy << '\n';
  std::cout << "LEMON Dijkstra:    ";
  std::cout << timer << '\n';
  std::cout << "Distance:          " << dijkstra.dist(target) << '\n';
#else
  if (dijkstra.dist(target) < 0)
    std::cout << "\tError" << std::flush;
  std::cout << '\t' << timer.realTime() << std::flush;
#endif
}
#endif

#ifdef USE_BOOST
struct WeightType {
  int value;
  WeightType(int v = 0) : value(v) {}
};

struct DistType {
  int value;
};

void boostBenchmark(GR* gr, LengthMap* len, GR::Node source, GR::Node target) {
  using namespace boost;
  using namespace boost::graph;

#ifdef BOOST_GRAPH_CSR
#define BOOST_GRAPH_USE_NEW_CSR_INTERFACE
  typedef compressed_sparse_row_graph<directedS, DistType, WeightType> graph_t;
  typedef graph_traits<graph_t>                                        traits_t;
  typedef traits_t::vertex_descriptor                                  vertex_t;
  typedef traits_t::edge_descriptor                                    edge_t;

  // Copy *gr to a boost graph g
  Timer             timer_copy;
  GR::NodeMap<int>* node_id = new GR::NodeMap<int>(*gr);
  int               i       = 0;
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    (*node_id)[n] = i++;
  }
  typedef std::pair<int, int> E;
  std::vector<E>*             edge_list   = new std::vector<E>;
  std::vector<WeightType>*    edge_length = new std::vector<WeightType>;
  for (GR::ArcIt a(*gr); a != INVALID; ++a) {
    (*edge_list)
        .push_back(E((*node_id)[gr->source(a)], (*node_id)[gr->target(a)]));
    (*edge_length).push_back((*len)[a]);
  }

  graph_t g(
      edges_are_unsorted_multi_pass,
      edge_list->begin(),
      edge_list->end(),
      edge_length->begin(),
      countNodes(*gr));
  vertex_t s = vertex((*node_id)[source], g);
  vertex_t t = vertex((*node_id)[target], g);

  delete node_id;
  delete edge_list;
  delete edge_length;
  delete len;
  delete gr;
  timer_copy.stop();

  // Run Dijkstra
  Timer timer;
  dijkstra_shortest_paths(
      g,
      s,
      weight_map(get(&WeightType::value, g))
          .distance_map(get(&DistType::value, g)));
  timer.stop();

#ifndef PLOT_OUTPUT
  std::cout << "LEMON->BOOST copy: ";
  std::cout << timer_copy << '\n';
  std::cout << "BOOST Dijkstra:    ";
  std::cout << timer << '\n';
  std::cout << "Distance:          " << get(&DistType::value, g)[t] << '\n';
#else
  if (get(&DistType::value, g)[t] < 0)
    std::cout << "\tError" << std::flush;
  std::cout << '\t' << timer.realTime() << std::flush;
#endif

#else
  typedef adjacency_list<
      BOOST_GRAPH,
      directedS,
      property<vertex_distance_t, int>,
      property<edge_weight_t, int>>
                                      graph_t;
  typedef graph_traits<graph_t>       traits_t;
  typedef traits_t::vertex_descriptor vertex_t;
  typedef traits_t::edge_descriptor   edge_t;

  // Copy *gr to a boost graph g
  Timer                                      timer_copy;
  graph_t                                    g;
  property_map<graph_t, edge_weight_t>::type length = get(edge_weight, g);
  GR::NodeMap<vertex_t>* node_ref = new GR::NodeMap<vertex_t>(*gr);
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    (*node_ref)[n] = add_vertex(g);
  }
  for (GR::ArcIt a(*gr); a != INVALID; ++a) {
    edge_t e =
        add_edge((*node_ref)[gr->source(a)], (*node_ref)[gr->target(a)], g)
            .first;
    length[e] = (*len)[a];
  }
  vertex_t s = (*node_ref)[source];
  vertex_t t = (*node_ref)[target];
  delete node_ref;
  delete len;
  delete gr;
  timer_copy.stop();

  // Run Dijkstra
  property_map<graph_t, vertex_distance_t>::type dist = get(vertex_distance, g);
  Timer                                          timer;
  dijkstra_shortest_paths(g, s, distance_map(dist));
  timer.stop();

#ifndef PLOT_OUTPUT
  std::cout << "LEMON->BOOST copy: ";
  std::cout << timer_copy << '\n';
  std::cout << "BOOST Dijkstra:    ";
  std::cout << timer << '\n';
  std::cout << "Distance:          " << dist[t] << '\n';
#else
  if (dist[t] < 0)
    std::cout << "\tError" << std::flush;
  std::cout << '\t' << timer.realTime() << std::flush;
#endif

#endif
}
#endif

#ifdef USE_LEDA
void ledaBenchmark(GR* gr, LengthMap* len, GR::Node source, GR::Node target) {
  using namespace leda;

#ifdef LEDA_STATIC_GRAPH
  typedef static_graph<directed_graph> st_graph;
  typedef st_graph::node               st_node;
  typedef st_graph::edge               st_edge;

  // Copy *gr to a leda graph g
  Timer    timer_copy;
  st_graph g;
  g.start_construction(countNodes(*gr), countArcs(*gr));
  edge_array<int, st_graph> length(g);
  length.init(g, countArcs(*gr), 0);
  GR::NodeMap<st_node>* node_ref = new GR::NodeMap<st_node>(*gr);
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    (*node_ref)[n] = g.new_node();
  }
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    st_node v = (*node_ref)[n];
    for (GR::OutArcIt a(*gr, n); a != INVALID; ++a) {
      st_edge e = g.new_edge(v, (*node_ref)[gr->target(a)]);
      length[e] = (*len)[a];
    }
  }
  g.finish_construction();
  st_node s = (*node_ref)[source];
  st_node t = (*node_ref)[target];
  delete node_ref;
  delete len;
  delete gr;
  timer_copy.stop();

  // Run Dijkstra
  node_array<int, st_graph> dist(g, std::numeric_limits<int>::max());
  Timer                     timer;
  dijkstra<int, st_graph>   alg;
  alg.run(g, s, length, dist);
  timer.stop();

#else
  // Copy *gr to a leda graph g
  Timer           timer_copy;
  graph           g;
  edge_array<int> length(g);
  length.init(g, countArcs(*gr), 0);
  GR::NodeMap<node>* node_ref = new GR::NodeMap<node>(*gr);
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    (*node_ref)[n] = g.new_node();
  }
  for (GR::ArcIt a(*gr); a != INVALID; ++a) {
    edge e = g.new_edge((*node_ref)[gr->source(a)], (*node_ref)[gr->target(a)]);
    length[e] = (*len)[a];
  }
  node s = (*node_ref)[source];
  node t = (*node_ref)[target];
  delete node_ref;
  delete len;
  delete gr;
  timer_copy.stop();

  // Run Dijkstra
  Timer           timer;
  node_array<int> dist(g, std::numeric_limits<int>::max());
  DIJKSTRA_T(g, s, length, dist);
  timer.stop();
#endif

#ifndef PLOT_OUTPUT
  std::cout << "LEMON->LEDA copy:  ";
  std::cout << timer_copy << '\n';
  std::cout << "LEDA Dijkstra:     ";
  std::cout << timer << '\n';
  std::cout << "Distance:          " << dist[t] << '\n';
#else
  if (dist[t] < 0)
    std::cout << "\tError" << std::flush;
  std::cout << '\t' << timer.realTime() << std::flush;
#endif
}
#endif

int main(int argc, const char* argv[]) {
#ifndef PLOT_OUTPUT
  std::cout << "Reading input graph... " << std::flush;
#endif

  GR*               gr    = new GR;
  GR::NodeMap<int>* sup   = new GR::NodeMap<int>(*gr);
  GR::ArcMap<int>*  lower = new GR::ArcMap<int>(*gr);
  GR::ArcMap<int>*  upper = new GR::ArcMap<int>(*gr);
  GR::ArcMap<int>*  cost  = new GR::ArcMap<int>(*gr);
  if (argc < 2) {
    readDimacsMin(std::cin, *gr, *lower, *upper, *cost, *sup);
  } else {
    std::ifstream input(argv[1]);
    readDimacsMin(input, *gr, *lower, *upper, *cost, *sup);
  }
  GR::Node s = gr->addNode();
  (*sup)[s]  = 0;
  GR::Node t = gr->addNode();
  (*sup)[t]  = 0;
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    if ((*sup)[n] > 0) {
      GR::Arc a   = gr->addArc(s, n);
      (*lower)[a] = 0;
      (*upper)[a] = (*sup)[n];
      (*cost)[a]  = 0;
      (*sup)[s] += (*sup)[n];
      (*sup)[n] = 0;
    }
    if ((*sup)[n] < 0) {
      GR::Arc a   = gr->addArc(n, t);
      (*lower)[a] = 0;
      (*upper)[a] = -(*sup)[n];
      (*cost)[a]  = 0;
      (*sup)[t] += (*sup)[n];
      (*sup)[n] = 0;
    }
  }

#ifndef PLOT_OUTPUT
  std::cout << "done." << std::endl;
  std::cout << "n = " << countNodes(*gr) << ", m = " << countArcs(*gr)
            << std::endl
            << std::endl;
#else
#ifdef USE_LEMON
  std::cout << countNodes(*gr) << std::flush;
#endif
#endif

  delete lower;
  delete upper;
  delete sup;

#ifdef USE_LEMON
  lemonBenchmark(gr, cost, s, t);
  std::cout << '\t';
  return 0;
#endif

#ifdef USE_BOOST
  boostBenchmark(gr, cost, s, t);
  std::cout << '\t';
  return 0;
#endif

#ifdef USE_LEDA
  ledaBenchmark(gr, cost, s, t);
  std::cout << std::endl;
  return 0;
#endif
}
