#include "ofApp.h"

#define internalFormat GL_RGB32F


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetDataPathRoot("../Resources/data/");
    
    
    showGUI = false;
    showInfo = true;
    settingFile = "settings-aure.json";
    // GUI
    ofxGuiSetBitmapFont();
    gui.setDefaultTextPadding(6);
    gui.setDefaultWidth(300);
    gui.setDefaultHeight(20);
    gui.setup(settingFile);
    gui.add(showGui);
    gui.add(enableFullScreen);
    gui.add(serverAdd);
    gui.add(portAdd);
    gui.add(saveSettings);
    
    gui.loadFromFile(settingFile);
    
    enableFullScreen.addListener(this, &ofApp::enableFullScreenHandler);
    saveSettings.addListener(this, &ofApp::saveSettingsHandler);
    
    
    // OSC
    receiver.setup(ofToInt(portAdd));
    
    
    //ofEnableSmoothing();
    settingsAudioFile = "settings-rawav-audio.xml";
    
    ofSetFrameRate(60.0);
    resolution = 1;
    w = ofGetWidth() * resolution;
    h = ofGetHeight() * resolution;
    scale = 1;
    
    //////////////////////////////////////////////////////////////////
    // LOAD & INIT FONTS
    //////////////////////////////////////////////////////////////////
    
    /* FFT SECTION */
    fftLive.setMirrorData(false);
    fftLive.setup();
    
    ofLog() << "Audio Default Buffer Size: " << fftLive.getBufferSize();
    
    // Show GUI and AUDIOGUI
    ofxSetDebugMode(false);
    
    //////////////////////////////////////////////////////////////////
    // INIT SCENES
    //////////////////////////////////////////////////////////////////
    c1                         = new c1_Template();
    c2                         = new c2_SuperShape();
    
    //////////////////////////////////////////////////////////////////
    // INIT CONTENT MANAGER & ADD SCENE CLASSES
    //////////////////////////////////////////////////////////////////
    mContentsManager.setup( w, h, internalFormat,4);
    mContentsManager.addContent(c1);
    mContentsManager.addContent(c2);
    
    mContentsManager.setDebug(ofxGetDebugMode());
    
    
    /////////////////////////////////////////////////////////////////
    // INIT MAIN GUI
    /////////////////////////////////////////////////////////////////
    ofxGUIGenerate();
    
    
    /////////////////////////////////////////////////////////////////
    // INIT SOUND GUI
    /////////////////////////////////////////////////////////////////
    guiAudio.setDefaultWidth(320);
    guiAudio.setup("AUDIO FFT PARAMETERS", settingsAudioFile, ofGetWidth() - 320 - 1, 0);
    guiAudio.add(activeScene.setup("SCENE",""));
    guiAudio.add(mContentsManager.getOpacityParameterGroup());
    guiAudio.add(audioThreshold.setup("audioThreshold", 1.0, 0.0, 1.0));
    guiAudio.add(audioPeakDecay.setup("audioPeakDecay", 0.915, 0.0, 1.0));
    guiAudio.add(audioMaxDecay.setup("audioMaxDecay", 0.995, 0.0, 1.0));
    guiAudio.add(audioMirror.setup("audioMirror", true));
    guiAudio.add(averagePeak.setup("Average PEAK",""));
    guiAudio.add(audioHighThres.set("High Range", glm::vec2(0, 0.5), glm::vec2(0), glm::vec2(10)));
    guiAudio.add(audioMidThres.set("Mid Range", glm::vec2(0, 0.5), glm::vec2(0), glm::vec2(10)));
    guiAudio.add(audioLowThres.set("Low Range", glm::vec2(0, 0.5), glm::vec2(0), glm::vec2(10)));
    guiAudio.loadFromFile(settingsAudioFile);
    
    
    /////////////////////////////////////////////////////////////////
    // INIT FX GUI
    /////////////////////////////////////////////////////////////////
    gFxAutoPilot.addListener(this, &ofApp::gFxAutoPilotPressed);
    fxManager.setup(w, h);
    
    
    
    // Generate Screen FBO
    generateScreenFbo();
    
    
    // SET DEFAULT CONTENT SCENE
    mContentsManager.switchContent(mContentsManager.getNumContents() - 1);
    
    
    /////////////////////////////////////////////////////////////////
    // INIT MAIN GUI
    /////////////////////////////////////////////////////////////////
    // Save Button for default settings
    ofxAppGui.setDefaultWidth(200);
    ofxAppGui.add(gAutoPilot.set("AUTO PILOT", true));
    ofxAppGui.add(gAutoPilotMix.set("AUTO PILOT SCENE MIX", false));
    ofxAppGui.add(gFxAutoPilot.set("FX AUTO PILOT", false));
    ofxAppGui.add(saveBtn);
    ofxAppGui.add(isHelpDisplay);
    
    
    
    // SETUP INFO SCREEN // todo: MOVE TO CLASS
    setHelpString();
    
    // LOAD GUI PREFERENCES
    ofxGUILoadSettings();
    
    // TIMELINE LISTENERS
    currentScene.addListener(this, &ofApp::currentSceneHandler);
    
    fxManager.loadSettings();
    
    //float linePoints[] = {0.1, 0.2};
    float linePoints[2];
    
    //ofEnableAntiAliasing();
}



