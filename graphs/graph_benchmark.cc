#include <iostream>
#include <fstream>
#include <cstdlib>

#include <lemon/list_graph.h>
#include <lemon/smart_graph.h>
#include <lemon/static_graph.h>
#include <lemon/dimacs.h>
#include <lemon/adaptors.h>
#include <lemon/random.h>
#include <lemon/time_measure.h>

using namespace std;
using namespace lemon;

const int K = 5;
const int P = 691;

#define LIST1
#define LIST2
#define SMART1
#define SMART2
#define STATIC

template <typename Graph>
class NodeIterator1 {
  const Graph &_gr;
  int _code;
public:
  NodeIterator1(const Graph &gr) : _gr(gr), _code(0) {}
  void operator()() {
    for (typename Graph::NodeIt n(_gr); n != INVALID; ++n)
      _code = _code * P ^ _gr.id(n);
  }
  int result() { return _code; }
};

template <typename Graph>
class NodeIterator2 {
  const Graph &_gr;
  int _code;
public:
  NodeIterator2(const Graph &gr) : _gr(gr), _code(0) {}
  void operator()() {
    typename Graph::Node n;
    for (_gr.first(n); n != INVALID; _gr.next(n))
      _code = _code * P ^ _gr.id(n);
  }
  int result() { return _code; }
};

template <typename Graph>
class ArcIterator1 {
  const Graph &_gr;
  int _code;
public:
  ArcIterator1(const Graph &gr) : _gr(gr), _code(0) {}
  void operator()() {
    for (typename Graph::ArcIt e(_gr); e != INVALID; ++e)
      _code = _code * P ^ _gr.id(e) ^
              _gr.id(_gr.source(e)) ^ _gr.id(_gr.target(e));
  }
  int result() { return _code; }
};

template <typename Graph>
class ArcIterator2 {
  const Graph &_gr;
  int _code;
public:
  ArcIterator2(const Graph &gr) : _gr(gr), _code(0) {}
  void operator()() {
    typename Graph::Arc e;
    for (_gr.first(e); e != INVALID; _gr.next(e))
      _code = _code * P ^ _gr.id(e) ^
              _gr.id(_gr.source(e)) ^ _gr.id(_gr.target(e));
  }
  int result() { return _code; }
};

template <typename Graph>
class OutArcIterator1 {
  const Graph &_gr;
  int _code;
public:
  OutArcIterator1(const Graph &gr) : _gr(gr), _code(0) {}
  void operator()() {
    for (typename Graph::NodeIt n(_gr); n != INVALID; ++n) {
      _code = _code * P + _gr.id(n);
      for (typename Graph::OutArcIt e(_gr, n); e != INVALID; ++e)
        _code = _code * P ^ _gr.id(e) ^ _gr.id(_gr.target(e));
    }
  }
  int result() { return _code; }
};

template <typename Graph>
class OutArcIterator2 {
  const Graph &_gr;
  int _code;
public:
  OutArcIterator2(const Graph &gr) : _gr(gr), _code(0) {}
  void operator()() {
    typename Graph::Node n;
    for (_gr.first(n); n != INVALID; _gr.next(n)) {
      _code = _code * P + _gr.id(n);
      typename Graph::Arc e;
      for (_gr.firstOut(e, n); e != INVALID; _gr.nextOut(e))
        _code = _code * P ^ _gr.id(e) ^ _gr.id(_gr.target(e));
    }
  }
  int result() { return _code; }
};

template <typename Graph>
class InArcIterator1 {
  const Graph &_gr;
  int _code;
public:
  InArcIterator1(const Graph &gr) : _gr(gr), _code(0) {}
  void operator()() {
    for (typename Graph::NodeIt n(_gr); n != INVALID; ++n) {
      _code = _code * P + _gr.id(n);
      for (typename Graph::InArcIt e(_gr, n); e != INVALID; ++e)
        _code = _code * P ^ _gr.id(e) ^ _gr.id(_gr.source(e));
    }
  }
  int result() { return _code; }
};

