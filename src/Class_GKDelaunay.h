#pragma once

#ifndef GK_DELAUNAY3D_H  
#define GK_DELAUNAY3D_H  

#include "ofMain.h"
#include "ofxGKUtils.h"

    class DelaCircle {
    public:
        DelaCircle() {}
        DelaCircle(const GKPoint& _gp,const float& _r) {
            center = _gp;
            radius = _r;
        }
        DelaCircle(const glm::vec3& _center, const float& _r) {
            center = GKPoint(_center);
            radius = _r;
        }
        ~DelaCircle(){}
        GKPoint center;  // 中心座標  
        float radius;  // 半径  
    };

    class DelaTriangle {
    public:
        DelaTriangle(){}
        DelaTriangle(const vector<GKPoint>& _gkpts) {
            for (auto& gp : _gkpts) {
                vertices.push_back(gp);
            }
        }
        ~DelaTriangle() {}
        vector<GKPoint> vertices;  // 頂点座標 
        GKPoint getCentroid() {
            GKPoint _c = GKPoint(glm::vec3(0));
            for (auto v : vertices) {
                _c.pos += v.pos;
            }
            _c.pos /= 3;
            return _c;
        }

        float getRadius(const GKPoint& _centroid) {
            return glm::distance(vertices[0].pos, _centroid.pos);
        }

        //-----------OPERATOR-----------// -> mainly For Sort Algo.

    };

    class DelaTetrahedron {
    public:       
        DelaTetrahedron() {}
        DelaTetrahedron(const vector<GKPoint>& _gkpts) {
            for (auto& gp : _gkpts) {
                vertices.push_back(gp);
            }
        }
        ~DelaTetrahedron() {}
        vector<GKPoint> vertices;
        bool hasCommonPoints(const DelaTetrahedron& _tet) {
            for (int i = 0; i <4; ++i)
                for (int j = 0; j < 4; ++j)
                    if (vertices[i].pos == _tet.vertices[j].pos) return true;
            return false;
        }
        GKPoint getCentroid() {
            GKPoint _centroid = GKPoint(glm::vec3(0));
            for (auto& v : vertices) {
                _centroid.pos += v.pos;
            }
            return _centroid.pos / 4;
        }
        float getRadius(const GKPoint& _centroid) {
            return glm::distance(vertices[0].pos, _centroid.pos);
        }

        //-----------OPERATOR-----------// -> mainly For Sort Algo.

    };

    class GKDelaunay3d{
    public:
        GKDelaunay3d() {}
        ~GKDelaunay3d() {}
        vector<DelaTriangle> getDelaunayTriangles(vector<GKPlane>& _gkPlanes) {
            vector<DelaTetrahedron> tetras;
            DelaTetrahedron hugeTetrahedron;
            {   
                vector<GKPoint> vertices;
                glm::vec3 max = glm::vec3(DBL_MIN);
                glm::vec3 min = glm::vec3(DBL_MAX);
                for (auto& gp : _gkPlanes) {
                    if (max.x < gp.centroid.x)max.x = gp.centroid.x;
                    if (gp.centroid.x < min.x)min.x = gp.centroid.x;
                    if (max.y < gp.centroid.y)max.y = gp.centroid.y;
                    if (gp.centroid.y < min.y)min.y = gp.centroid.y;
                    if (max.z < gp.centroid.z)max.z = gp.centroid.z;
                    if (gp.centroid.z < min.z)min.z = gp.centroid.z;
                }
                glm::vec3 center = (max-min)/2;            
                float radius = glm::distance(center, min) + 0.1;

                // 4つの頂点
                vertices.push_back(GKPoint(glm::vec3(center.x, center.y + 3. * radius, center.z)));
                vertices.push_back(GKPoint(glm::vec3(center.x - 2.0 * sqrt(2.0) * radius, center.y - radius, center.z)));
                vertices.push_back(GKPoint(glm::vec3(center.x + sqrt(2.0) * radius, center.y - radius, center.z + sqrt(6.0) * radius)));
                vertices.push_back(GKPoint(glm::vec3(center.x + sqrt(2.0) * radius, center.y - radius, center.z - sqrt(6.0) * radius)));
                hugeTetrahedron = DelaTetrahedron(vertices);
            }
            tetras.push_back(hugeTetrahedron);
             
            for (auto gpItr = _gkPlanes.begin(); gpItr != _gkPlanes.end();gpItr++){
                GKPoint gp = GKPoint(gpItr->centroid,gpItr->state);
                
                //
                // 追加候補の四面体を保持する一時マップ
                vector<DelaTetrahedron> tmpTetras;
                for (auto tetItr = tetras.begin(); tetItr != tetras.end();) {                 
                    DelaTetrahedron tet = *tetItr; 
                    DelaCircle c = getCircumcircle(tet);
                    float dist = glm::distance(c.center.pos, gp.pos);
                    if (dist < c.radius) {
                        vector<GKPoint> _vertices;
                        _vertices.push_back(gp);
                        _vertices.push_back(tet.vertices[0]);
                        _vertices.push_back(tet.vertices[1]);
                        _vertices.push_back(tet.vertices[2]);
                        tmpTetras.push_back(DelaTetrahedron(_vertices));

                        _vertices.erase(_vertices.begin(), _vertices.end());
                        _vertices.push_back(gp);
                        _vertices.push_back(tet.vertices[0]);
                        _vertices.push_back(tet.vertices[2]);
                        _vertices.push_back(tet.vertices[3]);
                        tmpTetras.push_back(DelaTetrahedron(_vertices));

                        _vertices.erase(_vertices.begin(), _vertices.end());
                        _vertices.push_back(gp);
                        _vertices.push_back(tet.vertices[0]);
                        _vertices.push_back(tet.vertices[3]);
                        _vertices.push_back(tet.vertices[1]);
                        tmpTetras.push_back(DelaTetrahedron(_vertices));

                        _vertices.erase(_vertices.begin(), _vertices.end());
                        _vertices.push_back(gp);
                        _vertices.push_back(tet.vertices[1]);
                        _vertices.push_back(tet.vertices[2]);
                        _vertices.push_back(tet.vertices[3]);
                        tmpTetras.push_back(DelaTetrahedron(_vertices));

                        tetItr = tetras.erase(tetItr);
                    }
                    else {
                        ++tetItr;
                    }
                }
                
                //
                //一時保持マップで重複チェックし、問題のないものだけ四面体セットに追加
                {
                    cout << "tetras-original :" << tmpTetras.size() << endl;
                    vector<bool> shouldDelete;
                    shouldDelete.reserve(tmpTetras.size());
                    for (int i = 0; i < tmpTetras.size(); i++) {
                        shouldDelete.push_back(false);
                    }
                    for (int i = 0; i < tmpTetras.size(); i++) {
                        for (int j = 0; j < tmpTetras.size(); j++) {
                            if (i != j) {
                                if (glm::distance(tmpTetras[i].getCentroid().pos,tmpTetras[j].getCentroid().pos)<0.1) {
                                    //if (abs(tmpTetras[i].getRadius(tmpTetras[i].getCentroid()) - tmpTetras[j].getRadius(tmpTetras[j].getCentroid())) < 1) {
                                        shouldDelete[i] = true;
                                        break;
                                    //}
                                }
                            }
                        }
                    }
                    for (int i = 0; i < shouldDelete.size(); i++) {
                        if (shouldDelete[i] == false) {
                            tetras.push_back(tmpTetras[i]);
                        }
                    }
                    cout << "tetras-returned :" << tetras.size() << endl;
                }
            }
            // 最後に、外部三角形の頂点を削除       
            for (auto tetItr = tetras.begin(); tetItr != tetras.end(); ) {
                if (hugeTetrahedron.hasCommonPoints(*tetItr)) {
                    tetItr = tetras.erase(tetItr);//tetItr++
                }
                else {
                    ++tetItr;
                }
            }

            // そして、伝説へ…  (triangleに返していく)
            
            vector<DelaTriangle> tmpTris;
            for (auto tetItr = tetras.begin(); tetItr != tetras.end(); ++tetItr) {
                DelaTetrahedron tetra = *tetItr;
                vector<GKPoint> _vertices;
                _vertices.push_back(tetra.vertices[0]);
                _vertices.push_back(tetra.vertices[1]);
                _vertices.push_back(tetra.vertices[2]);

                tmpTris.push_back(DelaTriangle(_vertices));

                _vertices.erase(_vertices.begin(), _vertices.end());
                _vertices.push_back(tetra.vertices[0]);
                _vertices.push_back(tetra.vertices[2]);
                _vertices.push_back(tetra.vertices[3]);
                tmpTris.push_back(DelaTriangle(_vertices));

                _vertices.erase(_vertices.begin(), _vertices.end());
                _vertices.push_back(tetra.vertices[0]);
                _vertices.push_back(tetra.vertices[3]);
                _vertices.push_back(tetra.vertices[1]);
                tmpTris.push_back(DelaTriangle(_vertices));

                _vertices.erase(_vertices.begin(), _vertices.end());
                _vertices.push_back(tetra.vertices[1]);
                _vertices.push_back(tetra.vertices[2]);
                _vertices.push_back(tetra.vertices[3]);
                tmpTris.push_back(DelaTriangle(_vertices));
            }
            
            vector<DelaTriangle> tris;
            {   
                cout << "tris-original: "<< tmpTris.size() << endl;
                vector<bool> shouldDelete;
                shouldDelete.reserve(tmpTris.size());
                for (int i = 0; i < tmpTris.size(); i++) {
                    shouldDelete.push_back(false);
                }
                for (int i = 0; i < tmpTris.size(); i++) {
                    for (int j = 0; j < tmpTris.size(); j++) {
                        if (i != j) {
                            if (glm::distance(tmpTris[i].getCentroid().pos,tmpTris[j].getCentroid().pos)<0.1) {
                                //if (abs(tmpTris[i].getRadius(tmpTris[i].getCentroid()) - tmpTris[j].getRadius(tmpTris[j].getCentroid())) < 0.01) {
                                    shouldDelete[i] = true;
                                    break;
                                //}
                            }
                        }
                    }
                }
                for (int i = 0; i < shouldDelete.size(); i++) {
                    if (shouldDelete[i] == false) {
                        tris.push_back(tmpTris[i]);
                    }
                }
                cout << "tris-returned: " << tris.size() << endl;
            }
            return tris;
        };
    private:
        // ======================================    
        // LU分解による三元一次連立方程式の解法  
        // ======================================  
        float lu(float a[3][3], int ip[3])
        {
            static const int n = 3;
            float weight[n];

            for (int k = 0; k < n; ++k)
            {
                ip[k] = k;
                float u = 0;
                for (int j = 0; j < n; ++j)
                {
                    float t = fabs(a[k][j]);
                    if (t > u) u = t;
                }
                if (u == 0) return 0;
                weight[k] = 1 / u;
            }
            float det = 1;
            for (int k = 0; k < n; ++k)
            {
                float u = -1;
                int m = 0;
                for (int i = k; i < n; ++i)
                {
                    int ii = ip[i];
                    float t = fabs(a[ii][k]) * weight[ii];
                    if (t > u)
                    {
                        u = t;
                        m = i;
                    }
                }
                int ik = ip[m];
                if (m != k)
                {
                    ip[m] = ip[k]; ip[k] = ik;
                    det = -det;
                }
                u = a[ik][k]; det *= u;
                if (u == 0) return 0;
                for (int i = k + 1; i < n; ++i)
                {
                    int ii = ip[i];
                    float t = (a[ii][k] /= u);
                    for (int j = k + 1; j < n; ++j)
                        a[ii][j] -= t * a[ik][j];
                }
            }
            return det;
        }
        void solve(float a[3][3], float b[3], int ip[3], float x[3])
        {
            const int n = 3;

            for (int i = 0; i < n; ++i)
            {
                int ii = ip[i];
                float t = b[ii];
                for (int j = 0; j < i; ++j)
                    t -= a[ii][j] * x[j];
                x[i] = t;
            }

            for (int i = n - 1; i >= 0; --i)
            {
                float t = x[i];
                int ii = ip[i];
                for (int j = i + 1; j < n; ++j)
                    t -= a[ii][j] * x[j];
                x[i] = t / a[ii][i];
            }
        }
        double gauss(float a[3][3], float b[3], float x[3])
        {
            static const int n = 3;
            int ip[n];
            double det = lu(a, ip);

            if (det != 0) solve(a, b, ip, x);
            return det;
        }
        // ======================================    
        // 与えられた四面体の外接球を求める  
        // ======================================  
        DelaCircle getCircumcircle(const DelaTetrahedron& _tetra) {
            const glm::vec3 p0 = _tetra.vertices[0].pos;
            const glm::vec3 p1 = _tetra.vertices[1].pos;
            const glm::vec3 p2 = _tetra.vertices[2].pos;
            const glm::vec3 p3 = _tetra.vertices[3].pos;
            DelaCircle _circle;

            float A[3][3] = {
              {(p1 - p0).x,(p1 - p0).y,(p1 - p0).z},
              {(p2 - p0).x,(p2 - p0).y,(p2 - p0).z},
              {(p3 - p0).x,(p3 - p0).y,(p3 - p0).z}
            };

            float b[3] = {
              0.5 * (pow(glm::length(p1),2.) - pow(glm::length(p0),2.)),
              0.5 * (pow(glm::length(p2),2.) - pow(glm::length(p0),2.)),
              0.5 * (pow(glm::length(p3),2.) - pow(glm::length(p0),2.))
            };


            float x[3] = { 0.0, 0.0, 0.0 };

            if (gauss(A, b, x) == 0)
            {
                _circle.center = GKPoint(glm::vec3(0));
                _circle.radius = -1;
            }
            else
            {
                _circle.center = GKPoint(glm::vec3(x[0], x[1], x[2]));
                _circle.radius = sqrt(pow((x[0] - p0.x), 2.) + pow((x[1] - p0.y), 2.) + pow((x[2] - p0.z), 2.)) + 0.1;
            }
            return _circle;
        }
        // ======================================    
        // 四面体の重複管理  
        // ======================================  
    };
#endif  
