#include "ofxSurfingTween.h"

//--------------------------------------------------------------
ofxSurfingTween::ofxSurfingTween()
{
	ofAddListener(ofEvents().update, this, &ofxSurfingTween::update);
	ofAddListener(ofEvents().draw, this, &ofxSurfingTween::draw, OF_EVENT_ORDER_AFTER_APP);
}

//--------------------------------------------------------------
ofxSurfingTween::~ofxSurfingTween()
{
	ofRemoveListener(ofEvents().update, this, &ofxSurfingTween::update);
	ofRemoveListener(ofEvents().draw, this, &ofxSurfingTween::draw, OF_EVENT_ORDER_AFTER_APP);

	exit();
}

//--------------------------------------------------------------
void ofxSurfingTween::setup(ofParameterGroup& aparams) {

	ofLogNotice() << __FUNCTION__ << " " << aparams.getName();

	setup();

	//--

	string n = aparams.getName();
	mParamsGroup.setName(n);//name

	//TODO:
	//group COPY
	mParamsGroup_COPY.setName(n + "_TWEENED");//name
	//mParamsGroup_COPY.setName(n + suffix);//name

	for (int i = 0; i < aparams.size(); i++) {
		addParam(aparams.get(i));
	}

	//--

	// already added all params content
	// build the smoothers
	// build the plots

	setupPlots();//NUM_VARS will be counted here..

	outputs.clear();
	outputs.resize(NUM_VARS);

	animator.setNameLabel("ANIMATOR");
	animator.setup(0, 1);//we use the animators normalized and the recalculate each param range from min to max.
	animator.setModeBrowse(false);

	inputs.resize(NUM_VARS);

	//--

	////TODO:
	//mParamsGroup_COPY.setName(aparams.getName() + "_COPY");//name
	//mParamsGroup_COPY = mParamsGroup;//this kind of copy links param per param. but we want to clone the "structure" only
}

//--------------------------------------------------------------
void ofxSurfingTween::setup() {
	ofLogNotice() << __FUNCTION__;

	path_Global = "ofxSurfingTween/";
	path_Settings = path_Global + "ofxSurfingTween_Settings.xml";
	ofxSurfingHelpers::CheckFolder(path_Global);


	// default plots position
	//ofRectangle r = ofGetCurrentViewport();
	int w = 300;
	int pad = 30;
	ofRectangle r = ofRectangle(ofGetWidth() - w - 2 * pad, pad, w, ofGetHeight() - 2 * pad);
	rectangle_PlotsBg.setRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());

	//-

	setupParams();

	//-

	enablersForParams.clear();// an enabler toggler for each param
	params_EditorEnablers.clear();// an enabler toggler for each param
	params_EditorEnablers.setName("Enabler Params");

	//--

	//help info
	std::string fontName;
	float fontSizeParam;
	fontName = "telegrama_render.otf";
	fontSizeParam = 11;
	std::string _path = "assets/fonts/"; // assets folder
	font.load(_path + fontName, fontSizeParam, true, true, true);

	//--

#ifdef USE_SURFING_TWEENER__GUI_LOCAL
	setup_ImGui();
#endif

#ifdef USE_SURFING_TWEENER__GUI_MANAGER
	guiManager.setup();
#endif

	//--

	mParamsGroup.setName("ofxSurfingTween");
	ofAddListener(mParamsGroup.parameterChangedE(), this, &ofxSurfingTween::Changed_Controls_Out);
}

//--------------------------------------------------------------
void ofxSurfingTween::startup() {

	//ofAddListener(mParamsGroup.parameterChangedE(), this, &ofxSurfingTween::Changed_ParamsInput);

	bDISABLE_CALLBACKS = false;

	//doReset();

	//--

	// settings
	ofxSurfingHelpers::loadGroup(params, path_Settings);

	doGo();
}

//--------------------------------------------------------------
void ofxSurfingTween::setupPlots() {
	//TODO:
	//should exclude not int/float types of counting..

	NUM_VARS = mParamsGroup.size();
	NUM_PLOTS = 2 * NUM_VARS;

	index.setMax(NUM_VARS - 1);
	plots.resize(NUM_PLOTS);

	//colors
#ifdef COLORS_MONCHROME
	colorPlots = (ofColor::green);
#endif
	colorBaseLine = ofColor(255, 48);
	colorSelected = ofColor(255, 150);

	//colors
	colors.clear();
	colors.resize(NUM_PLOTS);

	//alphas
	int a1 = 64;//input
	int a2 = 255;//output
	ofColor c;

#ifdef COLORS_MONCHROME
	for (int i = 0; i < NUM_VARS; i++)
	{
		c = colorPlots;
		colors[2 * i] = ofColor(c, a1);
		colors[2 * i + 1] = ofColor(c, a2);
	}
#endif

#ifndef COLORS_MONCHROME
	int sat = 255;
	int brg = 255;
	int hueStep = 255. / (float)NUM_VARS;
	for (int i = 0; i < NUM_VARS; i++)
	{
		c.setHsb(hueStep * i, sat, brg);
		colors[2 * i] = ofColor(c, a1);
		colors[2 * i + 1] = ofColor(c, a2);
	}
#endif

	for (int i = 0; i < NUM_PLOTS; i++)
	{
		string _name;
		string _name2;
		_name2 = ofToString(mParamsGroup[i / 2].getName());//param name
		//_name2 = ofToString(i / 2);//index as name

		bool b1 = (i % 2 == 0);//1st plot of each var. input
		_name = _name2;
		//if (b1) _name = "Input " + _name2;
		//else _name = "Output " + _name2;

		bool bTitle = !b1;
		bool bInfo = false;
		bool bBg = b1;
		bool bGrid = false && b1;

		plots[i] = new ofxHistoryPlot(NULL, _name, 60 * 4, false);//4 secs at 60fps
		plots[i]->setRange(0, 1);
		plots[i]->setColor(colors[i]);
		plots[i]->setDrawTitle(bTitle);
		plots[i]->setShowNumericalInfo(bInfo);
		plots[i]->setShowSmoothedCurve(false);
		plots[i]->setDrawBackground(bBg);
		plots[i]->setDrawGrid(bGrid);
	}

	// draggable rectangle
	ofColor c0(0, 90);
	//rectangle_PlotsBg.set
	rectangle_PlotsBg.setColorEditingHover(c0);
	rectangle_PlotsBg.setColorEditingMoving(c0);
	rectangle_PlotsBg.enableEdit();
}

