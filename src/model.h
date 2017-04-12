//
//  model.h
//  _vivid_assimpLoader
//
//  Created by Karina Jensen on 17/03/17.
//
//

#ifndef model_h
#define model_h

#include "ofxXmlSettings.h"
#include "define.h"
#include "ofxAssimpModelLoader.h"
#define LEDSPACING 1.379
class Verts {
public:
    ofVec3f v;
    int meshNum;
    bool operator<( const Verts& other ) const{
        // find angle to centre and compare.
        
        
        if(meshNum<12+18+6+5+2){
            float angle1 = atan2(v.y , v.z );
            float angle2 = atan2( other.v.y, other.v.z);
            
            if ( angle1 < angle2 )return true;
            else return false;
        }
        else{
//          //  float angle = atan2(p1.y - p2.y, p1.x - p2.x)
//            float angle1 = atan2(v.y - 24, v.z +23);
//            float angle2 = atan2( other.v.y - 24, other.v.z+23);
//            
            if ( v.y-v.x < other.v.y-other.v.x )return true;
            else return false;
        }
    
    }
private:
    
};


class Model{
public:

    
    int width = 0;
    void setup(){
        
        assimpModel.loadModel("model2.3ds");
        for(int i = 0; i<assimpModel.getMeshCount();i++){
            ofMesh mesh;
            ofMesh m = assimpModel.getMesh(i);
            m.mergeDuplicateVertices();
          //  cout << m.getNumVertices() << endl;
            mesh.setMode(OF_PRIMITIVE_POINTS);
            vector<int>reverse = {1,2,3,4,5,8,9,11,13,14,15,17};
            if(i<18){ // 16, 18?
                bool rev = false;
                for(int r = 0;r<reverse.size();r++)
                    if(i==reverse[r])rev=true;
                
                if(m.getNumVertices()>3){
                    LED l = *new LED;
                    
                    l.handle1 = ofVec2f(i*arcSpacing+10,0);
                    ofPolyline p;
                    int indx = 0;
//                    if(!rev){
//                        for(int j = 0; j<m.getNumVertices()-3;j+=3){
//                            ofVec3f a = m.getVertices()[j+1];
//                            ofVec3f b = m.getVertices()[j+2];
//                            p.addVertex(a.interpolate(b,0.5));
//                        }
//                    }
//                    else{
                    
                        for(int j = m.getNumVertices()-4; j>3;j-=3){
                            ofVec3f a = m.getVertices()[j];
                            ofVec3f b = m.getVertices()[j-1];
                            p.addVertex(a.interpolate(b,0.5));
                        }
                  //  }
                    // add to polyline and respace so leds get same distance!

                    
                    p=p.getResampledBySpacing(LEDSPACING);
                    numLeds+=p.getVertices().size();
                    
                    for(int j = 0; j<p.getVertices().size();j+=3){ // less coords for optimization
                        mesh.addVertex(p.getVertices()[j]);
                        mesh.addTexCoord(ofVec2f(i*arcSpacing+10,indx));
                        indx+=3;
                    }
                    
                    l.mesh = mesh;
                    l.handle2 = ofVec2f(i*arcSpacing+10,indx);
                    width=l.handle1.x+20;
                    leds.push_back(l);
                }
            }
            else {
                chevleds.push_back(getLedsFromChevron(m, i));
            
            }
        }
       // leds.erase(leds.begin()+leds.size()-1);
        
        cout << "number of leds total= "+ofToString(numLeds)<<endl;
        cout << "width = "+ofToString(width)<<endl;
        cout << "chevrons  = "+ofToString(chevleds.size())<<endl;
    }
    

    
    LED getLedsFromChevron(ofMesh m, int meshNum){
        int indx = 0;
        ofMesh newMesh;
        newMesh.setMode(OF_PRIMITIVE_POINTS);
        LED l = *new LED;
        vector<Verts>verts;
        
        // find a better way to organise this
        for(int i = 0; i<m.getNumVertices();i++){
            Verts o = * new Verts;
            o.v= m.getVertices()[i];
            verts.push_back(o);
            verts.back().meshNum=meshNum;
        }

        
        std::sort( verts.begin(), verts.end() );

        ofVec3f a = verts[0].v;
        ofVec3f b = verts[7].v;
        ofVec3f c = verts[11].v;
        
    
        
        float minDist = 10000;
        int thisVert1 = -1;
        int thisLed1 = -1;
        int thisLed2 = -1;
        for(int i = 0; i<leds.size();i++){
            for(int j = 0; j<leds[i].mesh.getVertices().size();j++){
                float dist = a.distance(leds[i].mesh.getVertices()[j]);
                if(dist < minDist && i!=6){
                    thisVert1 = j;
                    minDist = dist;
                    thisLed1 = i;
                }
            }
        }
        

        l.handle1=leds[thisLed1].mesh.getTexCoords()[thisVert1];
        
        thisVert1 = -1;
        minDist = 10000;
    
        for(int i = 0; i<leds.size();i++){
            for(int j = 0; j<leds[i].mesh.getVertices().size();j++){
                float dist = c.distance(leds[i].mesh.getVertices()[j]);
                if(dist < minDist && i!=thisLed1 && i!=6){ // 6 is the support arc
                    thisVert1 = j;
                    minDist = dist;
                    thisLed2 = i;
                }
            }
        }

        l.handle2=leds[thisLed2].mesh.getTexCoords()[thisVert1];
        
        
        
        //l.handle1 = ofVec2f(texCoordX*20+20,indx);
        ofPolyline p;
        // for(int j = 0; j<m.getNumVertices()-4;j+=4){
        
        
        
        p.addVertex(a);
        p.addVertex(b);
        p.addVertex(c);
        
        
        p=p.getResampledBySpacing(LEDSPACING);
        cout << p.getVertices().size() << endl;
        numLeds+=p.getVertices().size();
        for(int j = 0; j<p.getVertices().size();j+=3){
            newMesh.addVertex(p.getVertices()[j]);
            newMesh.addTexCoord(l.handle1.getInterpolated(l.handle2,float(j)/float(p.getVertices().size())));
        }
        //}
        
        
        chevCoord.push_back(l.handle1.getInterpolated(l.handle2, 0.5));
        //chevCoord.push_back(l.handle2);
        l.mesh = newMesh;
        return l;
    }
    void drawLedBoxes(){
        int ledsPrBox = 512*16 / 4;
        ofFill();
        int indx=0;
        for(int i = 0; i<leds.size();i++){
            for(int j = 0; j<leds[i].mesh.getTexCoords().size();j++){
                indx++;
                if(indx>ledsPrBox){
                    ofDrawRectangle(leds[i].mesh.getTexCoords()[j],5,5);
                    indx=0;
                }
            }
        }
        for(int i = 0; i<chevleds.size();i++){
            for(int j = 0; j<chevleds[i].mesh.getTexCoords().size();j++){
                indx++;
                if(indx>ledsPrBox){
                    ofDrawRectangle(chevleds[i].mesh.getTexCoords()[j],5,5);
                    indx=0;
                }
            }
        }
    }
    
    
    void drawLeds(){
        for(int i = 0; i<leds.size();i++){
            leds[i].mesh.draw();
        }
        for(int i = 0; i<chevleds.size();i++){
            chevleds[i].mesh.draw();
        }
    }
    void drawProfiles(){
        for(int i = 0; i<assimpModel.getMeshCount();i++){
            assimpModel.getMesh(i).draw();
        }
    }
    
