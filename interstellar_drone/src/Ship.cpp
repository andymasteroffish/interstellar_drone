//
//  Ship.cpp
//  interstellar_drone
//
//  Created by Andrew Wallace on 8/27/18.
//

#include "Ship.hpp"

void Ship::setup(){
    move_speed = 200;
    rot_speed = TWO_PI;
    
    pos.set(ofGetWidth()/2, ofGetHeight()/2);
    
    up_is_held = false;
    left_is_held = false;
    right_is_held = false;
    
}

void Ship::update(float delta_time){
    if (up_is_held){
        pos.x += cos(angle_rad) * move_speed * delta_time;
        pos.y += sin(angle_rad) * move_speed * delta_time;
    }
    if (left_is_held){
        angle_rad -= rot_speed * delta_time;
    }
    if (right_is_held){
        angle_rad += rot_speed * delta_time;
    }
}

void Ship::draw(){

    ofPushMatrix();
    
    ofTranslate(pos.x, pos.y);
    ofRotate( ofRadToDeg(angle_rad) );
    
    ofSetColor(255);
    ofFill();
    
    float dist = 15;
    ofDrawTriangle(dist, 0,  -dist, -dist,  -dist, dist);
    
    ofPopMatrix();
}

void Ship::keyPressed(int key){
    
    if (key == 'w'){
        up_is_held = true;
    }
    if (key == 'a'){
        left_is_held = true;
    }
    if (key == 'd'){
        right_is_held = true;
    }
    
}
void Ship::keyReleased(int key){
    if (key == 'w'){
        up_is_held = false;
    }
    if (key == 'a'){
        left_is_held = false;
    }
    if (key == 'd'){
        right_is_held = false;
    }
}
