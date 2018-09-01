#include "ofApp.h"

bool fixed_test = true;
int starting_sounds = 9;

//--------------------------------------------------------------
void ofApp::setup(){
    sampleRate 	= 44100;
    bufferSize	= 512; /* Buffer Size. you have to fill this buffer with sound using the for loop in the audioOut method */
    
    ofxMaxiSettings::setup(sampleRate, 2, bufferSize);
    
    ofBackground(80);
    
    masterVolume = 0.75f;
    
    timeline_duration = 8;
    
//    shiftIsHeld = false;
//    commandIsHeld = false;
    
    playbackPrc = basePlaybackPrc = 0;
    
    //setup the  other display
    //displayWindow->sounds = &sounds;
    
    
    //player
    ship.setup();
    
    selected_projectile = PROJ_TOGGLE;
    
    //setup the output
    soundStreamOut.setup(this, 2,0, sampleRate, bufferSize, 4);
    /* this has to happen at the end of setup - it switches on the DAC */
    
    loadFiles();
    
    //setup the health info vectors
    for (int i=0; i<NUM_HEALTH_BANDS; i++){
        float speed_cutoff = 0.75 * i;
        for (int c=0; c<NUM_CYCLES; c++){
            health_info[i][c].speed = speed_cutoff;
        }
    }
}


//--------------------------------------------------------------
void ofApp::loadFiles(){
    ofDirectory dir;
    dir.listDir("/Users/awallace/Documents/projects/droner/interstellar_sample_sounds");
    
    files  = dir.getFiles();
    
    //grab a few at random
    for (int i=0; i<starting_sounds; i++){
        makeNode();
    }
   
}

//--------------------------------------------------------------
void ofApp::makeNode(){
    int id_num = (int)ofRandom(files.size());
    ofFile file;
    file.open(files[id_num]);
    bool good_file = false;
    if (file.exists()){
        if (file.isFile()){
            if (file.getExtension() == "wav"){
                good_file = true;
                
            }
        }
    }
    
    if (!good_file){
        makeNode();
        return;
    }
    
    float padding = 50;
    float start_x = ofRandom(padding, ofGetWidth()-padding);
    float start_y = ofRandom(padding, ofGetHeight()-padding);
    Node * node = new Node(file.getAbsolutePath(), file.getFileName(), start_x, start_y, timeline_duration);
    
    if (fixed_test){
        int spacing_x = 300;
        int spacing_y = 200;
        int cols = 3;
        node->pos.x = 200 + (nodes.size() % cols) * spacing_x;
        node->pos.y = 200 + (nodes.size() / cols) * spacing_y;
        node->vel.set(0,0);
    }
    
    nodes.push_back(node);
}

//--------------------------------------------------------------
//void ofApp::addSoundTest(string filePath, string fileName){
//    //deselect everything
//    for (int i=0; i<sounds.size(); i++){
//        sounds[i]->isSelected = false;
//    }
//    
//    //make the new one
//    Sound * sound = new Sound(filePath, fileName);
//    sound->isSelected = true;
//    
//    //split it up
//    int startingCycles = 16;
//    for (int i=1; i<startingCycles; i++){
//        sound->addCycle();
//    }
//    
//    //combine a bunch
//    int cycles_to_combine = ofRandom(startingCycles/2, startingCycles);
//    for (int i=0; i<cycles_to_combine; i++){
//        sound->combineCycles( (int)ofRandom(sound->cycles.size()-1) );
//    }
//    
//    //turn a few on
//    int num_turned_on = 0;
//    for (int i=0; i<sound->cycles.size(); i++){
//        float chance_of_playing = 0.2;
//        //slow = more likely to play
//        if (sound->getPlaybackPrcForCycle(i, timeline_duration) < 1){
//            chance_of_playing = 0.75;
//        }
//        if (ofRandomuf() < chance_of_playing){
//            sound->cycles[i].muteCycle = false;
//            num_turned_on++;
//        }else{
//            sound->cycles[i].muteCycle = true;
//        }
//    }
//    
//    //if nothing is turned on, do that
//    int min_number_on = 1;
//    if (sound->cycles.size() > 5){
//        min_number_on = 2;
//    }
//    while (num_turned_on < min_number_on){
//        sound->cycles[ (int)ofRandom(sound->cycles.size()) ].muteCycle = false;
//        num_turned_on++;
//    }
//    
//    //turn the whole thing on
//    //sound->isActive = true;
//    
//    sounds.push_back(sound);
//}

