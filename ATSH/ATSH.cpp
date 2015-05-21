#include "ATSH.h"

#include "fileOpts.h"
#include <ctime>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "CommonFuns.h"

//void ATSH::vector_Normalization(vector<float>& vect)
//{
//	float total=0;
//	for(int i=0; i<vect.size(); ++i)  
//	{  
//		total += vect[i];
//	}
//	if (total >0)
//	{
//		for(int i=0; i<vect.size(); ++i)  
//		{  
//			vect[i] /= total;
//		}
//	}
//}
//
//bool ATSH::vector_Plus(vector<float>& vResult, vector<float>& vect2, float ratio)
//{
//	if (vResult.size() != vect2.size())	return false;
//
//	for (int i=0; i<vResult.size(); ++i)
//	{
//		vResult[i] += vect2[i]*ratio;
//		if (vResult[i] > m_vRank[0][i])
//		{
//			m_vRank[0][i] = vResult[i];
//		}
//	}
//	return true;
//}

ATSH::ATSH(string filename, string cpmFileName, int maxT)
{
	m_Graph = TSnap::LoadEdgeList<PGraph>(filename.c_str());
	m_maxT = maxT;
	m_cpmFileName = cpmFileName;
}

ATSH::~ATSH()
{

}

void ATSH::drawGraph()
{
	TSnap::DrawGViz(m_Graph, gvlNeato, "graph.gif", "", true);
}



int ATSH::initWQueue(string filename)
{
	m_CLabel.clear();
	mCommunities.clear();

	m_vNodeINbMap.clear();
	m_vNodeONbMap.clear();

	for (int i=0; i<=m_Graph->GetNodes(); ++i)
	{
		m_CLabel.push_back(0);
		m_vNodeINbMap.push_back(map<int,int>());
		m_vNodeONbMap.push_back(map<int,int>());
	}

	vector<string> data;

	readFromTxt(filename, data);
	for(int i=0;i<data.size();i++)
	{
		istringstream inp(data[i]);
		int Nid;
		int cId;

		// 插入新值
		inp >> Nid;
		inp >> cId;

		m_CLabel[Nid] = cId;

		map<int, vector<int> >::iterator iter = mCommunities.find(cId);
		if (iter == mCommunities.end())
			mCommunities.insert(make_pair(cId, vector<int>(1, Nid)));
		else
			iter->second.push_back(Nid);
	}
	return mCommunities.size();
}


void ATSH::initVRank(int csize, vector<vector<float> >& vRank)
{
	csize = csize +1;
	vRank.clear();
	vRank.push_back(vector<float>(csize,0));
	for (int i=1; i<m_CLabel.size(); ++i)
	{
		vRank.push_back(vector<float>(csize,0));
		int cid = m_CLabel[i];
		vRank[i][cid] = 1;
	}
}

void ATSH::initScore(int n)
{
	/*for (int i=0; i<n; ++i)
	{
	m_Score.push_back(0);

	}*/
	m_Score.assign(n, 0);
}

void ATSH::initNodeNbMap()
{
	PGraph::TObj::TNodeI nodeNI = m_Graph->BegNI();
	for (; nodeNI != m_Graph->EndNI(); nodeNI++)
	{
		int vID = nodeNI.GetId();
		// init in-nbs' community label counts 
		int numInNbs = nodeNI.GetInDeg();
		for(int j=0;j<numInNbs;j++)
		{
			int NbID = nodeNI.GetInNId(j);
			int NbClabel = m_CLabel[NbID];
			++m_vNodeINbMap[vID][NbClabel];
		}
		// init out-nbs' community label counts 
		
		int numOutNbs = nodeNI.GetOutDeg();
		for(int j=0;j<numOutNbs;j++)
		{
			int NbID = nodeNI.GetOutNId(j);
			int NbClabel = m_CLabel[NbID];
			++m_vNodeONbMap[vID][NbClabel];
		}
	}
}

//bool ATSH::isBelongCommunity(int cId, int vId)
//{
//	for (int i=0; i<m_CLabel[vId]; ++i)
//		if (m_CLabel[vId][i] == cId)	return true;
//	return false;
//}


