/************************************************************
************************************************************/
#include "ofApp.h"
#include <time.h>

/******************************
******************************/
ofApp::ofApp()
: b_DispGui(false)
, PlaySpeed(1.0)
, PlaySpeed_H(1.0)
, b_DetectClap(false)
, State(STATE__NORMAL)
, LastINT(0)
, t_from(0)
, Osc_ClapDetector("127.0.0.1", 12346, 12345)
{
	srand((unsigned) time(NULL));
	
	fp_Log = fopen("../../../data/Log.csv", "w");
}

/******************************
******************************/
ofApp::~ofApp()
{
	if(fp_Log)	fclose(fp_Log);
}

/******************************
******************************/
void ofApp::setup(){
	/********************
	********************/
	ofSetWindowTitle("Clap:Video");
	
	ofSetWindowShape( WINDOW_WIDTH, WINDOW_HEIGHT );
	/*
	ofSetVerticalSync(false);
	ofSetFrameRate(0);
	/*/
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	//*/
	
	ofSetEscapeQuitsApp(false);
	
	/********************
	********************/
	setup_Gui();
	Gui_Global->pos.addListener(this, &ofApp::Set_MovPos);
	
	fbo_mov.allocate(FBO_MOV_WIDTH, FBO_MOV_HEIGHT, GL_RGBA);
	clear_fbo(fbo_mov);
	fbo_mov.setAnchorPercent(0.5, 0.5);
	
	myGlitch.setup(&fbo_mov);
	DisableAllGlitch();
	
	setup_mov();
}

/******************************
******************************/
void ofApp::setup_mov()
{
	if(!mov.load("mov/video.mp4")) { ERROR_MSG(); std::exit(1); }
	
	mov.setVolume(Gui_Global->vol);
	mov.setLoopState(OF_LOOP_NORMAL);
	mov.play();
	
	mov.setSpeed(1.0);	// 何故か、ここでSpeed setしてもapplyされなかった。
						// updateでsetする。
}

/******************************
******************************/
void ofApp::clear_fbo(ofFbo& fbo)
{
	fbo.begin();
	ofClear(0, 0, 0, 0);
	fbo.end();
}

/******************************
description
	memoryを確保は、app start後にしないと、
	segmentation faultになってしまった。
******************************/
void ofApp::setup_Gui()
{
	/********************
	********************/
	Gui_Global = new GUI_GLOBAL;
	Gui_Global->setup("CLAP", "gui.xml", 1000, 10);
}

/******************************
******************************/
void ofApp::update(){
	/********************
	********************/
	int now = ofGetSystemTimeMillis();
	
	/********************
	********************/
	if(!mov.isLoaded())	return;
	
	/********************
	********************/
	while(Osc_ClapDetector.OscReceive.hasWaitingMessages()){
		ofxOscMessage m_receive;
		Osc_ClapDetector.OscReceive.getNextMessage(&m_receive);
		
		if(m_receive.getAddress() == "/DetectClap"){
			printf("> got OSC = DetectClap\n");
			m_receive.getArgAsInt32(0); // 読み捨て
			b_DetectClap = true;
		}
	}
	
	/********************
	********************/
	StateChart(now);
	
	/********************
	********************/
	if(!mov.isPaused()) mov.setSpeed(PlaySpeed); // setSpeed()でpauseが解除されてしまうので.
	mov.update();
	if(mov.isPaused() || mov.isFrameNew()){	// isPaused()時、isFrameNew()が立たないが、画面更新したい.
		mov.setVolume(Gui_Global->vol);
		drawFbo_mov();
	}
	
	/********************
	********************/
	LastINT = now;
}

/******************************
■イベントリスナー : GUI
	https://yoppa.org/ma2_14/5899.html
******************************/
void ofApp::Set_MovPos(float &val){
	mov.setPosition(val);
}

/******************************
******************************/
void ofApp::StateChart(int now){
	switch(State){
		case STATE__NORMAL:
			if(b_DetectClap)	Transition_To_Jump(now);
			break;
			
		case STATE__JUMP:
			if(b_DetectClap){
				Transition_To_Jump(now);
			}else if(d_Jump < now - t_from){
				State = STATE__NORMAL;
				DisableAllGlitch();
				PlaySpeed = 1.0;
				
				ImageScale = 1.0;
			}
			break;
	}
}

