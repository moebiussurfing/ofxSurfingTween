#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetCircleResolution(200);

	//
	// Workflow
	//
	// 1. The addon will clone all the params (sources) to new ones (targets).
	// 2. Before trig the tween:
	//		We will prepare the source params first.
	//		To tween the targets params to these sources params previously setted.
	// 3. We will configure the animation: 
	//		This defines the tween duration, speed, pre-pause, curve tween.. 
	// 4. Then, we trig the transition. GO! 
	// 5. Tweening starts and goes to the targets values 
	//		(for all the registered parameters.)
	//

	//-

	params.setName("paramsGroup"); // main container
	params.add(size1.set("size1", 1, 0, 2));
	params.add(size2.set("size2", ofGetHeight() * 0.5, 0, ofGetHeight() * 0.25));
	params.add(rotation.set("rotation", 1, 0, 2));
	params.add(rotationOffset.set("rotationOffset", 180, 0, 360));

	// tweener
	dataTween.setup(params);

	//--

	// local debug gui

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
	if (bGui && ofGetFrameNum() % 20 == 0)
	{
		float _size1 = dataTween.get(size1);
		float _size2 = dataTween.get(size2);
		float _rotation = dataTween.get(rotation);
		float _rotationOffset = dataTween.get(rotationOffset);

		// log
		string sp = "  \t  ";
		string str = "TWEENED >" + sp;
		str += size1.getName() + ":" + ofToString(_size1, 2); str += sp;
		str += size2.getName() + ":" + ofToString(_size2); str += sp;
		str += rotation.getName() + ":" + ofToString(_rotation); str += sp;
		str += rotationOffset.getName() + ":" + ofToString(_rotationOffset); str += sp;
		ofLogNotice(__FUNCTION__) << str;
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	drawScene();

	if (bGui) {
		guiSource.draw();
		guiTween.draw();
	}
}

//--------------------------------------------------------------
void ofApp::drawScene()
{
	// get tweened variables

	float _size1 = dataTween.get(size1);
	float _size2 = dataTween.get(size2);
	float _rotation = dataTween.get(rotation);
	float _rotationOffset = dataTween.get(rotationOffset);
	
	//--
	
	static ofColor colorBg = 32;
	ofClear(colorBg);

	ofPushStyle();
	ofPushMatrix();

	// some code from @Daandelange > https://github.com/Daandelange/ofxImGui/tree/master/example-sharedcontext

	ofParameter<ofFloatColor> background{ "Background", ofFloatColor::black };
	ofParameter<ofFloatColor> foreground{ "Foreground", ofFloatColor::black };
	ImVec4 color = { 0,0,0,1.0 };

	int a = ofMap(_size1, 0, 2, 255, 170);

	ofSetColor(color.x * 255, color.y * 255, color.z * 255, color.w * a);
	//ofSetColor(color.x * 255, color.y * 255, color.z * 255, color.w * 255);
	float _scale = 0.13f;
	float r = ofGetHeight()*_scale*_size1*(1);
	ofDrawCircle(ofGetWidth()*0.5f, ofGetHeight()*0.5f, r);

	float _scale2 = 0.2f;
	float staticAnimationPos = 1;
	int rectSize = _size2 + abs((((ofGetHeight() * _scale2 - _size2)*_size1))*(staticAnimationPos));

	ofTranslate(ofGetWidth()*.5f, ofGetHeight()*.5f);
	ofRotateDeg(ofGetElapsedTimef() * TWO_PI);

	float rot = _rotationOffset / 3.0;
	ofRotateDeg(ofGetElapsedTimef() * TWO_PI);
	ofRotateZDeg(_rotation * 45);
	ofScale(1.3);
	ofRotateDeg(rot);

	int a2 = ofMap(_size2, 0, size2.getMax(), 255, 225);

	ofSetColor(background->r * 255, background->g * 255, background->b * 255, background->a * a2);
	ofDrawRectangle(-rectSize * .5f, -rectSize * .5f, rectSize, rectSize);
	
	ofRotateDeg(rot);
	ofSetColor(background->r * 255, background->g * 255, background->b * 255, background->a * a2);
	ofDrawRectangle(-rectSize * .5f, -rectSize * .5f, rectSize, rectSize);
	
	ofRotateDeg(rot);
	ofSetColor(foreground->r * 255, foreground->g * 255, foreground->b * 255, foreground->a * a2);
	ofDrawRectangle(-rectSize * .5f, -rectSize * .5f, rectSize, rectSize);
	
	//// inner circle
	//ofFill();
	//ofSetColor(colorBg);
	//int r2 = ofMap(_size1, 0, size1.getMax(), 3, 20, true);
	//ofDrawCircle(0,0, r2);

	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	dataTween.keyPressed(key);

	if (key == OF_KEY_F9) bGui = !bGui;
}