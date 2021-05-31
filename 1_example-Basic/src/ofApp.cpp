#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	// parameters to smooth
	params.setName("paramsGroup");// main container
	params2.setName("paramsGroup2");// nested
	params3.setName("paramsGroup3");// nested
	params.add(lineWidth.set("lineWidth", 0.5, 0, 1));
	params.add(separation.set("separation", 50, 1, 100));
	params.add(speed.set("speed", 0.5, 0, 1));
	params.add(shapeType.set("shapeType", 0, -50, 50));
	params.add(size.set("size", 100, 0, 100));
	params.add(amount.set("amount", 10, 0, 25));
	params2.add(shapeType2.set("shapeType2", 0, -50, 50));
	params2.add(size2.set("size2", 100, 0, 100));
	params2.add(amount2.set("amount2", 10, 0, 25));
	params3.add(lineWidth3.set("lineWidth3", 0.5, 0, 1));
	params3.add(separation3.set("separation3", 50, 1, 100));
	params3.add(speed3.set("speed3", 0.5, 0, 1));
	params2.add(params3);
	params.add(params2);

	// smoother
	data.setup(params);

	//--

	// gui
	ofxSurfingHelpers::setThemeDark_ofxGui();

	// input
	gui.setup("Input"); // source
	gui.add(params);
	gui.setPosition(10, 10);

	// output
	guiSmooth.setup("Output"); // smoothed
	guiSmooth.add(data.getParamsSmoothed());
	guiSmooth.setPosition(220, 10);
}

//--------------------------------------------------------------
void ofApp::update() {

	/*
	// NOTE:
	// learn how to access the smoothed parameters.
	// notice that we can't overwrite the smoothing on the source parameters!
	// we can get the smoothed params/variables doing these different approaches:
	*/

	// 0. simple getters

	// slowdown log a bit
	static bool bDebug = false;
	if (bDebug && ofGetFrameNum() % 6 == 0) 
	{
		float _lineWidth = data.get(lineWidth);
		int _shapeType = data.get(shapeType);
		int _size = data.get(size);
		int _amount = data.get(amount);

		//log
		string sp = "   \t\t       ";
		string str = "SMOOTHED >" + sp;
		str += lineWidth.getName() + ":" + ofToString(_lineWidth, 2); str += sp;
		str += shapeType.getName() + ":" + ofToString(_shapeType); str += sp;
		str += size.getName() + ":" + ofToString(_size); str += sp;
		str += amount.getName() + ":" + ofToString(_amount); str += sp;
		ofLogNotice(__FUNCTION__) << str;
	}

	/*

	// more snippets for inspiration:

	// 1. just the param values
	int _shapeType = data.getParamIntValue(shapeType);
	int _amount = data.getParamIntValue(amount);
	float _speed = data.getParamFloatValue(speed);

	// 2. the parameter itself
	ofParameter<int> _amount = data.getParamInt(amount.getName());
	ofParameter<float> _lineWidth = data.getParamFloat(lineWidth.getName());
	ofParameter<float> _separation = data.getParamFloat(separation.getName());

	// 4. the ofAbstractParameter
	// to be casted to his correct type after
	auto &ap = data.getParamAbstract(lineWidth);
	{
		auto type = ap.type();
		bool isGroup = type == typeid(ofParameterGroup).name();
		bool isFloat = type == typeid(ofParameter<float>).name();
		bool isInt = type == typeid(ofParameter<int>).name();
		bool isBool = type == typeid(ofParameter<bool>).name();
		string str = ap.getName();
		if (isFloat)
		{
			ofParameter<float> fp = ap.cast<float>();
		}
		else if (isInt)
		{
			ofParameter<int> ip = ap.cast<int>();
		}
	}

	// 4. the whole group
	// requires more work after, like iterate the group content, get a param by name...etc.
	auto &group = data.getParamsSmoothed();
	for (int i = 0; i < group.size(); i++)
	{
		auto type = group[i].type();
		bool isGroup = type == typeid(ofParameterGroup).name();
		bool isFloat = type == typeid(ofParameter<float>).name();
		bool isInt = type == typeid(ofParameter<int>).name();
		bool isBool = type == typeid(ofParameter<bool>).name();
		string str = group[i].getName();
		if (isFloat)
		{
			ofParameter<float> fp = group[i].cast<float>();
			//do something with this parameter
			//like push_back to your vector or something
		}
		else if (isInt)
		{
			ofParameter<int> ip = group[i].cast<int>();
			//do something with this parameter
			//like push_back to your vector or something
		}
	}
	*/
}

//--------------------------------------------------------------
void ofApp::draw() {

	if (bGui) {
		gui.draw();
		guiSmooth.draw();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	data.keyPressed(key);

	if (key == 'h' || key == 'H') bGui = !bGui;
}