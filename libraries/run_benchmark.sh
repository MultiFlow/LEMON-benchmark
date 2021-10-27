#!/bin/bash

if [ $# -lt 1 -o $# -gt 3 -o "$1" == '--help' -o "$1" == '-help' -o "$1" == '-h' ]
then
  echo "Usage:"
  echo "  $0 alg seq maxid seed"
  echo "Where:"
  echo "  alg"
  echo "    Algorithm to benchmark. It can be 'dijk' for Dijkstra, 'max' for"
  echo "    max flow algorithms (e.g. Preflow) and 'mcf' for min cost flow"
  echo "    algorithms (e.g. NetworkSimplex)."
  echo "  seq"
  echo "    The problem sequence type. It can be 'lg' for networks in which"
  echo "    m=n*log_2(n) and 'sq' for networks in which m=n*sqrt(n)."
  echo "    The default is 'lg'."
  echo "  maxid"
  echo "    A number between 1 and 10. The instances with ids between 1 and"
  echo "    maxid will be generated using netgen and the proper benchmark"
  echo "    program will be executed for them. The default is 10."
  exit -1
fi

if [ "$1" == 'mcf' ]
then
  program_name=mincostflow
elif [ "$1" == 'max' ]
then
  program_name=maxflow
else
  program_name=dijkstra
fi

max_id=10
if [ "$2" == 'sq' ]
then
  max_id=7
fi
if [ $# -ge 3 ] && [ $3 -ge 1 ] && [ $3 -le 13 ]
then
  max_id=$3
fi


if [ "$2" != 'sq' ]
then
  for i in `seq 1 $max_id`
  do
    ./${program_name}_lemon < netgen_lg_$i.net
    ./${program_name}_boost < netgen_lg_$i.net
    ./${program_name}_leda  < netgen_lg_$i.net
  done
else
  for i in `seq 1 $max_id`
  do
    ./${program_name}_lemon < netgen_sq_$i.net
    ./${program_name}_boost < netgen_sq_$i.net
    ./${program_name}_leda  < netgen_sq_$i.net
  done
fi
