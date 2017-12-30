#include <iostream>
#include "appObj.h"

int main(int argc, char * argv[])
{
  if (argc < 5){
    cout << "usage: ./pagerank <graph_name> <num_threads> <num_walks> <1/(Damping Ratio)>\n";
    return -1;
  }
  int jumpProbability = stoi(argv[4]);
  int threads = stoi(argv[2]);
  int numWalks = stoi(argv[3]);
  PageRank go(argv[1], threads, numWalks, jumpProbability);
  return 0;
}
