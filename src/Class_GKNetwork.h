#pragma once

#ifndef GK_NETWORK_H  
#define GK_NETWORK_H  

#include "ofMain.h"
#include "ofxGKUtils.h"

class GKNetwork{
public:
	int mainIndex;
	set<int> cutterIndices;

	GKNetwork(){}
	GKNetwork(const int& _mainIndex) {
		mainIndex = _mainIndex;
	}
	~GKNetwork() {}

	void addCutterIndex(const int& _cutterIndex) {
		if (_cutterIndex != mainIndex) {
			cutterIndices.insert(_cutterIndex);
		}
	}

	bool operator < (const GKNetwork& _gkn) const {
		return mainIndex < _gkn.mainIndex;
	}
};
#endif