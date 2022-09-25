#pragma once

#include "GKScene.h"
#include "ofMain.h"
#include "ofxGKUtils.h"
#include "ofxGui.h"
#include "ofEasyCam.h"

#include "Class_Delaunay.h"
#include "Class_GKDelaunay.h"
#include "Class_GKSplit.h"
#include "Class_GKNetwork.h"



class Scene_Modeler : public GKScene{

	public:

		//-----------DEFAULT-----------//
		ofxGKUtils gk;		
		ofEasyCam cam;
		unsigned long int currentFrame;
		float time;
		stringstream ssGlobalLog;
		glm::vec2 mouseOnWorldPlane;

		//-----------GLOBAL-----------//
		bool bModified;
		bool bManualMode;
		bool bDebug;
		bool bHideSelectedPoint;
		bool bHideMainMesh;
		bool bHideAddedMesh;
		bool bHideGKPlane;
		bool bHideNetwork;
		bool bHideGKPlaneSplittedByDelaunay;
		bool bHideGKPlaneSplittedByCombi;
		bool bHideGKPlaneFinal;
		bool bHideLight;
		

		ofLight light;
		ofMesh mainMesh;
		ofNode modifyInfo;
		glm::vec3 selectingVertexPos;

		ofMesh meshToSave;

		//-----------SLIDER-----------//
		ofxGuiGroup guiOne;
		ofParameter<glm::vec3> rotationSlider;
		ofxGuiGroup guiTwo;
		ofParameter<int> selectWhichiPlanesToDelete;
		ofParameter<float> mainMeshAlphaSlider;
		
		void setup();
		void resetScene();
		void update();
		void draw();

		//-----------FOR-LIB-----------//		
		GKDelaunay3d gkDela;
		vector<DelaTriangle> gkDelaTriangles;

		GKSplit gkSplitUtil;
		
		vector<GKLineSimple> gkIntersectLines;
		vector<GKPlane> gkPlanes;
		vector<GKPlane> gkPlanesSplittedByDelaunay;
		vector<GKPlane> gkPlanesSplittedByCombi;
		vector<GKPlane> gkPlanesFinal;

		vector<GKSplit> gkSplits;
		vector<GKNetwork> gkNets;

		//-----------THIS-TIME-INITS-----------//
		void initParam();
		void initSet();
		void initSliders();
		void resetCamera();


		//-----------THIS-TIME-UTILS-----------//
		void drawCamPosition();		
		void createInfo(stringstream& _ssInstruct, stringstream& _ssProgramInfo, stringstream& _ssDebug);
		void loadLatestMesh(const string& _dirName, ofMesh* _mesh);
		ofMesh getMeshFromGKPlanes(vector<GKPlane>* _gkPlanes);


		//-----------THIS-TIME-FUNCS-----------//
		void importMesh();	
		void modifyMesh();
		void updateMesh();
		glm::vec3 getCurrentVertexMesh(const ofMesh& _mesh,stringstream& _ssDebug);
		glm::vec3 getCurrentVertexGKPlane(const vector<GKPlane>& _gkPlanes);
		vector < glm::vec3 > verticesPosHolder;
		void checkVerticesHolder();
		void addGKPlane();

		void draw3DBeforeModified();
		void draw3DAfterModified();
		void drawMainMesh();
		void drawGKPlanes();
	
		void setGKSplits();
		void runGKSplits();		
		void drawGKPlanesSplittedByDelaunay();
		
		void splitIntersectPlanes(GKPlane* _planeB, GKPlane* _planeA, vector<GKLineSimple>* _intersectLines);
		void runSplitByCombi();
		void drawGKPlanesSplittedByCombi();

		void drawGKPlanesFinal();

		//-----------DEBUG-FUNC-----------//
		void debugDot();
		void drawNetwork();
		void drawgkIntersectLines();

