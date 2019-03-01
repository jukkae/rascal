#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <set>

template<class T>
struct GraphNode {
public:
  int id;
  T value;
  std::vector<int> neighbours;
private:
  friend class boost::serialization::access;
  template<typename Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & id;
    ar & value;
    ar & neighbours;
  }
};

template<typename T>
using Graph = std::vector<GraphNode<T>>;
// template<class T>
// struct Graph {
// public:
//   std::vector<GraphNode<T>> nodes;
// private:
//   friend class boost::serialization::access;
//   template<typename Archive>
//   void serialize(Archive & ar, const unsigned int version) {
//     ar & nodes;
//   }
// };

template<typename T>
std::set<std::pair<int, int>> getEdges(Graph<T> graph) {
  std::set<std::pair<int, int>> ret {};
  for(auto& n : graph) {
    for(auto& nb : n.neighbours) {
      int a = n.id;
      int b = nb;
      if(a > b) std::swap(a, b);
      ret.insert({a, b});
    }
  }
  return ret;
}

#endif /* GRAPH_HPP */
