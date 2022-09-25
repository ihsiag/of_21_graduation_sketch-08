#pragma once

#ifndef CLASS_ANIMATION_H  
#define CLASS_ANIMATION_H 

#include "GKScene.h"
#include "ofMain.h"
#include "ofxGKUtils.h"
#include "ofxGui.h"
#include "ofEasyCam.h"

#include "Class_Delaunay.h"
#include "Class_GKDelaunay.h"
#include "Class_GKSplit.h"
#include "Class_GKNetwork.h"

class AnimationClass {
public:
	virtual void setup() = 0;
	virtual void resetAnimation() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual bool getThisAnimationEndState() = 0;
	virtual bool getNextAnimationTriggerState() = 0;
};

class AnimationClassOpening : public AnimationClass{
public:
	int animationFrame;
	int planesNumToShow;
	int dotsNumToShow;
	bool bDrawAll;
	bool bNextAnimationTrigger;

	glm::vec3 boxInfo;
	vector<GKPlane>* gpl;

	 void setup();
	 void setBoxInfo(const glm::vec3& _boxInfo) {
		 boxInfo = _boxInfo;
	 }
	 void resetAnimation() {}
	 void update() {}
	 void draw() {}
	 bool getThisAnimationEndState() {}
	 bool getNextAnimationTriggerState() {}
};

class AnimationClassMeshVertex : public AnimationClass {
public:
	int animationFrame;
	int planesNumToShow;
	int dotsNumToShow;
	bool bDrawAll;
	bool bNextAnimationTrigger;
	float alpha;
	vector<GKPlane>* gpl;

	AnimationClassMeshVertex() {}


	void setup() {
		initParam();
	}

	void setGPL(vector<GKPlane>* _gpl) {
		gpl = _gpl;
	}

	void initParam() {
		animationFrame = 0;
		planesNumToShow = 0;
		dotsNumToShow = 0;
		alpha = 1;
		bDrawAll = false;
		bNextAnimationTrigger = false;
	}

	void update() {
		animationFrame++;
		dotsNumToShow++;
		if (dotsNumToShow > 2) {
			dotsNumToShow = 0;
			planesNumToShow++;
		}
		if (planesNumToShow > gpl->size() / 5)bNextAnimationTrigger = true;
		if (planesNumToShow > gpl->size()) {
			planesNumToShow = gpl->size();
			dotsNumToShow = 3;
			bDrawAll = true;
		}
	}

	void draw() {
		glPointSize(10);
		glColor4f(1, 0, 0, 1);
		glBegin(GL_POINTS);
		for (int i = 0; i < planesNumToShow; i++) {
			if (i < planesNumToShow - 1) {
				for (int j = 0; j < 3; j++) {
					glm::vec3 _p = gpl->at(i).originalMesh.getVertex(j);
					glVertex3f(_p.x, _p.y, _p.z);
				}
			}
			else {
				for (int j = 0; j < dotsNumToShow; j++) {
					glm::vec3 _p = gpl->at(i).originalMesh.getVertex(j);
					glVertex3f(_p.x, _p.y, _p.z);
				}
			}
		}
		glEnd();
	}

	bool getThisAnimationEndState() {
		return bDrawAll;
	}

	bool getNextAnimationTriggerState() {
		return bNextAnimationTrigger;
	}

	void resetAnimation() {
		initParam();
	}
};

class AnimationClassMeshEdge : public AnimationClass {
public:
	int animationFrame;
	int planesNumToShow;
	int dotsNumToShow;
	bool bDrawAll;
	bool bNextAnimationTrigger;
	float alpha;
	vector<GKPlane>* gpl;

	AnimationClassMeshEdge() {}

	void setup() {
		initParam();
	}

	void setGPL(vector<GKPlane>* _gpl) {
		gpl = _gpl;
	}

	void initParam() {
		animationFrame = 0;
		planesNumToShow = 0;
		dotsNumToShow = 0;
		alpha = 1.0;
		bDrawAll = false;
		bNextAnimationTrigger = false;
	}

	void update() {
		animationFrame++;
		dotsNumToShow++;
		//if(animationFrame&2 == 0)dotsNumToShow++;
		if (dotsNumToShow > 4) {
			dotsNumToShow = 0;
			planesNumToShow++;
		}
		if (planesNumToShow > gpl->size() / 1.5) bNextAnimationTrigger = true;
		if (planesNumToShow > gpl->size()) {
			planesNumToShow = gpl->size();
			dotsNumToShow = 4;
			bDrawAll = true;
		}
	}

