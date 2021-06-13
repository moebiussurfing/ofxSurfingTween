#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	// Workflow
	// 1. The addon will clone all the params (sources) to new ones (targets).
	// 2. We will prepare the source params first.
	// 3. We will configure the animation: 
	//		duration, pre pause, curve tween etc... 
	//		This defines the tween speed, curve..
	// 4. Then, we will trig GO! 
	//		To tween the targets params to these sources params previously setted.
	// 5. Tweening starts and goes to the targets values 
	//		(for all the registered parameters.)

	params.setName("paramsGroup");// main container
	params.add(lineWidth.set("lineWidth", 0.5, 0, 1));
	params.add(separation.set("separation", 50, 1, 100));
	params.add(speed.set("speed", 0.5, 0, 1));
	params.add(shapeType.set("shapeType", 0, -50, 50));
	params.add(size.set("size", 100, 0, 100));
	params.add(amount.set("amount", 10, 0, 25));
	params2.setName("paramsGroup2");// nested
	params3.setName("paramsGroup3");// nested
	params2.add(shapeType2.set("shapeType2", 0, -50, 50));
	params2.add(size2.set("size2", 100, 0, 100));
	params2.add(amount2.set("amount2", 10, 0, 25));
	params3.add(lineWidth3.set("lineWidth3", 0.5, 0, 1));
	params3.add(separation3.set("separation3", 50, 1, 100));
	params3.add(speed3.set("speed3", 0.5, 0, 1));
	params2.add(params3);
	params.add(params2);

	// tweener
	dataTween.setup(params);

	//--

	// input
	guiSource.setup("Source"); // source
	guiSource.add(params);
	guiSource.setPosition(10, 650);

	// output
	guiTween.setup("Target"); // tweened
	guiTween.add(dataTween.getParamsSmoothed());
	guiTween.setPosition(220, 650);
}

//--------------------------------------------------------------
void ofApp::update() {

	/*
	
	NOTE:
	learn how to access the tweened parameters.
	notice that we can't overwrite the tween on the source parameters!
	
	*/

	// simple getters

	// slowdown log a bit
	static bool bDebug = bGui;
	if (bDebug && ofGetFrameNum() % 20 == 0) 
	{
		float _lineWidth = dataTween.get(lineWidth);
		int _shapeType = dataTween.get(shapeType);
		int _size = dataTween.get(size);
		int _amount = dataTween.get(amount);

		// log
		string sp = "  \t  ";
		string str = "TWEENED >" + sp;
		str += lineWidth.getName() + ":" + ofToString(_lineWidth, 2); str += sp;
		str += shapeType.getName() + ":" + ofToString(_shapeType); str += sp;
		str += size.getName() + ":" + ofToString(_size); str += sp;
		str += amount.getName() + ":" + ofToString(_amount); str += sp;
		ofLogNotice(__FUNCTION__) << str;
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	if (bGui) {
		guiSource.draw();
		guiTween.draw();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	dataTween.keyPressed(key);

	if (key == OF_KEY_F9) bGui = !bGui;
}