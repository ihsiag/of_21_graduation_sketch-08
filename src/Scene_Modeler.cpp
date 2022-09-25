#include "Scene_Modeler.h"


void Scene_Modeler::setup(){   
    initParam();
    initSet();
    initSliders();
    resetCamera();
    importMesh();  
}

void Scene_Modeler::resetScene() {
    initParam();
    resetCamera();

    gkPlanes.erase(gkPlanes.begin(), gkPlanes.end());
    gkPlanesSplittedByDelaunay.erase(gkPlanesSplittedByDelaunay.begin(), gkPlanesSplittedByDelaunay.end());
    gkPlanesSplittedByCombi.erase(gkPlanesSplittedByCombi.begin(), gkPlanesSplittedByCombi.end());
    gkPlanesFinal.erase(gkPlanesFinal.begin(), gkPlanesFinal.end());
    gkIntersectLines.erase(gkIntersectLines.begin(), gkIntersectLines.end());
    bModified = false;
    ssGlobalLog.str("");
    ssGlobalLog.clear(std::stringstream::goodbit);
    ssGlobalLog << "CLEARED LOG" << endl;
    ssGlobalLog << "CLEARED-ARRAYS" << endl;

    mainMesh.clear();
    importMesh();
}

void Scene_Modeler::update(){
    gk.defaultUpdate(&cam, &currentFrame, &time);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Scene_Modeler::draw(){
    //-----------INIT-----------//
    stringstream ssInstruct;
    stringstream ssProgramInfo;
    stringstream ssDebug;

    //-----------PROCESS-----------//
    if (!bModified)modifyMesh();
    if (bModified)checkVerticesHolder();
    
    //-----------3D-LAYER-----------//
    cam.begin();  
    glEnable(GL_DEPTH_TEST);
    gk.draw3DAxis();
    gk.draw3DPlaneGrid(10, 50, glm::vec3(0, 1, 0), 1, glm::vec4(glm::vec3(0.3), 1));
    if (!bHideLight) {
        ofEnableLighting();
        light.enable();
    }
    if (!bModified)draw3DBeforeModified();
    if (bModified)draw3DAfterModified();
    if (!bHideLight) {
        light.disable();
        ofDisableLighting();
    }
    glDisable(GL_DEPTH_TEST);
    cam.end();
    
    //-----------2D-LAYER-----------//
    if (bModified && !bManualMode)selectingVertexPos = getCurrentVertexMesh(mainMesh, ssDebug);
    if (bManualMode) selectingVertexPos = getCurrentVertexGKPlane(gkPlanesSplittedByCombi);

    //-----------INFO-----------//
    createInfo(ssInstruct, ssProgramInfo, ssDebug);

    //-----------FRONT-LAYER-----------//
    gk.drawGrid();
    gk.drawInfo(ssInstruct, 1);
    gk.drawInfo(ssProgramInfo, 0);
    gk.drawInfo(ssDebug, 4);
    gk.drawInfo(ssGlobalLog, 5);
    if (!bModified)guiOne.draw();
    else guiTwo.draw();
    drawCamPosition();
}

//-----------FOR-LIB-----------//
//addGKPlane();

//-----------THIS-TIME-INITS-----------//
void Scene_Modeler::initParam(){
    modifyInfo = ofNode();
    
    modifyInfo.setPosition(0, 0, 0);
    bModified = false;
    bManualMode = false;
    bDebug = false;
    bHideMainMesh = false;
    bHideAddedMesh = true;// false;
    bHideGKPlane = true;// false;
    bHideNetwork = false;
    bHideGKPlaneSplittedByDelaunay = false;
    bHideGKPlaneSplittedByCombi = false;
    bHideGKPlaneFinal = false;
    bHideSelectedPoint = false;
    bHideLight = false;

    verticesPosHolder.reserve(3);

    light.setPosition(80, 80, 80);
    light.setScale(glm::vec3(50));
}

void Scene_Modeler::initSet() {
    gk.setup(&ssGlobalLog);
    gk.setCam(&cam);
    gk.setGUI(guiOne,13);
    gk.setGUI(guiTwo,13);
}

void Scene_Modeler::initSliders() {
    guiOne.add(rotationSlider.set("ROTATION", glm::vec3(0), glm::vec3(-PI), glm::vec3(PI)));
    
    guiTwo.add(mainMeshAlphaSlider.set("MAIN-MESH ALPHA", 0.6, 0, 1));
    guiTwo.add(selectWhichiPlanesToDelete.set("SEL WHICH PLANES TO DEL", 0, 0, 4));
}

void Scene_Modeler::resetCamera() {
    cam.setDistance(14);
    //cam.enableOrtho();
    cam.setPosition(glm::vec3(180, 180, 180));
    //cam.setVFlip(true);
    cam.lookAt(ofVec3f(0, 0, 0), glm::vec3(0, 1, 0));
}


//-----------THIS-TIME-UTILS-----------//

void Scene_Modeler::drawCamPosition() {
    gk.setGraphGUI(12, glm::vec2(ofGetHeight() * 0.25 - 60), glm::vec2(10 * 50));
    gk.drawEachDataOnGraphGUI(12, glm::vec2(ofGetHeight() * 0.25 - 60), glm::vec2(10 * 50 * 2), cam.getPosition(), glm::vec3(0, 1, 0));
}

void Scene_Modeler::createInfo(stringstream& _ssInstruct, stringstream& _ssProgramInfo, stringstream& _ssDebug) {
    //-----------INFO-----------//--later put into update func.
    _ssInstruct << "INSTRUCTIONS: " << endl;
    _ssInstruct << "> DEBUG                 - H" << endl;
    _ssInstruct << "> FULL-SCREEN           - F" << endl;
    _ssInstruct << "> ROTATE CAMERA         - RIGHT-BUTTON" << endl;
    _ssInstruct << "> MOVE CAMERA           - SHIFT + RIGHT-BUTTON" << endl;
    _ssInstruct << "> RESET CAMERA          - R" << endl;
    _ssInstruct << "> UPDATE MESH           - U" << endl;
    _ssInstruct << "> EXPORT GKPLANES-MODEL - E" << endl;
    _ssInstruct << "> IMPORT GKPLANES-MODEL - I" << endl;
    _ssInstruct << "> HIDE MAIN-MESH        - 1" << endl;
    _ssInstruct << "> HIDE ADDED-MESH       - 2" << endl;
    _ssInstruct << "> HIDE ADDED-PLANE      - 3" << endl;
    _ssInstruct << "> HIDE NETWORK          - 4" << endl;
    _ssInstruct << "> HIDE SP-PLANE-BY-DELA - 5" << endl;
    _ssInstruct << "> HIDE SP-PLANE-BY-COMBI- 6" << endl;
    _ssInstruct << "> HIDE FINAL-PLANE      - 7" << endl;
    _ssInstruct << "> HIDE SELECTED-POINT   - 8" << endl;
    _ssInstruct << "> HIDE LIGHT            - 9" << endl;
    _ssInstruct << "> BACK                  - Z" << endl;
    _ssInstruct << "> CLEAR GLOBAL LOG      - L" << endl;
    _ssInstruct << "> CLEAR ALL             - C" << endl;
    _ssInstruct << "> SAVE IMG              - S" << endl;
    _ssInstruct << "> SAVE GENERATED-MESH   - M" << endl;
    _ssInstruct << "> SPLIT BY DELA         - SPACE-BAR" << endl;
    _ssInstruct << "> SPLIT BY COMBI        - B" << endl;
    _ssInstruct << "> MANUAL-MODE           - Q" << endl;
    

    _ssProgramInfo << "PROGRAM: " << "SIMULATED EXPERIENCE OR DREAM" << endl;
    _ssProgramInfo << "DEVELOPER: " << "GAISHI KUDO" << endl;
    _ssProgramInfo << "TIME: " << ofToString(time, 0) << endl;
    _ssProgramInfo << "FRAMERATE: " << ofToString(ofGetFrameRate(), 0) << endl;
    _ssProgramInfo << "CAMERA: " << cam.getPosition() << endl;
    _ssProgramInfo << "CAMERA LOOK DIR: " << cam.getLookAtDir() << endl;
    
    _ssDebug << "DEBUG STATE: " << bDebug << endl;
    _ssDebug << "MANUAL-MODE STATE: " << bManualMode << endl;
    _ssDebug << "HIDE MAIN-MESH STATE: " << bHideMainMesh << endl;
    _ssDebug << "HIDE ADDED-MESH STATE: " << bHideAddedMesh << endl;
    _ssDebug << "HIDE ADDED-PLANE STATE: " << bHideGKPlane << endl;
    _ssDebug << "HIDE NETWORK STATE: " << bHideNetwork << endl;
    _ssDebug << "HIDE SP-PLANE-BY-DELA STATE: " << bHideGKPlaneSplittedByDelaunay << endl;
    _ssDebug << "HIDE SP-PLANE-BY-COMBI STATE: " << bHideGKPlaneSplittedByCombi << endl;
    _ssDebug << "HIDE FINAL-PLANE STATE: " << bHideGKPlaneFinal << endl;
    _ssDebug << "HIDE SELECTED-POINT STATE: " << bHideSelectedPoint << endl;
    _ssDebug << "HIDE LIGHT STATE: " << bHideLight << endl;
    _ssDebug << "CURRENT MY-PLANE NUM: " << gkPlanes.size() << endl;
    _ssDebug << "CURRENT MY-PLANE-NEW NUM: " << gkPlanesSplittedByDelaunay.size() << endl;
    
    if (selectWhichiPlanesToDelete == 0) {
        _ssDebug << "CURRENT PLANES TO DEL: " << "ADDED-PLANE" << endl;
    }
    if (selectWhichiPlanesToDelete == 1) {
        _ssDebug << "CURRENT PLANES TO DEL: " << "SP-PLANE=BY-DELA" << endl;
    }
    if (selectWhichiPlanesToDelete == 2) {
        _ssDebug << "CURRENT PLANES TO DEL: " << "SP-PLANE-BY-COMBI" << endl;
    }
    if (selectWhichiPlanesToDelete == 3) {
        _ssDebug << "CURRENT PLANES TO DEL: " << "FINAL-PLANE" << endl;
    }
}

void Scene_Modeler::loadLatestMesh(const string& _dirName, ofMesh* _mesh) {
    ofDirectory _dir(_dirName);

    _dir.allowExt("ply");//only show {}file ex)png,mp3,css
    _dir.sort();
    _dir.listDir();
    if (_dir.size() > 0) {
        _mesh->load(_dir.getPath(_dir.size() - 1));
    }
    ssGlobalLog << "IMPORTED MESH" << endl;
}

ofMesh Scene_Modeler::getMeshFromGKPlanes(vector<GKPlane>* _gkPlanes) {
    ofMesh _meshContainer;
    for (auto gp : *_gkPlanes) {
        _meshContainer.append(gp.convertToMesh());
    }
    return _meshContainer;
}

//-----------THIS-TIME-FUNCS-----------//
void Scene_Modeler::importMesh() {
    loadLatestMesh("./meshExportedFromViewerToModeler/", &mainMesh);
}

void Scene_Modeler::modifyMesh() {
    glm::vec3 _theta = rotationSlider.get();
    glm::quat _quat = glm::quat(sin(_theta.x / 2), 0, 0, cos(_theta.x / 2)) * glm::quat(0, sin(_theta.y / 2), 0, cos(_theta.y / 2)) * glm::quat(0, 0, sin(_theta.z / 2), cos(_theta.z / 2));
    modifyInfo.setGlobalOrientation(_quat);
}

void Scene_Modeler::updateMesh() {
    mainMesh = gk.getModifiedMesh(mainMesh,modifyInfo);
    //gk.saveMesh(getModifiedMesh(&mainMesh), 1);
    //mainMesh.clear();
    //loadLatestMesh("./meshExport/", &mainMesh);
    modifyInfo.resetTransform();
    bModified = true;
}

glm::vec3 Scene_Modeler::getCurrentVertexMesh(const ofMesh& _mesh, stringstream& _ssDebug) {
    float nearestDistance = 0;
    glm::vec3 nearestVertex3D;
    glm::vec2 nearestVertex2D;
    int nearestIndex = 0;
    glm::vec3 mouse(ofGetMouseX(), ofGetMouseY(),0);
    glm::vec3 _camPos = cam.getPosition();
    for (int i = 0; i < _mesh.getNumVertices(); i++) {       
        glm::vec3 _vertPos = _mesh.getVertex(i);
        glm::vec3 _vertNormal = _mesh.getNormal(i);
        //if(glm::dot(_camPos-_vertPos,_vertNormal)>0){
            glm::vec3 cur = cam.worldToScreen(_vertPos);
            float distance = glm::distance(cur, mouse);
            if (i == 0 || distance < nearestDistance) {
                nearestDistance = distance;
                nearestVertex3D = _vertPos;
                nearestVertex2D = cur;
                nearestIndex = i;
            }
        //}
    }

    /*
    glLineWidth(1);
    glColor3f(0, 1, 0);
    ofDrawLine(nearestVertex2D, mouse);
    */

    ofNoFill();
    ofSetColor(ofColor::yellow);
    ofSetLineWidth(2);
    ofDrawCircle(nearestVertex2D, 4);
    ofSetLineWidth(1);

    glm::vec2 offset(10, -10);
    ofDrawBitmapStringHighlight(ofToString(nearestIndex), mouse + offset);
    _ssDebug << "VERTEX-POSITION: " << nearestVertex3D << endl;
    return nearestVertex3D;
}

glm::vec3 Scene_Modeler::getCurrentVertexGKPlane(const vector<GKPlane>& _gkPlanes) {
    float nearestDistance;
    glm::vec3 nearestVertex3D;
    glm::vec2 nearestVertex2D;
    int nearestIndex = 0;
    glm::vec3 mouse(ofGetMouseX(), ofGetMouseY(), 0);
    int indexCounter = 0;
    for (auto& gp : _gkPlanes) {
        for (auto& v : gp.vertices) {
            glm::vec3 cur = cam.worldToScreen(v);
            float distance = glm::distance(cur, mouse);
            if (indexCounter == 0 || distance < nearestDistance) {
                nearestDistance = distance;
                nearestVertex3D = v;
                nearestVertex2D = cur;
                nearestIndex = indexCounter;
            }
            indexCounter++;
        }
    }
    ofNoFill();
    ofSetColor(ofColor::yellow);
    ofSetLineWidth(2);
    ofDrawCircle(nearestVertex2D, 4);
    ofSetLineWidth(1);

    glm::vec2 offset(10, -10);
    ofDrawBitmapStringHighlight(ofToString(nearestIndex), mouse + offset);
    return nearestVertex3D;
}

void Scene_Modeler::checkVerticesHolder() {
    if (verticesPosHolder.size() > 2) {
        addGKPlane();
    }
}

void Scene_Modeler::addGKPlane() { 
    if (not bManualMode) {
        ofMesh _mesh;
        _mesh.addVertices(verticesPosHolder);
        gkPlanes.emplace_back(_mesh, gkPlanes.size() + 1);
        ssGlobalLog << "ADDED GKPLANE" << endl;
    }
    else {
        gkPlanesFinal.emplace_back(verticesPosHolder, 0);
        ssGlobalLog << "ADDED GKPLANE-FINAL" << endl;
    }
    verticesPosHolder.erase(verticesPosHolder.begin(), verticesPosHolder.end());
    
}

void Scene_Modeler::draw3DBeforeModified() {
    ofPushMatrix();
    ofMultMatrix(modifyInfo.getGlobalTransformMatrix());
    if (mainMesh.hasVertices()) {
        drawMainMesh();
    }
    if (bDebug) debugDot();
    ofPopMatrix();
}

void Scene_Modeler::draw3DAfterModified() {
    drawNetwork();
    drawGKPlanes();
    if(!bHideGKPlaneSplittedByDelaunay)drawGKPlanesSplittedByDelaunay();
    if (!bHideGKPlaneSplittedByCombi)drawGKPlanesSplittedByCombi();
    if (!bHideGKPlaneFinal)drawGKPlanesFinal();
    if (bDebug) {
        drawgkIntersectLines();
        //debugDot();
    }
    if (!bHideMainMesh)drawMainMesh();
}

void Scene_Modeler::drawMainMesh() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glLineWidth(1);
    glPointSize(3);
    glColor4f(0.4, 0.5, 0.6, mainMeshAlphaSlider);
    if (!bModified)glColor3f(0.5, 0, 0);
    mainMesh.draw();
    glColor3f(0.7, 0.7, 0.8);
    if (!bModified) glColor3f(0.8, 0, 0);
    mainMesh.drawWireframe();
    /*
    glColor3f(0.8, 0.8, 0.8);
    if (!bModified) glColor3f(0.8, 0, 0);
    mainMesh.drawVertices();
    */
    glPointSize(10);
    glColor3f(0, 1, 0);
    glBegin(GL_POINTS);
    for (auto& _v : verticesPosHolder) {
        glVertex3f(_v.x, _v.y, _v.z);
    }
    glEnd();
    glDisable(GL_CULL_FACE);
}

