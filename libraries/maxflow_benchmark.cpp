#include <lemon/dimacs.h>
#include <lemon/list_graph.h>
#include <lemon/smart_graph.h>
#include <lemon/static_graph.h>
#include <lemon/time_measure.h>

#include <iostream>

#include "benchmark_config.h"

#ifdef USE_LEMON
#include <lemon/preflow.h>
#endif

#ifdef USE_BOOST
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#endif

#ifdef USE_LEDA
#include <Kernel/graph/max_flow.h>
#include <LEDA/graph/graph.h>
#include <LEDA/graph/static_graph.h>
#include <LEDA/graph/templates/max_flow.h>
#endif

using namespace lemon;

typedef SmartDigraph    GR;
typedef LEMON_GRAPH     TGR;
typedef GR::ArcMap<int> CapMap;

#ifdef USE_LEMON
void lemonBenchmark(GR* cgr, CapMap* ccap, GR::Node cs, GR::Node ct) {
  // Copy *cgr to gr (static memory)
  Timer            timer_copy;
  TGR              gr;
  TGR::ArcMap<int> cap(gr);
  TGR::Node        source, target;
  digraphCopy(*cgr, gr)
      .arcMap(*ccap, cap)
      .node(cs, source)
      .node(ct, target)
      .run();
  timer_copy.stop();

  delete ccap;
  delete cgr;

  // Run Preflow
  Timer                          timer;
  Preflow<TGR, TGR::ArcMap<int>> preflow(gr, cap, source, target);
  preflow.run();
  timer.stop();

#ifndef PLOT_OUTPUT
  std::cout << "LEMON->LEMON copy: ";
  std::cout << timer_copy << '\n';
  std::cout << "LEMON Preflow:     ";
  std::cout << timer << '\n';
  std::cout << "Max flow value:    " << preflow.flowValue() << '\n';
#else
  if (preflow.flowValue() < 0)
    std::cout << "\tError" << std::flush;
  std::cout << '\t' << timer.realTime() << std::flush;
#endif
}
#endif

#ifdef USE_BOOST

using namespace boost;
using namespace boost::graph;

#ifdef BOOST_GRAPH_CSR
struct EdgeProperty {
  int cap;
  int res_cap;
  graph_traits<compressed_sparse_row_graph<directedS>>::edge_descriptor
      rev_edge;
  EdgeProperty(int c = 0, int rc = 0) : cap(c), res_cap(rc) {}
};
#endif

void boostBenchmark(GR* gr, CapMap* cap, GR::Node source, GR::Node target) {
#ifdef BOOST_GRAPH_CSR
#define BOOST_GRAPH_USE_NEW_CSR_INTERFACE
  typedef compressed_sparse_row_graph<directedS, no_property, EdgeProperty>
                                      graph_t;
  typedef graph_traits<graph_t>       traits_t;
  typedef traits_t::vertex_descriptor vertex_t;
  typedef traits_t::edge_descriptor   edge_t;

  // Copy *gr to a boost graph g
  Timer                  timer_copy;
  std::vector<GR::Node>* nodes   = new std::vector<GR::Node>;
  GR::NodeMap<int>*      node_id = new GR::NodeMap<int>(*gr);
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    (*node_id)[n] = nodes->size();
    (*nodes).push_back(n);
  }
  typedef std::pair<int, int> E;
  std::vector<E>*             edge_list = new std::vector<E>;
  std::vector<EdgeProperty>*  edge_prop = new std::vector<EdgeProperty>;
  int                         i         = 0;
  for (GR::ArcIt a(*gr); a != INVALID; ++a) {
    (*edge_list)
        .push_back(E((*node_id)[gr->source(a)], (*node_id)[gr->target(a)]));
    (*edge_prop).push_back(EdgeProperty((*cap)[a], i++));
    (*edge_list)
        .push_back(E((*node_id)[gr->target(a)], (*node_id)[gr->source(a)]));
    (*edge_prop).push_back(EdgeProperty(0, i++));
  }

  graph_t g(
      edges_are_unsorted_multi_pass,
      edge_list->begin(),
      edge_list->end(),
      edge_prop->begin(),
      nodes->size());
  std::vector<edge_t>* edge_desc = new std::vector<edge_t>(countArcs(*gr) * 2);
  BGL_FORALL_EDGES(e, g, graph_t) {
    (*edge_desc)[get(&EdgeProperty::res_cap, g)[e]] = e;
  }
  BGL_FORALL_EDGES(e, g, graph_t) {
    get(&EdgeProperty::rev_edge, g)[e] =
        (*edge_desc)[get(&EdgeProperty::res_cap, g)[e] ^ 1];
    get(&EdgeProperty::res_cap, g)[e] = get(&EdgeProperty::cap, g)[e];
  }
  vertex_t s = vertex((*node_id)[source], g);
  vertex_t t = vertex((*node_id)[target], g);

  delete nodes;
  delete node_id;
  delete edge_list;
  delete edge_prop;
  delete edge_desc;
  delete cap;
  delete gr;
  timer_copy.stop();

  // Run Preflow
  int   flow;
  Timer timer;
  flow = push_relabel_max_flow(
      g,
      s,
      t,
      capacity_map(get(&EdgeProperty::cap, g))
          .residual_capacity_map(get(&EdgeProperty::res_cap, g))
          .reverse_edge_map(get(&EdgeProperty::rev_edge, g)));
  timer.stop();

