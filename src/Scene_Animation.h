#pragma once
#ifndef SCENE_ANIMATION_H  
#define SCENE_ANIMATION_H 

#include "GKScene.h"
#include "ofMain.h"
#include "ofxGKUtils.h"
#include "ofxGui.h"
#include "ofEasyCam.h"

#include "Class_Delaunay.h"
#include "Class_GKDelaunay.h"
#include "Class_GKSplit.h"
#include "Class_GKNetwork.h"

#include "Class_Animation.h"


class DocumentationPart{
public:

	ofFbo* thisFbo;
	ofxGKUtils gk;
	ofEasyCam cam;
	stringstream* ssGlobalLog;
	bool bIsMouseOn;
	bool bScrollAuto;
	vector<ofImage> imgs;
	int totalImgsHeight;

	DocumentationPart(stringstream* _ssGlobalLog,ofFbo* _fbo) {
		ssGlobalLog = _ssGlobalLog;
		thisFbo = _fbo;
	}
	DocumentationPart(){}

	void setup(stringstream* _ssGlobalLog, ofFbo* _fbo){
		ssGlobalLog = _ssGlobalLog;
		thisFbo = _fbo;
		initParam();
		initSet();
		resetCamera();
		resetPart();
	}
	void resetPart() {
		initParam();
		resetCamera();
	}
	void update() {
		if(bScrollAuto)scrollCameraAuto();
	}
	void draw() {
		cam.begin();
		//glEnable(GL_DEPTH_TEST);
		ofPushMatrix();
		ofTranslate(-thisFbo->getWidth()/ 2, -thisFbo->getHeight()/ 2);

		ofFill();
		glColor3f(0.8, 0.8, 0.85);
		ofDrawRectangle(0, cam.getPosition().y, thisFbo->getWidth(),thisFbo->getHeight());
		ofNoFill();
		placeImgs();

		ofPopMatrix();
		//glDisable(GL_DEPTH_TEST);
		cam.end();
	}
	void run() {
		update();
		draw();
	}

	void initParam() {
		bIsMouseOn = false;
		bScrollAuto = true;
		loadImgs("./description/");
	}
	void initSet() {
		gk.setup(ssGlobalLog);
	}
	void resetCamera() {
		cam.enableOrtho();
		cam.setPosition(glm::vec3(0, 0, 500));
		cam.setVFlip(true);
		cam.lookAt(ofVec3f(0, 0, 0), glm::vec3(0, 1, 0));
		cam.disableMouseInput();
	}

	void loadImgs(const string& _dirPath) {
		imgs.erase(imgs.begin(), imgs.end());
		totalImgsHeight = 0;
		ofDirectory _dir(_dirPath);
		_dir.allowExt("png");//only show {}file ex)png,mp3,css
		_dir.sort();
		_dir.listDir();
		for (int i = 0; i < _dir.size(); i++) {
			ofImage _img;
			_img.load(_dir.getPath(i));
			float imgRatio = _img.getHeight() / _img.getWidth();
			totalImgsHeight += thisFbo->getWidth()*imgRatio;
			imgs.push_back(_img);
		}
	}

	void placeImgs() {
		int _counter = 0;
		for (auto& img : imgs) {
			float imgRatio = img.getHeight() / img.getWidth();
			img.draw(0, _counter*thisFbo->getWidth()*imgRatio, thisFbo->getWidth(), thisFbo->getWidth() * imgRatio);
			_counter++;
		}
	}

	bool getMouseState() { return bIsMouseOn; }
	void mousePressed(ofMouseEventArgs& args) {
		bScrollAuto = !bScrollAuto;
	}
	void mouseScrolled(int x, int y, float scrollX, float scrollY) {
		scrollCamera(scrollY);
	}
	void scrollCamera(const int& _scrollY) {
		if (_scrollY < 0)bScrollAuto = true;
		else bScrollAuto = false;
		int scrollScale = 30;
		if (cam.getPosition().y > -1) {
			cam.move(0, -1 * _scrollY * scrollScale, 0);
		}
		if (cam.getPosition().y < 0) {
			cam.setPosition(cam.getPosition().x, 0, cam.getPosition().z);
		}
	}
	void scrollCameraAuto() {
		float _scrollSpeed = 2;
		cam.move(0,  _scrollSpeed, 0);
		if (cam.getPosition().y > totalImgsHeight)cam.setPosition(0,0,500);
	}
};

class AnimationPart {
public:

	ofFbo* thisFbo;
	ofxGKUtils gk;
	ofEasyCam cam;
	stringstream* ssGlobalLog;
	bool bIsMouseOn;

