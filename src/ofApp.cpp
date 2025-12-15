#include "ofApp.h"

#define internalFormat GL_RGB32F


//--------------------------------------------------------------
void ofApp::setup(){
	
	//ofSetDataPathRoot("../Resources/data/");
	
	
	showGUI = false;
	showInfo = false;
	settingFile = "settings-aure.json";
	// GUI
	//ofxGuiSetBitmapFont();
	//gui.setDefaultTextPadding(6);
	//gui.setDefaultWidth(300);
	//gui.setDefaultHeight(20);
	//ofxAppGui.setup(settingFile);
	//gui.add(showGui);
	//gui.add(enableFullScreen);
	//gui.add(serverAdd);
	//gui.add(portAdd);
	//gui.add(saveSettings);
	
	//gui.loadFromFile(settingFile);
	
	enableFullScreen.addListener(this, &ofApp::enableFullScreenHandler);
	saveSettings.addListener(this, &ofApp::saveSettingsHandler);
	
	
	// OSC
	receiver.setup(ofToInt(portAdd));
	
	
	//ofEnableSmoothing();
	
	ofSetFrameRate(60.0);
	resolution = 1;
	w = ofGetWidth() * resolution;
	h = ofGetHeight() * resolution;
	scale = 1;
	
	//////////////////////////////////////////////////////////////////
	// LOAD & INIT FONTS
	//////////////////////////////////////////////////////////////////
	// Show GUI and AUDIOGUI
	ofxSetDebugMode(false);
	
	//////////////////////////////////////////////////////////////////
	// INIT SCENES
	//////////////////////////////////////////////////////////////////
	c1                         = new c1_Template();
	c2                         = new c2_SuperShape();
	c3						   = new c3_Circles();
	
	//////////////////////////////////////////////////////////////////
	// INIT CONTENT MANAGER & ADD SCENE CLASSES
	//////////////////////////////////////////////////////////////////
	mContentsManager.setup( w, h, internalFormat,4);
	mContentsManager.addContent(c1);
	mContentsManager.addContent(c2);
	mContentsManager.addContent(c3);
	
	mContentsManager.setDebug(ofxGetDebugMode());
	
	
	/////////////////////////////////////////////////////////////////
	// INIT MAIN GUI
	/////////////////////////////////////////////////////////////////
	ofxGUIGenerate();
	
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

//--------------------------------------------------------------
void ofApp::update(){
	
	// Parse OSC
	receiverParser();
	
	fxManager.updateValues();
	
	// Update Content Manager
	mContentsManager.update();
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
		//gui.draw();
		
		
		auto guiPosX = ofxAppGui.getPosition().x;
		auto guiPosY = ofxAppGui.getPosition().y;
		auto guiH    = ofxAppGui.getHeight();
		
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
	}
	
	mContentsManager.setDebug(ofxGetDebugMode());
}

//--------------------------------------------------------------
void ofApp::keyReleased(ofKeyEventArgs & key){
	
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
		string addr = m.getAddress();
		
		if(addr == "/par1") {
			if(mContentsManager.getCurrentContent() == 1) {
				c2->iterationBtnHandler();
			}else if(mContentsManager.getCurrentContent() == 0) {
				c1->setSize();
			}else if(mContentsManager.getCurrentContent() == 2) {
				c3->onPar1();
			}
			
			msgColors[currentMsgString] = ofColor::green;
		}else if(addr == "/par2") {
			if(mContentsManager.getCurrentContent() == 1) {
				c2->setm1();
			}else if(mContentsManager.getCurrentContent() == 0) {
				c1->setParameter2(true);
			}else if(mContentsManager.getCurrentContent() == 2) {
				c3->onPar2();
			}
			msgColors[currentMsgString] = ofColor::green;
		}else if(addr == "/par3") {
			if(mContentsManager.getCurrentContent() == 1) {
				c2->setm2();
			}else if(mContentsManager.getCurrentContent() == 0) {
				c1->setParameter3(true);
			}else if(mContentsManager.getCurrentContent() == 2) {
				c3->onPar3();
			}
			msgColors[currentMsgString] = ofColor::green;
		}else if(addr == "/par4") {
			if(mContentsManager.getCurrentContent() == 1) {
				c2->setRandomColor();
			}else if(mContentsManager.getCurrentContent() == 0) {
				c1->setParameter4();
			}else if(mContentsManager.getCurrentContent() == 2) {
				c3->onPar4();
			}
			msgColors[currentMsgString] = ofColor::green;
		}else if(addr == "/par5") {
			if(mContentsManager.getCurrentContent() == 1) {
				c2->setRandomRadius();
			}else if(mContentsManager.getCurrentContent() == 0) {
				c1->setParameter5();
			}else if(mContentsManager.getCurrentContent() == 2) {
				c3->onPar5();
			}
			msgColors[currentMsgString] = ofColor::green;
		}else if(addr == "/reset") {
			if(mContentsManager.getCurrentContent() == 1) {
				c2->setRadius(350);
			}else if(mContentsManager.getCurrentContent() == 0) {
				
			}
			msgColors[currentMsgString] = ofColor::green;
		}
		else if(addr == "/fx_on") {
			int randId = int(ofRandom(fxManager.getEffectNum()));
			fxManager.switchFX(randId);
			
			msgColors[currentMsgString] = ofColor::green;
			
		}else if(addr == "/fx_off") {
			fxManager.disableAll();
			msgColors[currentMsgString] = ofColor::green;
		}else if(addr == "/w_full_on") {
			if(!enableFullScreen) {
				enableFullScreen = true;
				ofSetFullscreen(enableFullScreen);
			}
			msgColors[currentMsgString] = ofColor::green;
		}else if(addr == "/w_full_off") {
			if(enableFullScreen) {
				enableFullScreen = false;
				ofSetFullscreen(enableFullScreen);
			}
			msgColors[currentMsgString] = ofColor::green;
		}// replace the repeated if/else chain in `ofApp.cpp` with this
		if (addr.rfind("/scene", 0) == 0) {
			string numStr = addr.substr(6); // "/scene" length == 6
			if (!numStr.empty() && std::all_of(numStr.begin(), numStr.end(), ::isdigit)) {
				int idx = std::stoi(numStr) - 1; // convert "1"->0, "2"->1, ...
				if (idx >= 0 && idx != mContentsManager.getCurrentContent()) {
					mContentsManager.switchContent(idx);
				}
				msgColors[currentMsgString] = ofColor::green;
			}
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
	//gui.saveToFile(settingFile);
	
}

void ofApp::enableFullScreenHandler(bool & val) {
	ofSetFullscreen(val);
}
