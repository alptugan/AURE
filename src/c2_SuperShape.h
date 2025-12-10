//
//  c2_SuperShape.h
//  AURE Beta
//
//  Created by alp tugan on 22.12.2020.
//

#ifndef c2_SuperShape_h
#define c2_SuperShape_h
#include "ofMain.h"
#include "ofxContentsManager.h"
#include "ofxDC_Universe.h"
#include "ofxEasing.h"


class c2_SuperShape : public ofxContentsManager::Content {
    
    
private:
    float lastRadSignal;
    ofEasyCam cam;
    ofLight light;
    
    ofMesh world;
    
    vector<ofColor> rainbow;
    vector<ofColor> bw;
    
    
    // Screen As Jpg
    ofPixels pix;
    ofFbo fbo;
    
    
    // GUI
    //ofxPanel gui;
    // "Set Lights & Environment Porperties"
    
    ofParameterGroup sfGui1, sfGui2;
    ofParameterGroup meshGui, spaceGUI, mainGui;
    
    
    ofParameter<glm::vec3> light1Pos;
    ofParameter<bool> showLight1;
    ofParameter<bool> invertBg = {"Invert Background", true};
    
    
    
    ofParameter<float> m1;
    ofParameter<float> n11;
    ofParameter<float> n12;
    ofParameter<float> n13;
    ofParameter<float> a1;
    ofParameter<float> b1;
    
    
    ofParameter<float> m2;
    ofParameter<float> m22;
    ofParameter<float> n21;
    ofParameter<float> n22;
    ofParameter<float> n23;
    ofParameter<float> a2;
    ofParameter<float> b2;
    
    // Tween Params
    float now, endTime;
    float tm1, tn11, tn12, tn13, ta1, tb1, tm2, tn21, tn22, tn23, ta2, tb2;
    float cr1,cg1,cb1, cr2,cg2,cb2;
    float rad1, rad2;
    
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
    float initTime;
    float endPosition;
    bool saveScreen;
    
    ofParameter<float> radius;
    ofParameter<int> total;
    ofParameter<ofColor> strokeColor;
    ofParameter<ofColor> fillColor = {"Set Solid Fill Color", ofColor(0,0,0,255), ofColor(0,0,0,0), ofColor(255, 255,255,255)};
    ofParameter<int> setMeshFillMode = {"Set Fill Mode", 0, 0, 3};
    ofParameter<bool> drawWireframe;
    ofParameter<bool> drawMesh;
    ofParameter<string> fileName = {"Set File Name",""};
    
    ofParameter<bool> animationShape = {"Enable Shape Shift", false};
    ofParameter<bool> animation1;
    ofParameter<bool> animation2;
    ofParameter<bool> mirrorParameters = {"Mirror Parameters", false};
    ofParameter<float> animation2Speed = {"Color Animation Speed",3,0.1,20};
    ofParameter<bool> enableLight = {"Enable/Disable Light", true};
    ofParameter<float> duration = {"Set Animation Duration", 4.0, 0.1, 100};
    ofParameter<int> selectedFrame = {"Color Variation Val", 79, 0, 500};
    ofxButton iterationBtn;
    ofParameter<bool> isRandom = {"Set Iteration Type Random", true};
    
    
    float rotValX;
    
    
    float hueOffSet;
    ofxButton saveBtn;
    ofxButton saveAsPngBtn;
    ofParameter<void> loadSettingsBtn = {"Load File"};
    
    ofParameter<bool> info;
    ofParameter<bool> help;
    
    float nextEventSeconds = 0;
    ofParameter<float> iterationPeriod = {"Iteration Period", 4, 0, 20};
    
    
    // Drop down menu
    vector<string> menuItems;
    int selectedId;
    ofParameterGroup itemList;
    vector<ofParameter<bool>> items;
    
    
    bool animateRadius;
    
    // Texture
    ofImage img;
    ofVboMesh vboMesh;
    
