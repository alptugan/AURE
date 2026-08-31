//
//  BaseContent.h
//  AURE Beta
//
//  Created by alp tugan on 30.08.2026.
//

#pragma once
#include "ofxContentsManager.h"

class BaseContent : public ofxContentsManager::Content {
public:
	virtual ~BaseContent() = default;

	virtual void setup() {}

	// Virtual parameter handlers - override in subclasses as needed
	virtual void onPar1() {}
	virtual void onPar2() {}
	virtual void onPar3() {}
	virtual void onPar4() {}
	virtual void onPar5() {}
	virtual void onReset() {}
};