//--------------------------------------------------------------
void ofxSurfingTween::update(ofEventArgs & args) {
	if (ofGetFrameNum() == 0) { startup(); }

	// engine
	updateSmooths();
	updateEngine();

	//-

	// tester
	// play timed randoms
	if (bPlay)
	{
		//static const int _secs = 2;
		//int max = ofMap(playSpeed, 0, 1, 60, 5) * _secs;
		//tf = ofGetFrameNum() % max;
		//tn = ofMap(tf, 0, max, 0, 1, true);//used to fade button border
		//if (tf == 0)
		//{
		//	doRandomize(true);
		//}

		int currtime = ofGetElapsedTimeMillis() - timerPlayer;
		tn = ofMap(currtime, 0, playTime, 0, 1, true);
		if (currtime > playTime)
		{
			timerPlayer = ofGetElapsedTimeMillis();
			doRandomize(true);
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingTween::updateSmooths() {

	for (int i = 0; i < mParamsGroup.size(); i++)
	{
		ofAbstractParameter& p = mParamsGroup[i];

		// 1. toggle enabler

		auto &_p = params_EditorEnablers[i]; // ofAbstractParameter
		auto type = _p.type();
		bool isBool = type == typeid(ofParameter<bool>).name();
		string name = _p.getName();
		ofParameter<bool> _bSmooth = _p.cast<bool>();

		// vec
		bool isVec2 = type == typeid(ofParameter<glm::vec2>).name();
		bool isVec3 = type == typeid(ofParameter<glm::vec3>).name();
		bool isVec4 = type == typeid(ofParameter<glm::vec4>).name();

		//-

		// 2. get normalized input param

		//string str = "";
		//string name = aparam.getName();
		float vn = 0; // normalized params

		if (p.type() == typeid(ofParameter<float>).name()) {
			ofParameter<float> _p = p.cast<float>();
			vn = ofMap(_p, _p.getMin(), _p.getMax(), 0, 1, true);

			auto pc = mParamsGroup_COPY.getFloat(_p.getName() + suffix);

			if (bEnableTween && _bSmooth) {
				float v = ofMap(outputs[i].getValue(), 0, 1, _p.getMin(), _p.getMax(), true);
				pc.set(v);
			}
			else {
				pc.set(_p.get());
			}
		}

		else if (p.type() == typeid(ofParameter<int>).name()) {
			ofParameter<int> _p = p.cast<int>();
			vn = ofMap(_p, _p.getMin(), _p.getMax(), 0, 1, true);

			auto pc = mParamsGroup_COPY.getInt(_p.getName() + suffix);

			if (bEnableTween && _bSmooth) {
				int v = ofMap(outputs[i].getValue(), 0, 1, _p.getMin(), _p.getMax() + 1, true);
				pc.set(v);
				//TODO: round fix +1...
			}
			else {
				pc.set(_p.get());
			}
		}

		else if (p.type() == typeid(ofParameter<bool>).name()) {
			ofParameter<bool> ti = p.cast<bool>();
		}

		// vec
		else if (isVec2) {
			ofParameter<ofDefaultVec2> _p = p.cast<ofDefaultVec2>();
			float v1 = ofMap(_p.get().x, _p.getMin().x, _p.getMax().x, 0, 1, true);
			float v2 = ofMap(_p.get().y, _p.getMin().y, _p.getMax().y, 0, 1, true);

			auto pc = mParamsGroup_COPY.getVec2f(_p.getName() + suffix);

			if (bEnableTween && _bSmooth) {
				float _v1 = ofMap(outputs[i].getValue(), 0, 1, _p.getMin().x, _p.getMax().x, true);
				float _v2 = ofMap(outputs[i].getValue(), 0, 1, _p.getMin().y, _p.getMax().y, true);
				pc.set(glm::vec2(_v1, _v2));
			}
			else {
				pc.set(_p.get());
			}
		}
		else if (isVec3) {
			ofParameter<ofDefaultVec3> _p = p.cast<ofDefaultVec3>();
			float v1 = ofMap(_p.get().x, _p.getMin().x, _p.getMax().x, 0, 1, true);
			float v2 = ofMap(_p.get().y, _p.getMin().y, _p.getMax().y, 0, 1, true);
			float v3 = ofMap(_p.get().z, _p.getMin().y, _p.getMax().z, 0, 1, true);

			auto pc = mParamsGroup_COPY.getVec3f(_p.getName() + suffix);

			if (bEnableTween && _bSmooth) {
				float _v1 = ofMap(outputs[i].getValue(), 0, 1, _p.getMin().x, _p.getMax().x, true);
				float _v2 = ofMap(outputs[i].getValue(), 0, 1, _p.getMin().y, _p.getMax().y, true);
				float _v3 = ofMap(outputs[i].getValue(), 0, 1, _p.getMin().z, _p.getMax().z, true);
				pc.set(glm::vec3(_v1, _v2, _v3));
			}
			else {
				pc.set(_p.get());
			}
		}
		else if (isVec4) {
			ofParameter<ofDefaultVec4> _p = p.cast<ofDefaultVec4>();
			float v1 = ofMap(_p.get().x, _p.getMin().x, _p.getMax().x, 0, 1, true);
			float v2 = ofMap(_p.get().y, _p.getMin().y, _p.getMax().y, 0, 1, true);
			float v3 = ofMap(_p.get().z, _p.getMin().y, _p.getMax().z, 0, 1, true);
			float v4 = ofMap(_p.get().w, _p.getMin().w, _p.getMax().w, 0, 1, true);

			auto pc = mParamsGroup_COPY.getVec3f(_p.getName() + suffix);

			if (bEnableTween && _bSmooth) {
				float _v1 = ofMap(outputs[i].getValue(), 0, 1, _p.getMin().x, _p.getMax().x, true);
				float _v2 = ofMap(outputs[i].getValue(), 0, 1, _p.getMin().y, _p.getMax().y, true);
				float _v3 = ofMap(outputs[i].getValue(), 0, 1, _p.getMin().z, _p.getMax().z, true);
				float _v4 = ofMap(outputs[i].getValue(), 0, 1, _p.getMin().w, _p.getMax().w, true);
				pc.set(glm::vec4(_v1, _v2, _v3, _v4));
			}
			else {
				pc.set(_p.get());
			}
		}

		//-

		else {
			continue;
		}

		//-

		inputs[i] = vn; // prepare and feed the input with the normalized parameter

		//--

		// tween
		float v = animator.getValue();
		outputs[i].update(v);
	}
}

//--------------------------------------------------------------
void ofxSurfingTween::updateEngine() {

	//TODO: crash when added other types than int/float
	//for (int i = 0; i < NUM_VARS && i < params_EditorEnablers.size(); i++)

	for (int i = 0; i < NUM_VARS; i++)
	{
		// toggle
		auto &p = params_EditorEnablers[i]; // ofAbstractParameter
		auto type = p.type();
		bool isBool = type == typeid(ofParameter<bool>).name();
		string name = p.getName();
		ofParameter<bool> _bSmooth = p.cast<bool>();

		// plots

		// input
		//float _input = ofClamp(inputs[i], minInput, maxInput);
		float _input = inputs[i];
		if (bShowPlots) plots[2 * i]->update(_input); // source

		// output
		if (bShowPlots) {
			if (bEnableTween && _bSmooth) plots[2 * i + 1]->update(outputs[i].getValue()); // filtered
			else plots[2 * i + 1]->update(_input); // source
		}

		// solo
		if (i == index) input = _input;
	}

	//----

	// solo

	// index selected
	if (params_EditorEnablers.size() > 0) {
		// toggle
		int i = index;
		auto &_p = params_EditorEnablers[i];// ofAbstractParameter
		auto type = _p.type();
		bool isBool = type == typeid(ofParameter<bool>).name();
		string name = _p.getName();
		ofParameter<bool> _bSmooth = _p.cast<bool>();

		// output
		if (bEnableTween && _bSmooth)
		{
			//if (bNormalized) output = outputs[index].getValueN();
			//else output = outputs[index].getValue();
			output = outputs[index].getValue();
		}
		// bypass
		else
		{
			output = input;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingTween::draw(ofEventArgs & args) {
	if (!bShowGui || !bGui) return;

	if (bShowPlots) {
		ofPushStyle();
		if (bFullScreen) drawPlots(ofGetCurrentViewport());
		else drawPlots(rectangle_PlotsBg);

		ofSetColor(ofColor(255, 4));
		rectangle_PlotsBg.draw();
		ofPopStyle();
	}

	//-

	// help info
	if (bShowHelp)
	{
		int pad = 10;
		auto _shape = ofxSurfingHelpers::getShapeBBtextBoxed(font, helpInfo);
		ofxSurfingHelpers::drawTextBoxed(font, helpInfo,
			//pad, // x left
			ofGetWidth() / 2 - _shape.x / 2, // x center
			//ofGetWidth() - _shape.x - pad, // y bottom right
			ofGetHeight() - _shape.y - pad); // y bottom left

		//ofDrawBitmapStringHighlight(helpInfo, ofGetWidth() - 280, 25);
	}

	if (bGui) draw_ImGui();
}

//--------
// actions

//--------------------------------------------------------------
void ofxSurfingTween::doRandomize(bool bGo) {
	ofLogVerbose(__FUNCTION__);

	for (int i = 0; i < mParamsGroup.size(); i++)
	{
		//apply only if enabled
		auto &pe = params_EditorEnablers[i];
		auto type = pe.type();
		bool isBool = type == typeid(ofParameter<bool>).name();
		if (isBool) {
			ofParameter<bool> pb = pe.cast<bool>();
			if (!pb.get()) continue;
		}

		//-

		auto &p = mParamsGroup[i];

		float v;

		if (p.type() == typeid(ofParameter<float>).name())
		{
			ofParameter<float> pr = p.cast<float>();
			pr = ofRandom(pr.getMin(), pr.getMax());

			v = ofMap(pr.get(), pr.getMin(), pr.getMax(), 0, 1, true);
		}
		else if (p.type() == typeid(ofParameter<int>).name())
		{
			ofParameter<int> pr = p.cast<int>();
			pr = ofRandom(pr.getMin(), pr.getMax());

			v = ofMap(pr.get(), pr.getMin(), pr.getMax(), 0, 1, true);
		}
	}

	//---

	if (bGo) doGo();
}

//--------------------------------------------------------------
void ofxSurfingTween::doGo() {
	ofLogVerbose(__FUNCTION__);

	for (int i = 0; i < mParamsGroup.size(); i++)
	{
		ofAbstractParameter &p = mParamsGroup[i];

		bool isFloat = p.type() == typeid(ofParameter<float>).name();
		bool isInt = p.type() == typeid(ofParameter<int>).name();

		//vec
		auto type = p.type();
		bool isVec2 = type == typeid(ofParameter<glm::vec2>).name();
		bool isVec3 = type == typeid(ofParameter<glm::vec3>).name();
		bool isVec4 = type == typeid(ofParameter<glm::vec4>).name();

		float vTo; // "to" target will be current input target
		float vFrom; // "from" will be the current output
		vFrom = outputs[i].getValue();

		if (isFloat)
		{
			ofParameter<float> pr = p.cast<float>();
			vTo = ofMap(pr.get(), pr.getMin(), pr.getMax(), 0, 1, true);
			outputs[i].from = vFrom;
			outputs[i].to = vTo;
		}
		else if (isInt)
		{
			ofParameter<int> pr = p.cast<int>();
			vTo = ofMap(pr.get(), pr.getMin(), pr.getMax(), 0, 1, true);
			outputs[i].from = vFrom;
			outputs[i].to = vTo;
		}

		//--

		//TODO: 
		//BUG:
		//params with multiple floats/multi dim. how to handle?

		//vec
		else if (isVec2)
		{
			ofParameter<ofDefaultVec2> pr = p.cast<ofDefaultVec2>();
			float vTo1 = ofMap(pr.get().x, pr.getMin().x, pr.getMax().x, 0, 1, true);
			float vTo2 = ofMap(pr.get().y, pr.getMin().x, pr.getMax().y, 0, 1, true);
			outputs[i].from = vFrom;
			outputs[i].to = vTo;
			//TODO: what about i+1+2?
		}
		else if (isVec3)
		{
			ofParameter<ofDefaultVec3> pr = p.cast<ofDefaultVec3>();
			float vTo1 = ofMap(pr.get().x, pr.getMin().x, pr.getMax().x, 0, 1, true);
			float vTo2 = ofMap(pr.get().y, pr.getMin().y, pr.getMax().y, 0, 1, true);
			float vTo3 = ofMap(pr.get().z, pr.getMin().z, pr.getMax().z, 0, 1, true);
		}
		else if (isVec4)
		{
			ofParameter<ofDefaultVec4> pr = p.cast<ofDefaultVec4>();
			float vTo1 = ofMap(pr.get().x, pr.getMin().x, pr.getMax().x, 0, 1, true);
			float vTo2 = ofMap(pr.get().y, pr.getMin().y, pr.getMax().y, 0, 1, true);
			float vTo3 = ofMap(pr.get().z, pr.getMin().z, pr.getMax().z, 0, 1, true);
			float vTo4 = ofMap(pr.get().w, pr.getMin().w, pr.getMax().w, 0, 1, true);
		}

		//--

		animator.start();
	}
}

//--------------------------------------------------------------
void ofxSurfingTween::drawToggles() {
	for (int i = 0; i < enablersForParams.size(); i++)
	{
		//numerize
		string tag;//to push ids
		string n = "#" + ofToString(i < 10 ? "0" : "") + ofToString(i);
		//ImGui::Dummy(ImVec2(0,10));
		ImGui::Text(n.c_str());
		ImGui::SameLine();
	}
}

//--------------------------------------------------------------
void ofxSurfingTween::drawPlots(ofRectangle r) {

	ofPushStyle();

	int hh = r.getHeight();
	int ww = r.getWidth();
	int x = r.getX();
	int y = r.getY();

	//ofPushMatrix();
	//ofTranslate(x,y)

	int h;
	if (!solo)
	{
		h = hh / NUM_VARS;
	}
	else // solo
	{
		h = hh; // full height on solo
	}

	for (int i = 0; i < NUM_VARS; i++)
	{
		if (solo) if (i != index) continue;

		int ii = 2 * i;

		////grid
		//int hg = h / 2;
		//plot[ii]->setGridUnit(hg);
		//plot[ii + 1]->setGridUnit(hg);

		plots[ii]->draw(x, y, ww, h);
		plots[ii + 1]->draw(x, y, ww, h);

		//baseline
		ofSetColor(colorBaseLine);
		ofSetLineWidth(1);
		ofLine(x, y + h, x + ww, y + h);

		//name
		if (!solo)  ofSetColor(colorSelected);
		else {
			if (i == index) ofSetColor(colorSelected);
			else ofSetColor(colorBaseLine);
		}
		string s = "#" + ofToString(i);

		//add param name
		//s += " " + mParamsGroup[i].getName();

		//add raw value
		string sp;
		int ip = i;
		//for (int ip = 0; ip < mParamsGroup.size(); ip ++) 
		{
			ofAbstractParameter& p = mParamsGroup[ip];
			if (p.type() == typeid(ofParameter<int>).name()) {
				ofParameter<int> ti = p.cast<int>();
				int intp = ofMap(outputs[ip].getValue(), 0, 1, ti.getMin(), ti.getMax());
				sp = ofToString(intp);
			}
			else if (p.type() == typeid(ofParameter<float>).name()) {
				ofParameter<float> ti = p.cast<float>();
				float floatp = ofMap(outputs[ip].getValue(), 0, 1, ti.getMin(), ti.getMax());
				sp = ofToString(floatp);
			}
			//else {
			//	continue;
			//}
		}
		string _spacing = "\t";
		s += " " + _spacing + sp;

		ofDrawBitmapString(s, x + 5, y + 11);

		ofColor _c1 = colorSelected;//monochrome
		ofColor _c2 = colorSelected;
		//ofColor _c1 = ofColor(colors[ii]);//colored
		//ofColor _c2 = ofColor(colors[ii]);

		float _a1 = ofxSurfingHelpers::Bounce(1.0);
		float _a2 = ofxSurfingHelpers::Bounce(0.5);

		//mark selected left line
		if (i == index && !solo)
		{
			ofSetLineWidth(1);
			ofSetColor(colorSelected);
			ofLine(x, y, x, y + h);
		}

		if (!solo) y += h;
	}

	//ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofxSurfingTween::keyPressed(int key) {
	if (!bKeys) return;

	//-

	if (key == 'g') bGui = !bGui;
	if (key == 'h') bShowHelp = !bShowHelp;
	if (key == 'p') bShowPlots = !bShowPlots;

	if (key == OF_KEY_F1 || key == OF_KEY_BACKSPACE) doRandomize(false);
	if (key == OF_KEY_F2) doGo();
	if (key == OF_KEY_F3 || key == ' ') doRandomize(true);
	if (key == OF_KEY_F4) bPlay = !bPlay;

	if (key == 's') solo = !solo;
	if (key == OF_KEY_UP) {
		index--;
		index = ofClamp(index, index.getMin(), index.getMax());
	}
	if (key == OF_KEY_DOWN) {
		index++;
		index = ofClamp(index, index.getMin(), index.getMax());
	}
	if (key == '-') {
		animator.previousCurve(true);
	}
	if (key == '+') {
		animator.nextCurve(true);
	}
}

//--------------------------------------------------------------
void ofxSurfingTween::setupParams() {
	ofLogNotice() << __FUNCTION__;

	//string name = "filter";
	////bool _bNormalized = true;
	//float _inputMinRange;
	//float _inputMaxRange;
	//float _outMinRange;
	//float _outMaxRange;
	////if (_bNormalized)//normalizad
	//{
	//	_inputMinRange = 0;
	//	_inputMaxRange = 1;
	//	_outMaxRange = 1;
	//	_outMinRange = 0;
	//}
	////else//midi range
	////{
	////    _inputMinRange = 0;
	////    _inputMaxRange = 1;
	////    _outMinRange = 0;
	////    _outMaxRange = 127;
	////}

	rectangle_PlotsBg.bEditMode.setName("Edit Plots");

	// params
	params.setName("Surfing Tweener");
	params.add(bGui);
	params.add(bKeys.set("Keys", true));
	params.add(index.set("Index", 0, 0, 0));
	params.add(bPlay.set("PLAY TEST", false));
	params.add(playTime.set("Time", 1000, 200, 3000));
	params.add(playSpeed.set("Speed", 0.5, 0, 1));
	params.add(bShowPlots.set("PLOTS", true));
	params.add(bShowInputs.set("INPUTS", true));
	params.add(bShowOutputs.set("OUTPUTS", true));
	params.add(bShowHelp.set("Help", false));
	params.add(bFullScreen.set("FULL SCREEN", false));
	params.add(bEnableLiveMode.set("LIVE EDIT MODE", true));
	params.add(bEnableTween.set("TWEEN ENABLE", true));
	params.add(solo.set("SOLO", false));
	params.add(bReset.set("Reset", false));

	params.add(animator.SHOW_Gui);

	// clamp/normalize
	//params.add(minInput.set("min Input", 0, _inputMinRange, _inputMaxRange));
	//params.add(maxInput.set("max Input", 1, _inputMinRange, _inputMaxRange));
	//params.add(minOutput.set("min Output", 0, _outMinRange, _outMaxRange));
	//params.add(maxOutput.set("max Output", 1, _outMinRange, _outMaxRange));
	//params.add(bNormalized.set("Normalized", false));
	//params.add(bClamp.set("CLAMP", true));
	//params.add(input.set("INPUT", 0, _inputMinRange, _inputMaxRange));
	//params.add(output.set("OUTPUT", 0, _outMinRange, _outMaxRange));

	// exclude
	input.setSerializable(false);
	output.setSerializable(false);
	solo.setSerializable(false);
	bReset.setSerializable(false);
	bEnableLiveMode.setSerializable(false);
	bPlay.setSerializable(false);

	ofAddListener(params.parameterChangedE(), this, &ofxSurfingTween::Changed_Params); // setup()

	//-

	// help info
	string s = "";
	s += "HELP KEYS";
	s += "\n\n";
	s += "H            :  SHOW HELP"; s += "\n";// for external ofApp..
	s += "G            :  SHOW GUI"; s += "\n";
	s += "+|-          :  CURVE TYPE"; s += "\n";
	s += "\n";
	s += "TESTER"; s += "\n";
	s += "F1|BSPACE    :  RANDOMiZE"; s += "\n";
	s += "F2           :  GO!"; s += "\n";
	s += "F3|SPACE     :  RANDOMiZE+GO!"; s += "\n";
	s += "F4           :  PLAY RANDOMiZER"; s += "\n";
	s += "\n";
	s += "SOLO"; s += "\n";
	s += "S            :  ENABLE SOLO"; s += "\n";
	s += "Up|Down      :  BROWSE PARAMS"; s += "\n";
	helpInfo = s;
}

//--------------------------------------------------------------
void ofxSurfingTween::exit() {
	ofRemoveListener(params.parameterChangedE(), this, &ofxSurfingTween::Changed_Params); // exit()
	ofRemoveListener(mParamsGroup.parameterChangedE(), this, &ofxSurfingTween::Changed_Controls_Out);

	ofxSurfingHelpers::saveGroup(params, path_Settings);
}

//--------------------------------------------------------------
void ofxSurfingTween::doReset() {
	ofLogNotice(__FUNCTION__);

	bEnableLiveMode = true;
	bEnableTween = true;
	output = 0;
	playSpeed = 0.5;
	playTime = 1000;
	//bPlay = false;
	//bShowPlots = true;
	//bShowInputs = true;
	//bShowOutputs = true;
	//minInput = 0;
	//maxInput = 1;
	//minOutput = 0;
	//maxOutput = 1;
	//bNormalized = false;
	//bClamp = true;

	animator.doReset();
}

// callback for a parameter group

////--------------------------------------------------------------
//void ofxSurfingTween::Changed_ParamsInput(ofAbstractParameter &e)
//{
//	if (bDISABLE_CALLBACKS) return;
//
//	string name = e.getName();
//	if (name != input.getName() && name != output.getName() && name != "")
//	{
//		ofLogVerbose() << __FUNCTION__ << " : " << name << " : with value " << e;
//	}
//
//	if (bEnableLiveMode) {
//
//	}
//}

//--------------------------------------------------------------
void ofxSurfingTween::Changed_Params(ofAbstractParameter &e)
{
	if (bDISABLE_CALLBACKS) return;

	string name = e.getName();
	if (name != input.getName() && name != output.getName() && name != "")
	{
		ofLogVerbose() << __FUNCTION__ << " : " << name << " : with value " << e;
	}

	if (name == bGui.getName())
	{
		if (!bGui) animator.SHOW_Gui = false;
	}

	if (name == bReset.getName() && bReset)
	{
		bReset = false;
		doReset();
	}
}

//--

// ImGui

#ifdef USE_SURFING_TWEENER__GUI_LOCAL
//TODO:
//swap to layout ImGui
//--------------------------------------------------------------
void ofxSurfingTween::setup_ImGui()
{
	ImGuiConfigFlags flags = ImGuiConfigFlags_DockingEnable;
	bool bRestore = true;
	bool bMouse = false;
	gui.setup(nullptr, bAutoDraw, flags, bRestore, bMouse);

	auto &io = ImGui::GetIO();
	auto normalCharRanges = io.Fonts->GetGlyphRangesDefault();

	//-

	// font
	std::string fontName;
	float fontSizeParam;
	fontName = "telegrama_render.otf";
	fontSizeParam = 11;

	std::string _path = "assets/fonts/"; // assets folder
	customFont = gui.addFont(_path + fontName, fontSizeParam, nullptr, normalCharRanges);
	io.FontDefault = customFont;

	//-

	// theme
	ofxImGuiSurfing::ImGui_ThemeMoebiusSurfing();
}
#endif

//----

#ifdef USE_SURFING_TWEENER__GUI_MANAGER
//--------------------------------------------------------------
void ofxSurfingTween::draw_ImGui()
{
	guiManager.begin(); // global begin
	{
		// widgets sizes
		float _w100;
		float _w50;
		float _w33;
		float _w25;
		float _h;
		ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);
		float _h50 = _h / 2;

		std::string n;
		bool bOpen;
		bOpen = false;
		ImGuiWindowFlags _flagw;
		_flagw = (bOpen ? ImGuiWindowFlags_NoCollapse : ImGuiWindowFlags_None);
		if (guiManager.bAutoResize) _flagw |= ImGuiWindowFlags_AlwaysAutoResize;

		{
			n = "TWEEN SURFER";
			guiManager.beginWindow(n.c_str(), NULL, _flagw);
			{
				ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);

				ImGuiTreeNodeFlags _flagt;
				_flagt = ImGuiTreeNodeFlags_None;
				_flagt = ImGuiTreeNodeFlags_DefaultOpen;

				// enable / bypass
				ofxImGuiSurfing::AddBigToggle(bEnableTween, _w100, _h);

				bool bOpen = true;
				ImGuiTreeNodeFlags _flagt2 = (bOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None);
				_flagt2 |= ImGuiTreeNodeFlags_Framed;

				if (ImGui::TreeNodeEx("ANIMATOR", _flagt2))
				{
					ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);

					ofxImGuiSurfing::AddBigToggle(animator.SHOW_Gui, _w100, _h);

					ImGui::TreePop();
				}

				//-

				//ImGui::Dummy(ImVec2(0.0f, 2.0f));

				if (bEnableTween)
				{
					if (guiManager.bExtra) {
						//ImGui::Dummy(ImVec2(0.0f, 2.0f));
						ofxImGuiSurfing::AddBigToggle(bEnableLiveMode, _w100, _h / 2);
						ofxImGuiSurfing::AddBigToggle(bKeys, _w100, _h50);
						ofxImGuiSurfing::AddBigToggle(bReset, _w100, _h50);
						ImGui::Dummy(ImVec2(0.0f, 2.0f));
					}
				}

				if (bEnableTween)
				{
					if (ImGui::CollapsingHeader("ACTIONS", _flagt))
					{
						if (ImGui::Button("RANDOMiZE", ImVec2(_w100, _h / 2))) {
							doRandomize(false);
						}
						if (ImGui::Button("GO!", ImVec2(_w100, _h / 2))) {
							doGo();
						}
						if (ImGui::Button("RANDOMiZE+GO", ImVec2(_w100, _h / 2))) {
							doRandomize(true);
						}

						//// blink by timer
						//bool b = bPlay;
						//float a;
						//if (b) a = 1 - tn;
						////if (b) a = ofxSurfingHelpers::getFadeBlink();
						//else a = 1.0f;
						//if (b) ImGui::PushStyleColor(ImGuiCol_Border, (ImVec4)ImColor::HSV(0.5f, 0.0f, 1.0f, 0.5 * a));
						//ofxImGuiSurfing::AddBigToggle(bPlay, _w100, _h / 2, false);
						//if (b) ImGui::PopStyleColor();

						ofxImGuiSurfing::AddBigToggle(bPlay, _w100, _h / 2, false);

						if (bPlay) {

							//--

							// draw progress bar timer
							{
								ImGuiStyle *style = &ImGui::GetStyle();
								ImGui::PushID("prog");
								const ImVec4 color = style->Colors[ImGuiCol_ButtonHovered];//we can force change this color on theme... only used here
								ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
								ImGui::ProgressBar(tn);
								ImGui::PopStyleColor();
								ImGui::PopID();
							}

							//--

							//TODO:
							// add bpm
							ofxImGuiSurfing::AddParameter(playTime);
							//ofxImGuiSurfing::AddParameter(playSpeed);
							//ImGui::SliderFloat("Speed", &playSpeed, 0, 1);
							//ImGui::ProgressBar(tn);


						}
					}
					ImGui::Dummy(ImVec2(0.0f, 2.0f));
				}

				//-

				if (bEnableTween)
				{
					_flagt = ImGuiTreeNodeFlags_None;
					//_flagt = ImGuiTreeNodeFlags_DefaultOpen;

					if (ImGui::CollapsingHeader("PANELS", _flagt))
					{
						ImGui::Indent();
						ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);

						//ImGui::Text("Panels:");
						ofxImGuiSurfing::AddBigToggle(animator.SHOW_Gui, _w100, _h50);
						ofxImGuiSurfing::AddBigToggle(bShowInputs, _w50, _h50); ImGui::SameLine();
						ofxImGuiSurfing::AddBigToggle(bShowOutputs, _w50, _h50);
						ofxImGuiSurfing::AddBigToggle(bShowPlots, _w50, _h50); ImGui::SameLine();
						ofxImGuiSurfing::AddBigToggle(bFullScreen, _w50, _h50);

						ImGui::Unindent();
					}

					//----

					// enable toggles

					//bOpen = false;
					//_flagw = (bOpen ? ImGuiWindowFlags_NoCollapse : ImGuiWindowFlags_None);

					_flagt = ImGuiTreeNodeFlags_None;
					//_flagt = ImGuiTreeNodeFlags_DefaultOpen;

					if (ImGui::CollapsingHeader("ENABLE PARAMETERS", _flagt))
					{
						ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);

						//ImGui::Text("ENABLE PARAMETERS");

						static bool bNone, bAll;
						if (ImGui::Button("NONE", ImVec2(_w50, _h / 2)))
						{
							doDisableAll();
						}
						ImGui::SameLine();
						if (ImGui::Button("ALL", ImVec2(_w50, _h / 2)))
						{
							doEnableAll();
						}
						ImGui::Dummy(ImVec2(0.0f, 2.0f));

						for (int i = 0; i < params_EditorEnablers.size(); i++)
						{
							auto &p = params_EditorEnablers[i];// ofAbstractParameter
							auto type = p.type();
							bool isBool = type == typeid(ofParameter<bool>).name();
							//bool isGroup = type == typeid(ofParameterGroup).name();
							//bool isFloat = type == typeid(ofParameter<float>).name();
							//bool isInt = type == typeid(ofParameter<int>).name();
							string name = p.getName();

							if (isBool)//just in case... 
							{
								// 1. toggle enable
								ofParameter<bool> pb = p.cast<bool>();
								ofxImGuiSurfing::AddBigToggle(pb, _w100, _h / 2, false);
								//ImGui::SameLine();
							}
						}
					}

					//ImGui::Dummy(ImVec2(0.0f, 5.0f));

					//-

					bOpen = false;
					ImGuiTreeNodeFlags _flagt = (bOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None);

					//TODO:
					//plots
					//ofxSurfingHelpers::AddPlot(input);
					//ofxSurfingHelpers::AddPlot(output);

					if (ImGui::CollapsingHeader("MONITOR", _flagt))
					{
						ofxImGuiSurfing::refreshImGui_WidgetsSizes(_w100, _w50, _w33, _w25, _h);

						ofxImGuiSurfing::AddBigToggle(solo, _w100, _h50);
						auto &_p = params_EditorEnablers[index];// ofAbstractParameter
						string name = _p.getName();
						ImGui::Text(name.c_str());

						if (ofxImGuiSurfing::AddStepper(index)) {
							index = ofClamp(index, index.getMin(), index.getMax());
						}
						ofxImGuiSurfing::AddParameter(index);
						ofxImGuiSurfing::AddParameter(input);
						ofxImGuiSurfing::AddParameter(output);
						ImGui::Dummy(ImVec2(0.0f, 2.0f));
					}

					ImGui::Dummy(ImVec2(0.0f, 2.0f));
					ofxImGuiSurfing::AddToggleRoundedButton(guiManager.bExtra);
					if (guiManager.bExtra) ofxImGuiSurfing::AddToggleRoundedButton(guiManager.bAdvanced);

					//ImGui::Dummy(ImVec2(0.0f, 1.0f));
					//if (ImGui::CollapsingHeader("EXTRA"))
					if (guiManager.bExtra)
					{
						//ImGui::Dummy(ImVec2(0.0f, 2.0f));
						ofxImGuiSurfing::AddParameter(bShowHelp);
						ofxImGuiSurfing::AddParameter(rectangle_PlotsBg.bEditMode);
						ImGui::Dummy(ImVec2(0.0f, 2.0f));

						//-

						guiManager.drawAdvancedSubPanel();
					}
				}
			}
			guiManager.endWindow();

			//----

			if (bShowInputs)
			{
				//bOpen = false;
				//_flagw = (bOpen ? ImGuiWindowFlags_NoCollapse : ImGuiWindowFlags_None);

				n = "Parameters_SOURCES";
				guiManager.beginWindow(n.c_str(), NULL, _flagw);
				{
					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
					flags |= ImGuiTreeNodeFlags_DefaultOpen;
					ofxImGuiSurfing::AddGroup(mParamsGroup, flags);
				}
				guiManager.endWindow();
			}

			//-

			if (bShowOutputs)
			{
				//bOpen = false;
				//_flagw = (bOpen ? ImGuiWindowFlags_NoCollapse : ImGuiWindowFlags_None);

				n = "Parameters_TWEENED";
				guiManager.beginWindow(n.c_str(), NULL, _flagw);
				{
					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
					flags |= ImGuiTreeNodeFlags_DefaultOpen;
					ofxImGuiSurfing::AddGroup(mParamsGroup_COPY, flags);
				}
				guiManager.endWindow();
			}
		}
	}
	guiManager.end(); // global end
}
#endif

//----

//--------------------------------------------------------------
void ofxSurfingTween::addParam(ofAbstractParameter& aparam) {

	string _name = aparam.getName();
	ofLogNotice() << __FUNCTION__ << " [ ofAbstractParameter ] \t " << _name;

	//--

	//TODO:
	//nested groups

	// https://forum.openframeworks.cc/t/ofxparametercollection-manage-multiple-ofparameters/34888/3
	auto type = aparam.type();

	bool isGroup = type == typeid(ofParameterGroup).name();
	bool isFloat = type == typeid(ofParameter<float>).name();
	bool isInt = type == typeid(ofParameter<int>).name();
	bool isBool = type == typeid(ofParameter<bool>).name();
	//vec
	bool isVec2 = type == typeid(ofParameter<glm::vec2>).name();
	bool isVec3 = type == typeid(ofParameter<glm::vec3>).name();
	bool isVec4 = type == typeid(ofParameter<glm::vec4>).name();

	ofLogNotice() << __FUNCTION__ << " " << _name << " \t [ " << type << " ]";

	if (isGroup)
	{
		auto &g = aparam.castGroup();

		////TODO:
		//group COPY 
		//nested groups
		//string n = g.getName();
		//ofParameterGroup gc{ n + suffix };
		//mParamsGroup_COPY.add(gc);

		for (int i = 0; i < g.size(); i++) {
			addParam(g.get(i));
		}
	}

	// add/queue each param
	// exclude groups to remove from plots
	if (!isGroup) mParamsGroup.add(aparam);

	//--

	// create a copy group
	// will be the output or target to be use params

	if (isFloat) {
		ofParameter<float> p = aparam.cast<float>();
		ofParameter<float> _p{ _name + suffix, p.get(), p.getMin(), p.getMax() };
		mParamsGroup_COPY.add(_p);

		//-

		ofParameter<bool> b0{ _name, true };
		enablersForParams.push_back(b0);
		params_EditorEnablers.add(b0);
	}
	else if (isInt) {
		ofParameter<int> p = aparam.cast<int>();
		ofParameter<int> _p{ _name + suffix, p.get(), p.getMin(), p.getMax() };
		mParamsGroup_COPY.add(_p);

		//-

		ofParameter<bool> b0{ _name, true };
		enablersForParams.push_back(b0);
		params_EditorEnablers.add(b0);
	}
	else if (isBool) {
		ofParameter<bool> p = aparam.cast<bool>();
		ofParameter<bool> _p{ _name + suffix, p.get() };
		mParamsGroup_COPY.add(_p);

		//-

		ofParameter<bool> b0{ _name, true };
		enablersForParams.push_back(b0);
		params_EditorEnablers.add(b0);
	}

	//TODO:
	//vec
	else if (isVec2) {
		ofParameter<glm::vec2> p = aparam.cast<glm::vec2>();
		ofParameter<glm::vec2> _p{ _name + suffix, p.get(), p.getMin(), p.getMax() };
		mParamsGroup_COPY.add(_p);

		//-

		ofParameter<bool> b0{ _name, true };
		enablersForParams.push_back(b0);
		params_EditorEnablers.add(b0);
	}
	else if (isVec3) {
		ofParameter<glm::vec3> p = aparam.cast<glm::vec3>();
		ofParameter<glm::vec3> _p{ _name + suffix, p.get(), p.getMin(), p.getMax() };
		mParamsGroup_COPY.add(_p);

		//-

		ofParameter<bool> b0{ _name, true };
		enablersForParams.push_back(b0);
		params_EditorEnablers.add(b0);
	}
	else if (isVec4) {
		ofParameter<glm::vec4> p = aparam.cast<glm::vec4>();
		ofParameter<glm::vec4> _p{ _name + suffix, p.get(), p.getMin(), p.getMax() };
		mParamsGroup_COPY.add(_p);

		//-

		ofParameter<bool> b0{ _name, true };
		enablersForParams.push_back(b0);
		params_EditorEnablers.add(b0);
	}

	else {
	}

	//-

	params.add(params_EditorEnablers);
}

//--------------------------------------------------------------
void ofxSurfingTween::add(ofParameterGroup aparams) {
	for (int i = 0; i < aparams.size(); i++) {
		addParam(aparams.get(i));
	}
}

//--------------------------------------------------------------
void ofxSurfingTween::add(ofParameter<float>& aparam) {
	addParam(aparam);
}
//--------------------------------------------------------------
void ofxSurfingTween::add(ofParameter<bool>& aparam) {
	addParam(aparam);
}
//--------------------------------------------------------------
void ofxSurfingTween::add(ofParameter<int>& aparam) {
	addParam(aparam);
}
//--------------------------------------------------------------
void ofxSurfingTween::add(ofParameter<glm::vec2>& aparam) {
	addParam(aparam);
}
//--------------------------------------------------------------
void ofxSurfingTween::add(ofParameter<glm::vec3>& aparam) {
	addParam(aparam);
}
//--------------------------------------------------------------
void ofxSurfingTween::add(ofParameter<glm::vec4>& aparam) {
	addParam(aparam);
}

//--

//--------------------------------------------------------------
void ofxSurfingTween::Changed_Controls_Out(ofAbstractParameter &e)
{
	if (bDISABLE_CALLBACKS) return;

	std::string name = e.getName();

	ofLogVerbose(__FUNCTION__) << name << " : " << e;

	if (bEnableLiveMode) {
		doGo();
	}
}

//------------

// API getters

// to get the smoothed parameters indiviauly and externaly

//simplified getters
//--------------------------------------------------------------
float ofxSurfingTween::get(ofParameter<float> &e) {
	string name = e.getName();
	auto &p = mParamsGroup_COPY.get(name);
	if (p.type() == typeid(ofParameter<float>).name())
	{
		return p.cast<float>().get();
	}
	else
	{
		ofLogError(__FUNCTION__) << "Not expected type: " << name;
		return -1;
	}
}
//--------------------------------------------------------------
int ofxSurfingTween::get(ofParameter<int> &e) {
	string name = e.getName();
	auto &p = mParamsGroup_COPY.get(name);
	if (p.type() == typeid(ofParameter<int>).name())
	{
		return p.cast<int>().get();
	}
	else
	{
		ofLogError(__FUNCTION__) << "Not expected type: " << name;
		return -1;
	}
}

////--------------------------------------------------------------
//glm::vec2 ofxSurfingTween::get(ofParameter<glm::vec2> &e) {
//	string name = e.getName();
//	auto &p = mParamsGroup_COPY.get(name);
//	if (p.type() == typeid(ofParameter<glm::vec2>).name())
//	{
//		return p.cast<glm::vec2>().get();
//	}
//	else
//	{
//		ofLogError(__FUNCTION__) << "Not expected type: " << name;
//		return -1;
//	}
//}

////--------------------------------------------------------------
//ofAbstractParameter& ofxSurfingTween::getParamAbstract(ofAbstractParameter &e) {
//	string name = e.getName();
//	auto &p = mParamsGroup.get(name);
//	auto i = mParamsGroup.getPosition(name);
//	float value = outputs[i].getValue();
//
//	return p;
//}
//
////--------------------------------------------------------------
//ofAbstractParameter& ofxSurfingTween::getParamAbstract(string name) {
//	auto &p = mParamsGroup.get(name);
//
//	auto i = mParamsGroup.getPosition(name);
//	float value = outputs[i].getValue();
//
//	return p;
//}
//
////--------------------------------------------------------------
//ofParameter<float>& ofxSurfingTween::getParamFloat(string name) {
//
//	auto &p = mParamsGroup_COPY.get(name);
//	auto i = mParamsGroup_COPY.getPosition(name);
//	if (p.type() == typeid(ofParameter<float>).name()) {
//		ofParameter<float> pf = p.cast<float>();
//		return pf;
//	}
//	else
//	{
//		ofParameter<float> pf{ "empty", -1 };
//		ofLogError(__FUNCTION__) << "Not expected type: " << name;
//		return pf;
//	}
//}
//
////--------------------------------------------------------------
//float ofxSurfingTween::getParamFloatValue(ofAbstractParameter &e) {
//	string name = e.getName();
//
//	auto &p = mParamsGroup_COPY.get(name);
//	auto i = mParamsGroup_COPY.getPosition(name);
//	if (p.type() == typeid(ofParameter<float>).name()) {
//		ofParameter<float> pf = p.cast<float>();
//		return pf.get();
//	}
//	else
//	{
//		ofLogError(__FUNCTION__) << "Not expected type: " << name;
//		return -1;
//	}
//}
//
////--------------------------------------------------------------
//int ofxSurfingTween::getParamIntValue(ofAbstractParameter &e) {
//	string name = e.getName();
//
//	auto &p = mParamsGroup_COPY.get(name);
//	auto i = mParamsGroup_COPY.getPosition(name);
//	if (p.type() == typeid(ofParameter<int>).name()) {
//		ofParameter<int> pf = p.cast<int>();
//		return pf.get();
//	}
//	else
//	{
//		ofLogError(__FUNCTION__) << "Not expected type: " << name;
//		return -1;
//	}
//}
//
////--------------------------------------------------------------
//ofParameter<int>& ofxSurfingTween::getParamInt(string name) {
//
//	auto &p = mParamsGroup_COPY.get(name);
//	auto i = mParamsGroup_COPY.getPosition(name);
//	if (p.type() == typeid(ofParameter<int>).name()) {
//		ofParameter<int> pf = p.cast<int>();
//		return pf;
//	}
//	else
//	{
//		ofParameter<int> pf{ "empty", -1 };
//		ofLogError(__FUNCTION__) << "Not expected type: " << name;
//		return pf;
//	}
//}

//--------------------------------------------------------------
void ofxSurfingTween::doSetAll(bool b) {
	ofLogNotice(__FUNCTION__) << b;

	for (int i = 0; i < params_EditorEnablers.size(); i++)
	{
		auto &p = params_EditorEnablers[i];//ofAbstractParameter
		auto type = p.type();
		bool isBool = type == typeid(ofParameter<bool>).name();
		string name = p.getName();

		if (isBool) {
			ofParameter<bool> pb = p.cast<bool>();
			pb.set(b);
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingTween::doDisableAll() {
	doSetAll(false);
}

//--------------------------------------------------------------
void ofxSurfingTween::doEnableAll() {
	doSetAll(true);
}