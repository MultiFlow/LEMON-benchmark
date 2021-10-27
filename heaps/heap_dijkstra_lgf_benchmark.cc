#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <lemon/smart_graph.h>
#include <lemon/lgf_reader.h>
#include <lemon/time_measure.h>
#include <lemon/dijkstra.h>
#include <lemon/maps.h>

#include <lemon/bin_heap.h>
#include <lemon/quad_heap.h>
#include <lemon/dheap.h>
#include <lemon/fib_heap.h>
#include <lemon/pairing_heap.h>
#include <lemon/radix_heap.h>
#include <lemon/binomial_heap.h>
#include <lemon/bucket_heap.h>

#include <test/test_tools.h>

using namespace lemon;
using namespace std;

typedef SmartDigraph GR;
DIGRAPH_TYPEDEFS(GR);

typedef GR::ArcMap<int> LengthMap;
typedef GR::NodeMap<int> DistMap;
typedef NullMap<Node, Arc> PredMap;
typedef GR::NodeMap<int> IntNodeMap;

GR gr;
LengthMap length(gr);
Node source;
PredMap pred;

bool first;
DistMap dist_test(gr);
double base_time;

template <typename Heap>
void dijkstraTest(const string& st) {
  DistMap dist(gr);

  Timer T;
  typename Dijkstra<GR, LengthMap>
    ::SetDistMap<DistMap>
    ::SetPredMap<PredMap>
    ::template SetStandardHeap<Heap, IntNodeMap>
    ::Create dijkstra(gr, length);
  dijkstra.distMap(dist).predMap(pred);
  dijkstra.run(source);
  T.stop();
  cout << st << "   " << T.realTime();
  if (first) base_time = T.realTime();
  cout << "  \t" << T.realTime() / base_time << endl;

  if (first) {
    for (NodeIt n(gr); n != INVALID; ++n)
      dist_test[n] = dist[n];
    first = false;
  } else {
    for (NodeIt n(gr); n != INVALID; ++n)
      check(dist_test[n] == dist[n], "Wrong dist.");
  }
}

int main(int argc, const char *argv[]) {
  typedef BinHeap<int, IntNodeMap> IntBinHeap;
  typedef QuadHeap<int, IntNodeMap> IntQuadHeap;
  typedef DHeap<int, IntNodeMap, 4> Int4Heap;
  typedef DHeap<int, IntNodeMap, 8> Int8Heap;
  typedef DHeap<int, IntNodeMap, 16> Int16Heap;
  typedef DHeap<int, IntNodeMap, 32> Int32Heap;
  typedef DHeap<int, IntNodeMap, 64> Int64Heap;
  typedef DHeap<int, IntNodeMap, 128> Int128Heap;
  typedef FibHeap<int, IntNodeMap> IntFibHeap;
  typedef PairingHeap<int, IntNodeMap> IntPairingHeap;
  typedef RadixHeap<IntNodeMap> IntRadixHeap;
  typedef BinomialHeap<int, IntNodeMap> IntBinomialHeap;
  typedef BucketHeap<IntNodeMap> IntBucketHeap;
  
  if (argc != 2) {
    std::cerr << "A filename must be given.";
    return -1;
  }
  
  cout << "Reading '" << argv[1] << "'..." << endl;
  ifstream inp(argv[1]);
  digraphReader(gr, inp).
    arcMap("capacity", length).
    node("source", source).
    run();
  first = true;

  std::cout << "Dijkstra benchmark test, n = " << 
    countNodes(gr) << ", m = " << countArcs(gr) << endl << endl;

  dijkstraTest<IntBinHeap>     ("BinHeap     ");
  dijkstraTest<IntQuadHeap>    ("QuadHeap    ");
  dijkstraTest<Int4Heap>       ("DHeap, D=4  ");
  dijkstraTest<Int8Heap>       ("DHeap, D=8  ");
  dijkstraTest<Int16Heap>      ("DHeap, D=16 ");
  dijkstraTest<Int32Heap>      ("DHeap, D=32 ");
  dijkstraTest<Int64Heap>      ("DHeap, D=64 ");
  dijkstraTest<Int128Heap>     ("DHeap, D=128");
  dijkstraTest<IntFibHeap>     ("FibHeap     ");
  dijkstraTest<IntPairingHeap> ("PairingHeap ");
  dijkstraTest<IntRadixHeap>   ("RadixHeap   ");
  dijkstraTest<IntBinomialHeap>("BinomialHeap");
  dijkstraTest<IntBucketHeap>  ("BucketHeap  ");

  return 0;
}