void ofApp::currentSceneHandler(int & val) {
    
    if(currentScene == 1) {
        if(prevScene != currentScene)
            mContentsManager.switchContent(currentScene);
    }
    
    if(prevScene != currentScene) {
        mContentsManager.switchContent(currentScene);
    }
    prevScene = currentScene;
}



void ofApp::gFxAutoPilotPressed(bool & _val) {
    if(!_val) {
        fxManager.disableAll();
    }
}

void ofApp::processFFT() {
    
    fftLive.setThreshold(audioThreshold);
    fftLive.setPeakDecay(audioPeakDecay);
    fftLive.setMaxDecay(audioMaxDecay);
    fftLive.setMirrorData(audioMirror);
    
    fftLive.update();
    
    averagePeak.setup("Average PEAK", ofToString(fftLive.getAveragePeak()));
}
//--------------------------------------------------------------
void ofApp::update(){
    
    // Parse OSC
    receiverParser();
    
    // Process FFT
    // processFFT();
    
    fxManager.updateValues();
    
    // Update Content Manager
    mContentsManager.update();
    
    autoPilot();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
    
    
    // Draw Content SAVE CONTENT
    if(saveBtn){
        scale = 1;
        
        //mContentsManager.windowResized(w * scale, h * scale);
        
        fileName = "export/" + mContentsManager.getContent(mContentsManager.getCurrentContent())->getName();
        
        generateScreenFbo();
        
        
        
        screenFbo.begin();
        ofClear(0, 0);
        fxManager.begin();
        ofPushMatrix();
        ofScale(1,-1);
        mContentsManager.draw(0, 0, w * scale, h * scale);
        ofPopMatrix();
        fxManager.end();
        screenFbo.end();
        
        screenFbo.getTexture().getTextureData().bFlipTexture = true;
        //        screenFbo.draw(0, 0);
        
        ofPixels pix;
        
        screenFbo.readToPixels(pix);
        
        if(ofSaveImage(pix, fileName + ofToString(ofGetFrameNum()) + ".jpg", OF_IMAGE_QUALITY_BEST)) {
            //mContentsManager.windowResized(w, h);
            ofLog() << "IMAGE SAVED TO "  << fileName;
        }
        
        saveBtn = false;
    }
    
    
    
    
    
    //screenFbo.begin();
    //ofClear(0, 0);
    // FX Begin
    ofScale(resolution);
    fxManager.begin();
    // Draw Content
    //ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ADD);
    mContentsManager.draw(0, 0, w, h);
    //ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_MULTIPLY);
    fxManager.end();
    // FX End
    //screenFbo.end();
    
    
    ofPopMatrix();
    
    
    // Draw GUI
    if( ofxGetDebugMode() ) {
        /*int ww = OFX_FFT_WIDTH + 4;
        int hh = OFX_FFT_HEIGHT + 1;
        int x = ofGetWidth() - ww - 4;
        int y = ofGetHeight() - hh - 1;
        fftLive.draw(x, y, ww, hh);
        guiAudio.draw();*/
        
        activeScene.setup("SCENE", ofToString(mContentsManager.getContent(mContentsManager.getCurrentContent())->getName()));
        
        
        fxManager.drawGui(ofxGetGUIWidth() + 2, 0);
        mContentsManager.drawContentGui();
        
    }
    
    
    if(isHelpDisplay) {
        ofPushMatrix();
        ofTranslate((ofGetWidth() - helpStringWidth) * 0.5, 0);
        ofPushStyle();
        //ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
        ofSetColor(0, 0, 0, 200);
        ofDrawRectangle(0, 0, helpStringWidth + 20, helpStringHeight + 20);
        ofSetColor(255, 255);
        ofDrawBitmapString(helpString, 10, 20);
        ofPopStyle();
        ofPopMatrix();
    }
    
    
    
    if(notify2.notifyPerSecond(0.5)) {
        //fxManager.switchFX(int(ofRandom(fxManager.getEffectNum())));
        fxManager.disableAll();
    }
    
    
    if(showGUI) {
        gui.draw();
        
        
        auto guiPosX = gui.getPosition().x;
        auto guiPosY = gui.getPosition().y;
        auto guiH    = gui.getHeight();
        
        // draw recent unrecognized messages
        for(int i = 0; i < NUM_MSG_STRINGS; i++){
            ofDrawBitmapStringHighlight(msgStrings[i], guiPosX, guiPosY + guiH  + 40 + 15 * i);
        }
        
        string buf = "LISTENING OSC MESSAGES on port " + ofToString(portAdd) + " from SONIC PI";
        ofDrawBitmapStringHighlight(buf, guiPosX, guiPosY + guiH + 20);
    }
    
    if(showInfo) {
        ofPushStyle();
        ofSetColor(0, 0, 0, 100);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        info.draw();
        ofPopStyle();
        
        string buf = " LISTENING OSC MESSAGES on port " + ofToString(portAdd) + " from SONIC PI";
        ofDrawBitmapStringHighlight(buf, 0, info.getHeightInfo().height + 40);
        
        // draw recent unrecognized messages
        for(int i = 0; i < NUM_MSG_STRINGS; i++){
            ofDrawBitmapStringHighlight(msgStrings[i], 0, info.getHeightInfo().height  + 60 + 15 * i, ofColor::black, msgColors[i]);
        }
        
        
    }
}

