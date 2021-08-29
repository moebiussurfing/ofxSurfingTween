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

		// scene params
		ofParameterGroup params;
		ofParameter<float> size1;
		ofParameter<float> rotation;
		ofParameter<float> rotationOffset;
		ofParameter<int> size2;
		
		ofxPanel guiSource;
		ofxPanel guiTween;
		bool bGui = false;

		ofxWindowApp windowApp;
};
