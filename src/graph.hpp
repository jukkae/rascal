#ifndef GRAPH_HPP
#define GRAPH_HPP

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

#endif /* GRAPH_HPP */
