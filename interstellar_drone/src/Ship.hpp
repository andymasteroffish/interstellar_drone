//
//  Ship.hpp
//  interstellar_drone
//
//  Created by Andrew Wallace on 8/27/18.
//

#ifndef Ship_hpp
#define Ship_hpp

#include "ofMain.h"

class Ship{
public:
    
    void setup();
    void update(float delta_time);
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    
    
    
    ofVec2f pos;
    float angle_rad;
    
    float move_speed;
    float rot_speed;
    
    float isolate_range;
    
    bool left_is_held, right_is_held, up_is_held;
    bool isolate_active;
    
    
    
};

#endif /* Ship_hpp */
