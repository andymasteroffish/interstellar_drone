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
#include "Ship.hpp"

class Node{
public:
    
    Node(string file_path, string file_name, float start_x, float start_y, float timeline_length, Ship * _ship);
    void update(float delta_time);
    void updateAudio(float originalPlaybackPrc);
    
    bool checkHit(ShipProjectile proj);
    void doHit(int type, int target_cycle);
    
    void draw(float timeline_length);
    
    vector<CycleInfo> assessHealth(float timeline_duration);
    
    void cleanUp();
    
    ofVec2f pos;
    ofVec2f vel;
    float angle_rad;
    
    float hit_size;
    
    Sound * sound;
    
    bool is_out_of_isolate_range;
    float cur_isolate_vol_adjust;
    
    bool kill_me;
    
    
    Ship * ship;
    
};

#endif /* Node_hpp */