    void drawHandles(){
        for(int i = 0; i<leds.size();i++){
            ofFill();
            ofDrawCircle(leds[i].handle1, 2);
            ofNoFill();
            ofDrawCircle(leds[i].handle2, 5);
            ofDrawLine(leds[i].handle1,leds[i].handle2);
        }
        for(int i = 0; i<chevleds.size();i++){
            ofFill();
            ofDrawCircle(chevleds[i].handle1, 3);
            ofNoFill();
            ofDrawCircle(chevleds[i].handle2, 3);
            ofDrawLine(chevleds[i].handle1,chevleds[i].handle2);
        }
    }
    
    // not in use atm
    void saveMapping(){
        ofxXmlSettings xml;
        xml.addTag("mapping");
        xml.pushTag("mapping");
        for(int i = 0; i<leds.size();i++){
            LED l = leds[i];
            xml.addTag("lednr"+ofToString(i));
            xml.pushTag("lednr"+ofToString(i));
            
            xml.addValue("first_x",l.handle1.x);
            xml.addValue("first_y",l.handle1.y);
            
            xml.addValue("second_x",l.handle2.x);
            xml.addValue("second_y",l.handle2.y);
            
            xml.popTag();
        }
        xml.popTag();
        xml.saveFile("mapping.xml");
    }
    
    
    
    
    void calculateCoords(){
        for(int i = 0 ; i<leds.size();i++){
            for(int v = 0; v<leds[i].mesh.getTexCoords().size();v++){
                // find % of length we are along the mapping line (0-1)
                float INC =float(v) / float(leds[i].mesh.getTexCoords().size());
                
                ofVec2f h1 = leds[i].handle1;
                ofVec2f h2 = leds[i].handle2;
                
                // get the 2d coordinate of the point between h1 and h2 at "inc" percent
                ofVec2f newCoord = h1.getInterpolated(h2, INC);
                leds[i].mesh.setTexCoord(v, newCoord);
            }
        }
        for(int i = 0 ; i<chevleds.size();i++){
            for(int v = 0; v<chevleds[i].mesh.getTexCoords().size();v++){
                // find % of length we are along the mapping line (0-1)
                float INC =float(v) / float(chevleds[i].mesh.getTexCoords().size());
                
                ofVec2f h1 = chevleds[i].handle1;
                ofVec2f h2 = chevleds[i].handle2;
                
                // get the 2d coordinate of the point between h1 and h2 at "inc" percent
                ofVec2f newCoord = h1.getInterpolated(h2, INC);
                chevleds[i].mesh.setTexCoord(v, newCoord);
                chevCoord[i]=h1.getInterpolated(h2, 0.5);
            }
        }
    }
    
    
    vector<LED>leds;
    vector<LED>chevleds;
    vector<ofMesh>shc;
    vector<ofMesh>chevs;
    vector<ofVec2f>chevCoord;
    
