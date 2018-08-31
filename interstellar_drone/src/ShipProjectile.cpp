//
//  ShipProjectile.cpp
//  interstellar_drone
//
//  Created by Andrew Wallace on 8/27/18.
//

#include "ShipProjectile.hpp"

void ShipProjectile::setup(float x, float y, float angle, int _type){
    pos.set(x, y);
    type = _type;
    
    hit_size = 7;
    
    float speed = 300;
    
    vel.x = cos(angle) * speed;
    vel.y = sin(angle) * speed;
    
}
void ShipProjectile::update(float delta_time){
    pos += vel * delta_time;
    
}
void ShipProjectile::draw(){
    
    ofSetColor(255);        //white means something is wrong because no type matched
    
    if (type == PROJ_COMBINE)  ofSetColor(0,0,255);
    if (type == PROJ_SPLIT)    ofSetColor(0,255,0);
    if (type == PROJ_TOGGLE)   ofSetColor(255,0,0);
    if (type == PROJ_VOL_UP || type == PROJ_VOL_DOWN) ofSetColor(66, 244, 197);
    if (type == PROJ_SWAP)     ofSetColor(224, 226, 104);
    
    ofFill();
    ofDrawCircle(pos.x, pos.y, hit_size);
    
    string letter = "";
    if (type == PROJ_COMBINE)  letter = "C";
    if (type == PROJ_SPLIT)    letter = "S";
    if (type == PROJ_TOGGLE)   letter = "T";
    if (type == PROJ_VOL_UP)   letter = "+";
    if (type == PROJ_VOL_DOWN) letter = "-";
    if (type == PROJ_SWAP)     letter = "?";
    ofSetColor(0);
    ofDrawBitmapString(letter, pos.x -4, pos.y+4);
}