void Scene_Modeler::drawGKPlanes() {
    for (auto& gkPlane : gkPlanes) {
        if (!bHideAddedMesh)gkPlane.drawInputMesh();
        if (!bHideGKPlane)gkPlane.drawGKPlane();
        if (bDebug) {
            gkPlane.drawGKPlaneNormal();
            gkPlane.drawGKPlaneCentroid();
        }
        if(!bHideSelectedPoint)gkPlane.drawInputMeshVertices();
    }
}

void Scene_Modeler::setGKSplits() {
    gkPlanesSplittedByDelaunay.erase(gkPlanesSplittedByDelaunay.begin(), gkPlanesSplittedByDelaunay.end());
    sort(gkPlanes.begin(), gkPlanes.end());
    ssGlobalLog << "PLANES NUM: " << gkPlanes.size() << endl;
    gkDelaTriangles.erase(gkDelaTriangles.begin(), gkDelaTriangles.end());
    gkDelaTriangles = gkDela.getDelaunayTriangles(gkPlanes);
    ssGlobalLog << "GK-DELA-TRIANGLES NUM: " << gkDelaTriangles.size() << endl;

    gkSplits.erase(gkSplits.begin(), gkSplits.end());
    for (auto& gpl : gkPlanes) {
        gkSplits.emplace_back(gpl);
    }
    for (auto& gdt : gkDelaTriangles) {
        gkSplits[gdt.vertices[0].state - 1].addCutterPlane(gkPlanes[gdt.vertices[1].state - 1]);
        gkSplits[gdt.vertices[0].state - 1].addCutterPlane(gkPlanes[gdt.vertices[2].state - 1]);
        gkSplits[gdt.vertices[1].state - 1].addCutterPlane(gkPlanes[gdt.vertices[0].state - 1]);
        gkSplits[gdt.vertices[1].state - 1].addCutterPlane(gkPlanes[gdt.vertices[2].state - 1]);
        gkSplits[gdt.vertices[2].state - 1].addCutterPlane(gkPlanes[gdt.vertices[1].state - 1]);
        gkSplits[gdt.vertices[2].state - 1].addCutterPlane(gkPlanes[gdt.vertices[0].state - 1]);
    }
}

