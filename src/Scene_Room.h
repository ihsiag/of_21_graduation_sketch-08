#pragma once

#include "ofMain.h"
#include "GKScene.h"
#include "ofxGKUtils.h"
#include "ofxBullet.h"
#include "ofxGui.h"
#include "ofEvent.h"
#include "Easing.h"


#include "GKControllerAngle.h"

#include "btGhostObject.h"


class Scene_Room : public GKScene{ //ofBaseApp {

public:
    //-----------BASIC-----------//
    ofxGKUtils gk;
    GKControllerAngle gkcAU;
    GKControllerAngle gkcAD;
    ofEasyCam cam;
    ofTrueTypeFont font;
    ofMesh mesh;
    float meshScaleFactor;
    float meshBB[6];

    

    stringstream ssGlobalLog;

    //-----------FBO-----------//
    ofFbo fbo;

    //-----------PHISICLIB-----------//
    ofxBulletWorldSoft world;

    ofxBulletBox* ground;
    glm::vec3 groundInfo;

    vector<ofLight> lights;
    vector<ofBoxPrimitive> lightBodies;
    glm::vec3 lightBodyInfo;

    ofMaterial defaultBodyMat;

    vector< shared_ptr<ofxBulletSoftTriMesh> > models;
    vector< shared_ptr<ofxBulletRigidBody> > rigidBodies;
    vector< shared_ptr<ofxBulletRigidBody>> crashers;
    vector< shared_ptr<ofxBulletRigidBody>> kinematicBodies;

    vector<ofxBulletRigidBody*> armsDowner;
    vector<ofxBulletRigidBody*> armsUpper;

    int							mousePickIndex;
    ofVec3f						mousePickPos;


    //-----------GLOBALVAL-----------//
    bool bDrawDebug;


    unsigned long int currentFrame;
    float time;
    int fontSize;

    glm::vec2 mouseOnWorldPlane;

    //-----------SLIDER-----------//
    //ofxPanel gui;
    ofxGuiGroup guiOne;
    ofParameter<glm::vec3> stiffness;
    ofParameter<float> slider_kVC;
    ofParameter<int>  slider_piteration;
    ofParameter<float> slider_kDF;
    ofParameter<float> slider_kSSHR_CL;
    ofParameter<float> slider_kSS_SPLT_CL;
    ofParameter<float> slider_kSKHR_CL;
    ofParameter<float> slider_kSK_SPLT_CL;
    ofParameter<int> slider_selectModelIndex;

    ofxGuiGroup guiTwo;
    ofParameter<float> slider_controller_angle;
    ofParameter<float> slider_controller_pressure;
    ofParameter<float> slider_armsDowner_r;
    ofParameter<float> slider_armsUpper_r;
    ofParameter<float> slider_power;


    bool bPlayOpeningScene;
    int openingPeriod;



    void setup();
    void resetScene();
    void update();
    void draw();

    //-----------THIS-TIME-INITS-----------//
    void initParam();
    void initGKSet();
    void initSliders();
    void resetCamera();
    void initListener();

    //-----------THISTIME-UTILS-----------//
    void loadFont();
    void loadMesh();
    void createInfo(stringstream& _ssInstruct, stringstream& _ssProgramInfo, stringstream& _ssDebug);
    void drawMyGraph();

    //-----------THISTIME-FUNCS-----------//
    void setWorld();
    void setStage();
    void setMachines();

    void turnOnLights();
    void drawStage();
    void turnOffLights();
   
    void updateMachines();
    void drawMachines();

    void addKinematicBody();
    void addBox();
    void addCylinder(const glm::vec2& _pos);
    void addModel(const glm::vec2& _pos);
    void deleteModel();
    void drawModelPos();
    void drawBodies();

    void makeControllerUI();
    void updateControllerUI();
    void drawControllerUI();

    void makeArms();
    void updateArms();
    void drawArms();

    void connectArmsToModel();

    void makeHammer();
    void updateHammer();
    void drawHammer();

    //-----------THISTIME-SCENE-BEIDGE-----------//
    void exportDataForNextScene();

    void openingScene();

    


    


    //-----------EVENT-FUNCS-----------//
    void mousePickEvent(ofxBulletMousePickEvent& e);
    //-----------EVENT-----------//
    void keyPressed(int key) {
        switch (key) {
        case 'r':
            resetCamera();
            break;
        case 'f':
            ofToggleFullscreen();
            break;
        case 's':
            gk.saveImage();
            break;
        case 'm':            
            exportDataForNextScene();
            break;
        case 'h':
            bDrawDebug = !bDrawDebug;
            if (bDrawDebug){
                ssGlobalLog << "DEBUG MODE ON" << endl;
            }
            else {
                ssGlobalLog << "DEBUG MODE OFF" << endl;
            }
            break;
        case 'b':
            addBox();
            break;
        case 'c':
            addCylinder(mouseOnWorldPlane);
            break;
        case 'a':
            addModel(mouseOnWorldPlane);
            break;
        case 'd':
            deleteModel();
            break;
        case ' ':
            connectArmsToModel();
            break;
        case 'l':
            // バッファをクリアします。
            ssGlobalLog.str("");
            // 状態をクリアします。
            ssGlobalLog.clear(std::stringstream::goodbit);
            ssGlobalLog << "CLEARED LOG" << endl;
            break;
        }
    }
    void keyReleased(int key) {
    };
	void mouseMoved(int x, int y ) {};
	void mouseDragged(int x, int y, int button) {};
	void mousePressed(int x, int y, int button) {
    };
	void mouseReleased(int x, int y, int button) {
        mousePickIndex = -1;
    };
    void mouseEntered(int x, int y) {};
    void mouseExited(int x, int y) {};
    void mouseScrolled(int x, int y, float scrollX, float scrollY) {};
	void windowResized(int w, int h) {
        gk.resizeGUI(guiOne,12);
        gk.resizeGUI(guiTwo,13);
    }
	void dragEvent(ofDragInfo dragInfo) {};
	void gotMessage(ofMessage msg) {};
   
};
