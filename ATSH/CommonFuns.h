/*
 * CommonFuns.h
 *
 *  Created on: Oct 14, 2011
 *      Author: Jerry
 */

#ifndef COMMONFUNS_H_
#define COMMONFUNS_H_

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <utility>
#include <string>

#include <ctime> // time()
#include <cmath>
//#include <unordered_set>

using namespace std;



struct sort_pair_INT_INT {
	bool operator()(const std::pair<int,int> &i, const std::pair<int,int> &j) {
		return i.second > j.second;
	}
};//sort_pair_intint_dec;


struct sort_INT_DEC {
	bool operator()(int i, int j) {
		return i > j;
	}
};

struct sort_INT_INC {
	bool operator()(int i, int j) {
		return i < j;
	}
};//sort_int_inc;

// Ωµ–Ú≈≈¡–

struct sort_pair_INT_T {
	template <class T>
	bool operator()(const std::pair<int,T> &i, const std::pair<int,T> &j) {
		return i.second > j.second;
	}
};//sort_pair_intint_dec;

template <class T>
void sortMapInt_T(map<int,T> & words, vector<pair<int,T> > & wordsvec){
	//Use the map to create a sorted pair vector
	//-------------------------------------
	//  map->vector->sort pair
	//-------------------------------------
	for ( map<int,T>::iterator it=words.begin() ; it != words.end(); it++ ){
		wordsvec.push_back(*it);//**
	}
	//-------------------
	sort (wordsvec.begin(), wordsvec.begin()+wordsvec.size(), sort_pair_INT_T());
};

// º∆À„”‡œ“œ‡À∆∂»
template <class T>
float ComputeCos(vector<T>& vect1, vector<T>& vect2)
{
	float vect1model = 0;
	float vect2model = 0;
	float crossProduct = 0;

	int n=vect1.size();

	for(int i=0; i<n; ++i)  
	{  
		vect1model += vect1[i]*vect1[i];  
		vect2model += vect2[i]*vect2[i];  

		crossProduct += vect1[i]*vect2[i];  
	}  
	vect1model = sqrt(vect1model);
	vect2model = sqrt(vect2model);
	return (crossProduct/(vect1model*vect2model)); 
};

template <class T1, class T2>
float VcrossProduct(vector<T1>& vect1, vector<T2>& vect2)
{
	float result = 0;
	for (int i=0; i<vect1.size(); ++i)
	{
		for (int j=0; j<vect2.size(); ++j)
		{
			if(i != j)
				result += vect1[i]*vect2[j];
		}
	}
	return result;
}

template <class T1, class T2>
float VdotProduct(vector<T1>& vect1, vector<T2>& vect2)
{
	float result = 0;
	for (int i=0; i<vect1.size(); ++i)
	{
		result += vect1[i]*vect2[i];
	}
	return result;
}



//-----------------
void sortMapInt_Int( map<int,int> & words, vector< pair<int,int> >& wordsvec);
void createHistogram(map<int,int>& hist, const vector<int>& wordsList);

double myround(double value);
string int2str(int i);
string dbl2str(double f);
string flt2str(float f);

#endif /* COMMONFUNS_H_ */
