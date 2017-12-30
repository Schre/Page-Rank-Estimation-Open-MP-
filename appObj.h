#ifndef APP_OBJ
#define APP_OBJ

#include <fstream>
#include "Graph.h"
#include <string.h>
#include <string>
#include <time.h>
#include <omp.h>
#include <algorithm>

class PageRank
{
 public:
  PageRank(char * docName, int numThreads, int numWalks, int D){
    srand(time(NULL));
    jumpProbability = D;
    strncpy(documentName,docName,strlen(docName));
    documentName[strlen(docName)]=0;
    threadCount = numThreads;
    printf("Set num_threads to %d\n", numThreads);
    omp_set_num_threads(numThreads);
    RunPageRank(numWalks);
  }
 private:
  void RunPageRank(int numWalks){
    printf("Starting page rank with %d random walks, and Damping Ratio = 1/%d!\n", numWalks, jumpProbability);
    Web = new Graph();
    LoadGraph();
    cout << "Finished loading graph!\n";
    double start = omp_get_wtime();
    RandomlyWalk(numWalks); // random walk over G
  }
  
  void LoadGraph(){
    std::cout << "Opening graph file: "  << documentName << "\n";
    file.open(documentName);
    if (file.is_open()){
      cout << "File opened!\n";
      // load graph
      BuildGraph();
    }
    else{
      cout << "Failed to open file!\n";
      return;
    }
    file.close();
  }
  void BuildGraph(){
    cout << "Building web...\n";

    while (!file.eof()){
      int vID1, vID2;
      char line[64];
      file.getline(line,64);
      if (line[0] == '/') // ignore comments
	continue;
      charToInt2(vID1, vID2, line);
      auto pair = std::make_pair(vID1, vID2);
      Node v1,v2;
      v1.id = vID1;
      v2.id = vID2;
      
      Web->addVertex(v1);  // function only adds vertex IF does not already exist
      Web->addVertex(v2);
      Web->_vertices[vID1]->addEdge(&Web->getVertex(vID2));
    }
  }
  void charToInt2(int &id1, int &id2, char *line){
    int index = 0;
    char c = 0;
    char temp1[10] = {'\0'};
    char temp2[10] = {'\0'};
    unordered_map<Node *, bool> table;

    c = line[index];
    while (c != '\t'){
      temp1[index] = c;
      c = line[++index];
    }
    c = line[++index];
    int oldIndex = index;
    while (c != '\0'){
      temp2[index-oldIndex]=c;
      c = line[++index];
    }
    id1 = atoi(temp1);
    id2 = atoi(temp2);
  }

  void RandomlyWalk(int walks){
    int i=0;
    int j=0;
    Node *pNode = nullptr, * edgeNode = nullptr;
    int edgeId;
    unsigned int seed;
    unsigned int base = rand();
    double start = omp_get_wtime(); 
    unsigned long totalVisits=0;

#pragma omp parallel for schedule(static) private(i,j, edgeNode, pNode, edgeId,seed) reduction(+:totalVisits)
    for (int i = 0; i < Web->_vertices.size(); ++i){ // for each node
      seed += base+omp_get_thread_num()*(i+1);
      pNode = Web->_vertices[i];
      if (!pNode)
	continue;
      pNode->incrementRank(1); // Visited node! Update rank...
      totalVisits++;
      // jumpProbability -> damping factor
      for (int j = 0; j < walks; ++j){
	// Determine if random jump
	int jump = rand_r(&seed)%jumpProbability; // "Flip coin"
	if (jump == 0 || pNode->m_outgoing_count == 0){ // If 0 or no outgoing edges, jump
	   // random jump
	  pNode = nullptr;
	  while (!pNode){
	    ++seed;
	    pNode = Web->_vertices[rand_r(&seed)%Web->_vertices.size()]; // Jump random node!
	  }
	  totalVisits++;
	  pNode->incrementRank(1); // Visited random node! Update rank...
	  continue;
	}
	++seed;
	// Determine target edge
	edgeId = rand_r(&seed) % (pNode->m_outgoing_count);
	++seed;
     
	edgeNode = pNode->getNthEdge(edgeId);
	edgeNode->incrementRank(1); 
	totalVisits++;
      }
    }
    
    double end = omp_get_wtime() - start;
    printf("Finished random walks!\n");
    cout << "Total visits = " << totalVisits << std::endl;
    for (auto element : Web->_vertices){
      if (element != nullptr){
	element->m_node_rnk/=totalVisits;
	rankings.push_back(element);
      }
    }
    std::sort(rankings.begin(), rankings.end(), Comp());
    for (int i = 0; i < 5; ++i){
       cout << "ID = " << rankings[i]->getId() << " rank = " << rankings[i]->m_node_rnk << "\n";
    }
    cout << endl;

    printf("%f seconds\n", end);
  }
  

  struct Comp{
    bool operator()(const Node * lhs, const Node * rhs)const{
      return lhs->m_node_rnk > rhs->m_node_rnk;
    }
  };
  
  int d=20; // Decay factor
  double maxRnk = INT32_MIN;
  int maxId;
  char documentName[256];
  Graph *Web;
  int jumpProbability;
  int threadCount;
  vector<Node *> rankings;
  //  std::priority_queue<Node *, vector<Node *>, Comp> topFive;
  std::ifstream file;
};
#endif