    // METHODS
    void generateSphereCoord(float _r, int _t) {
        world.clear();
        
        // Mirror Parameters
        if(mirrorParameters) {
            m2  = tm1;
            n21 = tn11;
            n22 = tn12;
            n23 = tn13;
            a2  = ta1;
            b2  = tb1;
        }
        
        
        // Enable Color Flowing Animation
        if(animation2)
            hueOffSet += animation2Speed;
        
        for (int i = 0; i < total; i++) {
            
            float lat = ofMap(i, 0, total - 1, -HALF_PI, HALF_PI);
            // keep it for debug purposes
            //float r2 = superShape(lat, 2, 10,10,10,1,1);
            float r2 = superShape(lat, tm2, tn21,tn22,tn23,ta2,tb2);
            
            int c1           = ofMap(i, 0, total, 0, 255 );
            ofColor hueColor = ofColor(0,255,255);
            c1 += hueOffSet;
            hueColor.setHue(c1 % 255);
            
            float r = 127 + 127 * sin(lat + ofDegToRad(selectedFrame));
            float g = 127 + 127 * sin(lat + 10 * sin(ofDegToRad(selectedFrame)));
            float b = 192 + 64 * sin(lat + ofDegToRad(45 * sin(ofDegToRad(selectedFrame))));
            ofColor hueColor2 = ofColor(r, g, b);
            
            for (int j = 0; j < total; j++) {
                
                float lon = ofMap(j, 0, total - 1, -PI, PI);
                
                // keep it for debug purposes
                //float r1 = superShape(lon, 8,60,100,30,1,1);
                float r1 = superShape(lon, tm1, tn11, tn12, tn13, ta1, tb1);
                
                float x = _r * r1 * cos(lon) * r2 * cos(lat);
                float y = _r * r1 * sin(lon) * r2 * cos(lat);
                float z = _r * r2 * sin(lat);
                
                
                
                world.addVertex(glm::vec3(x,y,z));
                
                if(setMeshFillMode == 0) {
                    world.addColor(hueColor);
                }
                else if(setMeshFillMode == 1) {
                    world.addColor(ofColor(cr1,cg1,cb1));
                }
                else if(setMeshFillMode == 2) {
                    world.addColor(ofColor(255));
                }
                else if(setMeshFillMode == 3) {
                    world.addColor(hueColor2);
                }
                
                if(setMeshFillMode == 2) {
                    world.addTexCoord( img.getTexture().getCoordFromPercent(0,0) );
                    world.addTexCoord( img.getTexture().getCoordFromPercent(1,0) );
                    world.addTexCoord( img.getTexture().getCoordFromPercent(1,1) );
                    world.addTexCoord( img.getTexture().getCoordFromPercent(0,1) );
                }
            }
        }
        
        
        
        for (int j = 0; j < total - 1; j++) {
            for (int i = 0 ; i < total - 1; i++) {
                
                world.addIndex(i+j*(total));         // 0
                world.addIndex((i+1)+j*(total));     // 1
                world.addIndex(i+(j+1)*(total));     // 6
                
                world.addIndex((i+1)+j* (total));     // 1
                world.addIndex((i+1)+(j+1) * (total)); // 7
                world.addIndex(i+(j+1)* (total));     // 6
                
            }
        }
        
        
        
        //createTextureMap();
        if(setMeshFillMode == 2) {
            world.addTexCoord( glm::vec2(0,0));
            world.addTexCoord( glm::vec2(10000, 0));
            world.addTexCoord( glm::vec2(10000, 10000));
            world.addTexCoord( glm::vec2(0, 10000));
        }
        
        
        
        
        if(showLight1 && enableLight)
            light.draw();
        
        
        if(animation1) {
            rotValX = rotValX + 0.2;
        }
        
        // Set Normals
        setNormals(world);
        
        ofPushMatrix();
        
        radius += sin(ofGetElapsedTimef()*2) * 0.3;
        ofTranslate(0, sin(ofGetElapsedTimef()*1)*10, 0); // levitation animation
        ofRotateDeg(rotValX, 0, 1, 0); // Rotation animation
        ofRotateDeg(rotValX*0.8, 0, 0, 1); // Rotation animation
        ofRotateDeg(rotValX*0.9, 0, 1, 0); // Rotation animation
        
        if(enableLight)
        {
            ofEnableLighting();
            light.enable();
        }
        
        world.enableColors();
        
        //ofSetColor(255,255);
        //ofFloatColor color(1.0,1.0,1.0,1.0);
        //glTexParameterfv(GL_TEXTURE_RECTANGLE, GL_TEXTURE_BORDER_COLOR, &color.r);
        
        if(setMeshFillMode == 2)
            img.getTexture().bind();
        
        if(drawMesh)
            world.draw();
        
        if(setMeshFillMode == 2)
            img.getTexture().unbind();
        
        world.disableColors();
        if(drawWireframe) {
            ofPushStyle();
            ofSetColor(strokeColor->r,strokeColor->g,strokeColor->b, strokeColor->a);
            world.drawWireframe();
            ofPopStyle();
        }
        
        if(enableLight)
        {
            ofDisableLighting();
            light.disable();
        }
        ofPopMatrix();
        
    }
    
    
    
    
    void animation1Handler(bool &_v) {
        
    }
    
public:
    c2_SuperShape() {
        ofLog() << "CONTENT :: " << this->getName() << " initialized...";
        
        
        initGUI(posGuiConent1);
        // Add gui here
		setup();
		
        loadGUISettings();
        
        
    }
    
