//
//  content.h
//  _vivid_assimpLoader
//
//  Created by Karina Jensen on 28/03/17.
//
//

#ifndef content_h
#define content_h

#include "define.h"
#include "ofxAutoReloadedShader.h"

class videoContent{
public:
    void setup(string file){
        video.load(file);
        video.setLoopState(OF_LOOP_NORMAL);
        video.play();
        parameters.setName(file);
        parameters.add(play.set("play",true));
    }
    void update(){
        if(play){
            video.play();
            video.update();
        }
        else video.stop();
    }
    void draw(bool stretch = true){
        
        if(play){
            ofPushStyle();
            ofSetColor(255);
            if(stretch)video.draw(0,0,TEX_W,TEX_H);
            else video.draw(0,0);
            ofPopStyle();
        }
    }
    ofParameterGroup parameters;
private:
    ofVideoPlayer video;
    ofParameter<float>alpha;
    ofParameter<bool>play;
};

class ShaderContent{
public:
    void setup(string file, int numParameters=7){
        shader.load(file);
        name = file;
        parameters.setName(file);
        parameters.add(alpha.set("alpha",1,0,1));
        parameters.add(tempo.set("tempo",1,0,10));
        if(numParameters>3)parameters.add(para1.set("para1",1,0,1));
        if(numParameters>4)parameters.add(para2.set("para2",1,0,1));
        if(numParameters>5)parameters.add(para3.set("para3",1,0,1));
        if(numParameters>6)parameters.add(bool1.set("direction",false));
        parameters.add(color.set("color", ofColor(255),ofColor(0),ofColor(255)));
    }
    void update(){
        if(interpolatedAlpha!=alpha){
            interpolatedAlpha=ease(ofGetElapsedTimef()-timeChange, prevAlpha, alpha, 10);
        }else{
            prevAlpha = alpha;
            timeChange=ofGetElapsedTimef();
        }
    }
    void draw(){
        time+=ofGetLastFrameTime()*tempo;
        if(interpolatedAlpha>0){
            shader.begin();
            shader.setUniform2f("iResolution", ofVec2f(TEX_W,TEX_H));
            shader.setUniform1f("iGlobalTime", time);
            shader.setUniform1i("u_bool", bool1);
            shader.setUniform1f("para1", para1);
            shader.setUniform1f("para2", para2);
            shader.setUniform1f("para3", para3);
            shader.setUniform1f("alpha", interpolatedAlpha);
            shader.setUniform3f("u_color", ofVec3f(color->r/255.0f,color->g/255.0f,color->b/255.0f));
            ofDrawRectangle(0, 0, TEX_W, TEX_H);
            shader.end();
        }
    }
    
    float ease(float t,float b,float c,float d){
        c-=b;
        t /= d/2;
        if (t < 1) return c/2*t*t*t + b;
        t -= 2;
        return c/2*(t*t*t + 2) + b;
    }
    
    ofParameterGroup parameters;
    
private:
    ofxAutoReloadedShader shader;
    ofParameter<bool> bool1;
    ofParameter<float> tempo;
    ofParameter<ofColor> color;
    ofParameter<float> para1,para2,para3;
    ofParameter<float> alpha;
    float interpolatedAlpha = 0.;
    float timeChange = 0.;
    float prevAlpha = 0.;
    float time;
    string name;
    
};

struct Line{
    ofVec2f pos;
    ofVec2f vel;
    float thickness;
};

class Lines{
public:
    void setup(string file){
        parameters.setName(file);
        parameters.add(intensity.set("intensity",1,0,1));
        parameters.add(intensity2.set("intensity2",1,0,1));
        
        parameters.add(tempo.set("tempo",1,0,100));
        parameters.add(thickNess.set("thickNess",1,0,10));
    
        parameters.add(color.set("color", ofColor(255),ofColor(0),ofColor(255)));
    }
    
    void update(){
        for(int i = 0;i<theLines.size();i++){
            theLines[i].pos.y+=theLines[i].vel.y * ofGetLastFrameTime();
            theLines[i].thickness-=ofGetLastFrameTime();
        }
        
        int indx = 0;
        for (vector<Line>::iterator it=theLines.begin(); it!=theLines.end();)    {
            if( (it->pos.y>TEX_H && it->vel.y>0 )||
                (it->pos.y<0 &&     it->vel.y<0 )||
                 it->thickness<0)
                
                it = theLines.erase(it);
            else
                ++it;
        }
    
        if(ofRandom(1)<intensity){
            Line l = *new Line;
            l.pos.set( int(ofRandom(18))*40 + 40 , 0);
            l.vel = ofVec2f(0,tempo);
            l.thickness = ofRandom(thickNess,thickNess*0.7);
            theLines.push_back(l);
        }
        if(ofRandom(1)<intensity2){
            Line l = *new Line;
            l.pos.set( int(ofRandom(18))*40 + 40 , TEX_H );
            l.vel = ofVec2f(0,-tempo);
            l.thickness = ofRandom(thickNess,thickNess*0.7);
            theLines.push_back(l);
        }
    }
    void draw(){
        ofPushStyle();
        ofSetColor(color);
        ofSetLineWidth(3);
        for(int i = 0; i<theLines.size();i++)
            ofDrawLine(theLines[i].pos.x, theLines[i].pos.y,
                       theLines[i].pos.x, theLines[i].pos.y-theLines[i].thickness);
        ofPopStyle();
    }
    