void Scene_Modeler::runGKSplits() {
    gkIntersectLines.erase(gkIntersectLines.begin(), gkIntersectLines.end());
    for (auto& gks : gkSplits) {
        gks.splitExcute(&gkPlanesSplittedByDelaunay,&gkIntersectLines);
    }
}

void Scene_Modeler::drawGKPlanesSplittedByDelaunay() {
    for (auto& gpsd : gkPlanesSplittedByDelaunay) {
        gpsd.drawGKPlane(glm::vec4(0.6, 0.0, 0.2, 0.9), glm::vec4(0.6, 0.8, 0.2, 1), 2);
    }
}

void Scene_Modeler::splitIntersectPlanes(GKPlane* _planeB, GKPlane* _planeA,vector<GKLineSimple>* _intersectLines) {
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

void Scene_Modeler::runSplitByCombi() {
    gkSplits.erase(gkSplits.begin(), gkSplits.end());
    gkPlanesSplittedByCombi.erase(gkPlanesSplittedByCombi.begin(), gkPlanesSplittedByCombi.end());
    gkPlanesSplittedByCombi = gkPlanes;
    for (auto& gkpsbc : gkPlanesSplittedByCombi) {
        for (auto& edge : gkpsbc.edges) {
            gkSplitUtil.scalePlaneEdge(&edge, gkpsbc.centroid, 2);
        }
    }
    //gkPlanesSplittedByCombi = gkPlanesSplittedByDelaunay;
    for (auto& combi : gk.getIndexList_nC2(gkPlanesSplittedByCombi.size())) {
        splitIntersectPlanes(&gkPlanesSplittedByCombi[combi.x], &gkPlanesSplittedByCombi[combi.y],&gkIntersectLines);
    }
}

void Scene_Modeler::drawGKPlanesSplittedByCombi() {
    for (auto& gpc : gkPlanesSplittedByCombi) {
        gpc.drawGKPlane();
    }
}

void Scene_Modeler::drawGKPlanesFinal() {
    for (auto& gpf : gkPlanesFinal) {
        gpf.drawGKPlane();
    }
}


//-----------DEBUG-----------//
void Scene_Modeler::debugDot() {
    for (int i = 0; i < mainMesh.getNumVertices(); i++) {
        glm::vec3 _camPos = cam.getPosition();
        glm::vec3 _vertPos = mainMesh.getVertex(i);
        glm::vec3 _vertNormal = mainMesh.getNormal(i);
        glLineWidth(0.5);
        glColor4f(1, 0, 1, 0.4);
        ofDrawLine(_camPos, _vertPos);
        if (glm::dot(_camPos - _vertPos, _vertNormal) < 0) {
            glColor4f(1, 0, 0, 0.8);
        }
        else {
            glColor4f(0, 0, 1, 0.8);
        }
        ofDrawLine(_vertPos, _vertPos + _vertNormal * 10);
        glPointSize(7);
        ofFill();
        glBegin(GL_POINTS);
        glVertex3f(_vertPos.x, _vertPos.y, _vertPos.z);
        glEnd();
    }
}

void Scene_Modeler::drawNetwork() {
    if (!bHideNetwork) {
        if (gkDelaTriangles.size()) {
            int counter = 0;
            for (auto& gdt : gkDelaTriangles) {
                glLineWidth(0.5);
                ofFill();
                glColor4f(0.2, 0.2, 0.9, 1);
                glBegin(GL_LINE_LOOP);
                for (auto& v : gdt.vertices) {
                    glVertex3f(v.pos.x, v.pos.y, v.pos.z);
                }
                glEnd();
                cam.end();
                ofDrawBitmapString(ofToString(counter), cam.worldToScreen(gdt.getCentroid().pos));
                cam.begin();
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

void Scene_Modeler::drawgkIntersectLines() {
    for (auto& intersectLine : gkIntersectLines) {
        glLineWidth(3);
        glColor3f(0.4, 0.4, 0.9);
        intersectLine.drawLine();
        glPointSize(4);
        intersectLine.drawVertices();
    }
}
//--------------------------------------------------------------

//-----------THISTIME-SCENE-BEIDGE-----------//
void Scene_Modeler::exportDataForNextScene() {
    meshToSave = getMeshFromGKPlanes(&gkPlanesSplittedByCombi);
    meshToSave.append(getMeshFromGKPlanes(&gkPlanesFinal));
    meshToSave.append(getMeshFromGKPlanes(&gkPlanesSplittedByDelaunay));
    gk.saveMesh(meshToSave, 1, "./meshExportedFromModeler/");
};

void Scene_Modeler::saveGK3D() {
    vector<GKPlane> gkPlanesToExport;
    //gkplanes + gkPlanesFinal
    //copy(gkPlanes.begin(), gkPlanes.end(), gkPlanesToExport);
    //gkPlanesToExport.insert(gkPlanesToExport.end(), gkPlanesFinal.begin(), gkPlanesFinal.end());
    gk.saveGK3D("./gk3dExported/",mainMesh, gkPlanes, gkPlanesFinal);
}

//-----------EVENT-----------//