    ~c2_SuperShape() {}
    
    void iterationBtnHandler() {
        animationShape = true;
        initTime = ofGetElapsedTimef();
        nextEventSeconds = initTime + iterationPeriod;
        
        float noiseMult[] = {0.5, 0.11, 0.6, 0.2, 1, 0.4, 0.64, 0.3, 1.1, 0.77, 0.223, 0.01};
        if(isRandom)
        {
            m1 = ofRandom(m1.getMin(),m1.getMax());
            n11 = ofRandom(n11.getMin(),n11.getMax());
            n12 = ofRandom(n12.getMin(),n12.getMax());
            n13 = ofRandom(n13.getMin(),n13.getMax());
            a1 = ofRandom(0.5,a1.getMax());
            b1 = ofRandom(0.5,b1.getMax());
            
            if(!mirrorParameters) {
                m2 = ofRandom(m2.getMin(),m2.getMax());
                n21 = ofRandom(n21.getMin(),n21.getMax());
                n22 = ofRandom(n22.getMin(),n22.getMax());
                n23 = ofRandom(n23.getMin(),n23.getMax());
                a2 = ofRandom(0.5,a2.getMax());
                b2 = ofRandom(0.5,b2.getMax());
            }
        }else{
            
            m1 = ofMap(ofNoise(ofGetElapsedTimef() * noiseMult[0]), 0, 1, m1.getMin(),m1.getMax());
            n11 = ofMap(ofNoise(ofGetElapsedTimef() * noiseMult[2]), 0, 1, n11.getMin(),n11.getMax());
            n12 = ofMap(ofNoise(ofGetElapsedTimef() * noiseMult[4]), 0, 1, n12.getMin(),n12.getMax());
            n13 = ofMap(ofNoise(ofGetElapsedTimef() * noiseMult[10]), 0, 1, n13.getMin(),n13.getMax());
            a1 = ofMap(ofNoise(ofGetElapsedTimef() * noiseMult[1]), 0, 1, 0.5,a1.getMax());
            b1 = ofMap(ofNoise(ofGetElapsedTimef() * noiseMult[3]), 0, 1, 0.5,b1.getMax());
            
            if(!mirrorParameters) {
                m2 = ofMap(ofNoise(ofGetElapsedTimef() * noiseMult[5]), 0, 1, m2.getMin(),m2.getMax());
                n21 = ofMap(ofNoise(ofGetElapsedTimef() * noiseMult[11]), 0, 1, n21.getMin(),n21.getMax());
                n22 = ofMap(ofNoise(ofGetElapsedTimef() * noiseMult[6]), 0, 1, n22.getMin(),n22.getMax());
                n23 = ofMap(ofNoise(ofGetElapsedTimef() * noiseMult[8]), 0, 1, n23.getMin(),n23.getMax());
                a2 = ofMap(ofNoise(ofGetElapsedTimef() * noiseMult[7]), 0, 1, 0.5,a2.getMax());
                b2 = ofMap(ofNoise(ofGetElapsedTimef() * noiseMult[9]), 0, 1, 0.5,b2.getMax());
            }
        }
    }
    
