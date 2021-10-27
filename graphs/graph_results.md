# 2021 - multiflow/dev - Release (gcc 10.3, -O3 flag)

Benchmark tests on a random digraph (n = 100000, m = 1000000)

## Building
| Graph type 	| time 	|
|-----------------|-----------|
| ListDigraph 	|0.0461838 	|
| ListDigraph (sorted arcs) 	|0.0355282	|
| SmartDigraph 	 |0.015209	|
| SmartDigraph (sorted arcs) 	|0.012264	|
| StaticDigraph 	|0.00368094	|

## Iteration (per graph type)
### ListDigraph
|               | Run 1       | Run 2       |
|---------------|-------------|-------------|
|   NodeIt      |0.000193644 | 0.000159168 |
|  ArcIt        |0.0646998 | 0.0632344 |
|  OutArcIt     |0.0471344 | 0.0495196 |
|  InArcIt      |0.0504632 | 0.049377 |
|  Out-InArcIt  |0.0876054 | 0.0881784 |

### ListDigraph (sorted arcs)
|               | Run 1       | Run 2       |
|---------------|-------------|-------------|
|   NodeIt      |0.000180197 | 0.000158978 |
|  ArcIt        |0.00385561 | 0.00397639 |
|  OutArcIt     |0.0038754 | 0.0037044 |
|  InArcIt      |0.0436422 | 0.0436286 |
|  Out-InArcIt  |0.0519424 | 0.0528604 |

### SmartDigraph
|               | Run 1       | Run 2       |
|---------------|-------------|-------------|
|   NodeIt      |0.000100994 | 9.77993e-05 |
|  ArcIt        |0.00153198 | 0.00161138 |
|  OutArcIt     |0.0418262 | 0.0395592 |
|  InArcIt      |0.0399434 | 0.0435232 |
|  Out-InArcIt  |0.078632 | 0.077047 |

### SmartDigraph (sorted arcs)
|               | Run 1       | Run 2       |
|---------------|-------------|-------------|
|   NodeIt      |0.00010519 | 9.56059e-05 |
|  ArcIt        |0.0015018 | 0.00168719 |
|  OutArcIt     |0.00330977 | 0.00324702 |
|  InArcIt      |0.0372546 | 0.0404882 |
|  Out-InArcIt  |0.0465688 | 0.0469232 |

### StaticDigraph
|               | Run 1       | Run 2       |
|---------------|-------------|-------------|
|   NodeIt      |0.000105238 | 9.53674e-05 |
|  ArcIt        |0.00136919 | 0.00152121 |
|  OutArcIt     |0.00140719 | 0.00258899 |
|  InArcIt      |0.0219664 | 0.020944 |
|  Out-InArcIt  |0.0274742 | 0.0311054 |


# 2010 

Benchmark tests on a random digraph (n = 100000, m = 1000000)

ListDigraph - Building                  1.45626
ListDigraph (sorted arcs) - Building    1.45136
SmartDigraph - Building                 0.996035
SmartDigraph (sorted arcs) - Building   0.967469
StaticDigraph - Building                1.39344

ListDigraph - Iteration
  NodeIt:    0.0072099          0.0067126
  ArcIt:     0.198539           0.198938
  OutArcIt:  0.155848           0.150801
  InArcIt:   0.154048           0.159836

ListDigraph (sorted arcs) - Iteration
  NodeIt:    0.00720448         0.00670629
  ArcIt:     0.194381           0.202951
  OutArcIt:  0.0869989          0.0806695
  InArcIt:   0.149996           0.146811

SmartDigraph - Iteration
  NodeIt:    0.00258529         0.00226789
  ArcIt:     0.0253954          0.0226732
  OutArcIt:  0.147429           0.149954
  InArcIt:   0.147432           0.14323

SmartDigraph (sorted arcs) - Iteration
  NodeIt:    0.00393109         0.00226779
  ArcIt:     0.0253948          0.0226794
  OutArcIt:  0.0962266          0.0759268
  InArcIt:   0.143696           0.14388

StaticDigraph - Iteration
  NodeIt:    0.00262511         0.00230808
  ArcIt:     0.0257938          0.0230783
  OutArcIt:  0.0338658          0.0437127
  InArcIt:   0.0912826          0.085961