void ofApp::autoPilot() {
    int numBins = 20;
    int numBuf = numBins;
    float * audioData = new float[numBuf];
    fftLive.getFftPeakData(audioData, numBuf);
    
    // todo: bass responsive, high, mid, responsive versions
    if(gAutoPilot) {
        gAutoPilotMix = false;
        // Auto SCENE Switch MID RANGE
        if(notify1.notifyPerSecond(0.5))
        {
            if(audioData[5] > audioMidThres->x && audioData[5] < audioMidThres->y) {
                //ofSeedRandom();
                int randId = ofRandom(0, mContentsManager.getNumContents());
                mContentsManager.switchContent(randId);
            }
        }
    }
    
    if(gAutoPilotMix) {
        gAutoPilot = false;
        for (int i = 0; i < mContentsManager.getAllContents().size(); i++) {
            if(i != 7 && i != 10 && i != 11 && i != 12 && i != 13 && i != 15 && i != 6)
                mContentsManager.mContents[i]->opacity = 1 + sin(ofGetFrameNum()*0.0002 * (i + 1 + fftLive.getAveragePeak())) - 1;
        }
        
    }
    
    
    // Process FX
    if(gFxAutoPilot) {
        
        if(audioData[2] > audioLowThres->x && audioData[2] < audioLowThres->y) {
            int randId = int(ofRandom(fxManager.getEffectNum()));
            
            
            if(randId != 20 && randId != 18 && randId != 5)
                fxManager.switchFX(randId);
        }
        
        if(fftLive.getAveragePeak() > audioHighThres->x && fftLive.getAveragePeak() < audioHighThres->y) {
            int randId = int(ofRandom(fxManager.getEffectNum()));
            
            if(randId != 20 && randId != 18 && randId != 5)
                fxManager.switchFX(randId);
        }
        
        if(notify2.notifyPerSecond(1.2)) {
            
            //fxManager.switchFX(int(ofRandom(fxManager.getEffectNum())));
        }
        
        if(notify3.notifyPerSecond(5)) {
            //fxManager.switchFX(int(ofRandom(fxManager.getEffectNum())));
        }
        
        if(notify1.notifyPerSecond(0.5)) {
            //fxManager.switchFX(int(ofRandom(fxManager.getEffectNum())));
            fxManager.disableAll();
        }
        
    }
    
    
    
    delete [] audioData;
}

