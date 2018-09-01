//
//  Node.cpp
//  interstellar_drone
//
//  Created by Andrew Wallace on 8/27/18.
//

#include "Node.hpp"

Node::Node(string file_path, string file_name, float start_x, float start_y, float timeline_length){
    
    pos.set(start_x, start_y);
    angle_deg = ofRandom(0,360);
    hit_size = 50;
    
    float speed = 25;
    float move_angle = ofRandom(TWOPI);
    vel.x = cos(move_angle) * speed;
    vel.y = sin(move_angle) * speed;
    
    kill_me = false;
    
    //make the new sound
    sound = new Sound(file_path, file_name);
    sound->isSelected = true;
    
    //split it up
    int startingCycles = 12;
    for (int i=1; i<startingCycles; i++){
        sound->addCycle();
    }
    
    //combine a bunch
    int cycles_to_combine = ofRandom(startingCycles/2, startingCycles);
    for (int i=0; i<cycles_to_combine; i++){
        sound->combineCycles( (int)ofRandom(sound->cycles.size()-1) );
    }
    
    //turn a few on
//    int num_turned_on = 0;
//    for (int i=0; i<sound->cycles.size(); i++){
//        float chance_of_playing = 0.2;
//        //slow = more likely to play
//        if (sound->getPlaybackPrcForCycle(i, timeline_length) < 1){
//            chance_of_playing = 0.75;
//        }
//        if (ofRandomuf() < chance_of_playing){
//            sound->cycles[i].is_active = true;
//            num_turned_on++;
//        }else{
//            sound->cycles[i].is_active = false;
//        }
//    }
//    
//    //if nothing is turned on, do that
//    int min_number_on = 1;
//    if (sound->cycles.size() > 5){
//        min_number_on = 2;
//    }
//    while (num_turned_on < min_number_on){
//        sound->cycles[ (int)ofRandom(sound->cycles.size()) ].is_active = true;
//        num_turned_on++;
//    }
    
    
}

void Node::update(float delta_time){
    pos += vel * delta_time;
    
    //bouncing
    if (pos.x-hit_size < 0){
        vel.x = abs(vel.x);
    }
    if(pos.x+hit_size > ofGetWidth()){
        vel.x = -abs(vel.x);
    }
    if (pos.y-hit_size < 0){
        vel.y = abs(vel.y);
    }
    if(pos.y+hit_size > ofGetHeight()){
        vel.y = -abs(vel.y);
    }
}

void Node::updateAudio(float playback_prc){
    sound->updateAudio(playback_prc);
}


bool Node::checkHit(ShipProjectile proj){
    //did it event hit?
    if (ofDistSquared(proj.pos.x, proj.pos.y, pos.x, pos.y) < powf(proj.hit_size + hit_size, 2) ){
        //if so, which cycle?
        float angle_to_center = atan2(proj.pos.y-pos.y, proj.pos.x-pos.x);
        if (angle_to_center < 0){
            angle_to_center += TWO_PI;
        }
        cout<<"angle to center :"<<angle_to_center<<endl;
        
        float angle_step = TWO_PI / (float)sound->totalCycles;
        
        float cur_angle = 0;
        for (int i=0; i<sound->cycles.size(); i++){
            cur_angle += (float)sound->cycles[i].duration * angle_step;
            if (angle_to_center < cur_angle){
                doHit(proj.type, i);
                return true;
            }
        }
    }
    return false;
}

void Node::doHit(int type, int target_cycle){
    if (type == PROJ_SWAP){
        cout<<"SWAP DOES NOT WORK RIGHT NOW"<<endl;
        return;
    }
    //volume changes the whole thing
    if (type == PROJ_VOL_UP){
        sound->masterVolume += 0.1;
        hit_size += 5;
        return;
    }
    if (type == PROJ_VOL_DOWN){
        sound->masterVolume = MAX(sound->masterVolume-0.1, 0);
        hit_size = MAX(hit_size-5, 5);
        return;
    }
    
    
    //find the next cycle. toggle can do any
    //int target_cycle = sound->curCycle;
//    if (type != PROJ_TOGGLE){
//        for (int i=0; i<sound->cycles.size(); i++){
//            if (sound->cycles[target_cycle].is_active){
//                break;
//            }else{
//                target_cycle++;
//                if (target_cycle >= sound->cycles.size()){
//                    target_cycle = 0;
//                }
//            }
//        }
//    }
    
    //can't combine the last cycle
    if (target_cycle == sound->cycles.size()-1 && type==PROJ_COMBINE){
        target_cycle -= 1;
    }
    
    //do the thing
    if (type == PROJ_COMBINE){
        //sound->cycles[target_cycle].is_active = true;
        sound->combineCycles(target_cycle);
    }
    if (type == PROJ_SPLIT){
        sound->breakCycle(target_cycle);
    }
    if (type == PROJ_TOGGLE){
        sound->cycles[target_cycle].is_active = !sound->cycles[target_cycle].is_active;
    }
    
    
}