/******************************
******************************/
void ofApp::Transition_To_Jump(int now){
	const int NumGlitchs = 3;
	
	State = STATE__JUMP;
	
	b_DetectClap = false;
	PlaySpeed = PlaySpeed_H;
	EnableRandomGlitch(NumGlitchs);
	
	ImageScale = ImageSize_Jump;
	
	t_from = now;
}

/******************************
******************************/
void ofApp::EnableRandomGlitch(int NUM){
	/********************
	********************/
	DisableAllGlitch();
	
	/********************
	********************/
	int Array[NUM_GLITCH];
	SJ_UTIL::FisherYates(Array, NUM_GLITCH);
	
	for(int i = 0; i < NUM; i++){
		myGlitch.setFx(ofxPostGlitchType(Array[i]), true);
	}
}

/******************************
******************************/
void ofApp::drawFbo_mov(){
	/********************
	********************/
	/*
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	*/
	
	ofDisableAlphaBlending();
	
	fbo_mov.begin();
		ofBackground(0, 0, 0, 255);
		ofSetColor(255, 255, 255);
		mov.draw(0, 0, fbo_mov.getWidth(), fbo_mov.getHeight());
	fbo_mov.end();
}

/******************************
******************************/
void ofApp::draw(){
	/********************
	********************/
	ofBackground(0);
	
	/********************
	********************/
	myGlitch.generateFx();
	
	// fbo_mov.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	ofPushStyle();
	ofPushMatrix();
		ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
		ofScale(ImageScale, ImageScale, 1.0);
		
		fbo_mov.draw(0, 0, ofGetWidth(), ofGetHeight());
	ofPopMatrix();
	ofPopStyle();
		
	
	/********************
	********************/
	if(b_DispGui)	Gui_Global->gui.draw();
}

/******************************
******************************/
void ofApp::DisableAllGlitch(){
	for(int i = 0; i < NUM_GLITCH; i++){
		myGlitch.setFx(ofxPostGlitchType(i), false);
	}
}

/******************************
******************************/
void ofApp::keyPressed(int key){
#ifdef CHECK_GLITCH_EFFECT

	if (key == '1') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, true);
	if (key == '2') myGlitch.setFx(OFXPOSTGLITCH_GLOW			, true);
	if (key == '3') myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, true);
	if (key == '4') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, true);
	if (key == '5') myGlitch.setFx(OFXPOSTGLITCH_TWIST			, true);
	if (key == '6') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, true);
	if (key == '7') myGlitch.setFx(OFXPOSTGLITCH_NOISE			, true);
	if (key == '8') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, true);
	if (key == '9') myGlitch.setFx(OFXPOSTGLITCH_SWELL			, true);
	if (key == '0') myGlitch.setFx(OFXPOSTGLITCH_INVERT			, true);

	if (key == 'q') myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
	if (key == 'w') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE	, true);
	if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE	, true);
	if (key == 'r') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE	, true);
	if (key == 't') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT	, true);
	if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT	, true);
	if (key == 'u') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT	, true);
	
#else

	switch(key){
		case 'c':
			b_DetectClap = true;
			break;
			
		case 'd':
			b_DispGui = !b_DispGui;
			break;
			
		case 'p':
			if(mov.isPaused())	mov.setPaused(false);
			else				mov.setPaused(true);
			break;
			
		case OF_KEY_RIGHT:
			mov.nextFrame();
			break;
			
		case OF_KEY_LEFT:
			mov.previousFrame();
			break;
	}
#endif
}

/******************************
******************************/
void ofApp::keyReleased(int key){
#ifdef CHECK_GLITCH_EFFECT
	DisableAllGlitch();
#endif
}

/******************************
******************************/
void ofApp::mouseMoved(int x, int y ){

}

/******************************
******************************/
void ofApp::mouseDragged(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mousePressed(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mouseReleased(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mouseEntered(int x, int y){

}

/******************************
******************************/
void ofApp::mouseExited(int x, int y){

}

/******************************
******************************/
void ofApp::windowResized(int w, int h){

}

/******************************
******************************/
void ofApp::gotMessage(ofMessage msg){

}

/******************************
******************************/
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
