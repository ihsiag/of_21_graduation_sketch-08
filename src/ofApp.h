#pragma once

#include "ofMain.h"
#include "GKScene.h"
#include "ofxGKUtils.h"

class ofApp : public ofBaseApp{
	//*work as scene manager *//
private:
	ofApp() {}
	~ofApp() {
		for (GKScene* gs : gkScenes) {
			delete gs;
		}
	}
	static std::shared_ptr<ofApp> instance;
public:
	static std::shared_ptr<ofApp> getInstance() {
		if (instance.get() == 0) instance.reset(new ofApp(), [](ofApp* p) { delete p; });
		return instance;
	}
	vector<GKScene*> gkScenes;
	void addGKScene(GKScene* _gkScene);

	int currentSceneIndex;
	float currentSceneSec;
	float currentSceneFloatFrame;
	int currentSceneFrame;
	float sceneStartSec;
	float lastSec;
	

	void setup();
	void update();
	void draw();

	void keyPressed(int key) {
		gkScenes[currentSceneIndex]->keyPressed(key);
		switch (key) {
		case OF_KEY_RIGHT:
			currentSceneIndex++;
			currentSceneIndex %= gkScenes.size();
			cout << "Scene changed" << endl;
			gkScenes[currentSceneIndex]->resetScene();
			sceneStartSec = ofGetElapsedTimef();
			break;

		case OF_KEY_LEFT:
			currentSceneIndex--;
			if (currentSceneIndex < 0) currentSceneIndex += gkScenes.size();
			cout << "Scene changed" << endl;
			gkScenes[currentSceneIndex]->resetScene();
			sceneStartSec = ofGetElapsedTimef();
			break;
		}
	};
	void keyReleased(int key) {
		gkScenes[currentSceneIndex]->keyReleased(key);
	};
	void mouseMoved(int x, int y) {
		gkScenes[currentSceneIndex]->mouseMoved(x, y);
	};
	void mouseDragged(int x, int y, int button) {
		gkScenes[currentSceneIndex]->mouseDragged(x, y,button);
	};
	void mousePressed(ofMouseEventArgs& args) {
		gkScenes[currentSceneIndex]->mousePressed(args);
	}
	void mousePressed(int x, int y, int button) {
		gkScenes[currentSceneIndex]->mousePressed(x, y, button);
	};
	void mouseReleased(int x, int y, int button) {
		gkScenes[currentSceneIndex]->mouseReleased(x, y, button);
	};
	void mouseEntered(int x, int y) {
		gkScenes[currentSceneIndex]->mouseEntered(x, y);
	};
	void mouseExited(int x, int y) {
		gkScenes[currentSceneIndex]->mouseExited(x, y);
	};
	void mouseScrolled(int x, int y, float scrollX, float scrollY) {
		gkScenes[currentSceneIndex]->mouseScrolled(x, y, scrollX, scrollY);
	};

	void windowResized(int w, int h) {
		gkScenes[currentSceneIndex]->windowResized(w, h);
	};
	void dragEvent(ofDragInfo dragInfo) {
		gkScenes[currentSceneIndex]->dragEvent(dragInfo);
	};
	void gotMessage(ofMessage msg) {
		gkScenes[currentSceneIndex]->gotMessage(msg);
	};
};
