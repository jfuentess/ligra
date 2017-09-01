#ifndef GRAPH_H
#define GRAPH_H
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "vertex.h"
#include "compressedVertex.h"
#include "parallel.h"
using namespace std;

// **************************************************************
//    ADJACENCY ARRAY REPRESENTATION
// **************************************************************

// Class that handles implementation specific freeing of memory 
// owned by the graph 
struct Deletable {
public:
  virtual void del() = 0;
};

template <class vertex>
struct graph {
  vertex *V;
  long n;
  long m;
  bool transposed;
  uintE* flags;
  Deletable *D;
graph(vertex* VV, long nn, long mm, Deletable* DD) : V(VV), n(nn), m(mm), D(DD), flags(NULL), transposed(0) {}
graph(vertex* VV, long nn, long mm, Deletable* DD, uintE* _flags) : V(VV), n(nn), m(mm), D(DD), flags(_flags), transposed(0) {}

  void del() {
    D->del();
    free(D);
  }

  void transpose() {
    if ((sizeof(vertex) == sizeof(asymmetricVertex)) || 
        (sizeof(vertex) == sizeof(compressedAsymmetricVertex))) {
      parallel_for(long i=0;i<n;i++) {
        V[i].flipEdges();
      }
      transposed = !transposed;
    }
  }
};

// **************************************************************
//  EXTENDED ADJACENCY ARRAY REPRESENTATION
// **************************************************************

template <class intE>
struct vertexExt {
  intE neighbors;
  void del() {free(neighbors);}
  vertexExt(intE N) : neighbors(N) {}
};

template <class intE>
struct edgeExt {
  intE u;
  intE v;
  intE cmp;
  edgeExt(intE f, intE s) : u(f), v(s), cmp(0) {}
  edgeExt(intE f, intE s, intE c) : u(f), v(s), cmp(c) {}
};

template <class intE>
struct graphExt {
  vertexExt<intE> *V;
  edgeExt<intE> *E;
  long n;
  long m;
  graphExt(vertexExt<intE>* VV, edgeExt<intE>* EE, long nn, long mm) 
    : V(VV), E(EE), n(nn), m(mm) {}
  graphExt(vertexExt<intE>* VV, long nn, long mm) 
    : V(VV), E(NULL), n(nn), m(mm) {}
  vertexExt<intE>* vertices() { return V; }
  edgeExt<intE> * edges() { return E; }
  graphExt copy() {
    vertexExt<intE>* VN = newA(vertexExt<intE>,n);
    edgeExt<intE>* EN = newA(edgeExt<intE>,m);
    for (long i=0; i < n; i++)
      VN[i].neighbors = V[i].neighbors;
    for (long i=0; i < m; i++) {
      EN[i].u = E[i].u;
      EN[i].v = E[i].v;
      EN[i].cmp = E[i].cmp;
    }
    return graphExt(VN, EN, n, m);
  }
  void del() {
    free(V);
    free(E);
  }
};

#endif