	vector<ofLight> lights;
	ofMesh meshToAnimate;
	vector<GKPlane> gkPlanesCreatedFromMeshToAnimate;
	vector<GKPlane> gkPlanesCreatedManuallyToAnimate;

	AnimationClassMeshVertex amv;
	AnimationClassMeshEdge ame;
	AnimationClassMeshFace amf;
	AnimationClassPlaneFace apf;
	AnimationClassFindCombi afc;
	
	vector<AnimationClass*> animationClasses;
	unsigned long int animationFrame;
	int animationIndex;
	bool bPlayAnimation;
	bool bHideMesh;

	AnimationPart(stringstream* _ssGlobalLog,ofFbo* _fbo) {
		ssGlobalLog = _ssGlobalLog;
		thisFbo = _fbo;
	}

	AnimationPart() {}

	void setup(stringstream* _ssGlobalLog, ofFbo* _fbo) {
		ssGlobalLog = _ssGlobalLog;
		thisFbo = _fbo;
		initParam();
		initSet();
		resetCamera();
		resetPart();

		setLights();
	};
	void resetPart() {
		gkPlanesCreatedFromMeshToAnimate.erase(gkPlanesCreatedFromMeshToAnimate.begin(), gkPlanesCreatedFromMeshToAnimate.end());
		gkPlanesCreatedManuallyToAnimate.erase(gkPlanesCreatedManuallyToAnimate.begin(), gkPlanesCreatedManuallyToAnimate.end());
		gk.importGK3D(gk.findLatestFilePath("./gk3dForDemo/", "gk3d"), meshToAnimate, gkPlanesCreatedFromMeshToAnimate, gkPlanesCreatedManuallyToAnimate);
		initAnimationClasses();
	}
	void update() {
		if (animationClasses[animationIndex]->getNextAnimationTriggerState())animationIndex++; //&& animationIndex < animationClasses.size() - 1
		if (animationIndex == animationClasses.size())resetAnimationClasses();
		for (int i = 0; i < animationIndex + 1; i++) {
			animationClasses[i]->update();
		}
		rotateCamera();
		animationFrame++;
	}
	void draw() {
		/*
		if (animationIndex > animationClasses.size()) {
			resetAnimationClasses();
			animationIndex = 0;
		}
		if (animationIndex > 3) {
			for (int i = 3; i < animationIndex + 1; i++) {
				animationClasses[i]->draw();
			}
		}
		else {
			for (auto& ac : animationClasses) { ac->draw(); }
		}*/

		for (auto& ac : animationClasses) { ac->draw(); }

		if (!bHideMesh)drawMainMesh();
	}

	void run() {
		cam.begin();
		glEnable(GL_DEPTH_TEST);
		ofEnableLighting();
		for (auto& l : lights)l.enable();
		//gk.draw3DPlaneGrid(10,50,glm::vec3(0,1,0),1,glm::vec4(0.6));
		gk.draw3DBox(glm::vec3(0), 100, 100, 160, 1, glm::vec4(1, 1, 1, 0.5));
		if (bPlayAnimation)update();
		draw();
		for (auto& l : lights)l.disable();
		ofDisableLighting();
		glDisable(GL_DEPTH_TEST);
		cam.end();
	}

	void initParam() {
		bIsMouseOn = false;
		bPlayAnimation = true;
		animationFrame = 0;
		animationIndex = 0;
		bHideMesh = false;
		
	};
	void initSet() {
		gk.setup(ssGlobalLog);
		gk.setCam(&cam);
	}
	void resetCamera() {
		//cam.setDistance(14);
		//cam.enableOrtho();
		cam.setPosition(glm::vec3(180, 180, 180));
		//cam.setVFlip(true);
		cam.lookAt(ofVec3f(0, 0, 0), glm::vec3(0, 1, 0));
	}
	bool getMouseState() { return bIsMouseOn; }

	void initAnimationClasses() {
		animationClasses.push_back(&amv);
		animationClasses.push_back(&ame);
		animationClasses.push_back(&amf);
		animationClasses.push_back(&apf);
		animationClasses.push_back(&afc);

		amv.setGPL(&gkPlanesCreatedFromMeshToAnimate);
		ame.setGPL(&gkPlanesCreatedFromMeshToAnimate);
		amf.setGPL(&gkPlanesCreatedFromMeshToAnimate);
		apf.setGPL(&gkPlanesCreatedFromMeshToAnimate);
		afc.setGPL(&gkPlanesCreatedFromMeshToAnimate);
		afc.setCam(&cam);
		afc.setHideMesh(&bHideMesh);

		for (auto& ac : animationClasses) {
			ac->setup();
		}
	}
	void resetAnimationClasses() {
		animationIndex = 0;
		for (auto& ac : animationClasses) {
			ac->resetAnimation();
		}
	}
	void toggleAnimate() {
		bPlayAnimation = !bPlayAnimation;
	}

