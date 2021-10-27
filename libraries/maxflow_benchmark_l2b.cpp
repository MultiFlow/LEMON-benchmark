#include <iostream>
#include <limits>

#include "benchmark_config.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

#include <lemon/smart_graph.h>
#include <lemon/list_graph.h>
#include <lemon/dimacs.h>
#include <lemon/time_measure.h>
#include <lemon/lemon_to_boost_graph.h>

using namespace lemon;

typedef LEMON_GRAPH GR;

void lemonToBoostBenchmark(GR *gr, GR::ArcMap<int> *cap,
                           GR::Node source, GR::Node target)
{
  using namespace boost;
  using namespace boost::graph;

  typedef lemon_graph<LEMON_GRAPH> graph_t;
  // function_requires<VertexListGraphConcept<graph_t> >();
  // function_requires<BidirectionalGraphConcept<graph_t> >();

  typedef graph_traits<graph_t> traits_t;
  typedef traits_t::vertex_descriptor vertex_t;
  typedef traits_t::edge_descriptor edge_t;

  // Copy *gr to an L2B graph g
  Timer timer_copy;
  graph_t g;
  lemon_graph_edge_property_map<LEMON_GRAPH, int> capacity(g), res_capacity(g);
  lemon_graph_edge_property_map<LEMON_GRAPH, edge_t> reverse(g);
  GR::NodeMap<vertex_t> *node_ref = new GR::NodeMap<vertex_t>(*gr);
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    (*node_ref)[n] = add_vertex(g);
  }
  for (GR::ArcIt a(*gr); a != INVALID; ++a) {
    edge_t e = add_edge((*node_ref)[gr->source(a)],
                        (*node_ref)[gr->target(a)], g).first;
    edge_t re = add_edge((*node_ref)[gr->target(a)],
                         (*node_ref)[gr->source(a)], g).first;
    capacity[e] = (*cap)[a];
    capacity[re] = 0;
    reverse[e] = re;
    reverse[re] = e;
  }
  vertex_t s = (*node_ref)[source];
  vertex_t t = (*node_ref)[target];
  delete node_ref;
  delete cap;
  delete gr;
  timer_copy.stop();

  // Run Preflow
  int flow;
  Timer timer;
  flow = push_relabel_max_flow(g, s, t,
              capacity_map(make_property_map_adaptor(capacity)).
              residual_capacity_map(make_property_map_adaptor(res_capacity)).
              reverse_edge_map(make_property_map_adaptor(reverse)));
  timer.stop();

#ifndef PLOT_OUTPUT
  std::cout << "LEMON->L2B copy:   ";
  std::cout << timer_copy << '\n';
  std::cout << "L2B Preflow:       ";
  std::cout << timer << '\n';
  std::cout << "Max flow value:    " << flow << '\n';
#else
  if (flow < 0) std::cout << "\tError" << std::flush;
  std::cout << '\t' << timer.realTime() << std::flush;
#endif
}

int main(int argc, const char *argv[]) {
#ifndef PLOT_OUTPUT
  std::cout << "Reading input graph... " << std::flush;
#endif

  GR *gr = new GR;
  GR::NodeMap<int> *sup = new GR::NodeMap<int>(*gr);
  GR::ArcMap<int> *lower = new GR::ArcMap<int>(*gr);
  GR::ArcMap<int> *upper = new GR::ArcMap<int>(*gr);
  GR::ArcMap<int> *cost = new GR::ArcMap<int>(*gr);
  if (argc < 2) {
    readDimacsMin(std::cin, *gr, *lower, *upper, *cost, *sup);
  } else {
    std::ifstream input(argv[1]);
    readDimacsMin(input, *gr, *lower, *upper, *cost, *sup);
  }
  GR::Node s = gr->addNode();
  (*sup)[s] = 0;
  GR::Node t = gr->addNode();
  (*sup)[t] = 0;
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    if ((*sup)[n] > 0) {
      GR::Arc a = gr->addArc(s, n);
      (*lower)[a] = 0;
      (*upper)[a] = int(MAX_FLOW_FACTOR * (*sup)[n]);
      (*cost)[a] = 0;
      (*sup)[s] += (*sup)[n];
      (*sup)[n] = 0;
    }
    if ((*sup)[n] < 0) {
      GR::Arc a = gr->addArc(n, t);
      (*lower)[a] = 0;
      (*upper)[a] = int(MAX_FLOW_FACTOR * -(*sup)[n]);
      (*cost)[a] = 0;
      (*sup)[t] += (*sup)[n];
      (*sup)[n] = 0;
    }
  }

#ifndef PLOT_OUTPUT
  std::cout << "done." << std::endl;
  std::cout << "n = " << countNodes(*gr) << ", m = " << countArcs(*gr)
    << std::endl << std::endl;
#else
  std::cout << countNodes(*gr) << std::flush;
#endif

  delete lower;
  delete cost;
  delete sup;

  lemonToBoostBenchmark(gr, upper, s, t);
#ifdef PLOT_OUTPUT
  std::cout << '\t';
#endif
  return 0;
}