void ATSH::randFind(int topk)
{
	vector<int> nodes;
	for (PGraph::TObj::TNodeI NI = m_Graph->BegNI(); NI < m_Graph->EndNI(); NI++)
	{
		nodes.push_back(NI.GetId());
	}

	float d = 0.85;
	float theta = (1-d)/m_Graph->GetNodes();
	float alpha = 0.6;
	float beta = 0.6;
	float epsilon = 0.00001;
	bool updateA = true;
	bool updateT = true;
	float deltaA = 0;
	float deltaT = 0;

	int t=1;
	for(;t<m_maxT && (updateA || updateT);t++)
	{
		//1.shuffle
		//cout<<"-------------t="<<t<<"---------------------"<<endl;
		cout<<"*"<<flush;
		srand (time(NULL)); // ***YOU need to use this, such that you can get a new one each time!!!!! seed the random number with the system clock
		random_shuffle (nodes.begin(), nodes.end());

		float tmpDeltaA = 0;
		float tmpDeltaT = 0;

		vector<vector<float> > tmpAuthority(m_vAuthority.size(), vector<float>(m_vAuthority[0].size(), 0));
		vector<vector<float> > tmpHub(m_vHub.size(), vector<float>(m_vHub[0].size(), 0));
		//vector<vector<float> > maxAuthority(m_vAuthority[0].size(), vector<float>(m_vAuthority[0].size(), 0));
		//vector<vector<float> > maxHub(m_vHub[0].size(), vector<float>(m_vHub[0].size(), 0));
		//2. do one iteration
		for(int i=0;i<nodes.size();i++)
		{
			int vID = nodes[i];
			PGraph::TObj::TNodeI NI = m_Graph->GetNI(vID);

			// 更新Authority
			if (updateA)
			{
				int numInNbs = NI.GetInDeg();
				for(int j=0;j<numInNbs;j++)
				{
					int NbID = NI.GetInNId(j);
					PGraph::TObj::TNodeI NbNI = m_Graph->GetNI(NbID);
					int NbOutNbs = NbNI.GetOutDeg();
					//vector_Plus(tmp[vID], m_vRank[NbID], 1.0);
					int NbsOutSameC = m_vNodeONbMap[NbID][m_CLabel[vID]];
					NbOutNbs = NbsOutSameC;

					if (m_CLabel[vID] == m_CLabel[NbID])
					{
						//int cId = m_CLabel[vID];
						//tmpAuthority[vID][cId] += m_vHub[NbID][cId];
						for (int k=1; k<m_vHub[NbID].size(); ++k)
						{
							
							if (m_CLabel[vID] == k)
							{
								//tmpAuthority[vID][k] += m_vHub[NbID][k];
								tmpAuthority[vID][k] += m_vAuthority[NbID][k]/NbOutNbs;
								/*if (tmpAuthority[0][k] < tmpAuthority[vID][k])
								{
									tmpAuthority[0][k] = tmpAuthority[vID][k];
								}*/
							}
							else
							{
								tmpAuthority[vID][k] += alpha*m_vAuthority[NbID][k]/NbOutNbs;
							}
						}
					}
					else
					{
						int cId = m_CLabel[NbID];
						//tmpAuthority[vID][cId] += m_vAuthority[NbID][cId];
						for (int k=1; k<m_vAuthority[NbID].size(); ++k)
						{
							if (m_CLabel[vID] != k)
							{
								if (m_CLabel[NbID] == k)
								{
									tmpAuthority[vID][k] += m_vAuthority[NbID][k];
								}
								else
								{
									tmpAuthority[vID][k] += m_vAuthority[NbID][k]/NbOutNbs;	
								}

							}
							/*if (tmpAuthority[0][k] < tmpAuthority[vID][k])
							{
								tmpAuthority[0][k] = tmpAuthority[vID][k];
							}*/
						}
					}
				}
				for(int k=1; k<m_vAuthority[vID].size(); ++k)
				{
					tmpAuthority[vID][k] = theta + d*tmpAuthority[vID][k];
					if (m_CLabel[vID] == k && tmpAuthority[0][k] < tmpAuthority[vID][k])
					{
						tmpAuthority[0][k] = tmpAuthority[vID][k];
					}
				}
			}

			// 更新Hub
			if (updateT)
			{
				int numOutNbs = NI.GetOutDeg();
				for(int j=0;j<numOutNbs;j++)
				{
					int NbID = NI.GetOutNId(j);
					PGraph::TObj::TNodeI NbNI = m_Graph->GetNI(NbID);
					int NbInNbs = NbNI.GetInDeg();
					//vector_Plus(tmp[vID], m_vRank[NbID], 1.0);
					int NbsInSameC = m_vNodeINbMap[NbID][m_CLabel[vID]];
					NbInNbs = NbsInSameC;

					if (m_CLabel[vID] == m_CLabel[NbID])
					{
						//int cId = m_CLabel[vID];
						//tmpAuthority[vID][cId] += m_vHub[NbID][cId];
						for (int k=1; k<m_vAuthority[NbID].size(); ++k)
						{
							if (m_CLabel[vID] == k)
							{
								//tmpHub[vID][k] += m_vAuthority[NbID][k];
								tmpHub[vID][k] += m_vHub[NbID][k]/NbInNbs;
								if (tmpHub[0][k] < tmpHub[vID][k])
								{
									tmpHub[0][k] = tmpHub[vID][k];
								}
							}
							else
							{
								tmpHub[vID][k] += beta*m_vHub[NbID][k]/NbInNbs;
							}
							
						}
					}
					else
					{
						int cId = m_CLabel[NbID];
						//tmpAuthority[vID][cId] += m_vAuthority[NbID][cId];
						for (int k=1; k<m_vHub[NbID].size(); ++k)
						{
							if (m_CLabel[vID] != k)
							{
								if (m_CLabel[NbID] == k)
								{
									tmpHub[vID][k] += m_vHub[NbID][k];//NbInNbs
								}
								else
								{
									tmpHub[vID][k] += m_vHub[NbID][k]/NbNI.GetInDeg();
								}

							}

							/*if (tmpHub[0][k] < tmpHub[vID][k])
							{
								tmpHub[0][k] = tmpHub[vID][k];
							}*/
						}
					}
				}
				for(int k=1; k<m_vHub[vID].size(); ++k)
				{
					tmpHub[vID][k] = theta + d*tmpHub[vID][k];
					if (m_CLabel[vID] == k && tmpHub[0][k] < tmpHub[vID][k])
					{
						tmpHub[0][k] = tmpHub[vID][k];
					}
				}
				
			}
		}	

		if (updateA)//t < m_maxT-1
		{
			for (int i=1; i<tmpAuthority.size(); ++i)
			{
				for (int j=1; j<tmpAuthority[i].size(); ++j)
				{
					float tmpDelta = 0;
					if (j == m_CLabel[i] && tmpAuthority[0][j] > 0)	
					{
						tmpDelta = abs(m_vAuthority[i][j] - tmpAuthority[i][j]/tmpAuthority[0][j]);
						m_vAuthority[i][j] = tmpAuthority[i][j]/tmpAuthority[0][j];
					}
					else
					{
						tmpDelta = abs(m_vAuthority[i][j] - tmpAuthority[i][j]);
						m_vAuthority[i][j] = tmpAuthority[i][j];
					}
					//deltaA = (deltaA > tmpDelta ? deltaA : tmpDelta);
					tmpDeltaA += tmpDelta;
				}
			}

			if(abs(deltaA-tmpDeltaA)<epsilon) 
			{
				updateA = false;
				for (int i=1; i<tmpAuthority.size(); ++i)
				{
					for (int j=0; j<tmpAuthority[i].size(); ++j)
					{
						m_vAuthority[i][j] = tmpAuthority[i][j];
					}
				}
			}
			else
				deltaA = tmpDeltaA;
		}

		if(updateT)
		{
			for (int i=1; i<tmpHub.size(); ++i)
			{
				for (int j=1; j<tmpHub[i].size(); ++j)
				{
					float tmpDelta = 0;
					if (j == m_CLabel[i] && tmpHub[0][j] > 0)
					{
						tmpDelta = abs(m_vHub[i][j] - tmpHub[i][j]/tmpHub[0][j]);
						m_vHub[i][j] = tmpHub[i][j]/tmpHub[0][j];
					}
					else
					{
						tmpDelta = abs(m_vHub[i][j] - tmpHub[i][j]);
						m_vHub[i][j] = tmpHub[i][j];
					}
					//deltaT = (deltaT > tmpDelta ? deltaT : tmpDelta);
					tmpDeltaT += tmpDelta;
				}
			}
			if(abs(deltaT-tmpDeltaT)<epsilon) 
			{
				updateT = false;
				/*for (int i=1; i<tmpHub.size(); ++i)
				{
				for (int j=0; j<tmpHub[i].size(); ++j)
				{
				m_vHub[i][j] = tmpHub[i][j];
				}
				}*/
			}
			else
				deltaT = tmpDeltaT;
		}
		//cout << t << endl;

		if (t == 10)
		{
			int a=0;
		}

		if (t == 90)
		{
			int a=0;
		}

		tmpAuthority.clear();
		tmpHub.clear();
		//maxAuthority.clear();
		//maxHub.clear();
	}
	cout << t << endl;
}