//--------------------------------------------------------------
void ofApp::keyPressed(ofKeyEventArgs & _key){
    
    const int key = _key.key;
    
    // CMD
    if(_key.modifiers == 16) {
        if( key == '1') {
            gAutoPilot = !gAutoPilot;
        }
        
        if( key == '2') {
            gFxAutoPilot = !gFxAutoPilot;
        }
        
        if( key == '3') {
            gAutoPilotMix = !gAutoPilotMix;
        }
        
        if (key == 'i')
        {
            //isHelpDisplay ^= true;
            showInfo = !showInfo;
        }
        
        else if (key == 's')
        {
            
            //fxManager.switchFX(int(ofRandom(fxManager.getEffectNum())));
        }
        else if (key == 'e')
        {
            saveBtn = true;
            
        }
    }else{
        // 1 SHIFT
        // 2 KNTRL
        // 4 ALT
        
        // switching content
        if (key == '1') mContentsManager.switchContent(0);
        if (key == '2') mContentsManager.switchContent(1);
        if (key == '3') mContentsManager.switchContent(2);
        if (key == '4') mContentsManager.switchContent(3);
        if (key == '5') mContentsManager.switchContent(4);
        if (key == '6') mContentsManager.switchContent(5);
        if (key == '7') mContentsManager.switchContent(6);
        if (key == '8') mContentsManager.switchContent(7);
        if (key == '9') mContentsManager.switchContent(8);
        if (key == '0') mContentsManager.switchContent(9);
        
        // next or previous content
        if (key == OF_KEY_LEFT)  mContentsManager.switchPreviousContent();
        if (key == OF_KEY_RIGHT) mContentsManager.switchNextContent();
        
        /*if (key == 'z')
         converge = !converge;
         if (key == 'x')
         glow = !glow;
         if (key == 'c')
         shaker = !shaker;
         if (key == 'v')
         cut = !cut;
         if (key == 'b')
         twist = !twist;
         if (key == 'n')
         if (key == 'm')
         if (key == 'a')
         if (key == 's')
         if (key == 'd')
         invert = !invert;*/
        
        // TIMELINE SHORTCUTS
        if (key == ' ')
        {
            
        }
        else if (key == OF_KEY_RETURN){
            
        }
        else if (key == 'f')
        {
            
        }
    }
    
    mContentsManager.setDebug(ofxGetDebugMode());
}

//--------------------------------------------------------------
void ofApp::keyReleased(ofKeyEventArgs & key){
    
}

