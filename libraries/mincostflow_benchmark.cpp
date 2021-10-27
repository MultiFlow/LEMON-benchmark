#include <iostream>
#include <lemon/static_graph.h>
#include <lemon/smart_graph.h>
#include <lemon/list_graph.h>
#include <lemon/dimacs.h>
#include <lemon/time_measure.h>

#include "benchmark_config.h"

#ifdef USE_LEMON
#include <lemon/network_simplex.h>
#include <lemon/cost_scaling.h>
#endif

#ifdef USE_LEDA
#include <LEDA/graph/graph.h>
#include <LEDA/graph/min_cost_flow.h>
//#include <LEDA/graph/static_graph.h>
//#include <LEDA/graph/node_slot.h>
//#include <LEDA/graph/edge_slot.h>
//#include <LEDA/graph/node_slot1.h>
//#include <LEDA/graph/templates/mcf_cost_scaling.h>
#endif

using namespace lemon;

typedef SmartDigraph GR;
typedef LEMON_GRAPH TGR;
typedef GR::NodeMap<int> NM;
typedef GR::ArcMap<int> AM;

#ifdef USE_LEMON
void lemonBenchmark(GR *cgr, AM *clower, AM *cupper, AM *ccost, NM *csupply)
{
  // Copy *cgr to gr (static memory)
  Timer timer_copy;
  TGR gr;
  TGR::ArcMap<int> lower(gr), upper(gr), cost(gr);
  TGR::NodeMap<int> supply(gr);
  digraphCopy(*cgr, gr)
    .arcMap(*clower, lower)
    .arcMap(*cupper, upper)
    .arcMap(*ccost, cost)
    .nodeMap(*csupply, supply)
    .run();

  delete clower;
  delete cupper;
  delete ccost;
  delete csupply;
  delete cgr;
  timer_copy.stop();

  // Run NetworkSimplex
  Timer timer;
  LEMON_MCF_SOLVER<TGR, int> ns(gr);
  bool res = ns.lowerMap(lower).upperMap(upper).costMap(cost)
               .supplyMap(supply).run() == ns.OPTIMAL;
  timer.stop();

#ifndef PLOT_OUTPUT
  std::cout << "LEMON->LEMON copy:         ";
  std::cout << timer_copy << '\n';
  std::cout << "LEMON NetworkSimplex:      ";
  std::cout << timer << '\n';
  std::cout << "Result, total flow cost:   "
            << res << " : " << ns.totalCost() << '\n';
#else
  if (!res || ns.totalCost() < 0) std::cout << "\tError" << std::flush;
  std::cout << '\t' << timer.realTime() << std::flush;
#endif

  if (!res) {
    std::cerr << std::endl << "Wrong result!" << std::endl;
    exit(-1);
  }
}
#endif