#else
  typedef adjacency_list_traits<vecS, vecS, directedS> traits_t;
  typedef adjacency_list<
      BOOST_GRAPH,
      directedS,
      no_property,
      property<
          edge_capacity_t,
          int,
          property<
              edge_residual_capacity_t,
              int,
              property<edge_reverse_t, traits_t::edge_descriptor>>>>
                                      graph_t;
  typedef traits_t::vertex_descriptor vertex_t;
  typedef traits_t::edge_descriptor   edge_t;

  // Copy *gr to a boost graph g
  Timer                  timer_copy;
  graph_t                g;
  GR::NodeMap<vertex_t>* node_ref = new GR::NodeMap<vertex_t>(*gr);
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    (*node_ref)[n] = add_vertex(g);
  }
  for (GR::ArcIt a(*gr); a != INVALID; ++a) {
    edge_t e =
        add_edge((*node_ref)[gr->source(a)], (*node_ref)[gr->target(a)], g)
            .first;
    edge_t re =
        add_edge((*node_ref)[gr->target(a)], (*node_ref)[gr->source(a)], g)
            .first;
    put(edge_capacity_t(), g, e, (*cap)[a]);
    put(edge_capacity_t(), g, re, 0);
    put(edge_reverse_t(), g, e, re);
    put(edge_reverse_t(), g, re, e);
  }
  vertex_t s = (*node_ref)[source];
  vertex_t t = (*node_ref)[target];
  delete node_ref;
  delete cap;
  delete gr;
  timer_copy.stop();

  // Run Preflow
  int   flow;
  Timer timer;
  flow = push_relabel_max_flow(g, s, t);
  timer.stop();
#endif

#ifndef PLOT_OUTPUT
  std::cout << "LEMON->BOOST copy: ";
  std::cout << timer_copy << '\n';
  std::cout << "BOOST Preflow:     ";
  std::cout << timer << '\n';
  std::cout << "Max flow value:    " << flow << '\n';
#else
  if (flow < 0)
    std::cout << "\tError" << std::flush;
  std::cout << '\t' << timer.realTime() << std::flush;
#endif
}
#endif

#ifdef USE_LEDA
void ledaBenchmark(GR* gr, CapMap* cap, GR::Node source, GR::Node target) {
  using namespace leda;

#ifdef LEDA_STATIC_GRAPH
  typedef static_graph<bidirectional_graph> st_graph;
  typedef st_graph::node                    st_node;
  typedef st_graph::edge                    st_edge;

  // Copy *gr to a leda graph g
  Timer    timer_copy;
  st_graph g;
  g.start_construction(countNodes(*gr), countArcs(*gr));
  edge_array<int, st_graph> capacity(g);
  capacity.init(g, countArcs(*gr), 0);
  GR::NodeMap<st_node>* node_ref = new GR::NodeMap<st_node>(*gr);
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    (*node_ref)[n] = g.new_node();
  }
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    st_node v = (*node_ref)[n];
    for (GR::OutArcIt a(*gr, n); a != INVALID; ++a) {
      st_edge e   = g.new_edge(v, (*node_ref)[gr->target(a)]);
      capacity[e] = (*cap)[a];
    }
  }
  g.finish_construction();
  st_node s = (*node_ref)[source];
  st_node t = (*node_ref)[target];
  delete node_ref;
  delete cap;
  delete gr;
  timer_copy.stop();

  // Run Preflow
  edge_array<int, st_graph> flow(g);
  Timer                     timer;
  max_flow<int, st_graph>   alg;
  int                       flow_value = alg.run(g, s, t, capacity, flow);
  timer.stop();

#else
  // Copy *gr to a leda graph g
  Timer           timer_copy;
  graph           g;
  edge_array<int> capacity(g);
  capacity.init(g, countArcs(*gr), 0);
  GR::NodeMap<node>* node_ref = new GR::NodeMap<node>(*gr);
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    (*node_ref)[n] = g.new_node();
  }
  for (GR::ArcIt a(*gr); a != INVALID; ++a) {
    edge e = g.new_edge((*node_ref)[gr->source(a)], (*node_ref)[gr->target(a)]);
    capacity[e] = (*cap)[a];
  }
  node s = (*node_ref)[source];
  node t = (*node_ref)[target];
  delete node_ref;
  delete cap;
  delete gr;
  timer_copy.stop();

  // Run Preflow
  edge_array<int> flow(g);
  Timer           timer;
  int             flow_value = MAX_FLOW_T(g, s, t, capacity, flow);
  timer.stop();
#endif

#ifndef PLOT_OUTPUT
  std::cout << "LEMON->LEDA copy:  ";
  std::cout << timer_copy << '\n';
  std::cout << "LEDA MAX_FLOW:     ";
  std::cout << timer << '\n';
  std::cout << "Max flow value:    " << flow_value << '\n';
#else
  if (flow_value < 0)
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
      (*upper)[a] = int(MAX_FLOW_FACTOR * (*sup)[n]);
      (*cost)[a]  = 0;
      (*sup)[s] += (*sup)[n];
      (*sup)[n] = 0;
    }
    if ((*sup)[n] < 0) {
      GR::Arc a   = gr->addArc(n, t);
      (*lower)[a] = 0;
      (*upper)[a] = int(MAX_FLOW_FACTOR * -(*sup)[n]);
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
  delete cost;
  delete sup;

#ifdef USE_LEMON
  lemonBenchmark(gr, upper, s, t);
  std::cout << '\t';
  return 0;
#endif

#ifdef USE_BOOST
  boostBenchmark(gr, upper, s, t);
  std::cout << '\t';
  return 0;
#endif

#ifdef USE_LEDA
  ledaBenchmark(gr, upper, s, t);
  std::cout << std::endl;
  return 0;
#endif
}