void ofApp::setHelpString()
{
    helpString = "[ APP SHORTCUTS ]  : DESCRIPTION\n";
    helpString += "---------------------------------------------\n";
    helpString += "[ CMD + g ]       : Show/Hide GUI Elements\n";
    helpString += "[ CMD + i ]       : Show/Hide Info\n";
    helpString += "[ CMD + f ]       : Enable/Disable FullScreen\n";
    helpString += "[ CMD + 1 ]       : Enable/Disable Autopilot\n";
    helpString += "[ CMD + 2 ]       : Enable/Disable FX Auto\n";
    helpString += "[ CMD + 3 ]       : Enable/Disable Scene Mix\n\n";
    helpString += "[ TIMELINE KEYS ]\n";
    helpString += "---------------------------------------------\n";
    helpString += "[ space bar ]     : Toggle Play\n";
    helpString += "[ return/enter ]  : Stop\n";
    helpString += "[ CMD + s ]       : Save Timeline\n";
    helpString += "[ CMD + l ]       : Load Timeline\n";
    helpString += "[ f ]             : Toggle AutoFillWindow mode\n\n";
    helpString += "[ FX KEYS ]\n";
    helpString += "---------------------------------------------\n";
    helpString += "[ z ]             : Converge\n";
    helpString += "[ x ]             : Glow\n";
    helpString += "[ c ]             : Shaker\n";
    helpString += "[ v ]             : Cut Slider\n";
    helpString += "[ b ]             : Twist\n";
    helpString += "[ n ]             : Outline\n";
    helpString += "[ m ]             : Noise\n";
    helpString += "[ a ]             : Slitscan\n";
    helpString += "[ s ]             : Swell\n";
    helpString += "[ d ]             : Invert\n\n";
    helpString += "[ SCENE KEYS ]\n";
    helpString += "---------------------------------------------\n";
    helpString += "[ 0 - 9 ]         : Switch Scenes\n";
    helpString += "[ UP ARROW ]      : Next Scene\n";
    helpString += "[ DOWN ARROW ]    : Previous Scene\n";
    helpString += "[ r ]             : Reallocate Scene Buffer\n";
    
    ofBitmapFont bf;
    helpStringWidth = bf.getBoundingBox(helpString, 0, 0).width;
    helpStringHeight = bf.getBoundingBox(helpString, 0, 0).height;
    
}

