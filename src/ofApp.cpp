#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    shader.load("glow");
    wind.load("heatmap");
    ikeda.load("ikeda");
    utz.load("utz");
    

//    cam.setPosition(0., 0., -35);
    cam.setCursorDrawEnabled(true);
    cam.setFov(70);
  //  cam.setGlobalPosition( 0,0,0 );
   
    center = ofVec3f(.2,1.2,0);
    cam.setGlobalPosition( -10.,9.,-200 );

    cam.lookAt(center);
    
    model.setup();
    ofDisableArbTex();
    fbo= shared_ptr<ofFbo>(new ofFbo);
    fbo->allocate(20*2*20+20, 1420.*PI/10.);
    chevronMask.allocate(20*2*20+20, 1420.*PI/10.);
    archMask.allocate(20*2*20+20, 1420.*PI/10.);
    ofEnableArbTex();
    final.allocate(20*2*20+20, 1420.*PI/10.);
  
    chevronMask.begin();
    ofClear(0);
    for(int i = 0; i < 20;i++){
        ofSetColor(0);
        ofDrawRectangle(40 + 40*i + 1, 0, 38, 1420.*PI/10.);
    }
    chevronMask.end();
    
    archMask.begin();
    ofClear(0);
    for(int i = 0; i < 20;i++){
        ofSetColor(0);
        ofDrawRectangle(40 + 40*i -1 , 0, 2 , 1420.*PI/10.);
    }
    archMask.end();
    
    
    global.setName("gui");
    global.add(calculateNewCoords.set("calculate",false));
    global.add(maskChevrons.set("maskChevrons",false));
    global.add(maskArches.set("maskArches",false));
    global.add(background.set("background",10,10,255));
    global.add(trail.set("trail",10,10,20));
    global.add(u_color1.set("u_color",ofColor(255,255,255),ofColor(0,0,0),ofColor(255,255,255)));
    global.add(color_auto.set("auto",false));
    
    
    utzShader.setName("generalAdjus");
//    utzShader.add(blurSize.set("blurSize",0,0,10.));
//    utzShader.add(intensity.set("intensity",0,0,10.));
    utzShader.add(saturation.set("saturation",0,0,1.));
    utzShader.add(brightness.set("brightness",0,0,1.));
    utzShader.add(contrast.set("contrast",0,0,5.));
    
//    ofParameter<float>blurSize;
//    ofParameter<float>intensity;
//    
//    ofParameter<float>saturation;
//    ofParameter<float>brightness;
//    ofParameter<float>contrast;
//    
    global.add(b_lines.set("lines",true));
    global.add(b_cloud.set("cloud",false));
    global.add(b_windData.set("wind",false));
    global.add(b_ikeda.set("ikeda",false));
    
    cloud.setName("cloud");
    cloud.add(u_balance.set("u_balance",0.,0.,1.));
//    cloud.add(cbrightness.set("cbrightness",0,0,255));
//    cloud.add(csaturation.set("csaturation",0,0,255));
    cloud.add(hue.set("hue",0,0,255));
//
    cloud.add(u_contrast.set("u_contrast",0.,0.,10.));
    cloud.add(u_zoom.set("u_zoom",0.,0.,100.));
    cloud.add(u_color2.set("u_color",ofColor(255,255,255),ofColor(0,0,0),ofColor(255,255,255)));
    
    
    ikedaGroup.setName("ikeda");
    ikedaGroup.add(ikeda_amount.set("u_amount",0.,0.,1.));
    ikedaGroup.add(ikeda_x.set("u_xGrid",0.,0.,1.));
    ikedaGroup.add(ikeda_y.set("u_yGrid",0.,0.,1.));
    ikedaGroup.add(ikeda_up.set("up",true));
   // ikedaGroup.add(u_color2.set("u_color",ofColor(255,255,255),ofColor(0,0,0),ofColor(255,255,255)));
    
    lineGroup.setName("line parameters");
    lineGroup.add(lineIntensity.set("line intensity",0,0,1.));
    lineGroup.add(linespeed.set("linespeed", 1.8,0.,60.));
    
    global.add(utzShader);
    global.add(cloud);
    global.add(ikedaGroup);
    global.add(lineGroup);
    gui.setup(global);
    gui.loadFromFile("settings.xml");

    glPointSize(2.);

}

