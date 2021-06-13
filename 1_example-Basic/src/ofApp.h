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
		void keyPressed(int key);

		ofxSurfingTween dataTween;

		ofParameterGroup params;
		ofParameter<float> lineWidth;
		ofParameter<float> separation;
		ofParameter<float> speed;
		ofParameter<int> shapeType;
		ofParameter<int> amount;
		ofParameter<int> size;
		ofParameterGroup params2;
		ofParameter<int> shapeType2;
		ofParameter<int> amount2;
		ofParameter<int> size2;
		ofParameterGroup params3;
		ofParameter<float> lineWidth3;
		ofParameter<float> separation3;
		ofParameter<float> speed3;
		ofParameter<int> shapeType3;
		
		ofxPanel guiSource;
		ofxPanel guiTween;
		bool bGui = false;

		ofxWindowApp windowApp;
};