    ofParameterGroup parameters;
    
private:
    ofParameter<float> tempo;
    ofParameter<float> thickNess;
    ofParameter<float> intensity;
    ofParameter<float> intensity2;
    
    ofParameter<ofColor> color;
    vector<Line>theLines;
    
};


struct Pulse{
    ofVec2f pos;
    ofVec2f vel;
    float dist;
    float thickness;
    float tempo;
};

class Forces{
public:
    void setup(string file, vector<ofVec2f>_chevs){
        chevs = _chevs;
        chevDist = abs(chevs[0].y - chevs[1].y);
        parameters.setName(file);
        
        parameters.add(intensity.set("intensity",1,0,1));
        parameters.add(tempo.set("tempo",1,0,100));
        parameters.add(thickNess.set("thickNess",1,0,10));
        
        parameters.add(color.set("color", ofColor(255),ofColor(0),ofColor(255)));
    }
    
    void update(){
        for(int i = 0;i<pulses.size();i++){
            pulses[i].dist+=ofGetLastFrameTime()*tempo;
            //if(pulses[i].dist>40)pulses[i].thickness-=ofGetLastFrameTime();
        }
        
        int indx = 0;
        for (vector<Pulse>::iterator it=pulses.begin(); it!=pulses.end();)    {
            if(it->dist>35)
                it = pulses.erase(it);
            else
                ++it;
        }
        
        if(ofRandom(2)<intensity){
            Pulse p = *new Pulse;
            int chev = ofRandom(chevs.size());
            p.pos = chevs[chev];
            int x = ofRandom(1)>0.5? 1:-1;
            p.vel = ofVec2f(x,0);
            p.tempo = tempo;
            p.thickness = ofRandom(thickNess,thickNess*0.7);
            pulses.push_back(p);
        }
        
      
        for(int i = 0; i<pulses.size();i++){
            Pulse * p = &pulses[i];
            if(p->dist>20 && abs(p->vel.x)>0 ){
                p->vel.y = ofRandom(1)>0.5? 1:-1;
                p->vel.x = 0;
            }
        }
        
        
    }
    void draw(){
        ofPushStyle();
        ofNoFill();
        ofSetColor(color);
        for(int i = 0; i<pulses.size();i++){
            Pulse p = pulses[i];
            float alpha = ofMap(p.dist, 0, 35, thickNess, 0);
            ofSetLineWidth(alpha);
            ofDrawCircle(p.pos,p.dist);

        }
            
        ofPopStyle();
    }
    
    ofParameterGroup parameters;
    
private:
    ofParameter<float> tempo;
    ofParameter<float> thickNess;
    ofParameter<float> intensity;
    float chevDist;
    ofParameter<ofColor> color;
    vector<Pulse>pulses;
    vector<ofVec2f>chevs;
};

struct Swip{
    float time;
    int l;
    float duration;
};
class Swiper{
public:
    void setup(string file, LEDcontrol * ledC){
        ledControl = ledC;
        parameters.setName(file);
        parameters.add(intensity.set("intensity",1,0,1));
        parameters.add(duration.set("duration",1,0,10));
        parameters.add(color.set("color", ofColor(255),ofColor(0),ofColor(255)));
    }
    
    void update(){
        for(int i = 0;i<times.size();i++){
            times[i].time+=ofGetLastFrameTime();
        }
        for (vector<Swip>::iterator it=times.begin(); it!=times.end();)    {
            if(it->time>it->duration)
                it = times.erase(it);
            else
                ++it;
        }
        
        if(ofRandom(2)<intensity){
            Swip p = *new Swip;
            p.l = int(ofRandom(ledControl->getLedCount()));
            p.time = 0.;
            p.duration = ofRandom(duration,duration*0.7) * ledControl->getNumLeds(p.l) * .1;
            times.push_back(p);
        }
    }
    void draw(){
        ofPushStyle();
        ofSetColor(color);
        for(int i = 0; i < times.size() ; i++){
            ledControl->swipeLed(times[i].l,times[i].time,0,1,times[i].duration);
        }
        ofPopStyle();
    }
    
    ofParameterGroup parameters;
private:
    ofParameter<float> tempo;
    ofParameter<float> duration;
    ofParameter<float> intensity;
    ofParameter<ofColor> color;
    vector<Swip>times;
    LEDcontrol * ledControl;

};

#endif /* content_h */