//--------------------------------------------------------------
void ofApp::update(){
    float delta_time = ofGetLastFrameTime();
    
    for (int i=0; i<nodes.size(); i++){
        nodes[i]->update(delta_time);
    }
    
    for (int p=ship_projectiles.size()-1; p>=0; p--){
        ship_projectiles[p].update(delta_time);
        
        //did it hit anything?
        for (int n=nodes.size()-1; n>=0; n--){
            if (nodes[n]->checkHit(ship_projectiles[p])){
                ship_projectiles.erase(ship_projectiles.begin() + p);
                break;
            }
            
//            if (ofDistSquared(ship_projectiles[p].pos.x, ship_projectiles[p].pos.y, nodes[n]->pos.x, nodes[n]->pos.y) < powf(ship_projectiles[p].hit_size + nodes[n]->hit_size, 2) ){
//                
//                //do the effect
//                if (ship_projectiles[p].type == PROJ_SWAP){
//                    float old_x = nodes[n]->pos.x;
//                    float old_y = nodes[n]->pos.y;
//                    nodes[n]->cleanUp();
//                    delete nodes[n];
//                    nodes.erase(nodes.begin()+n);
//                    makeNode();
//                    nodes[nodes.size()-1]->pos.set(old_x, old_y);
//                }else{
//                    nodes[n]->doHit(ship_projectiles[p].type);
//                }
//                
//                //remove the projectile
//                ship_projectiles.erase(ship_projectiles.begin() + p);
//                break;
//            }
        }
    }
    
    ship.update(delta_time);
    
    assessHealth();
}

//--------------------------------------------------------------
void ofApp::fire(int type, int x, int y, float angle){
    ShipProjectile proj;
    proj.setup(x, y, angle, type);
    ship_projectiles.push_back(proj);
}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
    
    /* Stick your maximilian 'play()' code in here ! Declare your objects in testApp.h.
     For information on how maximilian works, take a look at the example code at
     http://www.maximilian.strangeloop.co.uk
     under 'Tutorials'.
     */
    
    for (int b = 0; b < bufferSize; b++){
        double sampleOut = 0;
        basePlaybackPrc = timer.phasor(1.0/timeline_duration);
        
        playbackPrc = basePlaybackPrc;
        if (playbackPrc > 1)    playbackPrc -= 1;
        if (playbackPrc < 0)    playbackPrc += 1;
        
        for (int i=0; i<nodes.size(); i++){
            nodes[i]->updateAudio(playbackPrc);
        }
        
        for (int i=0; i<nodes.size(); i++){
            sampleOut += nodes[i]->sound->audioValue;
        }
        
        output[b*nChannels    ] = sampleOut * masterVolume;
        output[b*nChannels + 1] = sampleOut * masterVolume;
    }
    
    
}


