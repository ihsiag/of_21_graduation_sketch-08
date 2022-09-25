#include "ofApp.h"

std::shared_ptr<ofApp> ofApp::instance;

void ofApp::addGKScene(GKScene* _gkScene) {
		gkScenes.push_back(_gkScene);
}


//--------------------------------------------------------------
void ofApp::setup(){
	currentSceneIndex = 0;
	
	for (GKScene* gs : gkScenes) {
		glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		//ofPushStyle();
		gs->setup();
		//ofPopStyle();
		glPopAttrib();
		glPopClientAttrib();
	}
	
	
}

//--------------------------------------------------------------
void ofApp::update(){
	gkScenes[currentSceneIndex]->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	gkScenes[currentSceneIndex]->draw();
}