//void ATSH::Rank_Normalization()
//{
//	for (int i=1; i<m_vRank.size(); ++i)
//	{
//		for (int j=0; j<m_vRank[i].size(); ++j)
//		{
//			m_vRank[i][j] /= m_vRank[0][j];
//		}
//	}
//}

void ATSH::start()
{
	int csize = initWQueue(m_cpmFileName);
	initNodeNbMap();
	initVRank(csize, m_vAuthority);
	initVRank(csize, m_vHub);
	initScore(m_Graph->GetNodes()+1);
	randFind(10);

	crossProduct();
	vector<pair<int,float> > ScoreV;
	sortScore(ScoreV);

	writeA_HToTxt("m_vAuthority.txt", m_vAuthority);
	writeA_HToTxt("m_vHub.txt", m_vHub);
	writeScoreToTxt(-1, "SHscore.txt", ScoreV);
	//GetSubGraphRemoveTopk(5,ScoreV);
}


void ATSH::crossProduct()
{
	for (int i=1; i<m_Score.size(); ++i)
	{
		m_Score[i] = VcrossProduct(m_vAuthority[i], m_vHub[i]);
		//m_Score[i] -= m_vAuthority[i][m_CLabel[i]] * m_vHub[i][m_CLabel[i]];
	}
}