#ifdef USE_LEDA
void ledaBenchmark(GR *gr, AM *lower, AM *upper, AM *cost, NM *supply)
{
  using namespace leda;

#ifdef LEDA_STATIC_GRAPH
  // I cannot compile the template MCF algorithm with static_graph
  return;
/*
  typedef static_graph<opposite_graph, data_slots<7> > st_graph;
  typedef st_graph::node st_node;
  typedef st_graph::edge st_edge;

  typedef node_slot1<double,st_graph,1>  pot_array;  // 2 slots
  typedef node_slot<int,st_graph,3>      excess_array;
  typedef node_slot<int,st_graph,4>      dist_array;
  typedef node_slot<st_node,st_graph,5>  succ_array; 
  typedef node_slot<st_node,st_graph,6>  pred_array; 

  // Copy *gr to a leda graph g
  Timer timer_copy;
  st_graph g;
  g.start_construction(countNodes(*gr), countArcs(*gr));
  node_array<int, st_graph> s(g);
  s.init(g, countNodes(*gr), 0);
  edge_array<int, st_graph> l(g), u(g), c(g);
  l.init(g, countArcs(*gr), 0);
  u.init(g, countArcs(*gr), 0);
  c.init(g, countArcs(*gr), 0);
  GR::NodeMap<st_node> *node_ref = new GR::NodeMap<st_node>(*gr);
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    st_node v = g.new_node();
    (*node_ref)[n] = v;
    s[v] = (*supply)[n];
  }
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    st_node v = (*node_ref)[n];
    for (GR::OutArcIt a(*gr, n); a != INVALID; ++a) {
      st_edge e = g.new_edge(v, (*node_ref)[gr->target(a)]);
      l[e] = (*lower)[a];
      u[e] = (*upper)[a];
      c[e] = (*cost)[a];
    }
  }
  g.finish_construction();
  delete node_ref;
  delete lower;
  delete upper;
  delete cost;
  delete supply;
  delete gr;
  timer_copy.stop();

  // Run Cost Scaling
  edge_array<int, st_graph> flow(g);
  Timer timer;
  mcf_cost_scaling<int, st_graph, pot_array, excess_array, dist_array,
    succ_array, pred_array> alg;
  bool res = alg.run(g, l, u, c, s, flow);
  timer.stop();
*/
#else
  // Copy *gr to a leda graph g
  Timer timer_copy;
  graph g;
  node_array<int> s(g);
  s.init(g, countNodes(*gr), 0);
  edge_array<int> l(g), u(g), c(g);
  l.init(g, countArcs(*gr), 0);
  u.init(g, countArcs(*gr), 0);
  c.init(g, countArcs(*gr), 0);
  GR::NodeMap<node> *node_ref = new GR::NodeMap<node>(*gr);
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    node v = g.new_node();
    (*node_ref)[n] = v;
    s[v] = (*supply)[n];
  }
  for (GR::ArcIt a(*gr); a != INVALID; ++a) {
    edge e = g.new_edge((*node_ref)[gr->source(a)],
                        (*node_ref)[gr->target(a)]);
    l[e] = (*lower)[a];
    u[e] = (*upper)[a];
    c[e] = (*cost)[a];
  }

  delete node_ref;
  delete lower;
  delete upper;
  delete cost;
  delete supply;
  delete gr;
  timer_copy.stop();

  // Run Cost Scaling
  edge_array<int> flow(g);
  Timer timer;
  bool res = MIN_COST_FLOW(g, l, u, c, s, flow);
  timer.stop();

  int total = 0;
  edge e;
  forall_edges(e, g) total += flow[e] * c[e];

#ifndef PLOT_OUTPUT
  std::cout << "LEMON->LEDA copy:          ";
  std::cout << timer_copy << '\n';
  std::cout << "LEDA MIN_COST_FLOW:        ";
  std::cout << timer << '\n';
  std::cout << "Result, total flow cost:   "
            << res << " : " << total << '\n';
#else
  if (!res || total < 0) std::cout << "\tError" << std::flush;
  std::cout << '\t' << timer.realTime() << std::flush;
#endif

  if (!res) {
    std::cerr << std::endl << "Wrong result!" << std::endl;
    exit(-1);
  }
#endif
}
#endif


int main(int argc, const char *argv[]) {
#ifdef USE_BOOST
  return 0;
#else

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
      (*upper)[a] = (*sup)[n];
      (*cost)[a] = 0;
      (*sup)[s] += (*sup)[n];
      (*sup)[n] = 0;
    }
    if ((*sup)[n] < 0) {
      GR::Arc a = gr->addArc(n, t);
      (*lower)[a] = 0;
      (*upper)[a] = -(*sup)[n];
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
#ifdef USE_LEMON
  std::cout << countNodes(*gr) << std::flush;
#endif
#endif

#ifdef USE_LEMON
  lemonBenchmark(gr, lower, upper, cost, sup);
  std::cout << '\t';
  return 0;
#endif

#ifdef USE_LEDA
  ledaBenchmark(gr, lower, upper, cost, sup);
  std::cout << std::endl;
  return 0;
#endif

#endif
}