	void draw() {
		glLineWidth(2);
		glColor4f(1, 0, 0, 1);
		for (int i = 0; i < planesNumToShow; i++) {
			glBegin(GL_LINE_STRIP);
			if (i < planesNumToShow - 1) {
				for (int j = 0; j < 4; j++) {
					glm::vec3 _p = gpl->at(i).originalMesh.getVertex(j % 3);
					glVertex3f(_p.x, _p.y, _p.z);
				}
			}
			else {
				for (int j = 0; j < dotsNumToShow; j++) {
					glm::vec3 _p = gpl->at(i).originalMesh.getVertex(j % 3);
					glVertex3f(_p.x, _p.y, _p.z);
				}
			}
			glEnd();
		}
	}

	bool getNextAnimationTriggerState() {
		return bNextAnimationTrigger;
	}

	bool getThisAnimationEndState() {
		return bDrawAll;
	}

	void resetAnimation() {
		initParam();
	}

};

class AnimationClassMeshFace : public AnimationClass {
public:
	int animationFrame;
	int planesNumToShow;
	bool bDrawAll;
	bool bNextAnimationTrigger;
	vector<GKPlane>* gpl;

	AnimationClassMeshFace() {}

	void setup() {
		initParam();
	}

	void setGPL(vector<GKPlane>* _gpl) {
		gpl = _gpl;
	}

	void initParam() {
		animationFrame = 0;
		planesNumToShow = 0;
		bDrawAll = false;
		bNextAnimationTrigger = false;
	}

	void update() {
		animationFrame++;
		planesNumToShow++;
		if (planesNumToShow > gpl->size())bNextAnimationTrigger = true;
		if (planesNumToShow > gpl->size()) {
			planesNumToShow = gpl->size();
			bDrawAll = true;
		}
	}

	void draw() {
		glLineWidth(2);
		glColor3f(1, 0, 0);
		for (int i = 0; i < planesNumToShow; i++) {
			gpl->at(i).drawInputMesh();
		}
	}

	bool getNextAnimationTriggerState() {
		return bNextAnimationTrigger;
	}

	bool getThisAnimationEndState() {
		return bDrawAll;
	}

	void resetAnimation() {
		initParam();
	}

};

class AnimationClassPlaneFace : public AnimationClass {
public:
	int animationFrame;
	int planesNumToShow;
	bool bDrawAll;
	bool bNextAnimationTrigger;
	vector<GKPlane>* gpl;

	AnimationClassPlaneFace() {}

	void setup() {
		initParam();
	}

	void setGPL(vector<GKPlane>* _gpl) {
		gpl = _gpl;
	}

	void initParam() {
		animationFrame = 0;
		planesNumToShow = 0;
		bDrawAll = false;
		bNextAnimationTrigger = false;
	}

	void update() {
		animationFrame++;
		planesNumToShow++;
		if (planesNumToShow > gpl->size())bNextAnimationTrigger = true;
		if (planesNumToShow > gpl->size()) {
			planesNumToShow = gpl->size();
			bDrawAll = true;
		}
	}

	void draw() {
		glLineWidth(2);
		glColor3f(1, 0, 0);
		for (int i = 0; i < planesNumToShow; i++) {
			gpl->at(i).drawGKPlane(glm::vec4(0.1, 0.1, 0.1, 0.1), glm::vec4(0.8, 0.8, 1, 0.8), 1);
		}
	}

	bool getNextAnimationTriggerState() {
		return bNextAnimationTrigger;
	}

	bool getThisAnimationEndState() {
		return bDrawAll;
	}

	void resetAnimation() {
		initParam();
	}

};

class AnimationClassFindCombi :public AnimationClass {
public:
	int animationFrame;
	int planesNumToShow;
	int dotsNumToShow;
	bool bDrawAll;
	bool bNextAnimationTrigger;
	vector<GKPlane>* gpl;

	bool* bHideMainMesh;
	bool bDrawBegin;
	ofEasyCam* cam;
	GKDelaunay3d gkDela;
	vector<DelaTriangle> gkDelaTriangles;

	AnimationClassFindCombi() {}

	void setup() {
		initParam();
		makeDela();
	}

	void setGPL(vector<GKPlane>* _gpl) {
		gpl = _gpl;
	}

	void setCam(ofEasyCam* _cam) {
		cam = _cam;
	}

	void setHideMesh(bool* _bHideMesh) {
		bHideMainMesh = _bHideMesh;
	}

	void initParam() {
		animationFrame = 0;
		planesNumToShow = 0;
		bDrawAll = false;
		bNextAnimationTrigger = false;
		*bHideMainMesh = false;
		bDrawBegin = false;
		gkDelaTriangles.erase(gkDelaTriangles.begin(), gkDelaTriangles.end());
	}

	void makeDela() {
		gkDelaTriangles.erase(gkDelaTriangles.begin(), gkDelaTriangles.end());
		gkDelaTriangles = gkDela.getDelaunayTriangles(*gpl);
	}

	void update() {
		animationFrame++;
		*bHideMainMesh = true;
		bDrawBegin = true;
		if (animationFrame > 60 * 4) {
			bDrawAll = true;
			bNextAnimationTrigger = true;
		}
	}