    void setup() {
        initTime = 0;
        
        light.setup();
        
        saveBtn.addListener(this, &c2_SuperShape::saveBtnHandler);
        saveAsPngBtn.addListener(this, &c2_SuperShape::saveAsPngBtnHandler);
        loadSettingsBtn.addListener(this, &c2_SuperShape::loadFile);
        
        radius = 200;
        total = 100;
        saveScreen = false;
        
        
        // ADD GUI Listeners
        
        
        // LOAD TEXTURE
        ofDisableArbTex();
        //ofEnableNormalizedTexCoords();
        img.load("/Users/alptugan/oF/assets/tex/matcap.jpg");
        //img.load("/Users/alptugan/Design/stock/Textures/oavv3DC.jpg");
        //img.getTexture().setTextureWrap( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );
        //img.getTexture().setTextureWrap( GL_REPEAT, GL_REPEAT );
        img.getTexture().setTextureWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
        img.getTexture().generateMipmap();
        img.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        //img.setUseTexture(true);
        
        
        //world.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
       
        
    //    gui.setup( "Super Formula Generator", xmlSettingsPath );
	
        
        sfGui1.setName("Super Formula Parameters 1");
        sfGui1.add(m1.set("m1 value", 7, 0, 20));
        sfGui1.add(n11.set("n I-I value", 0.2, 0, 20));
        sfGui1.add(n12.set("n I-II value", 1.7, 0, 20));
        sfGui1.add(n13.set("n I-III value", 1.7, 0, 20));
        sfGui1.add(a1.set("a I value", 1, 0, 1));
        sfGui1.add(b1.set("b I value", 1, 0, 1));
        
        
        sfGui2.setName("Super Formula Parameters 2");
        sfGui2.add(m2.set("m2 value", 7, 0, 20));
        sfGui2.add(n21.set("n II-I value", 0.2, 0, 20));
        sfGui2.add(n22.set("n II-II value", 1.7, 0, 20));
        sfGui2.add(n23.set("n II-III value", 1.7, 0, 20));
        sfGui2.add(a2.set("a II value", 1, 0, 1));
        sfGui2.add(b2.set("b II value", 1, 0, 1));
        
        meshGui.setName("Mesh Parameters");
        meshGui.add(radius.set("Mesh Radius",200,1,1000));
        meshGui.add(total.set("Mesh Resolution",100,3,1000));
        meshGui.add(strokeColor.set("Stroke Color",ofColor(0,0,0,100),ofColor(0,0,0,0),ofColor(255,255,255,255)));
        meshGui.add(drawWireframe.set("Show Wireframe",true));
        meshGui.add(drawMesh.set("Show Faces",true));
        meshGui.add(setMeshFillMode);
        meshGui.add(fillColor);
        
        
        itemList.setName("Selected Mesh Mode TRIANGLES");
        // Drop down list
        menuItems = {"TRIANGLES","TRIANGLE FAN","TRIANGLE STRIP","LINES","LINE STRIP","LINE LOOP","POINTS"};
        
        items.resize(menuItems.size());
        
        selectedId = 0;
        for (int i = 0; i < menuItems.size(); i++) {
            if(i != 0) {
                items[i].set(menuItems.at(i), false);
                items[i].addListener(this, &c2_SuperShape::filesCallback);
            }else{
                items[i].set(menuItems.at(i), true);
            }
            itemList.add(items[i]);
        }
        
        meshGui.add(selectedFrame);
        meshGui.add(itemList);
        // end of dropdown
        meshGui.add(mirrorParameters);
        meshGui.add(animationShape);
        meshGui.add(animation1.set("Animate Rotation",false));
        
        animation2.addListener(this, &c2_SuperShape::animation1Handler);
        meshGui.add(animation2.set("Animate Color",false));
        meshGui.add(animation2Speed);
        
        gui.add(sfGui1);
        gui.add(sfGui2);
        gui.add(meshGui);
        
        gui.getGroup("Mesh Parameters").getGroup("Stroke Color").minimize();
        gui.getGroup("Mesh Parameters").getGroup("Selected Mesh Mode TRIANGLES").minimize();
        
        gui.add(duration);
        gui.add(iterationPeriod);
        gui.add(isRandom);
        gui.add(iterationBtn.setup("Iterate Random"));
		
        
        // Lights and Environment
        spaceGUI.setName("Set Lights and Environment Porperties");
        spaceGUI.add(invertBg);
        spaceGUI.add(enableLight);
        spaceGUI.add(showLight1.set("Show Light A",true));
        spaceGUI.add(light1Pos.set("Light Position A",glm::vec3(0,0,0),glm::vec3(-1000,-1000,-1000),glm::vec3(1000,1000,1000)));
        
		
		gui.add(spaceGUI);
		
        // MAin Group
		/*mainGui.setName("Main Parameters");
        mainGui.add(info.set("Show Info",true));
        mainGui.add(help.set("Show Help Menu",false));
        
        mainGui.add(fileName);
        mainGui.add(saveBtn.setup("Save Mesh as 3D .ply File"));
        mainGui.add(saveAsPngBtn.setup("Save Mesh as PNG File"));
        mainGui.add(loadSettingsBtn);
        
        */
        
        /*spaceGUI.loadFromFile(xmlSpaceSettingsPath);
        mainGui.loadFromFile(xmlMainSettingsPath);
        gui.loadFromFile(xmlSettingsPath);*/
        
        // Randomize Button
        iterationBtn.addListener(this, &c2_SuperShape::iterationBtnHandler);
        
        
        radius = 300;
        rad1 = radius;
        rad2 = 300;
        cout << "Is normalized texture coord. " << ofGetUsingNormalizedTexCoords() << endl;
    }
    
