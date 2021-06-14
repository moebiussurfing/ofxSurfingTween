#pragma once

#include "ofMain.h"

#include "ofxSurfingTween.h"

#include "ofxGui.h" // only to local gui
#include "ofxWindowApp.h" // only to handle window settings

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void drawScene();
		void keyPressed(int key);

		ofxSurfingTween dataTween;

		ofParameterGroup params;
		ofParameter<float> sizef;
		ofParameter<float> speedf;
		ofParameter<float> rotationOffset;
		ofParameter<int> minSize;
		ofParameter<int> shapeType;
		
		ofxPanel guiSource;
		ofxPanel guiTween;
		bool bGui = false;

		ofxWindowApp windowApp;
};
