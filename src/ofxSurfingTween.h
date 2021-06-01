#pragma once

#include "ofMain.h"


/*

TODO:

+ add colors types, vectors, using templates..
+ avoid crash to unsuported types
+ independent settings for each param.. ?
+ "real" nested sub-groups tree levels.. ?
+ add param to calibrate max history smooth/speed..
+ plotting int type should be stepped/not continuous..

*/


#include "FloatAnimator.h"
#include "ofxHistoryPlot.h"
#include "ofxImGui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "ofxSurfingHelpers.h"
#include "ofxSurfing_Timers.h"
#include "ofxSurfing_ImGuiBundle.h"
#include "ofxInteractiveRect.h"

#define COLORS_MONCHROME // vs iterated hue

//----

class myTweenerClass
{
private:
	float value;

public:
	float from;
	float to;
	float getValue() {
		return value;
	}
	void update(float percent) {
		value = ofMap(percent, 0, 1, from, to);
	}
};


//--------------------------------------------------------------
class ofxSurfingTween /*: public ofBaseApp*/ {

public:
	ofxSurfingTween();
	~ofxSurfingTween();

private:
	ofParameterGroup params_EditorEnablers;//the enabled params to randomize
	vector<ofParameter<bool>> enablersForParams;
	void drawToggles();

	void doSetAll(bool b);
	void doDisableAll();
	void doEnableAll();

public:
	//required to set to false when only one ImGui instance is created
	//--------------------------------------------------------------
	void setImGuiAutodraw(bool b) {
		bAutoDraw = b;
	}

	//--------------------------------------------------------------
	void setImGuiSharedMode(bool b) {
		gui.setSharedMode(b); // Force shared context
	}

	//----

public:
	void update(ofEventArgs & args);
	void draw(ofEventArgs & args);
	void exit();
	void keyPressed(int key);

	//---

	// api 
	// initializers

public:
	void setup(ofParameterGroup& aparams);//main setup method. to all pass the params with one line

	void add(ofParameterGroup aparams);
	void add(ofParameter<float>& aparam);
	void add(ofParameter<bool>& aparam);
	void add(ofParameter<int>& aparam);
	void addParam(ofAbstractParameter& aparam);

	//-

	// api 
	// getters

public:
	//--------------------------------------------------------------
	ofParameterGroup& getParamsSmoothed() {
		return mParamsGroup_COPY;
	}

public:
	float get(ofParameter<float> &e);
	int get(ofParameter<int> &e);
	float getParamFloatValue(ofAbstractParameter &e);
	int getParamIntValue(ofAbstractParameter &e);
	ofAbstractParameter& getParamAbstract(ofAbstractParameter &e);
	ofAbstractParameter& getParamAbstract(string name);
	ofParameter<float>& getParamFloat(string name);
	ofParameter<int>& getParamInt(string name);

public:
	void doGo();
	void doRandomize(bool bGo = false);//do and set random in min/max range for all params

	//---

private:
	void setup();
	void startup();
	void setupPlots();
	void updateSmooths();
	void updateEngine();
	void drawPlots(ofRectangle r);

	//----

private:
	ofParameterGroup mParamsGroup;

	ofParameterGroup mParamsGroup_COPY;//TODO:
	string suffix = "";
	//string suffix = "_COPY";

	void Changed_Controls_Out(ofAbstractParameter &e);

private:
	FloatAnimator animator;
	vector<myTweenerClass> outputs;
	vector<float> inputs;//feed normnalized signals here
	vector<float> generators;//testing signals

	string path_Global;
	string path_Settings;

	ofxInteractiveRect rectangle_PlotsBg = { "Rect_Plots", "ofxSurfingTween/" };

	int NUM_PLOTS;
	int NUM_VARS;

	vector<ofxHistoryPlot *> plots;
	vector<ofColor> colors;

	ofColor colorSelected;
	ofColor colorBaseLine;
#ifdef COLORS_MONCHROME
	ofColor colorPlots;
#endif

private:
	void Changed_Params(ofAbstractParameter &e);
	bool bDISABLE_CALLBACKS = true;

	ofParameterGroup params;
	ofParameter<bool> enable;
	ofParameter<bool> bFullScreen;
	ofParameter<bool> bShowPlots;
	ofParameter<bool> bShowInputs;
	ofParameter<bool> bShowOutputs;
	ofParameter<bool> bShowHelp;
	ofParameter<bool> solo;
	ofParameter<int> index;
	ofParameter<bool> enableTween;
	ofParameter<float> input;//index selected
	ofParameter<float> output;
	ofParameter<bool> bReset;
	ofParameter<bool> bPlay;
	ofParameter<float> playSpeed;
	//ofParameter<bool> bClamp;
	//ofParameter<float> minInput;
	//ofParameter<float> maxInput;
	//ofParameter<bool> bNormalized;
	//ofParameter<float> minOutput;
	//ofParameter<float> maxOutput;

	//tester timers
	int tf;
	float tn;

	void doReset();
	void setupParams();

	bool bTrigManual = false;//flip first
	bool bModeFast = false;//fast generators

	//--

	void setup_ImGui();
	bool bAutoDraw = true;
	void draw_ImGui();
	ofxImGui::Gui gui;
	ofxImGui::Settings mainSettings = ofxImGui::Settings();
	ImFont* customFont = nullptr;
	ofParameter<bool> auto_resize{ "Auto Resize", true };
	ofParameter<bool> bLockMouseByImGui{ "Mouse Locked", false };
	ofParameter<bool> auto_lockToBorder{ "Lock GUI", false };

	//--

public:
	ofParameter<bool> bGui{ "SURFING TWEENER", true };

private:
	string helpInfo;
	ofTrueTypeFont font;

public:
	//--------------------------------------------------------------
	std::string getHelpInfo() {
		return helpInfo;
	}

public:
	ofParameter<bool> bShowGui{ "SHOW SMOOTH SURFER", true };// exposed to use in external gui's
};