    void update() {
        if(enableLight)
            light.setPosition(light1Pos);
        
        //  Tween Shape Shift Animation
        if(animationShape) {
            if(now > nextEventSeconds) {
                // iterationBtnHandler();
                
            }
            animationShape = false;
            
        }
        
        if(ofGetElapsedTimef() - lastRadSignal > 1 && animateRadius == true) {
            animateRadius = false;
        }
        
        
        now = ofGetElapsedTimef();
        endTime = initTime + duration;
        
        tm1 = ofxeasing::map_clamp(now, initTime, endTime, tm1, m1, easings[0]);
        tn11 = ofxeasing::map_clamp(now, initTime + 0.1, endTime, tn11, n11, easings[0]);
        tn12 = ofxeasing::map_clamp(now, initTime + .073, endTime, tn12, n12, easings[0]);
        tn13 = ofxeasing::map_clamp(now, initTime + 0.05, endTime, tn13, n13, easings[0]);
        ta1 = ofxeasing::map_clamp(now, initTime + 0.031, endTime, ta1, a1, easings[0]);
        tb1 = ofxeasing::map_clamp(now, initTime + 0.045, endTime, tb1, b1, easings[0]);
        
        cr1 = ofxeasing::map_clamp(now, initTime, endTime, cr1, fillColor->r, easings[0]);
        cg1 = ofxeasing::map_clamp(now, initTime, endTime, cg1, fillColor->g, easings[0]);
        cb1 = ofxeasing::map_clamp(now, initTime, endTime, cb1, fillColor->b, easings[0]);
        
        if(animateRadius)
            radius = ofxeasing::map_clamp(now, initTime, endTime - 0.7, rad1, rad2, easings[2]);
        
        if(!mirrorParameters)
        {
            tm2 = ofxeasing::map_clamp(now, initTime + 0.02, endTime, tm2, m2, easings[0]);
            tn21 = ofxeasing::map_clamp(now, initTime + .051, endTime, tn21, n21, easings[0]);
            tn22 = ofxeasing::map_clamp(now, initTime + 0.09, endTime, tn22, n22, easings[0]);
            tn23 = ofxeasing::map_clamp(now, initTime + 0.0719, endTime, tn23, n23, easings[0]);
            ta2 = ofxeasing::map_clamp(now, initTime + .00321, endTime, ta2, a2, easings[0]);
            tb2 = ofxeasing::map_clamp(now, initTime + 0.03, endTime, tb2, b2, easings[0]);
        }
        
    }
    