//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableAlphaBlending();
    
    //other info
    ofSetColor(255);
    ofDrawBitmapString("period: "+ofToString(timeline_duration)+" seconds", 20, 20);
    
    
    //player
    ship.draw();
    
    //nodes
    for (int i=0; i<nodes.size(); i++){
        nodes[i]->draw(timeline_duration);
    }
    
    //projetciles
    for (int p=ship_projectiles.size()-1; p>=0; p--){
        ship_projectiles[p].draw();
    }
    
    
    
    
    
    //show the projectile
    
    string projectile_name = "Projectile : ";
    if (selected_projectile == PROJ_COMBINE)  projectile_name += "Combine";
    if (selected_projectile == PROJ_SPLIT)    projectile_name += "Split";
    if (selected_projectile == PROJ_TOGGLE)   projectile_name += "Toggle";
    if (selected_projectile == PROJ_VOL_UP)   projectile_name += "Vol Up";
    if (selected_projectile == PROJ_VOL_DOWN) projectile_name += "Vol Down";
    if (selected_projectile == PROJ_SHIFT_UP) projectile_name += "Shift Up";
    if (selected_projectile == PROJ_SHIFT_DOWN) projectile_name += "Shift Down";
    if (selected_projectile == PROJ_SWAP)     projectile_name += "Swap";
    
    ofSetColor(255);
    ofDrawBitmapString(projectile_name, 20, 35);
    
    drawHealth();
    
    ofDisableAlphaBlending();
}
//--------------------------------------------------------------
void ofApp::drawHealth(){
    
    
    ofPushMatrix();
    ofTranslate(80, ofGetHeight() - 100);
    
    int box_w = 40;
    int box_h = 30;
    
    for (int b=0; b<NUM_HEALTH_BANDS; b++){
        for (int c=0; c<NUM_CYCLES; c++){
            
            string vol_text = ofToString( (int)(health_info[b][c].volume*100) / 100.0 );
            
            ofPushMatrix();
            ofTranslate(box_w * c, box_h * b);
            
            
            //fill
            if (health_info[b][c].is_on){
                ofFill();
                ofSetColor(255,255,255,100);
                ofDrawRectangle(0, 0, box_w, box_h);
            }
            
            
            //outine
            ofNoFill();
            ofSetColor(0);
            ofDrawRectangle(0, 0, box_w, box_h);
            
            //text
            if (health_info[b][c].is_on){
                 ofDrawBitmapString(vol_text, box_w/2 - vol_text.size()*4, box_h/2+8);
            }
            
            if (c == 0){
                ofDrawBitmapString(">"+ofToString(health_info[b][0].speed)+"%", -50, box_h/2+8);
            }
            
            if (c==NUM_CYCLES-1){
                ofDrawBitmapString(ofToString( (int)(health_band_total_vol[b]*100) / 100.0 ), box_w + 10, box_h/2+8);
            }
            
            ofPopMatrix();
            
        }
    }
    
    ofPopMatrix();
    
}

