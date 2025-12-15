//
//  c3_Circles.h
//  AURE Beta
//
//  Created by alp tugan on 15.12.2025.
//
#pragma once

#include "ofMain.h"
#include "ofxContentsManager.h"
#include "ofxEasing.h"
#include "globalParameters.h"

class  c3_Circles : public ofxContentsManager::Content {
private:
	std::vector<ofxeasing::function> easings{
		ofxeasing::linear::easeNone,
		ofxeasing::quad::easeOut,
		ofxeasing::cubic::easeIn,
	};

	// Example GUI parameters
	ofParameter<float> intensity = {"Intensity", 0.5f, 0.0f, 1.0f};
	ofParameter<ofColor> baseColor = {"Base Color", ofColor::white, ofColor(0), ofColor(255)};

	// Trigger state (pattern used in c1_Template)
	float triggerTimePar1 = 0.0f;
	float triggerTimePar2 = 0.0f;
	bool triggerPar2 = false;

public:
	c3_Circles() {
		ofLog() << "CONTENT :: " << this->getName() << " initialized...";

		initGUI(posGuiConent1);
		gui.add(intensity);
		gui.add(baseColor);

		loadGUISettings();
		setup();
	}

	~c3_Circles() {}

	void setup() {
		// allocate meshes / fbo / load assets here
	}

	// OSC hooks (map these from ofApp::receiverParser)
	void onPar1() {
		triggerTimePar1 = ofGetElapsedTimef();
		intensity = ofRandom(0.0f, 1.0f);
	}

	void onPar2(bool val = true) {
		triggerPar2 = val;
		triggerTimePar2 = ofGetElapsedTimef();
		baseColor = ofColor(ofRandom(255), ofRandom(255), ofRandom(255));
	}

	void onPar3() {
		// optional: another trigger/randomize
	}

	void onPar4() {
		// optional: another trigger/randomize
	}

	void onPar5() {
		// optional: another trigger/randomize
	}

	void update() {
		// Use global window dims if needed
		float ww = static_cast<float>(w);
		float hh = static_cast<float>(h);
		(void)ww;
		(void)hh;

		// Example: time-based decay similar to c1_Template
		if(triggerPar2 && (ofGetElapsedTimef() - triggerTimePar2) > 0.3f) {
			triggerPar2 = false;
		}
	}

	void draw() {
		ofPushStyle();
		ofSetColor(baseColor);

		// Example: short easing-driven flash after onPar1()
		float t = ofxeasing::map_clamp(
			ofGetElapsedTimef(), triggerTimePar1, triggerTimePar1 + 0.25f,
			0.0f, 1.0f, easings[1]
		);

		float radius = 50.0f + 400.0f * t * intensity;
		ofDrawCircle(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f, radius);
		ofPopStyle();
	}
};
