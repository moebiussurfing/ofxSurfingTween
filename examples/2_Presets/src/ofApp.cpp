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
	params.add(sizef.set("sizef", 1, 0, 2));
	params.add(speedf.set("speedf", 1, 0, 2));
	params.add(rotationOffset.set("rotationOffset", 180, 0, 360));
	params.add(minSize.set("minSize", ofGetHeight() * 0.5, 0, ofGetHeight() * 0.25));
	params.add(shapeType.set("shapeType", 0, 0, 1));

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
		float _sizef = dataTween.get(sizef);
		float _speedf = dataTween.get(speedf);
		float _rotationOffset = dataTween.get(rotationOffset);
		float _minSize = dataTween.get(minSize);
		float _shapeType = dataTween.get(shapeType);

		// log
		string sp = "  \t  ";
		string str = "TWEENED >" + sp;
		str += sizef.getName() + ":" + ofToString(_sizef, 2); str += sp;
		str += speedf.getName() + ":" + ofToString(_speedf); str += sp;
		str += rotationOffset.getName() + ":" + ofToString(_rotationOffset); str += sp;
		str += minSize.getName() + ":" + ofToString(_minSize); str += sp;
		str += shapeType.getName() + ":" + ofToString(_shapeType); str += sp;
		ofLogNotice(__FUNCTION__) << str;
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	if (bGui) {
		guiSource.draw();
		guiTween.draw();
	}

	drawScene();
}

//--------------------------------------------------------------
void ofApp::drawScene()
{
	// get tweened variables
	float _sizef = dataTween.get(sizef);
	float _speedf = dataTween.get(speedf);
	float _rotationOffset = dataTween.get(rotationOffset);
	float _minSize = dataTween.get(minSize);

	//-

	// code from @Daandelange > https://github.com/Daandelange/ofxImGui/tree/master/example-sharedcontext

	ofParameter<ofFloatColor> background{ "Background", ofFloatColor::black };
	ofParameter<ofFloatColor> foreground{ "Foreground", ofFloatColor::grey };
	ImVec4 color = { 1,1,1,1 };

	ofSetColor(color.x * 255, color.y * 255, color.z * 255, color.w * 255);
	float _scale = 0.2f;
	ofDrawCircle(ofGetWidth()*0.5f, ofGetHeight()*0.5f, ofGetHeight()*_scale*_sizef*((fmod(1*_speedf, 1.f) - 0.5f)*2.f));
	//ofDrawCircle(ofGetWidth()*.5f, ofGetHeight()*.5f, ofGetHeight()*.5f*_sizef*((fmod(ofGetElapsedTimef()*_speedf, 1.f) - 0.5f)*2.f));

	float _scale2 = 0.2f;
	float staticAnimationPos = ((cos(1*TWO_PI*_speedf)));
	//float staticAnimationPos = ((cos(ofGetElapsedTimef()*TWO_PI*_speedf)));
	//int rectSize = _minSize + abs((((ofGetHeight() - _minSize)*_sizef))*(staticAnimationPos));
	int rectSize = _minSize + abs((((ofGetHeight() * _scale2 - _minSize)*_sizef))*(staticAnimationPos));

	ofPushMatrix();
	ofTranslate(ofGetWidth()*.5f, ofGetHeight()*.5f);

	ofRotateDeg((-_rotationOffset * (staticAnimationPos)));
	ofSetColor(background->r * 255, background->g * 255, background->b * 255, background->a * 255);
	ofDrawRectangle(-rectSize * .5f, -rectSize * .5f, rectSize, rectSize);
	ofRotateDeg(_rotationOffset*(staticAnimationPos));
	ofSetColor(color.x * 255, color.y * 255, color.z * 255, color.w * 255);
	ofDrawRectangle(-rectSize * .5f, -rectSize * .5f, rectSize, rectSize);
	ofRotateDeg(_rotationOffset*(staticAnimationPos));
	ofSetColor(foreground->r * 255, foreground->g * 255, foreground->b * 255, foreground->a * 255);
	ofDrawRectangle(-rectSize * .5f, -rectSize * .5f, rectSize, rectSize);
	
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	dataTween.keyPressed(key);

	if (key == OF_KEY_F9) bGui = !bGui;
}