//
//  ShipProjectile.hpp
//  interstellar_drone
//
//  Created by Andrew Wallace on 8/27/18.
//

#ifndef ShipProjectile_hpp
#define ShipProjectile_hpp

#include "ofMain.h"

#define PROJ_COMBINE 0
#define PROJ_SPLIT 1
#define PROJ_TOGGLE 2
#define PROJ_VOL_UP 3
#define PROJ_VOL_DOWN 4
#define PROJ_SHIFT_UP 5
#define PROJ_SHIFT_DOWN 6
#define PROJ_SWAP 7
#define NUM_PROJECTILES 8

class ShipProjectile{
public:
    
    void setup(float x, float y, float angle, int _type);
    void update(float delta_time);
    void draw();
    
    ofVec2f pos;
    ofVec2f vel;
    int type;
    
    float hit_size;
    
};

#endif /* ShipProjectile_hpp */
