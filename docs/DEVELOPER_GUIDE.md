# AURE Developer Guide
Quick setup and scene creation guide for openFrameworks developers.

## 1. Prerequisites & Compilation
Refer to the `Dependencies` section on `README.md` file.


## 2. Adding a New Scene 
### Step 1: Create the Scene Header
Create a new header file in `src/` (e.g. `src/cN_MyScene.h`) deriving from `BaseContent`.

```cpp
#pragma once

#include "ofMain.h"
#include "BaseContent.h"
#include "ofxEasing.h"
#include "globalParameters.h"

class c4_MyScene : public BaseContent {
private:
    ofParameter<float> size = {"Size", 100.0f, 10.0f, 500.0f};
    ofParameter<ofColor> color = {"Color", ofColor::white, ofColor(0), ofColor(255)};
    float triggerTime = 0.0f;

public:
    c4_MyScene() {
        initGUI(posGuiConent1);
        gui.add(size);
        gui.add(color);
        loadGUISettings();
        setup();
    }

    ~c4_MyScene() override = default;

    void setup() override {
        // Allocate FBOs, meshes, or load assets here
    }

    void update() override {
        // Update scene animations and logic
    }

    void draw() override {
        ofPushStyle();
        ofSetColor(color);
        ofDrawCircle(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f, size);
        ofPopStyle();
    }

    // --- Optional OSC Hooks (called automatically via /par1 - /par5, /reset) ---
    void onPar1() override {
        size = ofRandom(20.0f, 300.0f);
    }

    void onPar2() override {
        color.set(ofColor(ofRandom(255), ofRandom(255), ofRandom(255)));
    }

    void onReset() override {
        size = 100.0f;
    }
};
```


### Step 2: Register the Scene in `ofApp`

1. **In `src/ofApp.h`:**
   Include the new header and declare the scene pointer:
   ```cpp
   #include "c4_MyScene.h"

   // Inside ofApp class:
   c4_MyScene *c4;
   ```

2. **In `src/ofApp.cpp` (`setup()`):**
   Instantiate the scene and add it to `mContentsManager`:
   ```cpp
   c4 = new c4_MyScene();
   mContentsManager.addContent(c4);
   ```