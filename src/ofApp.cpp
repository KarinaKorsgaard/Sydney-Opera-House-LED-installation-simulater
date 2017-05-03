#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

// load shader to adjust contrast brightness and saturation
    utz.load("utz");
    testImg.load("testImg.png");
    
    // setup camera
    cam.setCursorDrawEnabled(true);
    cam.setFov(70);
    center = ofVec3f(.2,1.2,0);
    cam.setGlobalPosition( -10.,9.,-200 );
    cam.lookAt(center);
    
    // setup model
    model.setup();
    ledControl.leds = &model.leds;
    
    
    // allocate framebuffers with arbTex disabled
    ofDisableArbTex();
    fbo= shared_ptr<ofFbo>(new ofFbo);
    fbo->allocate(model.width, TEX_H);
    ofEnableArbTex();
    
    // final framebuffer
    final.allocate(model.width, TEX_H);
  

    
    
    // setup general gui
    global.setName("gui");
    global.add(calculateNewCoords.set("calculate",false));
  //  global.add(maskChevrons.set("maskChevrons",false));
  //  global.add(maskArches.set("maskArches",false));
    global.add(background.set("background",10,10,255));
    
    utzShader.setName("generalAdjus");
    utzShader.add(saturation.set("saturation",0,0,1.));
    utzShader.add(brightness.set("brightness",0,0,1.));
    utzShader.add(contrast.set("contrast",0,0,5.));

    // setup various instances of shader class
    shaders[0].setup("heatmap",0);
  //  shaders[1].setup("glow");
    shaders[1].setup("ikeda");
    
    // setup bubbles
    bubbles.setup("bubbles");
    forces.setup("force",&model.chevCoord);
   // swiper.setup("swiper",&ledControl);
    video.setup("video0.mov");
    // add all the parameters to the general gui
    global.add(utzShader);
    
    global.add(video.parameters);
    global.add(bubbles.parameters);
    global.add(forces.parameters);

    for(int i = 0; i<2;i++){
        global.add(shaders[i].parameters);
    }
    
    gui.setup(global);
    gui.loadFromFile("settings.xml");

    // set point size for led mesh in visualisation
    glPointSize(1);
    ofEnableAlphaBlending();
    

}

//--------------------------------------------------------------
void ofApp::update(){
    
    cam.setMouseActionsEnabled(!debug);
    
    bubbles.update();
    forces.update();
 //   swiper.update();
    video.update();
    
    for(int i = 0; i<2;i++){
        shaders[i].update();
    }
    
    
    // draw all shaders and lines to the framebuffer
    fbo->begin();
    ofBackground(0);
    video.draw();
    for(int i = 0; i<2; i++){
        shaders[i].draw();
    }

    bubbles.draw();
    forces.draw();
   // swiper.draw();
    
    fbo->end();
 
    // use the texture from the framebuffer as input to the shader controlling general brightness, contrast and saturation
    final.begin();
    ofClear(0);
    utz.begin();
    utz.setUniform2f("iResolution", ofVec2f(fbo->getWidth(),fbo->getHeight()));
    utz.setUniform1f("brightness", brightness);
    utz.setUniform1f("contrast", contrast);
    utz.setUniform1f("saturation", saturation);
    utz.setUniformTexture("tex", fbo->getTexture(), 1);
    ofDrawRectangle(0, 0, fbo->getWidth(), fbo->getHeight());
    utz.end();
    
    if(test)testImg.draw(0,0,fbo->getWidth(),fbo->getHeight());
    final.end();

    
    // re calculate texture coords if mapping is changed
    if(calculateNewCoords){
        model.calculateCoords();
        calculateNewCoords=false;
    }
    
    
    cam.lookAt(center);
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
   
    ofBackground(background);
    
    cam.begin();
    
    // draw background and plane
    ofSetColor(background - 10);
    ofPushMatrix();
    ofRotateX(90);
    ofDrawPlane(0, 0, 1000, 1000);
    ofPopMatrix();
    ofSetColor(255);
    
    ofRotateX(-90);
    ofScale(0.1f,0.1f,0.1f);
    // draw leds with final texture
    final.getTexture().bind();
    model.drawLeds();
    final.getTexture().unbind();
    
    // draw profiles
    ofSetColor(10);
    if(showProfile) model.drawProfiles();
    ofSetColor(255,0,0);
    
   
    cam.end();

    //for debug, draw texture and led-mapping
    if(debug){
        ofPushMatrix();
        
        ofSetColor(255);
      //  ofScale(0.2f,0.2f);
        ofTranslate(ofGetWidth()-TEX_W, 0);
        final.draw(0,0);
        ofSetColor(255, 0, 0,200);
        model.drawHandles();
        
      //  ofSetColor(255, 0, 255,200);
     //   model.drawLedBoxes();
        ofPopMatrix();
        
        
    }
    
    ofFill();
    ofSetColor(255);
    gui.draw();

        
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if(key=='o')cam.setPosition(0, 0, 20);
    if(key=='+')meshVertic++;
    if(key=='-')meshVertic--;
    if(key=='d')debug = !debug;
    if(key=='s')model.saveMapping();
    if(key=='r')recordMask();
    if(key=='p')showProfile=!showProfile;
    if(key=='t')test=!test;
    if(key-'0'<4)
        video.loadNew("video"+ofToString(key-'0')+".mov");
}

void ofApp::recordMask(){
    ofFbo f;
    f.allocate(final.getWidth(), final.getHeight());
    f.begin();
    ofClear(0);
    ofSetColor(255);
    ofSetLineWidth(8);

    for(int i = 0; i<model.leds.size();i++){
        
        ofDrawLine(model.leds[i].handle1,model.leds[i].handle2);
    }
    for(int i = 0; i<model.chevleds.size();i++){

        ofDrawLine(model.chevleds[i].handle1,model.chevleds[i].handle2);
    }
    f.end();
    ofPixels pix;
    f.readToPixels(pix);
    ofSaveImage(pix, "texMask.png");
    
 
    
    ofDisableArbTex();
    ofLoadImage(shaders[0].texMask,"texMask.png");
    ofLoadImage(shaders[1].texMask,"texMask.png");
    ofEnableArbTex();
    
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    float xx = x - (ofGetWidth()-fbo->getWidth());
    float yy=y;
    if(drag){
        for(int i = 0; i<model.leds.size();i++){
            if(abs(xx-model.leds[i].handle1.x) + abs(yy-model.leds[i].handle1.y)<15){
                first = i;
                second = 0;
                drag =false;
            }
            else if(abs(xx-model.leds[i].handle2.x) + abs(yy-model.leds[i].handle2.y)<15){
                first = i;
                second = 1;
                drag =false;
            }
            
               // model.leds[i].rect
        }
    }
    if(debug){
        if(first!=-1 && second==0){
            model.leds[first].handle1.x = xx;
            model.leds[first].handle1.y = yy;
        }
        else if(first!=-1 && second==1){
            model.leds[first].handle2.x = xx;
            model.leds[first].handle2.y = yy;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    drag = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    drag = false;
    first = -1;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