void ATSH::sortScore(vector<pair<int,float> >& ScoreV)
{
	map<int, float> Id_ScoreMap;
	for (int i=1; i<m_Score.size(); ++i)
	{
		Id_ScoreMap.insert(make_pair(i, m_Score[i]));
	}

	sortMapInt_T(Id_ScoreMap, ScoreV);
}

void ATSH::GetSubGraphRemoveTopk(int topk, vector<pair<int,float> >& ScoreV)
{
	TIntV SubGIdV;

	for (int i=topk; i<ScoreV.size(); ++i) 
	{
		{
			SubGIdV.Add(ScoreV[i].first);
		}
		//printf("node id %d with out-degree %d and in-degree %d\n",NI.GetId(), NI.GetOutDeg(), NI.GetInDeg());
	}
	PGraph SubG = TSnap::GetSubGraph(m_Graph, SubGIdV);
	TSnap::SaveEdgeList(SubG, "subGraph.txt", "Save as SubGraph remove Topk");
}

void ATSH::writeScoreToTxt(int topk, string fileName, vector<pair<int,float> >& data)
{
	fstream fp;
	int n;
	if (topk==-1)
		n = data.size();
	else
		n = topk;

	//if(isappend)
	//fp.open(fileName.c_str(),fstream::app);//|fstream::out|fstream::app
	//else
	fp.open(fileName.c_str(),fstream::out);//|fstream::out|fstream::app

	if(fp.is_open()){//if it does not exist, then failed

		for(int i=0;i<n;i++){
			fp << data[i].first << "\t" << data[i].second <<endl;
		}
		fp.close();

		//cout<<"write to "<<fileName<<endl;
	}
	else{
		cout<<"open failed"<<endl;
	}
}

void ATSH::writeA_HToTxt(string fileName, vector<vector<float> >& data)
{
	fstream fp;
	int n = data.size();

	fp.open(fileName.c_str(),fstream::out);//|fstream::out|fstream::app

	if(fp.is_open()){//if it does not exist, then failed

		for(int i=0;i<n;i++){
			fp << i;
			for (int j=0;j<data[i].size();j++){
				fp << "\t" << data[i][j]; 
			}
			fp << endl;
		}
		fp.close();

		//cout<<"write to "<<fileName<<endl;
	}
	else{
		cout<<"open failed"<<endl;
	}
}