	void setLights() {
		lights.emplace_back(ofLight());
		lights.emplace_back(ofLight());
		float rotateR = sqrt(pow(140, 2) * 2);
		lights[0].setPosition(rotateR * cos(PI / 4), cam.getPosition().y, rotateR * sin(PI / 4));
		lights[1].setPosition(rotateR * cos(PI / 2), cam.getPosition().y, rotateR * sin(PI / 4));
	}

	void rotateCamera() {
		float rotateR = sqrt(pow(140, 2) * 2);
		float rotationSpeed = animationFrame * 0.002;
		cam.setPosition(glm::vec3(rotateR * cos(PI / 4 + rotationSpeed), 100, rotateR * sin(PI / 4 + rotationSpeed)));
		cam.lookAt(ofVec3f(0, 0, 0), glm::vec3(0, 1, 0));
	}
	
	void drawMainMesh() {
		glColor4f(0, 0, 1, 0.4);
		meshToAnimate.drawFaces();
		glLineWidth(1);
		glColor4f(0, 1, 0, 0.4);
		meshToAnimate.drawWireframe();
	}
};


class Scene_Animation : public GKScene {
public:
	ofxGKUtils gk;
	stringstream ssGlobalLogAnimation;
	stringstream ssGlobalLogDocumentation;
	unsigned long int currentFrame;
	float time;


	ofFbo animationScreen;
	ofFbo documentationScreen;

	AnimationPart animationPart;
	DocumentationPart documentationPart;
	
	float infoBarAlpha;

	void setup() {
		initSet();
		resizeFBO();
		animationPart.setup(&ssGlobalLogAnimation, &animationScreen);
		documentationPart.setup(&ssGlobalLogDocumentation, &documentationScreen);
	}
		
	void resetScene() {
		initParam();
		resetCamera();
		resizeFBO();
		animationPart.setup(&ssGlobalLogAnimation, &animationScreen);
		documentationPart.setup(&ssGlobalLogDocumentation, &documentationScreen);
		ssGlobalLogAnimation.str("");
		ssGlobalLogAnimation.clear(std::stringstream::goodbit);
		ssGlobalLogAnimation << "CLEARED LOG" << endl;
		ssGlobalLogAnimation << "CLEARED-ARRAYS" << endl;
		ssGlobalLogDocumentation.str("");
		ssGlobalLogDocumentation.clear(std::stringstream::goodbit);
		ssGlobalLogDocumentation << "CLEARED LOG" << endl;
		ssGlobalLogDocumentation << "CLEARED-ARRAYS" << endl;
	}
	void update() {
		gk.defaultUpdate(&currentFrame, &time);
		glClear(GL_DEPTH_BUFFER_BIT);
		animationScreen.begin();
		ofClear(0);
		//glClearColor(0, 0, 0, 0);
		//glClear(GL_COLOR_BUFFER_BIT);
		//glColor3f(0, 0, 0);
		animationPart.run();
		animationScreen.end();
		documentationScreen.begin();
		ofClear(0);
		documentationPart.run();
		documentationScreen.end();
	}
	void draw() {
		//-----------INIT-----------//
		stringstream ssInstructDocumentation, ssInstructAnimation;
		stringstream ssProgramInfoDocumentation, ssProgramInfoAnimation;
		stringstream ssDebugDocumentation, ssDebugAnimation;
		//-----------PROCESS-----------//
		//-----------3D-LAYER-----------//
		//-----------2D-LAYER-----------//
		animationScreen.draw(0, 0, animationScreen.getWidth(), animationScreen.getHeight());
		documentationScreen.draw(animationScreen.getWidth(), 0, documentationScreen.getWidth(),documentationScreen.getHeight());
		gk.drawGrid();
		//-----------INFO-----------//
		createInfoDocumentation(ssInstructDocumentation, ssProgramInfoDocumentation, ssDebugDocumentation);
		createInfoAnimation(ssInstructAnimation, ssProgramInfoAnimation, ssDebugAnimation);
		infoBarAlpha = 0.4 *cos(-PI/2 + currentFrame*0.02) + 0.4;
		//createInfoBar();

		//-----------FRONT-LAYER-----------//
		glColor3f(0, 0, 0);
		gk.drawInfo(ssInstructDocumentation, 9, true, glm::vec2(60, 0));
		gk.drawInfo(ssProgramInfoDocumentation, 8, true, glm::vec2(60, 60));
		gk.drawInfo(ssDebugDocumentation, 12, true, glm::vec2(60,0));
		glColor3f(1, 1, 1);
		gk.drawInfo(ssInstructAnimation, 1);
		gk.drawInfo(ssProgramInfoAnimation, 0);
		gk.drawInfo(ssDebugAnimation, 4);
		//gk.drawInfo(ssGlobalLog, 5);
	}

	
	void initParam() {
		infoBarAlpha = 0;
	}
	void initSet() {
		//gk.setup(&ssGlobalLog);
	}
	void initSliders() {};
	void resetCamera() {};
	void resizeFBO() {
		animationScreen.allocate(ofGetWidth() / 2, ofGetHeight(), GL_RGB, 4);
		documentationScreen.allocate(ofGetWidth() / 2, ofGetHeight(), GL_RGB, 8);
	}
	void createInfoAnimation(stringstream& _ssInstruct, stringstream& _ssProgramInfo, stringstream& _ssDebug) {
		//-----------INFO-----------//--later put into update func.
		_ssInstruct << "INSTRUCTIONS: " << endl;
		_ssInstruct << ">RESET: R" << endl;

		_ssProgramInfo << "PROGRAM: " << "DEMO" << endl;
		_ssProgramInfo << "DEVELOPER: " << "GAISHI KUDO" << endl;
		_ssProgramInfo << "TIME: " << ofToString(time, 0) << endl;
	}

