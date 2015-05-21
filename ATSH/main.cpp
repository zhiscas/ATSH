//#include "stdafx.h"
#include <stdlib.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "fileOpts.h"

//#include "RFSH.h"
#include "ATSH.h"
//#include "PRSH.h"

using namespace std;
//void directedGraph();

void dataTransform(string filename)
{
	vector<string> data;

	readFromTxt(filename, data);
	writeToTxt("network_sample.txt", false, data);
	for(int i=0;i<data.size();i++)
	{
		istringstream inp(data[i]);
		int Nid;
		int cId;
	}

}

int main()
{
	time_t st=time(NULL);
	//RFSH myRFSH("Graph.txt", "community.dat");
	//myRFSH.start();

	//dataTransform("weibo_network.txt");
	
	ATSH myATSH("data7.txt", "community7.dat", 1000);	
	//   | "Graph.txt"  "community.dat"   "network.txt", "weibo_community_final.txt"
	
	myATSH.drawGraph();
	myATSH.start();
	
	//PRSH myPRSH("Graph.txt", "community.dat");
	//myPRSH.start();
	




	//mySpla.drawGraph();

	//mySpla.start(0.09, 0.19);
	//mySpla.structuralHole("community.dat", 0, 0.3);	//community.dat  cpm.txt

	cout<<" Take all time:" <<difftime(time(NULL),st)<< " seconds."<<endl;
	return 1;

}