//--------------------------------------------------------------
void ofApp::assessHealth(){
    
    //cout<<"-- HEALTH --"<<endl;
    
    for (int b=0; b<NUM_HEALTH_BANDS; b++){
        for (int c=0; c<NUM_CYCLES; c++){
            health_info[b][c].is_on = false;
            health_info[b][c].volume = 0;
        }
        health_band_total_vol[b] = 0;
    }
    
    //go through each sound and get that info
    for (int n=0; n<nodes.size(); n++){
        vector<CycleInfo> node_info = nodes[n]->assessHealth(timeline_duration);
        
        //cout<<"chekcing "<<nodes[n]->sound->sampleFileName<<":"<<endl;
        for (int i=0; i<NUM_CYCLES; i++){
            if (node_info[i].is_on){
                //cout<<"X";
               
                //check which band this fits into
                for (int b=NUM_HEALTH_BANDS-1; b>=0; b--){
                    if (node_info[i].speed > health_info[b][0].speed){
                        health_info[b][i].is_on = true;
                        //cout<<"  dis vol "<<node_info[i].volume<<endl;
                        health_info[b][i].volume += node_info[i].volume;
                        health_band_total_vol[b] += node_info[i].volume;
                        //cout<<"  globvol "<<health_info[b][i].volume<<endl;
                        break;
                    }
                }
                
            }else{
                //cout<<"-";
            }
        }
        //cout<<endl;
    }
    
//    cout<<"GLOBAL:"<<endl;
//    for (int b=0; b<NUM_HEALTH_BANDS; b++){
//        cout<<"speed "<<health_info[b][0].speed<<":"<<endl;
//        for (int c=0; c<NUM_CYCLES; c++){
//            if (health_info[b][c].is_on){
//                cout<<"X";
//            }else{
//                cout<<"-";
//            }
//        }
//        cout<<endl;
//    }
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    ship.keyPressed(key);
    
    //testing
    if (key == 'r'){
        int node_count = nodes.size();
        for (int i=nodes.size()-1; i>=0; i--){
            nodes[i]->cleanUp();
            delete nodes[i];
            nodes.erase(nodes.begin()+i);
        }
        for (int i=0; i<node_count; i++){
            makeNode();
        }
    }
    
    if (key == 'h'){
        assessHealth();
    }
    
    if (key == 't'){
        selected_projectile = PROJ_TOGGLE;
    }
    if (key == 'c'){
        selected_projectile = PROJ_COMBINE;
    }
    if (key == 'b'){
        selected_projectile = PROJ_SPLIT;
    }
    if (key == '.'){
        selected_projectile = PROJ_SHIFT_UP;
    }
    if (key == ','){
        selected_projectile = PROJ_SHIFT_DOWN;
    }
    
    if (key == '='){
        makeNode();
    }
    
    //shooting
    if (key == OF_KEY_LEFT || key == OF_KEY_DOWN){
        selected_projectile--;
        if (selected_projectile < 0){
            selected_projectile = NUM_PROJECTILES-1;
        }
    }
    if (key == OF_KEY_RIGHT || key == OF_KEY_UP){
        selected_projectile++;
        if (selected_projectile >= NUM_PROJECTILES){
            selected_projectile = 0;
        }
    }
    if (key == ' '){
        fire(selected_projectile, ship.pos.x, ship.pos.y, ship.angle_rad);
    }
    
//    if (key == 2304){
//        shiftIsHeld = true;
//    }
//    if (key == 4352){
//        commandIsHeld = true;
//    }
//
//    if (key == '-'){
//        timeline_duration -= 0.2f;
//    }
//    if (key == '='){
//        timeline_duration += 0.2f;
//    }
//
//    //up arrow
//    if (key == 357 && !commandIsHeld){
//        scrollSelectedSound(-1);
//    }
//    //down arrow
//    if (key == 359 && !commandIsHeld){
//        scrollSelectedSound(1);
//    }
//
//    //check all sounds
//    for (int i=sounds.size()-1; i>=0; i--){
//        sounds[i]->keyPress(key, shiftIsHeld, commandIsHeld);
//        if (sounds[i]->killMe){
//            sounds[i]->cleanUp();
//            delete sounds[i];
//            sounds.erase(sounds.begin()+i);
//        }
//    }
//
//    if (key == '1'){
//        masterVolume -= 0.05;
//        if (masterVolume < 0.05){
//            masterVolume = 0.05;
//        }
//    }
//    if (key == '2'){
//        masterVolume += 0.05;
//        if (masterVolume > 1){
//            masterVolume = 1;
//        }
//    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
//    if (key == 2304){
//        shiftIsHeld = false;
//    }
//    if (key == 4352){
//        commandIsHeld = false;
//    }
    
    ship.keyReleased(key);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    fire(selected_projectile,x,y,0);

//    for (int i=sounds.size()-1; i>=0; i--){
//        sounds[i]->mousePressed(x, y, button);
//    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
//    if (dragInfo.files.size() > 0){
//        for(int i = 0; i < dragInfo.files.size(); i++){
//            ofFile file;
//            file.open(dragInfo.files[i]);
//            if (file.exists()){
//                if (file.isFile()){
//                    if (file.getExtension() == "wav"){
//                        addSound(file.getAbsolutePath(), file.getFileName());
//                    }
//                }
//            }
//        }
//    }
}

//--------------------------------------------------------------
//void ofApp::addSound(string filePath, string fileName){
//    //deselect everything
//    for (int i=0; i<sounds.size(); i++){
//        sounds[i]->isSelected = false;
//    }
//
//    //make the new one
//    Sound * sound = new Sound(filePath, fileName);
//    sound->isSelected = true;
//
//    //split it up to roughly fit
//    int startingCycles = timeline_duration/sound->sampleDuration;
//    for (int i=1; i<startingCycles; i++){
//        sound->addCycle();
//    }
//
//    sounds.push_back(sound);
//}

//--------------------------------------------------------------
void ofApp::scrollSelectedSound(int dir){
//    int curSelected = -1;
//    for (int i=0; i<sounds.size(); i++){
//        if (sounds[i]->isSelected){
//            curSelected = i;
//            sounds[i]->deselect();
//        }
//    }
//
//    curSelected += dir;
//    if (curSelected<0)              curSelected = sounds.size()-1;
//    if (curSelected>=sounds.size()) curSelected = 0;
//    sounds[curSelected]->isSelected = true;
//    sounds[curSelected]->cycles[0].isSelected = true;
}
