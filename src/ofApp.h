#pragma once

#include "ofMain.h"

#include "ofxDC_Utilities.h"
#include "ofxDC_App.h"
#include "ofxPostProcessingManager.h"
#include "globalParameters.h"
#include "ofxContentsManager.h"
#include "ofxGui.h"

#include "c1_Template.h"
#include "c2_SuperShape.h"

//#include "ofxDC_Supershape.h"
#include "ofxOsc.h"
#include "ofxDC_InfoLayer.h"


// Todo:
/*
 c2 content: tidy up gui section
 
 */

#define NUM_MSG_STRINGS 20

class ofApp : public ofxDC_App{

	public:
		void setup();
		void update();
		void draw();


    void keyPressed(ofKeyEventArgs & key);
    void keyReleased(ofKeyEventArgs & key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void exit();
    
    void drawGui(ofEventArgs & args);
    
    void receiverParser();
    
    ofxContentsManager::Manager mContentsManager;
    // MAIN GUI
    ofxLabel activeScene;
    ofxLabel averagePeak;
    ofParameter<bool> gAutoPilot;
    ofParameter<bool> gAutoPilotMix;
    
    // FX GUI
    ofParameter<bool> gFxAutoPilot;
    
    //--------------------------------------------------------------
    // POST EFFECTS & FILTERS & SHADERS
    //--------------------------------------------------------------
    /*Addon instance*/
    ofxPostProcessingManager fxManager;
    ofFbo screenFbo;
    void generateScreenFbo();
    void gFxAutoPilotPressed(bool & _val);
    
    
    int prevScene;
    ofParameter<int> currentScene = {"Current Scene", 0, 0, 16};
    void currentSceneHandler(int & val);
    
    
    ofParameter<bool> isHelpDisplay = {"Show HELP", true};
    ofParameter<bool> saveBtn = {"SAVE IMAGE", false};
    
    ofParameter<glm::vec4> test = {"TEST", glm::vec4(1,0,0,0), glm::vec4(-10,-1920,0,0), glm::vec4(10,1920,1920,1920)};
    
    string helpString;
    float helpStringWidth, helpStringHeight;
    
    ofxDC_InfoLayer info;
    
    string settingFile;
    bool showGUI;
    bool showInfo;
    bool scene1,scene2;
    
    ofParameter<bool> showGui = {"Hide gui", false};
    ofParameter<bool> enableFullScreen = {"Enable fullscreen", false};
    ofParameter<string> serverAdd = {"Server Address", "localhost"};
    ofParameter<string> portAdd = {"COM Port", "9333"};
    ofParameter<void> saveSettings = {"SAVE SETTINGS"};
    
    void enableFullScreenHandler(bool & val);
    void saveSettingsHandler();
    
    // OSC
    ofxOscReceiver receiver;
    int currentMsgString;
    string msgStrings[NUM_MSG_STRINGS];
    ofColor msgColors[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    //--------------------------------------------------------------
    // UTILS
    //--------------------------------------------------------------
    ofxDC_Utilities notify1;
    ofxDC_Utilities notify2;
    ofxDC_Utilities notify3;
    
    // CONTENTS
    c1_Template *c1;
    c2_SuperShape *c2;
    
    string fileName;
    float scale, resolution;
};
