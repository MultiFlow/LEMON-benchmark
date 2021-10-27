#!/bin/bash

seed=58037216
if [ $# -ge 1 ]
then
  seed=$1
fi

netgen_path="../../generator/netgen/netgen"

# NETGEN params:        #        n   src  trg         m  costs      supply             caps
netgen_params_lg[1]="   1     1000    32   32      9966  1 10000    320000  0 0 0 100  1 10000"
netgen_params_lg[2]="   2     2000    45   45     21932  1 10000    450000  0 0 0 100  1 10000"
netgen_params_lg[3]="   3     5000    71   71     61439  1 10000    640000  0 0 0 100  1 10000"
netgen_params_lg[4]="   4    10000   100  100    132877  1 10000    910000  0 0 0 100  1 10000"
netgen_params_lg[5]="   5    20000   141  141    285754  1 10000   1280000  0 0 0 100  1 10000"
netgen_params_lg[6]="   6    50000   224  224    780482  1 10000   1810000  0 0 0 100  1 10000"
netgen_params_lg[7]="   7   100000   316  316   1660964  1 10000   2560000  0 0 0 100  1 10000"
netgen_params_lg[8]="   8   200000   447  447   3521928  1 10000   3620000  0 0 0 100  1 10000"
netgen_params_lg[9]="   9   500000   707  707   9465784  1 10000   5120000  0 0 0 100  1 10000"
netgen_params_lg[10]=" 10  1000000  1000 1000  19931569  1 10000   7240000  0 0 0 100  1 10000"

# NETGEN params:        #        n   src  trg         m  costs      supply             caps
netgen_params_sq[1]="   1     1000    32   32     31623  1 10000    320000  0 0 0 100  1 10000"
netgen_params_sq[2]="   2     2000    45   45     89443  1 10000    450000  0 0 0 100  1 10000"
netgen_params_sq[3]="   3     5000    71   71    353553  1 10000    640000  0 0 0 100  1 10000"
netgen_params_sq[4]="   4    10000   100  100   1000000  1 10000    910000  0 0 0 100  1 10000"
netgen_params_sq[5]="   5    20000   141  141   2828427  1 10000   1280000  0 0 0 100  1 10000"
netgen_params_sq[6]="   6    50000   224  224  11180340  1 10000   1810000  0 0 0 100  1 10000"
netgen_params_sq[7]="   7   100000   316  316  31622777  1 10000   2560000  0 0 0 100  1 10000"

# NETGEN params:        #        n   src  trg         m  costs      supply             caps
#netgen_params_lg[1]="   1     1024    32   32     10240  1 10000    320000  0 0 0 100  1 10000"
#netgen_params_lg[2]="   2     2048    45   45     22528  1 10000    450000  0 0 0 100  1 10000"
#netgen_params_lg[3]="   3     4096    64   64     49152  1 10000    640000  0 0 0 100  1 10000"
#netgen_params_lg[4]="   4     8192    91   91    106496  1 10000    910000  0 0 0 100  1 10000"
#netgen_params_lg[5]="   5    16384   128  128    229376  1 10000   1280000  0 0 0 100  1 10000"
#netgen_params_lg[6]="   6    32768   181  181    491520  1 10000   1810000  0 0 0 100  1 10000"
#netgen_params_lg[7]="   7    65536   256  256   1048576  1 10000   2560000  0 0 0 100  1 10000"
#netgen_params_lg[8]="   8   131072   362  362   2228224  1 10000   3620000  0 0 0 100  1 10000"
#netgen_params_lg[9]="   9   262144   512  512   4718592  1 10000   5120000  0 0 0 100  1 10000"
#netgen_params_lg[10]=" 10   524288   724  724   9961472  1 10000   7240000  0 0 0 100  1 10000"
#netgen_params_lg[11]=" 11  1048576  1024 1024  20971520  1 10000  10240000  0 0 0 100  1 10000"

# NETGEN params:        #        n   src  trg         m  costs      supply             caps
#netgen_params_sq[1]="   1     1024    32   32     32768  1 10000    320000  0 0 0 100  1 10000"
#netgen_params_sq[2]="   2     2048    45   45     92682  1 10000    450000  0 0 0 100  1 10000"
#netgen_params_sq[3]="   3     4096    64   64    262144  1 10000    640000  0 0 0 100  1 10000"
#netgen_params_sq[4]="   4     8192    91   91    741455  1 10000    910000  0 0 0 100  1 10000"
#netgen_params_sq[5]="   5    16384   128  128   2097152  1 10000   1280000  0 0 0 100  1 10000"
#netgen_params_sq[6]="   6    32768   181  181   5931642  1 10000   1810000  0 0 0 100  1 10000"
#netgen_params_sq[7]="   7    65536   256  256  16777216  1 10000   2560000  0 0 0 100  1 10000"

for i in `seq 1 10`
do
  echo $seed${netgen_params_lg[$i]} | $netgen_path > netgen_lg_$i.net
done
for i in `seq 1 7`
do
  echo $seed${netgen_params_sq[$i]} | $netgen_path > netgen_sq_$i.net
done
