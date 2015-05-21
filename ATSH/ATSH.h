#pragma once

#include "Snap.h"

#include <string>
#include <vector>
//#include <hash_map>
#include <map>

using namespace std;

typedef PNGraph PGraph;  //   directed graph

class ATSH
{
public:
	ATSH(string filename, string cpmFileName, int maxT=100);
	~ATSH();

	int initWQueue(string filename);
	void initVRank(int csize, vector<vector<float> >& vRank);
	void initScore(int n);
	void initNodeNbMap();
	void randFind(int topk);
	void start();
	void drawGraph();

	void crossProduct();
	void sortScore(vector<pair<int,float> >& ScoreV);
	void GetSubGraphRemoveTopk(int topk, vector<pair<int,float> >& ScoreV);
	//void vector_Normalization(vector<float>& vect);
	//bool vector_Plus(vector<float>& vResult, vector<float>& vect2, float ratio);
	//void Rank_Normalization();
	//bool isBelongCommunity(int cId, int vId);
	void writeScoreToTxt(int topk, string fileName, vector<pair<int,float> >& data);
	void writeA_HToTxt(string fileName, vector<vector<float> >& data);

	PGraph m_Graph;	// 
	vector<int> m_CLabel;
	vector<map<int, int> > m_vNodeINbMap;
	vector<map<int, int> > m_vNodeONbMap;


	vector<float> m_Score;
	vector<vector<float> > m_vAuthority;
	vector<vector<float> > m_vHub;
	map<int, vector<int> > mCommunities;

	int m_maxT;
	string m_cpmFileName;
};