template <typename Graph>
class InArcIterator2 {
  const Graph &_gr;
  int _code;
public:
  InArcIterator2(const Graph &gr) : _gr(gr), _code(0) {}
  void operator()() {
    typename Graph::Node n;
    for (_gr.first(n); n != INVALID; _gr.next(n)) {
      _code = _code * P + _gr.id(n);
      typename Graph::Arc e;
      for (_gr.firstIn(e, n); e != INVALID; _gr.nextIn(e))
        _code = _code * P ^ _gr.id(e) ^ _gr.id(_gr.source(e));
    }
  }
  int result() { return _code; }
};

template <typename Graph>
class OutInArcIterator1 {
  const Graph &_gr;
  int _code;
public:
  OutInArcIterator1(const Graph &gr) : _gr(gr), _code(0) {}
  void operator()() {
    for (typename Graph::NodeIt n(_gr); n != INVALID; ++n) {
      _code = _code * P + _gr.id(n);
      for (typename Graph::OutArcIt e(_gr, n); e != INVALID; ++e)
        _code = _code * P ^ _gr.id(e) ^ _gr.id(_gr.target(e));
      for (typename Graph::InArcIt e(_gr, n); e != INVALID; ++e)
        _code = _code * P ^ _gr.id(e) ^ _gr.id(_gr.source(e));
    }
  }
  int result() { return _code; }
};

template <typename Graph>
class OutInArcIterator2 {
  const Graph &_gr;
  int _code;
public:
  OutInArcIterator2(const Graph &gr) : _gr(gr), _code(0) {}
  void operator()() {
    typename Graph::Node n;
    for (_gr.first(n); n != INVALID; _gr.next(n)) {
      typename Graph::Arc e;
      _code = _code * P + _gr.id(n);
      for (_gr.firstOut(e, n); e != INVALID; _gr.nextOut(e))
        _code = _code * P ^ _gr.id(e) ^ _gr.id(_gr.target(e));
      for (_gr.firstIn(e, n); e != INVALID; _gr.nextIn(e))
        _code = _code * P ^ _gr.id(e) ^ _gr.id(_gr.source(e));
    }
  }
  int result() { return _code; }
};

template <typename GR>
void benchmarkGraph(const GR &gr, int n, int m) {
  Timer tni1, tni2, tai1, tai2, toai1, toai2, tiai1, tiai2, toiai1, toiai2;
  NodeIterator1<GR> ni1(gr);
  NodeIterator2<GR> ni2(gr);
  ArcIterator1<GR> ai1(gr);
  ArcIterator2<GR> ai2(gr);
  OutArcIterator1<GR> oai1(gr);
  OutArcIterator2<GR> oai2(gr);
  InArcIterator1<GR> iai1(gr);
  InArcIterator2<GR> iai2(gr);
  OutInArcIterator1<GR> oiai1(gr);
  OutInArcIterator2<GR> oiai2(gr);
  
  tni1.restart();
  for (int i=0; i!=K; ++i) ni1();
  tni1.stop();
  tni2.restart();
  for (int i=0; i!=K; ++i) ni2();
  tni2.stop();
  tai1.restart();
  for (int i=0; i!=K; ++i) ai1();
  tai1.stop();
  tai2.restart();
  for (int i=0; i!=K; ++i) ai2();
  tai2.stop();
  toai1.restart();
  for (int i=0; i!=K; ++i) oai1();
  toai1.stop();
  toai2.restart();
  for (int i=0; i!=K; ++i) oai2();
  toai2.stop();
  tiai1.restart();
  for (int i=0; i!=K; ++i) iai1();
  tiai1.stop();
  tiai2.restart();
  for (int i=0; i!=K; ++i) iai2();
  tiai2.stop();
  toiai1.restart();
  for (int i=0; i!=K; ++i) oiai1();
  toiai1.stop();
  toiai2.restart();
  for (int i=0; i!=K; ++i) oiai2();
  toiai2.stop();

  cout << "  NodeIt:      " << tni1.realTime() / K
       << "  \t"            << tni2.realTime() / K << endl;
  cout << "  ArcIt:       " << tai1.realTime() / K
       << "  \t"            << tai2.realTime() / K << endl;
  cout << "  OutArcIt:    " << toai1.realTime() / K
       << "  \t"            << toai2.realTime() / K << endl;
  cout << "  InArcIt:     " << tiai1.realTime() / K
       << "  \t"            << tiai2.realTime() / K << endl;
  cout << "  Out-InArcIt: " << toiai1.realTime() / K
       << "  \t"            << toiai2.realTime() / K << endl;
  cout << endl;
  
  if (ni1.result() != ni2.result()) exit(-1);
  if (ai1.result() != ai2.result()) exit(-1);
  if (oai1.result() != oai2.result()) exit(-1);
  if (iai1.result() != iai2.result()) exit(-1);
  if (oiai1.result() != oiai2.result()) exit(-1);
}