		//-----------EVENT-----------//
		void keyPressed(int key) {
			//for (auto& gkPlane : gkPlanes) {
			//	gkPlane.keyPressed(key);// used:key -> none
			//}
			switch (key) {
			case 'h':
				bDebug = !bDebug;
				break;
			case 'f':
				ofToggleFullscreen();
				break;
			case 'r':
				resetCamera();
				break;
			case 'u':
				updateMesh();
				break;
			case 'e':
				saveGK3D();
				break;
			case 'i':
				break;
			case '1':
				bHideMainMesh = !bHideMainMesh;
				break;
			case '2':
				bHideAddedMesh = !bHideAddedMesh;
				break;
			case '3':
				bHideGKPlane = !bHideGKPlane;
				break;
			case '4':
				bHideNetwork = !bHideNetwork;;
				break;
			case '5':
				bHideGKPlaneSplittedByDelaunay = !bHideGKPlaneSplittedByDelaunay;
				break;
			case '6':
				bHideGKPlaneSplittedByCombi = !bHideGKPlaneSplittedByCombi;
				break;
			case '7':
				bHideGKPlaneFinal = !bHideGKPlaneFinal;
				break;
			case '8':
				bHideSelectedPoint = !bHideSelectedPoint;
				break;
			case '9':
				bHideLight = !bHideLight;
				break;
			case 'z':
				if (selectWhichiPlanesToDelete == 0 && gkPlanes.size()) {
					gkPlanes.pop_back();
					ssGlobalLog << "ERASE GKPALNE" << endl;
				}
				if (selectWhichiPlanesToDelete == 1 && gkPlanesSplittedByDelaunay.size()) {
					gkPlanes.pop_back();
					gkPlanesSplittedByDelaunay.pop_back();
					ssGlobalLog << "ERASE GKPALNE" << endl;
					ssGlobalLog << "ERASE SP-PLANE-BY-DELA" << endl;
				}
				if (selectWhichiPlanesToDelete == 2 && gkPlanesSplittedByCombi.size()) {
					gkPlanes.pop_back();
					gkPlanesSplittedByCombi.pop_back();
					ssGlobalLog << "ERASE GKPALNE" << endl;
					ssGlobalLog << "CLEARED LOG" << endl;
				}
				if (selectWhichiPlanesToDelete == 3 && gkPlanesFinal.size()) {
					gkPlanesFinal.pop_back();
					ssGlobalLog << "ERASE GKPALNE" << endl;
					ssGlobalLog << "ERASE SP-PLANE-BY-COMBI" << endl;
				}
				if (selectWhichiPlanesToDelete == 4 && gkPlanes.size()) {
					gkPlanes.pop_back();
					if(gkPlanesSplittedByDelaunay.size())gkPlanesSplittedByDelaunay.pop_back();
					if(gkPlanesSplittedByCombi.size())gkPlanesSplittedByCombi.pop_back();
					if(gkPlanesFinal.size())gkPlanesFinal.pop_back();
					if(gkIntersectLines.size())gkIntersectLines.pop_back();
					ssGlobalLog << "ERASE KINDS OF PLANES" << endl;
				}
				break;

			case 'l':
				ssGlobalLog.str("");
				ssGlobalLog.clear(std::stringstream::goodbit);
				ssGlobalLog << "CLEARED LOG" << endl;
				break;
			case 'c':
				gkPlanes.erase(gkPlanes.begin(), gkPlanes.end());
				gkPlanesSplittedByDelaunay.erase(gkPlanesSplittedByDelaunay.begin(), gkPlanesSplittedByDelaunay.end());
				gkPlanesSplittedByCombi.erase(gkPlanesSplittedByCombi.begin(), gkPlanesSplittedByCombi.end());
				gkPlanesFinal.erase(gkPlanesFinal.begin(), gkPlanesFinal.end());
				gkIntersectLines.erase(gkIntersectLines.begin(), gkIntersectLines.end());
				bModified = !bModified;
				ssGlobalLog.str("");
				ssGlobalLog.clear(std::stringstream::goodbit);
				ssGlobalLog << "CLEARED LOG" << endl;
				ssGlobalLog << "CLEARED-ARRAYS" << endl;
				break;
			case 's':
				gk.saveImage();
				break;
			case 'm':
				exportDataForNextScene();
				break;
			case ' ':
				//findPlaneIntersectionsBeta();
				setGKSplits();
				runGKSplits();
				break;
			case 'b':
				runSplitByCombi();
				break;
			case 'q':
				bManualMode = !bManualMode;
				break;
			}
		}
		void mousePressed(ofMouseEventArgs& args) {
			if (args.button == OF_MOUSE_BUTTON_LEFT) {
				if (bModified) {
					verticesPosHolder.push_back(selectingVertexPos);
				}
			}
		}
		void mousePressed(int x, int y, int button) {};
		void windowResized(int w, int h) {
			gk.resizeGUI(guiOne,13);
			gk.resizeGUI(guiTwo,13);
		}

		//-----------THISTIME-SCENE-BEIDGE-----------//
		void exportDataForNextScene();
		void saveGK3D();

		//-----------NO-InUSE-----------//
		void keyReleased(int key) {};
		void mouseMoved(int x, int y) {};
		void mouseDragged(int x, int y, int button) {};
		void mouseReleased(int x, int y, int button) {};
		void mouseEntered(int x, int y) {};
		void mouseExited(int x, int y) {};
		void mouseScrolled(int x, int y, float scrollX, float scrollY) {};
		void dragEvent(ofDragInfo dragInfo) {};
		void gotMessage(ofMessage msg) {};
};
