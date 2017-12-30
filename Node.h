#ifndef NODE_H
#define NODE_H
#include <iostream>
#include <vector>
#include <unordered_map>
#include <omp.h>

class Node
{
 public:
  std::unordered_map<int, Node*> edges;
  Node (){
    id=-1;
  }
  Node(Node & node){
    this->id = node.id;
    m_outgoing_count = 0;
    m_node_rnk = 0;
    omp_init_lock(&lock);
  }

  int getId() const{
    return id;
  }
  void addEdge(Node * node){
    edges[node->getId()]=node;
    this->m_outgoing_count++;
  }
  Node *& getEdge(int id){
    return edges[id];
  }
  std::unordered_map<int, Node*> & getEdges(){
    return edges;
  }
  Node *& getNthEdge(int id){
    for (auto edge : edges){
      
      if (id == 0)
	return edge.second;
      id--;
    }
  }
  void incrementRank(double value){
    omp_set_lock(&lock);
    // #pragma omp atomic
    m_node_rnk += value;
    omp_unset_lock(&lock);
  }
  unsigned int getRank() const{
    return m_node_rnk;
  }
  unsigned int m_outgoing_count;
  long double m_node_rnk;
  omp_lock_t lock;
  int id;
};


template <typename A, typename B>
class ThreadMap{
 public:
  std::unordered_map<A, B> map;
  ThreadMap(){
    omp_init_lock(&lock);
  }
  void addPair(A key, B value){
    if (map.find(key) != map.end())
      return;
    omp_set_lock(&lock);
    map[key] = value;
    omp_unset_lock(&lock);
  }
 private:
  omp_lock_t lock;
};
bool operator==(Node const & lhs, Node const & rhs){
  return (lhs.getId() == rhs.getId()) ? true : false;
}
#endif
