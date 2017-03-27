#pragma once

#include "ofMain.h"
#include "ofxAutoReloadedShader.h"
#include "ofxGui.h"
#include "model.h"
#include "ofxGrabCam.h"

class ofApp : public ofBaseApp{

	public:
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
    bool moveX;
    int adjX=0;
    int adjZ=0;
    ofVec3f center;
    map<int,float>data;
    bool showProfile = true;
    int meshVertic = 0;
    bool debug = false;
    bool drag;
    int first = -1;
    int second;
    ofParameter<int>background,trail;
    
    ofParameter<bool>maskChevrons;
    ofParameter<bool>maskArches;
    ofxGrabCam cam;
    shared_ptr <ofFbo> fbo;
    float hueDirection=0.3;
    
    ofxAutoReloadedShader shader;
    ofxAutoReloadedShader wind,ikeda,utz;
    
    
    
    ofParameter<float>blurSize;
    ofParameter<float>intensity,linespeed;
    
    ofParameter<float>saturation;
    ofParameter<float>brightness;
    ofParameter<float>contrast;
    
    ofParameter<float>lineIntensity;
    
    vector<ofVec2f>texPoints;
    ofxPanel gui;
    ofParameter<float>u_balance,u_contrast,u_zoom,ikeda_amount, ikeda_x, ikeda_y;
    ofParameterGroup cloud,global, utzShader, lineGroup;
    ofParameterGroup ikedaGroup;
    ofParameter<ofColor>u_color1,u_color2,u_color3;
    ofParameter<bool>color_auto, b_lines, b_cloud, b_windData, b_ikeda,ikeda_up;
    ofParameter<float> cbrightness, csaturation, hue;
    vector<ofPoint>lines;
    
    Model model;
    
    ofParameter<bool>calculateNewCoords;
    
    ofFbo chevronMask;
    ofFbo archMask;

    ofFbo final;
};

