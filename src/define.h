//
//  define.h
//  _vivid_assimpLoader
//
//  Created by Karina Jensen on 29/03/17.
//
//

#ifndef define_h
#define define_h
#define arcSpacing 102

#define TEX_W 1764
#define TEX_H 1607//(180.f/inc)*.5


struct LED{
    
    ofRectangle rect;
    int numLEDS;
    ofMesh mesh;
    
    ofPoint handle1;
    ofPoint handle2;
};

class LEDcontrol{
    
public:
    
    
    vector<LED>*leds;
    void swipeLed(int l, float time, float begin, float end, float duration){
        if(l < leds->size()){
            ofVec2f a = leds->at(l).handle1;
            ofVec2f b = leds->at(l).handle2;
            
            ofVec2f beginP = a.getInterpolated(b,begin);
            ofVec2f endP = a.getInterpolated(b,end);
            
            ofVec2f currentP = ease2(time,beginP,endP,duration);
            ofDrawCircle(currentP,3);
        }
    }
    
    ofVec2f getTexCoord(int l, float p){
        if(l < leds->size()){
            return leds->at(l).handle1.getInterpolated(leds->at(l).handle2,p);
        }
        else return ofVec2f(0,0);
    }
    int getLedCount(){
        return leds->size();
    }
    float getNumLeds(int l){
        return leds->at(l).numLEDS;
    }

    
    //------------------------easing fucntions------------------------
    //----------------------------------------------------------------
    float ease(float t,float b,float c,float d){
        c-=b;
        t /= d/2;
        if (t < 1) return c/2*t*t*t + b;
        t -= 2;
        return c/2*(t*t*t + 2) + b;
    }
    
    float exp(float t,float b,float c,float d){
        c-=b;
        c * pow( 2, 10 * (t/d - 1) ) + b;
    }
    float quadOut(float t,float b,float c,float d){
        c-=b;
        t /= d;
        return -c * t*(t-2) + b;
    }
    ofVec2f ease2(float t,ofVec2f b,ofVec2f c,float d){
        c-=b;
        t /= d/2;
        if (t < 1)return c/2*t*t*t + b;
        t -= 2;
        return c/2*(t*t*t + 2) + b;
    }
    ofVec3f ease3(float t,ofVec3f b, ofVec3f c,float d){
        c-=b;
        return c*t/d + b;
    }
    private:
    
};


#endif /* define_h */