    void draw() {
        
        ofPushStyle();
        ofPushMatrix();
        
        if(!invertBg)
            ofBackgroundGradient( ofColor(40,40,40), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);
        else
            ofBackgroundGradient( ofColor(255), ofColor(190), OF_GRADIENT_CIRCULAR);
        
        ofEnableDepthTest();
        cam.lookAt(glm::vec3(0));
        cam.begin();
        
        // Generate SuperShape Function
   
        ofPushStyle();
        ofSetColor(30, 30, 30);
        //ofDrawRectangle(-ofGetWidth() * 0.5, -ofGetHeight() * 0.5, -2000, ofGetWidth(), ofGetHeight());
        ofPopStyle();
        generateSphereCoord(radius,total);
        cam.end();
        ofDisableDepthTest();


        ofPopStyle();
        ofPopMatrix();
    }

    // SETTER & GETTER
    /*
     
     n12 = ofRandom(n12.getMin(),n12.getMax());
     n13 = ofRandom(n13.getMin(),n13.getMax());
     a1 = ofRandom(0.5,a1.getMax());
     b1 = ofRandom(0.5,b1.getMax());
     
     if(!mirrorParameters) {
     n21 = ofRandom(n21.getMin(),n21.getMax());
     n22 = ofRandom(n22.getMin(),n22.getMax());
     n23 = ofRandom(n23.getMin(),n23.getMax());
     a2 = ofRandom(0.5,a2.getMax());
     b2 = ofRandom(0.5,b2.getMax());
     }
     */
    void setm1() {
        resetAnimTimer();
        m1 = ofRandom(m1.getMin(),m1.getMax());
        b1 = ofRandom(0.5,b1.getMax());
        a1 = ofRandom(0.5,a1.getMax());
        b1 = ofRandom(0.5,0.8);
        a2 = 0.4;
        n22 = ofRandom(n22.getMin(),n22.getMax());
        n13 = ofRandom(n13.getMin(),n13.getMax());
    }
    
    void setn11() {
        resetAnimTimer();
        n11 = ofRandom(n11.getMin(),n11.getMax());
        b2 = ofRandom(0.5,b2.getMax());
        a2 = ofRandom(0.5,a2.getMax());
        b1 = ofRandom(0.5,b1.getMax());
    }
    
    void setm2() {
        resetAnimTimer();
        m2 = ofRandom(m2.getMin(),m2.getMax());
        n12 = ofRandom(n12.getMin(),n12.getMax());
        n13 = ofRandom(n13.getMin(),n13.getMax());
        a1 = ofRandom(0.5,a1.getMax());
        b1 = ofRandom(0.5,b1.getMax());
    }
    
    void setRandomColor() {
        resetAnimTimer();
        fillColor.set(ofColor(ofRandom(0,255),ofRandom(0,255),ofRandom(0,255)));
        cr2 = ofRandom(0,255);
        cg2 = ofRandom(0,255);
        cb2 = ofRandom(0,255);
    }
    
    void setRandomRadius() {
        resetAnimTimer();
        rad1 = ofRandom(350, 450);
        rad2 = 300;
        animateRadius = true;
        lastRadSignal = ofGetElapsedTimef();
    }
    
    void setRadius(int val) {
        rad1 = val;
        rad2 = val;
    }
    
    
    
private:
    void resetAnimTimer() {
        initTime = ofGetElapsedTimef();
        nextEventSeconds = initTime + iterationPeriod;
    }
    void saveBtnHandler() {
        string fileNamePre;
        if(fileName.get() == "") {
            fileNamePre = ofGetTimestampString();
        }else{
            fileNamePre = ofGetTimestampString() + " - " + fileName.get();
        }
        
        world.save(fileNamePre + ".ply");
        gui.saveToFile(fileNamePre + ".xml");
    }
    
    void saveAsPngBtnHandler() {
        //ofSaveFrame();
        saveScreen = true;
        //ofSaveViewport("testtt.jpg");
        
        fbo.clear();
        fbo.allocate(ofGetWidth()*3, ofGetHeight()*3,GL_RGBA);
    }
    
