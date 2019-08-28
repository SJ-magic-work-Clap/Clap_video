/************************************************************
************************************************************/
#pragma once

// #define CHECK_GLITCH_EFFECT

/************************************************************
************************************************************/
#include "ofMain.h"

#include "ofxPostGlitch.h"
#include "ofxOsc_BiDirection.h"

#include "sj_common.h"
#include "util.h"

/************************************************************
************************************************************/
class ofApp : public ofBaseApp{
private:
	enum _ofxPostGlitchType{
		POSTGLITCH_CONVERGENCE,
		POSTGLITCH_GLOW,
		POSTGLITCH_SHAKER,
		POSTGLITCH_CUTSLIDER,
		POSTGLITCH_TWIST,
		POSTGLITCH_OUTLINE,
		POSTGLITCH_NOISE,
		POSTGLITCH_SLITSCAN,
		POSTGLITCH_SWELL,
		POSTGLITCH_INVERT,
		POSTGLITCH_CR_HIGHCONTRAST,
		POSTGLITCH_CR_BLUERAISE,
		POSTGLITCH_CR_REDRAISE,
		POSTGLITCH_CR_GREENRAISE,
		POSTGLITCH_CR_REDINVERT,
		POSTGLITCH_CR_BLUEINVERT,
		POSTGLITCH_CR_GREENINVERT,
		
		NUM_GLITCH,
	};
	
	enum STATE{
		STATE__NORMAL,
		STATE__JUMP,
		STATE__LANDING,
	};
	
	/****************************************
	****************************************/
	STATE State;
	
	bool b_DispGui;
	
	ofFbo fbo_mov;
	ofVideoPlayer mov;
	
	ofxPostGlitch myGlitch;
	
	const double PlaySpeed_H;
	double PlaySpeed;
	
	const int d_Jump = 800;
	const double ImageSize_Jump = 1.1;
	double ImageScale = 1.0;
	
	bool b_DetectClap;
	
	int LastINT;
	int t_from;
	
	OSC_TARGET Osc_ClapDetector;
	
	/****************************************
	****************************************/
	void setup_Gui();
	void clear_fbo(ofFbo& fbo);
	void setup_mov();
	void drawFbo_mov();
	void DisableAllGlitch();
	void StateChart(int now);
	void EnableRandomGlitch(int NUM);
	void Transition_To_Jump(int now);
	void Set_MovPos(float &val);
	
public:
	/****************************************
	****************************************/
	ofApp();
	~ofApp();
	
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