void ofApp::receiverParser() {
    // hide old messages
    for(int i = 0; i < NUM_MSG_STRINGS; i++){
        if(timers[i] < ofGetElapsedTimef()){
            msgStrings[i] = "";
        }
    }
    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        // unrecognized message: display on the bottom of the screen
        string msgString;
        msgString = "Incoming Message -> ";
        msgString += m.getAddress();
        
        
        if(m.getAddress() == "/par1") {
            if(mContentsManager.getCurrentContent() == 1) {
                c2->iterationBtnHandler();
            }else if(mContentsManager.getCurrentContent() == 0) {
                c1->setSize();
            }
            
            msgColors[currentMsgString] = ofColor::green;
        }else if(m.getAddress() == "/par2") {
            if(mContentsManager.getCurrentContent() == 1) {
                c2->setm1();
            }else if(mContentsManager.getCurrentContent() == 0) {
                c1->setParameter2(true);
            }
            msgColors[currentMsgString] = ofColor::green;
        }else if(m.getAddress() == "/par3") {
            if(mContentsManager.getCurrentContent() == 1) {
                c2->setm2();
            }else if(mContentsManager.getCurrentContent() == 0) {
                c1->setParameter3(true);
            }
            msgColors[currentMsgString] = ofColor::green;
        }else if(m.getAddress() == "/par4") {
            if(mContentsManager.getCurrentContent() == 1) {
                c2->setRandomColor();
            }else if(mContentsManager.getCurrentContent() == 0) {
                c1->setParameter4();
            }
            msgColors[currentMsgString] = ofColor::green;
        }else if(m.getAddress() == "/par5") {
            if(mContentsManager.getCurrentContent() == 1) {
                c2->setRandomRadius();
            }else if(mContentsManager.getCurrentContent() == 0) {
                c1->setParameter5();
            }
            msgColors[currentMsgString] = ofColor::green;
        }else if(m.getAddress() == "/onset_off") {
            if(mContentsManager.getCurrentContent() == 1) {
                c2->setRadius(350);
            }else if(mContentsManager.getCurrentContent() == 0) {
                
            }
            msgColors[currentMsgString] = ofColor::green;
        }
        else if(m.getAddress() == "/fx_on") {
            int randId = int(ofRandom(fxManager.getEffectNum()));
            
            if(randId != 20 && randId != 18 && randId != 5)
                fxManager.switchFX(randId);
            
            msgColors[currentMsgString] = ofColor::green;
            
        }else if(m.getAddress() == "/fx_off") {
            fxManager.disableAll();
            msgColors[currentMsgString] = ofColor::green;
        }else if(m.getAddress() == "/w_full_on") {
            if(!enableFullScreen) {
                enableFullScreen = true;
                ofSetFullscreen(enableFullScreen);
            }
            msgColors[currentMsgString] = ofColor::green;
        }else if(m.getAddress() == "/w_full_off") {
            if(enableFullScreen) {
                enableFullScreen = false;
                ofSetFullscreen(enableFullScreen);
            }
            msgColors[currentMsgString] = ofColor::green;
        }else if(m.getAddress() == "/scene1") {
            if(mContentsManager.getCurrentContent() != 0)
                mContentsManager.switchContent(0);
            msgColors[currentMsgString] = ofColor::green;
        }else if(m.getAddress() == "/scene2") {
            if(mContentsManager.getCurrentContent() != 1)
                mContentsManager.switchContent(1);
            
            msgColors[currentMsgString] = ofColor::green;
        }
        else{
            msgColors[currentMsgString] = ofColor::red;
            msgString = "Unsupported Message-> ";
            msgString += m.getAddress();
            
            for(size_t i = 0; i < m.getNumArgs(); i++){
                
                // get the argument type
                msgString += " : ";
                msgString += m.getArgTypeName(i);
                msgString += ":";
                
                // display the argument - make sure we get the right type
                if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                    msgString += ofToString(m.getArgAsInt32(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                    msgString += ofToString(m.getArgAsFloat(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                    msgString += m.getArgAsString(i);
                }
                else{
                    msgString += "unhandled argument type " + m.getArgTypeName(i);
                }
            }
        }
        // add to the list of strings to display
        int prevId = (currentMsgString > 0) ? currentMsgString - 1 : NUM_MSG_STRINGS - 1;
        if(msgColors[prevId] != ofColor::red)
            msgColors[prevId] = ofColor(0, 255, 0, 100);
        msgStrings[currentMsgString] = msgString;
        timers[currentMsgString] = ofGetElapsedTimef() + 2.0f;
        
        currentMsgString = (currentMsgString + 1) % NUM_MSG_STRINGS;
        
        // clear the next line
        msgStrings[currentMsgString] = "";
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int _w, int _h){
    w = _w;
    h = _h;
    
    generateScreenFbo();
    
    fxManager.windowResized(w, h);
    mContentsManager.windowResized(w, h);
    
    guiAudio.setPosition(ofGetWidth() - 320, 0);
}

void ofApp::generateScreenFbo() {
    screenFbo.clear();
    screenFbo.allocate(w * scale, h * scale, internalFormat,4);
    screenFbo.begin();
    ofClear(0, 0);
    screenFbo.end();
}


//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

void ofApp::exit() {
    ofSoundStreamStop();
}



void ofApp::saveSettingsHandler() {
    gui.saveToFile(settingFile);
    
}

void ofApp::enableFullScreenHandler(bool & val) {
    ofSetFullscreen(val);
}


/*

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key=='f') {
        ofToggleFullscreen();
    }
    
    else if(key=='g') {
        //showGUI = !showGUI;
    }
    
    else if(key == ' ') {
        superShaper.iterationBtnHandler();
    }
    
    else if(key == 'x') {
        //invertBg = !invertBg;
    }
    
    else if(key == '<') {
        //superShaper.showGui = !superShaper.showGui;
    }
}

*/