    void filesCallback(bool & _val) {
        int i;
        
        if(_val) {
            for(i = 0; i < menuItems.size(); i++){
                if(&items[i].get() == &_val){
                    selectedId = i;
                    break;
                }
            }
            
            
            for(int i = 0; i < menuItems.size(); i++){
                items[i].removeListener(this, &c2_SuperShape::filesCallback);
                items[i].set(false);
                items[i].addListener(this, &c2_SuperShape::filesCallback);
            }
            
            items[selectedId] = true;
        }
        
        
        if(selectedId == 0) {
            world.setMode(OF_PRIMITIVE_TRIANGLES);
        }else if(selectedId == 1) {
            world.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        }else if(selectedId == 2) {
            world.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        }else if(selectedId == 3) {
            world.setMode(OF_PRIMITIVE_LINES);
        }else if(selectedId == 4) {
            world.setMode(OF_PRIMITIVE_LINE_STRIP);
        }else if(selectedId == 5) {
            world.setMode(OF_PRIMITIVE_LINE_LOOP);
        }else if(selectedId == 6) {
            world.setMode(OF_PRIMITIVE_POINTS);
        }
        
        itemList.setName("Selected Mesh Mode " + items[selectedId].getName());
        gui.getGroup("Mesh Parameters").getGroup("Selected Mesh Mode " + items[selectedId].getName()).minimize();
    }
    
    void setNormals( ofMesh &mesh,bool bNormalize = true ) {
        //The number of the vertices
        int nV = mesh.getNumVertices();
        
        //The number of the triangles
        int nT = mesh.getNumIndices() / 3;
        
        
        
        vector<glm::vec3> norm( nV ); //Array for the normals
        
        //Scan all the triangles. For each triangle add its
        //normal to norm's vectors of triangle's vertices
        for (int t=0; t<nT; t++) {
            
            //Get indices of the triangle t
            int i1 = mesh.getIndex( 3 * t );
            int i2 = mesh.getIndex( 3 * t + 1 );
            int i3 = mesh.getIndex( 3 * t + 2 );
            
            //Get vertices of the triangle
            const glm::vec3 &v1 = mesh.getVertex( i1 );
            const glm::vec3 &v2 = mesh.getVertex( i2 );
            const glm::vec3 &v3 = mesh.getVertex( i3 );
            
            //Compute the triangle's normal
            //ofPoint dir = ( (v2 - v1).crossed( v3 - v1 ) ).normalized();
            glm::vec3 dir = glm::cross( (v2 - v1) , ( v3 - v1 ) );
            dir = glm::normalize(dir);
            
            
            //Accumulate it to norm array for i1, i2, i3
            norm[ i1 ] += dir;
            norm[ i2 ] += dir;
            norm[ i3 ] += dir;
        }
        
        //Normalize the normal's length
        for (int i=0; i<nV; i++) {
            norm[i] = glm::normalize(norm[i]);
        }
        
        //Set the normals to mesh
        mesh.clearNormals();
        
        mesh.addNormals( norm );
    }
    
    // SUpershape methods
    float superShape(float _theta, float _m, float _n1, float _n2, float _n3, float _a, float _b) {
        //_m2 = _m1;
        float t1 = abs((1 / _a) * cos((_m * _theta) / 4.f));
        t1 = pow(t1, _n2);
        
        float t2 = abs((1 / _b) * sin((_m * _theta) / 4.f));
        t2 = pow(t2, _n3);
        
        float t3 = t1 + t2;
        
        float r = pow(t3, (-1 / _n1));
        
        return r;
    }
    
    
    
    void loadFile() {
        string dataFolder = "../../../" + ofFilePath::getCurrentWorkingDirectory();
        
        //Open the Open File Dialog
        ofFileDialogResult openFileResult = ofSystemLoadDialog("Select xml file", false,  dataFolder + "/data");
        
        //Check if the user opened a file
        if (openFileResult.bSuccess){
            
            //We have a file, check it and process it
            ofFile file (openFileResult.getPath());
            
            if (file.exists()){
                string fileExtension = ofToUpper(file.getExtension());
                
                //We only want xml files
                if (fileExtension == "XML") {
                    gui.loadFromFile(openFileResult.getPath());
                }
            }
        }else {
            ofLog() << "User hit cancel";
        }
    }
};

#endif /* c2_SuperShape_h */