//--------------------------------------------------------------
void ofApp::update(){
    
    //cam.setCursorDrawEnabled(!debug);
    cam.setMouseActionsEnabled(!debug);
//    if(color_auto){
//        hue+=hueDirection;
//        if(hue<1)hueDirection=0.3;
//        if(hue>254)hueDirection=-0.3;
//    }
    
   // ofColor c;
   // c.setHsb(hue, csaturation, cbrightness);
   // u_color2=c;

    fbo->begin();
   // ofSetColor(0,0,0,trail);
   // ofDrawRectangle(0, 0, fbo->getWidth(), fbo->getHeight());
  //  ofClear(0);
    ofBackground(0);
    if(b_windData){
        
        wind.begin();
        wind.setUniform2f("iResolution", ofVec2f(fbo->getWidth(),fbo->getHeight()));
        wind.setUniform1f("iGlobalTime", ofGetElapsedTimef());
       // wind.setUniform2f("iMouse", ofVec2f(ofGetMouseX(),ofGetMouseY()));
        ofDrawRectangle(0, 0, fbo->getWidth(), fbo->getHeight());
        wind.end();
    }
    if(b_cloud){
        shader.begin();
        shader.setUniform2f("iResolution", ofVec2f(fbo->getWidth(),fbo->getHeight()));
        shader.setUniform1f("iGlobalTime", ofGetElapsedTimef());
        shader.setUniform1f("u_balance", u_balance);
        shader.setUniform1f("u_contrast", u_contrast);
        shader.setUniform1f("u_zoom", u_zoom);
        shader.setUniform3f("u_color", ofVec3f(u_color2->r/255.0f,u_color2->g/255.0f,u_color2->b/255.0f));
        ofDrawRectangle(0, 0, fbo->getWidth(), fbo->getHeight());
        shader.end();
    }
    
    if(b_ikeda){
        
        ikeda.begin();
        ikeda.setUniform2f("iResolution", ofVec2f(fbo->getWidth(),fbo->getHeight()));
        ikeda.setUniform1f("iGlobalTime", ofGetElapsedTimef());
        ikeda.setUniform1f("u_amount", ikeda_amount);
        ikeda.setUniform1f("u_xGrid", ikeda_x);
        ikeda.setUniform1f("u_yGrid", ikeda_y);
        ikeda.setUniform1i("up", ikeda_up);
        //ikeda.setUniforms(ikedaGroup);
        // ("up", ikeda_up);
        ofDrawRectangle(0, 0, fbo->getWidth(), fbo->getHeight());
        ikeda.end();
    }
    
    

    if(maskChevrons)chevronMask.draw(0,0);
    if(maskArches)archMask.draw(0,0);
    
    if(b_lines){
        ofPushStyle();
        ofSetColor(u_color1);
        ofSetLineWidth(3);
        ofNoFill();
        for(int i = 0;i<lines.size();i++){
            lines[i].y+=linespeed*ofGetLastFrameTime();
            ofDrawLine(lines[i].x ,lines[i].y-1 , lines[i].x  ,lines[i].y+1);
           
        }

        int indx = 0;
        for (vector<ofPoint>::iterator it=lines.begin(); it!=lines.end();)    {
            if(it->y>fbo->getHeight())
                it = lines.erase(it);
            else
                ++it;
        }
        ofPopStyle();
        if(ofRandom(1)<lineIntensity)lines.push_back(ofPoint(  int(ofRandom(18))*40 + 40 , 0) );
    }
    
    fbo->end();
    //dof.setFocalDistance(ofMap(sin(ofGetElapsedTimef()/2),-1,1, 20, 150));
    
    
  //  fbo->getTexture().bind();
    final.begin();
    ofClear(0);
    utz.begin();
    utz.setUniform2f("iResolution", ofVec2f(fbo->getWidth(),fbo->getHeight()));
    utz.setUniform1f("brightness", brightness);
    utz.setUniform1f("contrast", contrast);
    utz.setUniform1f("saturation", saturation);
    
    utz.setUniform1f("blurSize", blurSize);
    utz.setUniform1f("intensity", intensity);
    utz.setUniformTexture("tex", fbo->getTexture(), 1);
    
    ofDrawRectangle(0, 0, fbo->getWidth(), fbo->getHeight());
    utz.end();
    final.end();
   // fbo->getTexture().unbind();
    
    if(calculateNewCoords){
        for(int i = 0 ; i<model.leds.size();i++){
            
            
            for(int v = 0; v<model.leds[i].mesh.getTexCoords().size();v++){
                float inc =float(v) / model.leds[i].mesh.getTexCoords().size();
                ofVec2f h1 = model.leds[i].handle1;
                ofVec2f h2 = model.leds[i].handle2;
                ofVec2f newCoord = h1.interpolate(h2, inc);
                model.leds[i].mesh.setTexCoord(v, newCoord);
                
                
            
            }
        }
        calculateNewCoords=false;
    }
    cam.lookAt(center);
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
   
    ofBackground(background);
    
  //  ofEnableDepthTest();
    cam.begin();
 //   ofPushMatrix();
 //   ofTranslate(700, 400);
    
    ofSetColor(background - 10);
    ofPushMatrix();
    ofRotateX(90);
    //ofSetColor(100,20,20);
    ofDrawPlane(0, 0, 1000, 1000);
    ofPopMatrix();
    
    ofSetColor(255);
    
    final.getTexture().bind();
    model.drawLeds();
    final.getTexture().unbind();
    
    ofSetColor(10);
    if(showProfile) model.drawProfiles();
    ofSetColor(255,0,0);
    
 //   ofDrawSphere(center,2.0);
    

    cam.end();

    if(debug){
        ofPushMatrix();
        ofTranslate(ofGetWidth()-(fbo->getWidth() ) ,0);
        //ofScale(0.5, 0.5);
        ofSetColor(255);
        final.draw(0,0,fbo->getWidth(),fbo->getHeight());
        
        ofSetColor(255, 0, 0,200);
        model.drawHandles();
        //  for(int i = 0 ; i<texPoints.size();i+=199)ofDrawCircle(texPoints[i], 1);//(leds[i].rect);
        ofPopMatrix();
        
    }
    ofFill();
        ofSetColor(255);
        gui.draw();
        ofDrawBitmapString("camera position: " +ofToString(cam.getPosition().x)+", "+ofToString(cam.getPosition().y)+", "+ofToString(cam.getPosition().z), 10, 600);
        
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if(key=='o')cam.setPosition(0, 0, 20);
    if(key=='+')meshVertic++;
    if(key=='-')meshVertic--;
    if(key=='d')debug = !debug;
    if(key=='s')model.saveMapping();
    
    if(key=='p')showProfile=!showProfile;
    
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
