//
//  c1_Template.h
//  rawAV
//
//  Created by alp tugan on 10.02.2020.
//

#ifndef c1_Template_h
#define c1_Template_h

#include "ofMain.h" 
#include "ofxContentsManager.h"
#include "ofxDC_Particle.h"
#include "ofxEasing.h"

class c1_Template : public ofxContentsManager::Content {
    
    
private:
    std::vector<ofxeasing::function> easings{
        ofxeasing::linear::easeNone,
        ofxeasing::linear::easeIn,
        ofxeasing::quad::easeOut,
        ofxeasing::cubic::easeIn,
        ofxeasing::quart::easeIn,
        ofxeasing::quint::easeIn,
        ofxeasing::circ::easeIn,
        ofxeasing::sine::easeIn,
        ofxeasing::exp::easeIn,
        ofxeasing::elastic::easeOut,
        ofxeasing::bounce::easeOut,
        ofxeasing::back::easeOut,
    };
    float initTime,initTime2, initTime3, initTime4, initTime5;
    float cr1, cg1, cb1, cr2, cg2, cb2;
    
    ofLight light; // creates a light and enables lighting
    
    ofCamera mCam;
    glm::vec3 mCamLook;
    ofParameter<glm::vec3> mCamPosition;
    float mCamSpeed;
    glm::vec3 mTargerPos;
    
    float mRotationSpeed;
    float mBoxSize;
    glm::vec3 mPos;
    
    ofBoxPrimitive mBox;
    
    //    ofImage mImg;
    ofTexture mTex;
    
    ofParameter<glm::vec3> mLigPos;
    ofParameter<float> mRotSpeed;
    ofParameter<float> mSize;
    
    // PARTICLES
    ofParameter<int> particleDens = {"Density", 4, 1, 20};
    ofParameter<glm::vec4> particleAcc = {"Accelaration Range", glm::vec4(0), glm::vec4(-5), glm::vec4(5)};
    ofParameter<glm::vec4> particleVel = {"Velocity Range", glm::vec4(0), glm::vec4(-5), glm::vec4(5)};
    ofParameter<glm::vec2> particleFadeOut = {"Fade Out Speed", glm::vec2(2), glm::vec2(0.02), glm::vec2(10)};
    ofParameter<glm::vec2> rad = {"Set Size", glm::vec2(5, 10), glm::vec2(0.0001), glm::vec2(20)};
    ofParameter<int> refreshRate = {"Background Clear Rate", 10, 0, 255};
    ofParameter<ofColor> fillColor = {"Fill Color" , ofColor(255), ofColor(0), ofColor(255)};
    ofParameter<glm::vec2> distThres = {"Line Thres", glm::vec2(100), glm::vec2(5), glm::vec2(1000)};
    
    ofParameter<ofColor> lineColor = {"Line Color", ofColor(255), ofColor(0), ofColor(255)};
    
    ofFbo fbo;
    ofVboMesh vboMesh;
    
    vector<ofxDC_Particle> particles;
    glm::vec2 particlesBornPos;
    float incthres;
    
    float mVecRot;
    
    float maxBoxSize = 100;
    float thresTimePar2;
    bool par2;
    
    float thresTimePar3;
    bool par3;
    
    float thresTimePar4;
    
    int counterId;
    int numBins = 12;
public:
    c1_Template() {
        ofLog() << "CONTENT :: " << this->getName() << " initialized...";
        
        mRotationSpeed = 0.1;
        mBoxSize = 100;
        
        light.setup();
        initGUI(posGuiConent1);
        gui.add(mLigPos.set("lig_pos", glm::vec3(0,0,0), glm::vec3(-2200), glm::vec3(2200)));
        gui.add(mCamPosition.set("CAM POSITION", glm::vec3(0,0,1400), glm::vec3(-6000), glm::vec3(6000)));
        gui.add(mRotSpeed.set("rot_speed", 0.5, 0, 1));
        gui.add(mSize.set("size", 1, 0, 2));
        
        gui.add(refreshRate);
        gui.add(distThres);
        gui.add(rad);
        gui.add(particleDens);
        gui.add(particleAcc);
        gui.add(particleVel);
        gui.add(particleFadeOut);
        
        particlesBornPos = glm::vec2(0);
        
        mCam.setPosition(0, 0, 150);
        mCamLook = glm::vec3(0, 0, 0);
        mCamSpeed = 0.001;
        
        mBox.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        mBox.setResolution(8);
        
        mVecRot = 0;
        
        loadGUISettings();
        
        setup();
    }
    