    int numLeds = 0;
private:
    ofxAssimpModelLoader assimpModel;
};

#endif /* model_h */




// a boolean function that tells ofSort how to compare two items


//class Model{
//public:
//    ofxXmlSettings xml;
//    void setup(){
//
//
//
//        float scale = 10.f;
//        float dist = 40.;
//        xml.load("mapping.xml");
//        xml.pushTag("mapping");
//        vector<float>angles = {
//            180,0,0,0,
//            180,50,
//            180,35,35 };
//        float a;
//        for(int i= 1; i<4;i++){
//            int n = i;
//            angles[i]=27.+3.18*2. *n+float(n)*0.35;
//            n++;
//            a =       27.+3.18*2. *n+float(n)*0.35;
//        }
//
//        float tilt = 45.f;
//
//        for(int i = 0; i<angles.size();i++){
//            addArch(angles[i], dist*i + 40, ofVec3f(tilt,0,0), ofVec3f(0,0,0), 14.20*scale, true);
//            tilt-= (135./9.);
//        }
//        addArch(360, dist*angles.size() + 40, ofVec3f(tilt,0,0), ofVec3f(0,0,0), 14.20*scale, true);
//
//        addArch(180, dist*(angles.size()+1) + 40, ofVec3f(0,63.7,0), ofVec3f(3.5*scale,0.*scale,1.6*scale) , 12.0*scale, true);
//
//        tilt=45.-(135./9.);
//
//        for(int i = 0 ; i<5;i++){
//            addArch(a-(i*3.18*2. + float(i)*0.35 ),40+ dist*i+dist*(angles.size()+2 ), ofVec3f(tilt,235.,0.), ofVec3f(7.45*scale +60./5., 0 , -4.5f*scale+50./5 ), 14.20*scale, true);
//
//            tilt-= (135./9.);
//        }
//
//
//        // create chevrons from led mesh
//        vector<ofMesh>l,l2;
//        for(int i = 0; i<leds.size()-6;i++)l.push_back(leds[i].mesh);
//        for(int i = leds.size()-5; i<leds.size();i++)l2.push_back(leds[i].mesh);
//
//        addChevrons(l);
//        addChevrons(l2);
//
//        leds.erase(leds.begin()+angles.size()); // dumb.
//
//
//        addArch(85, dist*(angles.size()+ 7) + 40, ofVec3f(0,63.7-130.,0), ofVec3f(3.5*scale +4 ,0.*scale,1.6*scale - 39 ) , 12.*scale, true);
//
////        addArch(125, dist*(angles.size()+3) + 40, ofVec3f(0,63.7-160.,0), ofVec3f(3.5*scale +4 ,0.*scale,1.6*scale - 39 ) , 11.*scale, true , 85);
//
//
//        // add indices
//        for(int i = 0; i<shc.size();i++){
//            shc[i].enableIndices();
//            cout << shc[i].getVertices().size() << endl;
//            for(int v = 0; v < shc[i].getVertices().size()-4 ; v+=4){
//                //   int i = 0;
//                shc[i].addIndex(0 + v);
//                shc[i].addIndex(3 + v);
//                shc[i].addIndex(4 + v);
//
//                shc[i].addIndex(3 + v);
//                shc[i].addIndex(4 + v);
//                shc[i].addIndex(7 + v);
//
//                shc[i].addIndex(3 + v);
//                shc[i].addIndex(2 + v);
//                shc[i].addIndex(7 + v);
//
//                shc[i].addIndex(2 + v);
//                shc[i].addIndex(7 + v);
//                shc[i].addIndex(6 + v);
//
//                shc[i].addIndex(2 + v);
//                shc[i].addIndex(5 + v);
//                shc[i].addIndex(6 + v);
//
//                shc[i].addIndex(2 + v);
//                shc[i].addIndex(5 + v);
//                shc[i].addIndex(1 + v);
//
//            }
//            // ends
//            shc[i].addIndex(0 + shc[i].getVertices().size()-4);
//            shc[i].addIndex(1 + shc[i].getVertices().size()-4);
//            shc[i].addIndex(2 + shc[i].getVertices().size()-4);
//
//            shc[i].addIndex(0 + shc[i].getVertices().size()-4);
//            shc[i].addIndex(2 + shc[i].getVertices().size()-4);
//            shc[i].addIndex(3 + shc[i].getVertices().size()-4);
//        }
//    }
//
//    void drawLeds(){
//        for(int i = 0; i<leds.size();i++){
//            leds[i].mesh.draw();
//        }
//    }
//    void drawProfiles(){
//        for(int i = 0; i<shc.size();i++){
//            shc[i].draw();
//        }
//    }
//    void drawChevrons(){
//        for(int i = 0; i<chevs.size();i++){
//            chevs[i].draw();
//        }
//    }
//    void drawRects(){
//        for(int i = 0; i<leds.size();i++){
//            ofDrawRectangle(leds[i].rect);
//        }
//    }
//    void drawHandles(){
//        for(int i = 0; i<leds.size();i++){
//            ofFill();
//            ofDrawCircle(leds[i].handle1, 3);
//            ofNoFill();
//            ofDrawCircle(leds[i].handle2, 3);
//            ofDrawLine(leds[i].handle1,leds[i].handle2);
//        }
//    }
//
//    void addArch(float angle, float xCoord, ofVec3f rot, ofVec3f trans, float diameter = 14.20, bool addleds = true, float beginAngle = 0.){
//
//        float hollow_low = 0.1;
//        float hollow_high = 3.;
//       // float ledDist = 0.05;
//        //float dist = 20; // mapping dist
//
//        float h_y = 0.0; // hollows elevation from org
//        float width = 2.; // struct width
//
//        int indx= 0;
//        ofMesh mesh;
//        ofMesh shc_mesh;
//        // ofMesh shc_mesh_top;
//        shc_mesh.setMode(OF_PRIMITIVE_TRIANGLES);
//        mesh.setMode(OF_PRIMITIVE_POINTS);
//        //mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
//        double angle2 = beginAngle;
//
//
//        while (angle2<angle) {
//
//            if(addleds){
//                float x = cos(ofDegToRad(angle2))*(diameter/2);
//                float y = sin(ofDegToRad(angle2))*(diameter/2);
//              //  float z = -width/4.;
//                float z = 0;
//                mesh.addVertex(transRot(ofVec3f(x,y,z),rot,trans));
//
//                z = width/4.;
//              //  mesh.addVertex(transRot(ofVec3f(x,y,z),rot,trans));
//              //  mesh.addTexCoord(ofVec2f(xCoord,indx));
//                mesh.addTexCoord(ofVec2f(xCoord,indx*texCoordSpacing));
//            }
//
//            float x = cos(ofDegToRad(angle2))*((diameter + hollow_low  )/2);
//            float y = sin(ofDegToRad(angle2))*((diameter + hollow_low )/2) + h_y;
//            float z = -width/2.;
//
//
//            shc_mesh.addVertex(transRot(ofVec3f(x,y,z),rot,trans));
//
//            z = width/2.;
//            shc_mesh.addVertex(transRot(ofVec3f(x,y,z),rot,trans));
//
//            x = cos(ofDegToRad(angle2))*((diameter+ hollow_high)/2);
//            y = sin(ofDegToRad(angle2))*((diameter+ hollow_high)/2) + h_y;
//            shc_mesh.addVertex(transRot(ofVec3f(x,y,z),rot,trans));
//
//            z = -width/2.;
//            shc_mesh.addVertex(transRot(ofVec3f(x,y,z),rot,trans));
//
//
//            angle2+=inc;
//            indx ++;
//        }
//
//        if(shc_mesh.getVertices().size()>50)
//            shc.push_back(shc_mesh);
//
//        if(addleds){
//            LED l = *new LED;
//            l.mesh = mesh;
//
//            string str = ofToString(leds.size());
//            if(xml.getValue("lednr"+str+":first_x",0.0) > 0){
//                l.handle1.set(xml.getValue("lednr"+str+":first_x",0.0), xml.getValue("lednr"+str+":first_y",0.0));
//                l.handle2.set(xml.getValue("lednr"+str+":second_x",0.0), xml.getValue("lednr"+str+":second_y",0.0));
//            }
//          //  else{
//                l.handle1.set(xCoord, 0);
//                l.handle2.set(xCoord, indx*texCoordSpacing);
//          //  }
//            l.numLEDS = indx;
//            leds.push_back(l);
//        }
//    }
//
//    void addChevrons(vector<ofMesh>meshes){
//        int spacing = 5*4;
//        float width = 1.;
//
//        // double angleSpacing = 360.f/((arcSpacing.0*PI)/1.); // diameter is in metres * scale// scale
//        int ledSpacing = 1.; // 0.1 m * scale
//        int chevronSpacing = 70;
//        int chevronStartAt = 90;
//
//        //vector<ofMesh>chevrons;
//        for(int i = 0 ; i< meshes.size()-1;i++){
//            ofMesh m1 = meshes[i];
//            ofMesh m2 = meshes[i+1];
//
//            //  if(m1.getNumVertices()<1700 || m2.getNumVertices()<1700){
//            for(int m = chevronStartAt ; m< m1.getVertices().size() ;m+= chevronSpacing){
//
//
//                if(m+15 < m2.getVertices().size() && m + 15<m1.getVertices().size()){
//
//                    ofMesh c;
//                    c.setMode(OF_PRIMITIVE_POINTS);
//
//                    ofMesh profile;
//                    profile.setMode(OF_PRIMITIVE_TRIANGLES);
//
//                    float interpol = 0.f;
//
//                    LED led;
//                    led.handle1 = m1.getTexCoord(m);
//                    led.handle2 = m2.getTexCoord(m);
//                    float dist = ofDist(led.handle1.x,led.handle1.y,led.handle2.x,led.handle2.y);
//                    dist = 1./dist * 3.;
//                    chevCoord.push_back(m1.getTexCoord(m).getInterpolated(m2.getTexCoord(m),0.5));
//                    int indx = 0;
//                    while(interpol<2){
//                        ofVec3f v1 = m1.getVertices()[m];
//                        ofVec3f v3 ;
//                        ofVec3f c1 = m1.getVertices()[m-15];
//                        ofVec3f c2 = m2.getVertices()[m-15];
//
//                        v3 = c1.interpolate(c2,0.5);
//
//                        float interpolHalfed = interpol;
//                        if(interpol > 1){
//                            interpolHalfed-=1;
//                            v1 = c1;
//                            v3 = m2.getVertices()[m];
//                        }
//                        v1.interpolate(v3,interpolHalfed);
//
//                        c.addVertex(v1);
//
//                        ofVec2f t1 = m1.getTexCoord(m);
//                        ofVec2f t2 = m2.getTexCoord(m);
//                        c.addTexCoord(t1.interpolate(t2,interpol/2.));
//                        interpol+=dist;
//                        indx++;
//                    }
//                    led.numLEDS = indx;
//
//
//                    // profiles:
//                    int profileW = 6;
//                    ofVec3f v1 = m1.getVertices()[m-profileW];
//                    ofVec3f v2 = m1.getVertices()[m+profileW];
//
//
//                    profile.addVertex(v1);
//                    profile.addVertex(v2);
//                    profile.addVertex(v2);
//                    profile.addVertex(v1);
//
//                    ofVec3f c1 = m1.getVertices()[m-15-profileW].getInterpolated(m2.getVertices()[m-15-profileW],0.5);
//                    ofVec3f c2 = m1.getVertices()[m-15+profileW].getInterpolated(m2.getVertices()[m-15+profileW],0.5);
//
//                    profile.addVertex(c1);
//                    profile.addVertex(c2);
//                    profile.addVertex(c2);
//                    profile.addVertex(c1);
//
//                    //
//
//                    v1 = m2.getVertices()[m-profileW];
//                    v2 = m2.getVertices()[m+profileW];
//
//                    profile.addVertex(v1);
//                    profile.addVertex(v2);
//                    profile.addVertex(v2);
//                    profile.addVertex(v1);
//
//                    shc.push_back(profile);
//
//                    //  chevrons.push_back(c);
//                    led.mesh = c;
//                    leds.push_back(led);
//                }
//            }
//        }
//        // return chevrons;
//    }
//
//    void saveMapping(){
//        ofxXmlSettings xml;
//        xml.addTag("mapping");
//        xml.pushTag("mapping");
//        for(int i = 0; i<leds.size();i++){
//            LED l = leds[i];
//            xml.addTag("lednr"+ofToString(i));
//            xml.pushTag("lednr"+ofToString(i));
//
//            xml.addValue("first_x",l.handle1.x);
//            xml.addValue("first_y",l.handle1.y);
//
//            xml.addValue("second_x",l.handle2.x);
//            xml.addValue("second_y",l.handle2.y);
//
//            xml.popTag();
//        }
//        xml.popTag();
//        xml.saveFile("mapping.xml");
//    }
//
//    ofVec3f transRot(ofVec3f fin, ofVec3f rot, ofVec3f translation){
//        ofVec3f result = rotateX(fin.x,fin.y,fin.z,rot.x);
//        result = rotateY(result.x,result.y,result.z,rot.y);
//        result = rotateZ(result.x,result.y,result.z,rot.z);
//
//        result+=translation;
//
//        return result;
//    }
//
//    ofVec3f rotateX(float x, float y, float z, float a){
//
//        float t = ofDegToRad(a);
//
//        float y1 = y*cos(t)-z*sin(t);
//        float z1 = y*sin(t) + z*cos(t);
//
//        return ofVec3f(x , y1, z1 );
//    }
//
//    ofVec3f rotateY(float x, float y, float z, float a){
//
//        float t = ofDegToRad(a);
//        float z1 = z*cos( t ) - x*sin( t );
//        float x1 = z*sin ( t ) + x*cos ( t );
//
//        return ofVec3f(x1,y,z1);
//    }
//
//
//    ofVec3f rotateZ(float x, float y, float z, float a){
//
//        float t = ofDegToRad(a);
//        float x1 = x*cos( t ) - y*sin( t );
//        float y1 = x*sin ( t ) + y*cos ( t );
//
//        return ofVec3f(x1,y1,z);
//    }
//
//    vector<LED>leds;
//    vector<ofMesh>shc;
//    vector<ofMesh>chevs;
//    vector<ofVec2f>chevCoord;
//};


