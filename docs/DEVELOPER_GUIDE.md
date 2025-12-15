# AURE Developer Guide (Technical)

Users with prior knowledge of OpenFrameworks may bypass the introductory material and proceed directly to the `Scene Blueprint` section.


This document is for contributors who want to extend AURE (openFrameworks) with new generative “scenes” and wire them to OSC.


## Creating a new scene (step-by-step)
Below is a compact template that follows the structure used by default scenes in AURE: GUI parameters, optional easing, time-based triggers, and simple OSC-facing methods.


1. Create a new header in `src/` (pattern: `cN_MyScene.h`) and derive from `ofxContentsManager::Content`.
2. In the constructor:
   - call `initGUI(...)` (choose a GUI x-offset; many scenes use `posGuiConent1`)
   - add any `ofParameter` fields to `gui` via `gui.add(...)`
   - call `loadGUISettings()` if you want persisted GUI state
   - call `setup()`
3. Implement `setup()`, `update()`, `draw()`.
4. Register it in `ofApp`:
   - include the header in [src/ofApp.h](../src/ofApp.h)
   - create and store a pointer like `c3_MyScene *c3;`
   - instantiate it in `ofApp::setup()` and call `mContentsManager.addContent(c3)`
5. Add OSC mapping in `ofApp::receiverParser()`:
   - decide how `/par1..5` affect your scene
   - in each OSC branch, add a `else if(mContentsManager.getCurrentContent() == <yourIndex>) { ... }`

Tip: Keep OSC methods small and “trigger-like” (set a flag, set a timer, randomize a parameter). This matches how `c1_Template` uses `initTime*` and booleans to create short-lived transitions.

### Scene blueprint
```cpp 
#pragma once

#include "ofMain.h"
#include "ofxContentsManager.h"
#include "ofxEasing.h"
#include "globalParameters.h"

class c3_NewScene : public ofxContentsManager::Content {
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
    c3_NewScene() {
        ofLog() << "CONTENT :: " << this->getName() << " initialized...";

        initGUI(posGuiConent1);
        gui.add(intensity);
        gui.add(baseColor);

        loadGUISettings();
        setup();
    }

    ~c3_NewScene() override = default;

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
```


## High-level architecture

- App entrypoint: [src/main.cpp](../src/main.cpp)
  - Creates an OpenGL 2.1 window (default 1920×1080) and runs `ofApp` via `ofRunAppWithAppUtils(new ofApp())`.
- Application glue: [src/ofApp.h](../src/ofApp.h) + [src/ofApp.cpp](../src/ofApp.cpp)
  - Owns:
    - `ofxContentsManager::Manager mContentsManager` (scene manager)
    - `ofxPostProcessingManager fxManager` (post effects)
    - `ofxOscReceiver receiver` (OSC input)
  - Frame loop:
    - `update()` parses OSC then calls `mContentsManager.update()`
    - `draw()` calls `mContentsManager.draw(...)` wrapped by `fxManager.begin()/end()`
- Scenes (“Content”): classes derived from `ofxContentsManager::Content` (e.g. [src/c1_Template.h](../src/c1_Template.h), [src/c2_SuperShape.h](../src/c2_SuperShape.h))
  - Each scene typically:
    - sets up its own GUI parameters via the inherited `gui` and `initGUI(...)`
    - implements `setup()`, `update()`, `draw()`
    - exposes a small set of methods that `ofApp::receiverParser()` triggers when OSC commands arrive

## Globals you will see

- [src/globalParameters.h](../src/globalParameters.h)
  - `w, h` are global window width/height used by scenes.
  - `posGuiConent1` is a shared GUI positioning constant used by available scenes (`c1_Template` and `c2_SuperShape`).

## How scenes are registered and switched

Scene creation and registration currently happens in `ofApp::setup()`:

- allocate scenes with `new`
- call `mContentsManager.setup(w, h, internalFormat, 4)`
- register each scene with `mContentsManager.addContent(scenePtr)`
- pick an initial scene via `mContentsManager.switchContent(...)`

Scene switching inputs:

- Keyboard numeric keys call `mContentsManager.switchContent(index)`.
- OSC `/scene1` and `/scene2` explicitly switch to indices `0` and `1`.

Important: indices are purely the registration order in `ofApp::setup()`.

## OSC flow (where to hook new controls)

OSC is parsed in `ofApp::receiverParser()`.

- `/par1`..`/par5` are dispatched manually based on `mContentsManager.getCurrentContent()`.
  - Example mapping today:
    - Scene index 0 (`c1_Template`): 
        - `/par1` → `setSize()` 
        - `/par2` → `setParameter2(true)`
        - `/par3` → `setParameter3(true)`
        - `/par4` → `setParameter4()`
        - `/par5` → `setParameter5()`
    - Scene index 1 (`c2_SuperShape`): 
        - `/par1` → `iterationBtnHandler()`
        - `/par2` → `setm1()`
        - `/par3` → `setm2()`
        - `/par4` → `setRandomColor()`
        - `/par5` → `setRandomRadius()`

If you add a new scene, you will typically add a new OSC dispatch branch for it in `receiverParser()`.

## Practical notes

- GUI visibility: content GUIs are drawn only when debug mode is enabled (see `ofxGetDebugMode()` usage in `ofApp::draw()`).
- Window resize: `ofApp::windowResized()` updates `w/h` and calls `mContentsManager.windowResized(w, h)`; if your scene caches FBOs, reallocate them in `windowResized` inside the scene (if you add such a hook) or re-check sizes in `update()`.
- Data path: runtime assets/config live under `bin/.../Resources/data/` (and a mirrored `data/` folder). If you load files, prefer OF data path conventions (`data/`).