    ~c1_Template() {}
    
    void setup() {
        vboMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
        incthres = distThres->x;
    }
    
    void setSize() {
        initTime2 = ofGetElapsedTimef();
        maxBoxSize = ofRandom(500,1700);
    }
    
    void setParameter2(bool val) {
        par2 = val;
        counterId = ofRandom(0, numBins);
        thresTimePar2 = ofGetElapsedTimef();
        
        initTime = ofGetElapsedTimef();
    }
    
    void setParameter3(bool val) {
        par3 = val;
        thresTimePar3 = ofGetElapsedTimef();
        counterId = ofRandom(0, numBins);
    }
    
    void setParameter4(bool val = true) {
        initTime4 = ofGetElapsedTimef();
        fillColor.set(ofColor(ofRandom(0,255),ofRandom(0,255),ofRandom(0,255)));
    }
    
    void setParameter5(bool val = true) {
        initTime5 = ofGetElapsedTimef();
        lineColor.set(ofColor(ofRandom(0,255),ofRandom(0,255),ofRandom(0,255)));
    }
    
    void update() {
        float movementSpeed = mRotSpeed;
        float cloudSize = w / 2;
        
        float spacing = 0.35;
        
        float t = (ofGetElapsedTimef() * spacing) * movementSpeed;
        mPos = glm::vec3(
                 ofSignedNoise(t, 0, 0),
                 ofSignedNoise(0, t, 0),
                 ofSignedNoise(0, 0, t));
        mBoxSize = maxBoxSize * ofNoise(mPos.x, mPos.y, mPos.z) * mSize;
        
        mPos *= cloudSize;
        
        //particlesBornPos = glm::vec2(mPos.x, mPos.y);
        //mCam.setPosition(glm::vec3(mPos.x, mPos.y * 2, mCamPosition->z));
        mCam.lookAt(glm::vec3(0));

    }
    