void Node::draw(float timeline_length){
    
    ofColor on_col(255,115,76);
    ofColor off_col(255,115,76,100);
    
    ofSetColor(on_col);
    
    ofPushMatrix();
    ofTranslate(pos.x, pos.y);
    ofRotate(angle_deg);
    
    //outline
//    ofNoFill();
//    ofDrawCircle(0, 0, hit_size);
    
    //radial sections
    float wedge_angle = TWO_PI / (float)sound->totalCycles;
    float cur_angle = 0;
    for (int i=0; i<sound->cycles.size(); i++){
        float start_angle = cur_angle;
        vector<ofVec2f> pnts;
        if(sound->cycles.size() > 1){
            pnts.push_back(ofVec2f(0, 0));
        }
        for (int p=0; p<sound->cycles[i].duration; p++){
            pnts.push_back(ofVec2f(cos(cur_angle)*hit_size, sin(cur_angle)*hit_size));
            cur_angle += wedge_angle;
        }
        pnts.push_back(ofVec2f(cos(cur_angle)*hit_size, sin(cur_angle)*hit_size));
        
        //fill
        ofFill();
        if (sound->cycles[i].is_active){
            ofSetColor(on_col);
        }else{
            ofSetColor(off_col);
        }
        ofBeginShape();
        for (int p=0; p<pnts.size(); p++){
            ofVertex(pnts[p].x, pnts[p].y);
        }
        ofEndShape(true);
        
        //outline
        ofNoFill();
        ofSetColor(0);
        ofBeginShape();
        for (int p=0; p<pnts.size(); p++){
            ofVertex(pnts[p].x, pnts[p].y);
        }
        ofEndShape(true);
        
        
        //progress line
        if (sound->curCycle == i){
            float line_angle = start_angle * (1.0-sound->curSamplePrc) + cur_angle * sound->curSamplePrc;
            ofDrawLine(0, 0, cos(line_angle)*hit_size, sin(line_angle)*hit_size);
        }
        
        //show the percentage
        float avg_angle = (cur_angle+start_angle)/2.0;
        float text_dist = hit_size + 18;
        ofSetColor(0);
        string prc_text = ofToString( (int)(sound->getPlaybackPrcForCycle(i, timeline_length)*100) ) + "%";
        ofDrawBitmapString(prc_text, cos(avg_angle)*text_dist - prc_text.size()*4, sin(avg_angle)*text_dist + 4);
        //ofDrawCircle( cos(avg_angle)*text_dist, sin(avg_angle)*text_dist, 2);
    }
    
     ofPopMatrix();
    
    
    //label
    ofFill();
    ofSetColor(0);
    ofDrawBitmapString(sound->sampleFileName, pos.x  - (int)sound->sampleFileName.length()*4 , pos.y + hit_size+38);
    
   
    
}

void Node::cleanUp(){
    sound->cleanUp();
    delete sound;
}


vector<CycleInfo> Node::assessHealth(float timeline_duration){
    vector<CycleInfo> vals;
    int cur_cycle = 0;
    int cur_duration = 0;
    
    for (int i=0; i<sound->totalCycles; i++){
        //cout<<"cycle: "<<cur_cycle<<"   duration: "<<cur_duration<<endl;
        CycleInfo this_info;
        this_info.is_on = sound->cycles[cur_cycle].is_active;
        this_info.volume = sound->masterVolume;
        this_info.speed = sound->getPlaybackPrcForCycle(cur_cycle, timeline_duration);
        vals.push_back(this_info);
        
        //cout<<"  vol: "<<this_info.volume<<endl;
        
        cur_duration++;
        if (cur_duration >= sound->cycles[cur_cycle].duration){
            cur_cycle++;
            cur_duration = 0;
        }
    }
    
    return  vals;
}


