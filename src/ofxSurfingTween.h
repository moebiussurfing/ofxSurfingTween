#pragma once

#include "ofMain.h"


/*

TODO:

+ fix plotting / output when smooth disabled
+ avoid crash to unsuported types
+ add colors types, vectors, using templates..

+ use ofxParameterCololection
+ "real" nested sub-groups tree levels.. ?
+ add param to calibrate max history smooth/speed.. ?
+ plotting int type should be stepped/not continuous.. ?

*/

//--

#define USE_SURFING_PRESETS

#define USE_SURFING_TWEENER__GUI_MANAGER
//#define USE_SURFING_TWEENER__GUI_LOCAL

//--


#include "FloatAnimator.h"
#include "ofxHistoryPlot.h"
#include "ofxInteractiveRect.h"
#include "ofxSurfingImGui.h"
//#include "ofxImGui.h"
//#include "imgui.h"
//#include "imgui_internal.h"
#include "ofxSurfingHelpers.h"
#include "ofxSurfing_Timers.h"

#define COLORS_MONCHROME // vs iterated hue

#ifdef USE_SURFING_PRESETS
#include "ofxSurfingPresets.h"
#endif

//----

//--------------------------------------------------------------
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

//----

//--------------------------------------------------------------
class ofxSurfingTween /*: public ofBaseApp*/ {

	//----

#ifdef USE_SURFING_TWEENER__GUI_MANAGER
public:
	ofxSurfing_ImGui_Manager guiManager; // In MODE A ofxGui will be instatiated inside the class
	// then you can simplify the ofxImGui setup procces and:
	// - initialize ofxImGui 
	// - speed up creation of windows and trees
	// Also you use the ofxSurfing_ImGui_WidgetsTypes.h aka Types Engine Manager. 
	// this class helps to populate responsive layouts or to define wich widget type to draw a typed parameter.
	// then you can draw a float ofParameter as a slider, drag number or +/- stepped buttons.
	// * hide a parameter of a group
	// * define to draw in same line next param
	// * define to add vertical spacing after the parameter

	// MODE B
	//ofxImGui::Gui gui; // can be instantiated outside the class (locally to maybe share with other classes)
#endif

	//--

	void draw_ImGui();

#ifdef USE_SURFING_TWEENER__GUI_LOCAL
	// ImGui
	void setup_ImGui();
	bool bAutoDraw = true;
	ofxImGui::Gui gui;
	ofxImGui::Settings mainSettings = ofxImGui::Settings();
	ImFont* customFont = nullptr;
	ofParameter<bool> auto_resize{ "Auto Resize", true };
	ofParameter<bool> bLockMouseByImGui{ "Mouse Locked", false };
	ofParameter<bool> auto_lockToBorder{ "Lock GUI", false };
#endif

	//----

#ifdef USE_SURFING_PRESETS
		ofxSurfingPresets presets;
#endif

	//----

public:
	ofxSurfingTween();
	~ofxSurfingTween();

private:
	ofParameterGroup params_EditorEnablers;// the enabled params to randomize
	vector<ofParameter<bool>> enablersForParams;
	void drawToggles();

	void doSetAll(bool b);
	void doDisableAll();
	void doEnableAll();

public:
	// required to set to false when only one ImGui instance is created
	//--------------------------------------------------------------
	void setImGuiAutodraw(bool b) {
#ifdef USE_SURFING_TWEENER__GUI_LOCAL
		bAutoDraw = b;
#endif
	}

	//--------------------------------------------------------------
	void setImGuiSharedMode(bool b) {
#ifdef USE_SURFING_TWEENER__GUI_LOCAL
		gui.setImGuiSharedMode(b); // Force shared context
#endif
	}

	//--

public:
	void update(ofEventArgs & args);
	void draw(ofEventArgs & args);
	void exit();
	void keyPressed(int key);

	//---

	// api 
	// initializers

public:
	void setup(ofParameterGroup& aparams);// main setup api/method. to pass all the params using just one line

	//-

	//internal params
private:
	void add(ofParameterGroup aparams);
	void add(ofParameter<float>& aparam);
	void add(ofParameter<bool>& aparam);
	void add(ofParameter<int>& aparam);
	void add(ofParameter<glm::vec2>& aparam);
	void add(ofParameter<glm::vec3>& aparam);
	void add(ofParameter<glm::vec4>& aparam);
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
	// simple getters
	float get(ofParameter<float> &e);
	int get(ofParameter<int> &e);

	// complex getters
	//float getParamFloatValue(ofAbstractParameter &e);
	//int getParamIntValue(ofAbstractParameter &e);
	//ofAbstractParameter& getParamAbstract(ofAbstractParameter &e);
	//ofAbstractParameter& getParamAbstract(string name);
	//ofParameter<float>& getParamFloat(string name);
	//ofParameter<int>& getParamInt(string name);

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

	ofParameterGroup mParamsGroup_COPY;
	string suffix = "";
	//string suffix = "_COPY";

	void Changed_Controls_Out(ofAbstractParameter &e);

private:
	FloatAnimator animator;
	vector<myTweenerClass> outputs;
	vector<float> inputs;//feed normnalized signals here
	
	//void Changed_ParamsInput(ofAbstractParameter &e);

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
	ofParameter<bool> bFullScreen;
	ofParameter<bool> bShowPlots;
	ofParameter<bool> bShowInputs;
	ofParameter<bool> bShowOutputs;
	ofParameter<bool> bShowHelp;
	ofParameter<bool> bKeys;
	ofParameter<bool> solo;
	ofParameter<int> index;
	ofParameter<float> input;//index selected
	ofParameter<float> output;
	ofParameter<bool> bReset;
	ofParameter<bool> bPlay;
	ofParameter<float> playSpeed;

	//clamp normalize
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

	//--

public:
	ofParameter<bool> bGui{ "SURFING TWEENER", true };
	ofParameter<bool> bEnableTween;
	ofParameter<bool> bEnableLiveMode;

	//--------------------------------------------------------------
	void setLiveEditMode(bool b) {
		bEnableLiveMode = b;
	}

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

	//TODO:
	////--------------------------------------------------------------
	//template<typename ParameterType>
	//void AddParameter(ofParameter<ParameterType>& parameter)
	//{
	//	auto tmpRef = parameter.get();
	//	const auto& info = typeid(ParameterType);
	//	if (info == typeid(float))
	//	{
	//	}
	//	if (info == typeid(int))
	//	{
	//	}
	//	if (info == typeid(bool))
	//	{
	//	}
	//	ofLogWarning(__FUNCTION__) << "Could not create GUI element for type " << info.name();
	//}

	//TODO:
	//--------------------------------------------------------------
	template<typename ParameterType>
	void getParam(ofParameter<ParameterType>& parameter)
	{
		string name = e.getName();
		auto &p = mParamsGroup_COPY.get(name);
		if (p.type() == typeid(ofParameter<ParameterType>).name())
		{
			return p.cast<ParameterType>().get();
		}
		else
		{
			ofLogError(__FUNCTION__) << "Not expected type: " << name;
			return -1;
		}
	}
};
