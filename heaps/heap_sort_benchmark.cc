#include <iostream>
#include <string>
#include <vector>

#include <lemon/random.h>
#include <lemon/maps.h>
#include <lemon/time_measure.h>

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

const int SIZE = 100000;
const int FACTOR = 10;
vector<int> test_seq;

double base_time;

template <typename Heap>
void heapSortTest(const string& st) {
  int size = SIZE;
  RangeMap<int> map(size, -1);
  Heap heap(map);

  vector<int> v(size);
  for (int i = 0; i < size; ++i) {
    v[i] = test_seq[i];
  }
  sort(v.begin(), v.end());
  
  Timer T;
  for (int i = 0; i < size; ++i) {
    heap.push(i, v[i]);
  }
  for (int i = 0; i < size; ++i) {
    check(v[i] == heap.prio(), "Wrong order in heap sort.");
    heap.pop();
  }
  T.stop();

  cout << st << "   " << T.realTime();
  if (base_time == 0) base_time = T.realTime();
  cout << "  \t" << T.realTime() / base_time << endl;
}

int main() {
  for (int i = 0; i < SIZE; ++i) {
    test_seq.push_back(FACTOR * i);
  }
  for (int i = 0; i < SIZE; ++i) {
    int j = i + rnd[SIZE - i];
    int x = test_seq[i];
    test_seq[i] = test_seq[j];
    test_seq[j] = x;
  }
  base_time = 0.0;

  typedef RangeMap<int> ItemIntMap;
  
  typedef BinHeap<int, ItemIntMap> IntBinHeap;
  typedef QuadHeap<int, ItemIntMap> IntQuadHeap;
  typedef DHeap<int, ItemIntMap, 4> Int4Heap;
  typedef DHeap<int, ItemIntMap, 8> Int8Heap;
  typedef DHeap<int, ItemIntMap, 16> Int16Heap;
  typedef DHeap<int, ItemIntMap, 32> Int32Heap;
  typedef DHeap<int, ItemIntMap, 64> Int64Heap;
  typedef DHeap<int, ItemIntMap, 128> Int128Heap;
  typedef FibHeap<int, ItemIntMap> IntFibHeap;
  typedef PairingHeap<int, ItemIntMap> IntPairingHeap;
  typedef RadixHeap<ItemIntMap> IntRadixHeap;
  typedef BinomialHeap<int, ItemIntMap> IntBinomialHeap;
  typedef BucketHeap<ItemIntMap> IntBucketHeap;
  typedef SimpleBucketHeap<ItemIntMap> IntSBucketHeap;
  
  std::cout << "Heap sort benchmark test, N = " << SIZE << endl << endl;

  heapSortTest<IntBinHeap>     ("BinHeap     ");
  heapSortTest<IntQuadHeap>    ("QuadHeap    ");
  heapSortTest<Int4Heap>       ("DHeap, D=4  ");
  heapSortTest<Int8Heap>       ("DHeap, D=8  ");
  heapSortTest<Int16Heap>      ("DHeap, D=16 ");
  heapSortTest<Int32Heap>      ("DHeap, D=32 ");
  heapSortTest<Int64Heap>      ("DHeap, D=64 ");
  heapSortTest<Int128Heap>     ("DHeap, D=128");
  heapSortTest<IntFibHeap>     ("FibHeap     ");
  heapSortTest<IntPairingHeap> ("PairingHeap ");
  heapSortTest<IntRadixHeap>   ("RadixHeap   ");
  heapSortTest<IntBinomialHeap>("BinomialHeap");
  heapSortTest<IntBucketHeap>  ("BucketHeap  ");
  heapSortTest<IntSBucketHeap> ("SBucketHeap ");

  return 0;
}
