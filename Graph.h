#ifndef GRAPH_H
#define GRAPH_H
#include <vector>
#include <queue>
#include <functional>
#include <climits>

#include "Node.h"
using namespace std;
class Graph
{

public:
  std::vector<Node *> _vertices;

  Graph(){

  }
  // Allocates memory and adds vertex
  void addVertex(Node vertex)
  {
    if (vertex.getId() >= _vertices.size())
      {
	// Allocate more memory
	for (int i = _vertices.size(); i <= vertex.getId(); i++)
	  _vertices.push_back(nullptr);
      }
    if (_vertices[vertex.getId()] == nullptr) // not added
      _vertices[vertex.getId()] = new Node(vertex);
  }

  bool containsVertex(Node vert)
  {
    return (_vertices.size() > vert.getId() && *_vertices[vert.getId()] == vert) ? true : false;
  }

  bool containsVertex(int id)
  {
    return (_vertices.size() > id && _vertices[id] != nullptr) ? true : false;
  }

  Node & getVertex(int id)
    {
      return *_vertices[id];
    }

};

#endif
