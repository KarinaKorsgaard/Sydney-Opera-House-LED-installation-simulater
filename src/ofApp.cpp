#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

// load shader to adjust contrast brightness and saturation
    utz.load("utz");
    
    
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
    fbo->allocate(TEX_W, TEX_H);
    chevronMask.allocate(TEX_W, TEX_H);
    archMask.allocate(TEX_W, TEX_H);
    ofEnableArbTex();
    
    // final framebuffer
    final.allocate(TEX_W, TEX_H);
  
    // draw a mask in the chevron framebuffer
    chevronMask.begin();
    ofClear(0);
    for(int i = 0; i < 20;i++){
        ofSetColor(0);
        ofDrawRectangle(40 + 40*i + 1, 0, 38, TEX_H);
    }
    chevronMask.end();
    
    // draw a mask in the arch framebuffer
    archMask.begin();
    ofClear(0);
    for(int i = 0; i < 20;i++){
        ofSetColor(0);
        ofDrawRectangle(40 + 40*i -1 , 0, 2 , TEX_H);
    }
    archMask.end();
    
    
    // setup general gui
    global.setName("gui");
    global.add(calculateNewCoords.set("calculate",false));
    global.add(maskChevrons.set("maskChevrons",false));
    global.add(maskArches.set("maskArches",false));
    global.add(background.set("background",10,10,255));
    
    utzShader.setName("generalAdjus");
    utzShader.add(saturation.set("saturation",0,0,1.));
    utzShader.add(brightness.set("brightness",0,0,1.));
    utzShader.add(contrast.set("contrast",0,0,5.));

    // setup various instances of shader class
    shaders[0].setup("heatmap",0);
    shaders[1].setup("glow");
    shaders[2].setup("ikeda");
    
    // setup bubbles
    bubbles.setup("bubbles");
    forces.setup("force",model.chevCoord);
    swiper.setup("swiper",&ledControl);
    video.setup("video0.mov");
    // add all the parameters to the general gui
    global.add(utzShader);
    
    global.add(video.parameters);
    global.add(bubbles.parameters);
    global.add(forces.parameters);
    global.add(swiper.parameters);
    
    for(int i = 0; i<3;i++){
        global.add(shaders[i].parameters);
    }
    
    gui.setup(global);
    gui.loadFromFile("settings.xml");

    // set point size for led mesh in visualisation
    glPointSize(.1);
    ofEnableAlphaBlending();
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    cam.setMouseActionsEnabled(!debug);
    
    bubbles.update();
    forces.update();
    swiper.update();
    video.update();
    
    for(int i = 0; i<3;i++){
        shaders[i].update();
    }
    
    
    // draw all shaders and lines to the framebuffer
    fbo->begin();
    ofBackground(0);
    video.draw();
    for(int i = 0; i<3; i++){
        shaders[i].draw();
    }

    if(maskChevrons)chevronMask.draw(0,0);
    if(maskArches)archMask.draw(0,0);
    bubbles.draw();
    forces.draw();
    swiper.draw();
    
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
    final.end();

    
    // re calculate texture coords if mapping is changed
    if(calculateNewCoords){
        for(int i = 0 ; i<model.leds.size();i++){
            for(int v = 0; v<model.leds[i].mesh.getTexCoords().size();v++){
                // find % of length we are along the mapping line (0-1)
                float INC =float(v) / float(model.leds[i].mesh.getTexCoords().size());
                
                ofVec2f h1 = model.leds[i].handle1;
                ofVec2f h2 = model.leds[i].handle2;
                
                // get the 2d coordinate of the point between h1 and h2 at "inc" percent
                ofVec2f newCoord = h1.getInterpolated(h2, INC);
                model.leds[i].mesh.setTexCoord(v, newCoord);
            }
        }
        // uncheck calculate when done, so it does not run this loop constantly
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
        ofTranslate(ofGetWidth()-(fbo->getWidth() ) ,0);
        ofSetColor(255);
        final.draw(0,0,fbo->getWidth(),fbo->getHeight());
        ofSetColor(255, 0, 0,200);
        model.drawHandles();
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
    
    if(key=='p')showProfile=!showProfile;
    
    if(key-'0'<4)
        video.loadNew("video"+ofToString(key-'0')+".mov");
    
//    if(key == 'x')moveX=true;
//    if(key == 'y')moveX=false;
//    
//    if(moveX){
//        if(key=='+'){
//          //  for(int i = model.shc.size()-5;i<model.shc.size();i++){
//                for(int v = 0; v<model.shc.back().getNumVertices();v++){
//                    model.shc.back().getVertices()[v].x ++;
//                    
//                }
//                adjX++;
//            //}
//        }
//        if(key=='-'){
//         //   for(int i = model.shc.size()-5;i<model.shc.size();i++){
//                for(int v = 0; v<model.shc.back().getNumVertices();v++){
//                    model.shc.back().getVertices()[v].x --;
//                    
//                }
//                adjX--;
//           // }
//        }
//    }
//    if(!moveX){
//        if(key=='+'){
//          //  for(int i = model.shc.size()-5;i<model.shc.size();i++){
//                for(int v = 0; v<model.shc.back().getNumVertices();v++){
//                    model.shc.back().getVertices()[v].z ++;
//                    
//                }
//                adjZ++;
//          //  }
//        }
//        if(key=='-'){
//        //    for(int i = model.shc.size()-5;i<model.shc.size();i++){
//                for(int v = 0; v<model.shc.back().getNumVertices();v++){
//                    model.shc.back().getVertices()[v].z --;
//                    
//                }
//                adjZ--;
//     //       }
//        }
//    }
//   cout << "x: "+ofToString(adjX)+"  z: "+ofToString(adjZ)<<endl;
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