    void draw() {
        
        cr2 = ofxeasing::map_clamp(ofGetElapsedTimef(), initTime5, initTime5 + 0.6, cr1, lineColor->r, easings[0]);
        cg2 = ofxeasing::map_clamp(ofGetElapsedTimef(), initTime5, initTime5 + 0.6, cg1, lineColor->g, easings[0]);
        cb2 = ofxeasing::map_clamp(ofGetElapsedTimef(), initTime5, initTime5 + 0.6, cb1, lineColor->b, easings[0]);
        
        if(par2 == false && !par3 ) {
            counterId = -1;
        }
        
        float audioVal;
        ofPushStyle();
        ofPushMatrix();
        ofEnableDepthTest();
        
        ofEnableLighting();
        //ofEnableNormalizedTexCoords();
        light.enable();
        light.setPosition(mLigPos);
        
        
        mCam.begin();
        
		int ww = numBins * 500;
        
        // Lines
        vboMesh.clear();
        if(par3 == true)
        {
            for (int i = 0; i < particles.size(); i++) {
                for (int j = i + 1; j < particles.size(); j++) {
                    float dst = ofDist(particles[i].getPos().x, particles[i].getPos().y, particles[j].getPos().x, particles[j].getPos().y);

                    if(dst > distThres->x && dst < distThres->y) {
                        //ofDrawLine(particles[i].getPos().x, particles[i].getPos().y, particles[j].getPos().x, particles[j].getPos().y);
                        float alpha = ofMap(dst, distThres->x, incthres, 255, 0);
                        vboMesh.addColor(ofColor(cr2, cg2, cb2, alpha));
                        vboMesh.addVertex(glm::vec3(particles[i].getPos().x, particles[i].getPos().y, 0));
                        vboMesh.addColor(ofColor(cr2, cg2, cb2, alpha));
                        vboMesh.addVertex(glm::vec3(particles[j].getPos().x, particles[j].getPos().y, 0));
                    }
                    //}
                }
            }
        }
        vboMesh.draw();
        
        if(ofGetElapsedTimef() - thresTimePar3 > 0.5 && par3 == true) {
            par3 = false;
        }
        
        
        
        if(ofGetElapsedTimef() - initTime4 > 1) {
            fillColor.set(ofColor(255,100));
        }
        
        
        if(ofGetElapsedTimef() - initTime5 > 1) {
            lineColor.set(ofColor(255,200));
        }
        
        // Particles
            //particlesBornPos = glm::vec2(ofRandom(-ofGetWidth()*0.5, ofGetWidth()*0.5), ofRandom(-ofGetHeight()*0.5, ofGetHeight()*0.5));
        for (int i = 0; i < numBins; i++) {
            
            if(counterId == i)
            {
                // int mp = ofMap(i, 0, particleDens, 0, numBins);
                
                particlesBornPos = glm::vec2((ofGetWidth()) * 0.5 - ((i+1) * 500), 0);
                
                ofxDC_Particle temp;
                temp.setup(particlesBornPos, ofColor(cr2, cg2, cb2));
                //temp.setup(glm::vec2(ofRandom(-ofGetWidth()*0.5, ofGetWidth()*0.5), ofRandom(-ofGetHeight()*0.5, ofGetHeight()*0.5)));
                temp.setAcceleration(glm::vec2(ofRandom(particleAcc->x, particleAcc->y), ofRandom(particleAcc->z, particleAcc->w)));
                temp.setVelocity(glm::vec2(ofRandom(particleVel->x, particleVel->y), ofRandom(particleVel->z, particleVel->w)));
                temp.setFadeOutSpeed(ofRandom(particleFadeOut->x, particleFadeOut->y));
                temp.setRadius(ofRandom(rad->x, rad->y));
                particles.push_back(temp);
                
            }
        }
       
        
        for (int i = 0; i < numBins; i++) {
            if(i == counterId) {
                audioVal = ofxeasing::map_clamp(ofGetElapsedTimef(), initTime, initTime + 0.18, 0, 60, easings[2]);
                //float audioVal = audioData[i];
                ofSetColor(ofColor(cr2, cg2, cb2));
                
            }else{
                ofSetColor(cr2, cg2, cb2, 180);
                audioVal = 0;
            }
            
            //ofDrawCircle( (i * 80)), h*0.5, 5 + 50 * audioVal);
            ofDrawCircle( (ww) * 0.5 - ((i+1) * 500), sin(ofGetElapsedTimef() * 0.15 * i) * 20, 35 + audioVal);
        }
        
        for (int i = 0; i < particles.size(); i++) {
            particles[i].update();
            
            if(par2 == true)
                particles[i].draw();
            
            if(particles[i].isDead()) {
                particles.erase(particles.begin() + i);
            }
        }
        
        if(ofGetElapsedTimef() - thresTimePar2 > 0.3 && par2 == true) {
            par2 = false;
        }
        
        //---------------------------------------------------------------------
        mCam.setPosition(mCamPosition);
        
        
        float raddi = ofxeasing::map_clamp(ofGetElapsedTimef(), initTime2, initTime2 + 0.4, 0, maxBoxSize, easings[2]);
        
        ofPushMatrix();
        //ofTranslate(ofxGetWinCenterX(), ofxGetWinCenterY());
 
        //ofSetColor(255,255);
        mBox.set(raddi + 1000);
        //mBox.setScale(mBoxSize);
        glm::quat q;
        mBox.setOrientation(q);
        mBox.rotateDeg(mPos.x, 1, 0, 0);
        mBox.rotateDeg(mPos.y, 0, 1, 0);
        //    mBox.rotate(mPos.z, 0, 0, 1);
        
        //mBox.move(ofMap(fftLive.getAveragePeak(), 0 ,1, -1, 1), 0, 0);;
        
        // image
        
        
        cr1 = ofxeasing::map_clamp(ofGetElapsedTimef(), initTime4, initTime4 + 0.6, cr1, fillColor->r, easings[0]);
        cg1 = ofxeasing::map_clamp(ofGetElapsedTimef(), initTime4, initTime4 + 0.6, cg1, fillColor->g, easings[0]);
        cb1 = ofxeasing::map_clamp(ofGetElapsedTimef(), initTime4, initTime4 + 0.6, cb1, fillColor->b, easings[0]);
        
        
        
        
        // frame
        ofSetBoxResolution(8);
        if (mTex.isAllocated()) {
            mTex.bind();
            mBox.draw();
            mTex.unbind();
        } else {
            ofSetColor(ofColor(cr1, cg1, cb1));
            mBox.draw();
            //mBox.drawWireframe();
        }
        
        ofPopMatrix();
        
        mCam.end();
        
        //ofDisableNormalizedTexCoords();
        light.disable();
        ofDisableLighting();
        
        
        //---------------------------------------------------------------------
        
        
        ofDisableDepthTest();
        ofPopStyle();
        ofPopMatrix();
    }
};

#endif /* c1_Template_h */
