#include <iostream>
#include <limits>

#include "benchmark_config.h"

#include <lemon/smart_graph.h>
#include <lemon/dimacs.h>
#include <lemon/time_measure.h>
#include <lemon/boost_to_lemon_graph.h>
#include <lemon/dijkstra.h>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

using namespace lemon;

typedef SmartDigraph GR;

void boostToLemonBenchmark(GR *gr, GR::ArcMap<int> *len,
                           GR::Node source, GR::Node target)
{
  using namespace boost;
  using namespace boost::graph;

  typedef BoostDigraph<boost::adjacency_list<BOOST_GRAPH, directedS,
    boost::property<boost::vertex_distance_t, int>, 
    boost::property<boost::edge_index_t, std::size_t,
      boost::property<boost::edge_weight_t, int> > > > Bg;
  typedef Bg::InternalMap<boost::edge_weight_t> LengthMap;

  // Copy *gr to a B2L graph g
  Timer timer_copy;
  Bg g;
  LengthMap length(g);
  GR::NodeMap<Bg::Node> *node_ref = new GR::NodeMap<Bg::Node>(*gr);
  for (GR::NodeIt n(*gr); n != INVALID; ++n) {
    (*node_ref)[n] = g.addNode();
  }
  for (GR::ArcIt a(*gr); a != INVALID; ++a) {
    Bg::Arc e = g.addArc((*node_ref)[gr->source(a)],
                         (*node_ref)[gr->target(a)]);
    length[e] = (*len)[a];
  }
  Bg::Node s = (*node_ref)[source];
  Bg::Node t = (*node_ref)[target];
  delete node_ref;
  delete len;
  delete gr;
  timer_copy.stop();

  // Run Dijkstra
  typedef Bg::InternalMap<boost::vertex_distance_t> DistMap;
  DistMap dist(g);
  typedef NullMap<Bg::Node, Bg::Arc> PredMap;
  PredMap predMap;
  Timer timer;
  Dijkstra<Bg, LengthMap>
    ::SetPredMap<PredMap>
    ::SetDistMap<DistMap>
    ::Create dijkstra(g, length);
  dijkstra.predMap(predMap);
  dijkstra.distMap(dist);
  dijkstra.run(s);
  timer.stop();

#ifndef PLOT_OUTPUT
  std::cout << "LEMON->B2L copy: ";
  std::cout << timer_copy << '\n';
  std::cout << "B2L Dijkstra:    ";
  std::cout << timer << '\n';
  std::cout << "Distance:        " << dist[t] << '\n';
#else
  if (dist[t] < 0) std::cout << "\tError" << std::flush;
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
  std::cout << countNodes(*gr) << std::flush;
#endif

  delete lower;
  delete upper;
  delete sup;

  boostToLemonBenchmark(gr, cost, s, t);
#ifdef PLOT_OUTPUT
  std::cout << '\t';
#endif
  return 0;
}