	void createInfoDocumentation(stringstream& _ssInstruct, stringstream& _ssProgramInfo, stringstream& _ssDebug) {
		//-----------INFO-----------//--later put into update func.
		_ssInstruct << "INSTRUCTIONS: " << endl;
		_ssInstruct << ">SCROLL               - MOUSE MIDDLE " << endl;
		_ssInstruct << ">STOP / START TO READ - MOUSE LEFT " << endl;

		_ssProgramInfo << "PROGRAM: " << "DOCUMENTATION" << endl;
		_ssProgramInfo << "AUTHOR: " << "GAISHI KUDO" << endl;
		_ssProgramInfo << "TIME: " << ofToString(time, 0) << endl;
	}


	void createInfoBar() {
		
		
		string infoToShow = "PRESS KEY < -/ ->TO CHANGE SCENES";
		ofRectangle bb = getBitmapStringBoundingBox(infoToShow);

		ofFill();
		glColor4f(0, 0, 0,infoBarAlpha);
		ofDrawRectangle(0, ofGetHeight() / 2, ofGetWidth(), bb.height);
		glColor4f(0.9, 0.9, 0.95,infoBarAlpha);
		ofDrawBitmapString( infoToShow, ofGetWidth()/2-bb.width/2, ofGetHeight()/2  + 11);
	}
	ofRectangle getBitmapStringBoundingBox(string text) {
		vector<string> lines = ofSplitString(text, "\n");
		int maxLineLength = 0;
		for (int i = 0; i < (int)lines.size(); i++) {
			// tabs are not rendered
			const string& line(lines[i]);
			int currentLineLength = 0;
			for (int j = 0; j < (int)line.size(); j++) {
				if (line[j] == '\t') {
					currentLineLength += 8 - (currentLineLength % 8);
				}
				else {
					currentLineLength++;
				}
			}
			maxLineLength = MAX(maxLineLength, currentLineLength);
		}

		int padding = 4;
		int fontSize = 8;
		float leading = 1.7;
		int height = lines.size() * fontSize * leading - 1;
		int width = maxLineLength * fontSize;
		return ofRectangle(0, 0, width, height);
	}

	//-----------NO-InUSE-----------//


	void keyPressed(int key) {
		switch (key) {
		case 'f':
			ofToggleFullscreen();
			break;
		case 's':
			animationPart.toggleAnimate();
			break;
		case 'r':
			animationPart.resetAnimationClasses();
			documentationPart.resetPart();
			break;
		}
	}
	void mousePressed(ofMouseEventArgs& args) { 
		documentationPart.mousePressed(args);
	}
	void mousePressed(int x, int y, int button) {
	};
	void mouseScrolled(int x, int y, float scrollX, float scrollY) {
		documentationPart.mouseScrolled(x, y, scrollX, scrollY);
	};
	void windowResized(int w, int h) {
		//gk.resizeGUI(guiOne, 13);
		//gk.resizeGUI(guiTwo, 13);
		resizeFBO();
	}

	//-----------NO-InUSE-----------//
	void keyReleased(int key) {};
	void mouseMoved(int x, int y) {};
	void mouseDragged(int x, int y, int button) {};
	void mouseReleased(int x, int y, int button) {};
	void mouseEntered(int x, int y) {};
	void mouseExited(int x, int y) {};
	
	void dragEvent(ofDragInfo dragInfo) {};
	void gotMessage(ofMessage msg) {};

private:

};

#endif
