//
//  sequencer.h
//  _vivid_assimpLoader
//
//  Created by Karina Jensen on 30/03/17.
//
//

#ifndef sequencer_h
#define sequencer_h
class Sequence{
public:
    void update(ofParameterGroup group){
        time+=ofGetLastFrameTime();
        
        
    }
    
    
private:
    double time;

};

#endif /* sequencer_h */
