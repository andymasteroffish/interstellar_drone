//
//  Node.hpp
//  interstellar_drone
//
//  Created by Andrew Wallace on 8/27/18.
//

#ifndef Node_hpp
#define Node_hpp

#include "ofMain.h"
#include "Sound.hpp"
#include "ShipProjectile.hpp"
#include "CycleInfo.h"

class Node{
public:
    
    Node(string file_path, string file_name, float start_x, float start_y, float timeline_length);
    void update(float delta_time);
    void updateAudio(float originalPlaybackPrc);
    
    void doHit(int type);
    
    void draw(float timeline_length);
    
    vector<CycleInfo> assessHealth(float timeline_duration);
    
    void cleanUp();
    
    ofVec2f pos;
    ofVec2f vel;
    float angle_deg;
    
    float hit_size;
    
    Sound * sound;
    
    bool kill_me;
    
};

#endif /* Node_hpp */