int main(int argc, char *argv[])
{
  if (argc < 3) {
    cout << "Usage: " << argv[0] << " n m" << endl;
    return -1;
  }
  
  int n = atoi(argv[1]);
  int m = atoi(argv[2]);
  
  cout << "Benchmark tests on a random digraph (n = " << n
       << ", m = " << m << ")" << endl << endl;
  
  vector<pair<int, int> > arcs1;
  for (int j = 0; j != m; ++j) {
    arcs1.push_back(make_pair(rnd[n], rnd[n]));
  }
  vector<pair<int, int> > arcs2(arcs1);
  sort(arcs2.begin(), arcs2.end());

  // Building graphs
#ifdef LIST1 
  cout << "ListDigraph - Building                  ";
  Timer tlgr1;
  ListDigraph lgr1;
  vector<ListDigraph::Node> lnodes1;
  for (int i = 0; i != n; ++i) {
    lnodes1.push_back(lgr1.addNode());
  }  
  for (int j = 0; j != m; ++j) {
    lgr1.addArc(lnodes1[arcs1[j].first], lnodes1[arcs1[j].second]);
  }
  tlgr1.stop();
  cout << tlgr1.realTime() << endl;
#endif

#ifdef LIST2 
  cout << "ListDigraph (sorted arcs) - Building    ";
  Timer tlgr2;
  ListDigraph lgr2;
  vector<ListDigraph::Node> lnodes2;
  for (int i = 0; i != n; ++i) {
    lnodes2.push_back(lgr2.addNode());
  }  
  for (int j = 0; j != m; ++j) {
    lgr2.addArc(lnodes2[arcs2[j].first], lnodes2[arcs2[j].second]);
  }
  tlgr2.stop();
  cout << tlgr2.realTime() << endl;
#endif

#ifdef SMART1 
  cout << "SmartDigraph - Building                 ";
  Timer tsgr1;
  SmartDigraph sgr1;
  vector<SmartDigraph::Node> snodes1;
  for (int i = 0; i != n; ++i) {
    snodes1.push_back(sgr1.addNode());
  }  
  for (int j = 0; j != m; ++j) {
    sgr1.addArc(snodes1[arcs1[j].first], snodes1[arcs1[j].second]);
  }
  tsgr1.stop();
  cout << tsgr1.realTime() << endl;
#endif

#ifdef SMART2 
  cout << "SmartDigraph (sorted arcs) - Building   ";
  Timer tsgr2;
  SmartDigraph sgr2;
  vector<SmartDigraph::Node> snodes2;
  for (int i = 0; i != n; ++i) {
    snodes2.push_back(sgr2.addNode());
  }  
  for (int j = 0; j != m; ++j) {
    sgr2.addArc(snodes2[arcs2[j].first], snodes2[arcs2[j].second]);
  }
  tsgr2.stop();
  cout << tsgr2.realTime() << endl;
#endif

#ifdef STATIC
  cout << "StaticDigraph - Building                ";
  Timer tstgr;
  StaticDigraph stgr;
  stgr.build(n, arcs2.begin(), arcs2.end());
  tstgr.stop();
  cout << tstgr.realTime() << endl;
#endif
  
  cout << endl;
  
  // Iteration test
#ifdef LIST1 
  cout << "ListDigraph - Iteration" << endl;
  benchmarkGraph(lgr1, n, m);
#endif

#ifdef LIST2 
  cout << "ListDigraph (sorted arcs) - Iteration" << endl;
  benchmarkGraph(lgr2, n, m);
#endif

#ifdef SMART1 
  cout << "SmartDigraph - Iteration" << endl;
  benchmarkGraph(sgr1, n, m);
#endif

#ifdef SMART2 
  cout << "SmartDigraph (sorted arcs) - Iteration" << endl;
  benchmarkGraph(sgr2, n, m);
#endif

#ifdef STATIC
  cout << "StaticDigraph - Iteration" << endl;
  benchmarkGraph(stgr, n, m);
#endif
  
  return 0;
}