	void draw() {
		if (bDrawBegin) {
			for (auto& g : *gpl) {
				g.drawGKPlaneCentroid(glm::vec4(1, 0, 1, 1), 10, 5);
			}
			if (gkDelaTriangles.size()) {
				int counter = 0;
				for (auto& gdt : gkDelaTriangles) {
					glLineWidth(1);
					ofFill();
					glColor4f(0, 1, 0, 1);
					glBegin(GL_LINE_LOOP);
					for (auto& v : gdt.vertices) {
						glVertex3f(v.pos.x, v.pos.y, v.pos.z);
					}
					glEnd();
					counter++;
				}
				/*
				for (auto& gks : gkSplits) {
					cam.end();
					ofDrawBitmapStringHighlight(ofToString(gks.mainPlane.state), cam.worldToScreen(gks.mainPlane.centroid));
					cam.begin();
				}
				*/
			}
		}
	}

	bool getNextAnimationTriggerState() {
		return bNextAnimationTrigger;
	}

	bool getThisAnimationEndState() {
		return bDrawAll;
	}

	void resetAnimation() {
		initParam();
		makeDela();
	}
};

class AnimationClassFindIntersection :public AnimationClass {
public:
	AnimationClassFindIntersection() {}
};

class AnimationClassSplit : public AnimationClass {
	int animationFrame;
	int planesNumToShow;
	int dotsNumToShow;
	bool bDrawAll;
	bool bNextAnimationTrigger;
	vector<GKPlane>* gpl;

	ofxGKUtils gk;
	GKSplit gkSplitUtil;
	vector<GKSplit> gkSplits;
	vector<GKPlane> gkPlanesSplittedByCombi;
	vector<GKLineSimple> gkIntersectLines;



	AnimationClassSplit() {}
	void setup() {
		initParam();
	}
	void setGPL(vector<GKPlane>* _gpl) {
		gpl = _gpl;
	}
	void initParam() {
		animationFrame = 0;
		planesNumToShow = 0;
		bDrawAll = false;
		bNextAnimationTrigger = false;
	}
	void update() {
		runSplitByCombi();
	}
	void draw() {
		for (auto& gpsbc : gkPlanesSplittedByCombi) {
			gpsbc.drawGKPlane();
		}
	}
	bool getNextAnimationTriggerState() {
		return bNextAnimationTrigger;
	}

	bool getThisAnimationEndState() {
		return bDrawAll;
	}

	void resetAnimation() {
		initParam();
	}

	void runSplitByCombi() {
		gkSplits.erase(gkSplits.begin(), gkSplits.end());
		gkPlanesSplittedByCombi.erase(gkPlanesSplittedByCombi.begin(), gkPlanesSplittedByCombi.end());
		gkPlanesSplittedByCombi = *gpl;
		for (auto& gkpsbc : gkPlanesSplittedByCombi) {
			for (auto& edge : gkpsbc.edges) {
				gkSplitUtil.scalePlaneEdge(&edge, gkpsbc.centroid, 2);
			}
		}
		//gkPlanesSplittedByCombi = gkPlanesSplittedByDelaunay;
		for (auto& combi : gk.getIndexList_nC2(gkPlanesSplittedByCombi.size())) {
			splitIntersectPlanes(&gkPlanesSplittedByCombi[combi.x], &gkPlanesSplittedByCombi[combi.y], &gkIntersectLines);
		}
	}

	void splitIntersectPlanes(GKPlane* _planeB, GKPlane* _planeA, vector<GKLineSimple>* _intersectLines) {
		vector<glm::vec3> _intersectPointsA = gkSplitUtil.getPlaneIntersection(*_planeB, *_planeA);
		if (_intersectPointsA.size() == 2) {
			vector<glm::vec3> _intersectPointsB = gkSplitUtil.getPlaneIntersection(*_planeA, *_planeB);
			if (_intersectPointsB.size() == 2) {

				GKLineSimple _intersectLineA = GKLineSimple(_intersectPointsA[0], _intersectPointsA[1]);
				_intersectLines->push_back(_intersectLineA); //gkIntersectLines
				GKPlane _gkPlaneNewA = gkSplitUtil.splitPlaneWithIntersectLine(*_planeA, _intersectLineA);

				GKLineSimple _intersectLineB = GKLineSimple(_intersectPointsB[0], _intersectPointsB[1]);
				_intersectLines->push_back(_intersectLineB); //gkIntersectLines
				GKPlane _gkPlaneNewB = gkSplitUtil.splitPlaneWithIntersectLine(*_planeB, _intersectLineB);

				//mainPlane = _gkPlaneNew;
				*_planeA = _gkPlaneNewA; //maybe you should store data tmpPlane and after every check then you should put your data into realPlane
				*_planeB = _gkPlaneNewB;
			}

		}
	}

};


#endif
#pragma once
