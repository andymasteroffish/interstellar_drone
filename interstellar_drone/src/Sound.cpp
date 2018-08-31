//
//  Sound.cpp
//  droner
//
//  Created by Andrew Wallace on 11/25/17.
//
//

#include "Sound.hpp"


Sound::Sound(string filePath, string fileName){
    isSelected = false;
    isActive = true;
    
    addCycle();
    //addCycle();
    
    curSamplePrc = 0;
    startPrcShift = 0;
    
    masterVolume = 1;
    
    volumeModSound = NULL;  //not used
    
    
    fft.setup(512*2, 512, 256);
    oct.setup(44100, 512*2, 10);
    
    loadSound(filePath, fileName);
    
    //cout<<"size "<<sample.myDataSize<<endl;
    //cout<<"length "<<sample.length<<endl;
    
    
    
    normalColor.set(20, 80, 20);
    selectedColor.set(10, 180, 10);
    
    bgColorNorm.set(170,170,170);
    bgColorOff.set(80,80,80);
    
    cycleSelectedColor.set(180, 180, 20, 100);
    
    killMe = false;
    
    pixelPos = -1;
    pixelCurColor.set(0,0,0);
    ampToMovePixel = 400;
    ampLeftToMovePixelPos = ampToMovePixel;
    //pixelH = ofRandom(0,255); //setting this in load sound
    pixelB = ofRandom(100,255);
    pixelS = ofRandom(190,255);
    biggestFFTLastFrame = -1;
    framesWithTheSameFFT=0;
}

void Sound::loadSound(string filePath, string fileName){
    sampleFileName = fileName;
    sample.load(ofToDataPath(filePath));
    sampleDuration = ((float)sample.length/(float)sample.mySampleRate);
    
    //run through the whole thing for the sake of the fft
    double sample_total = 0;
    double high_sample = 0;
    for (int i=0; i<sample.length; i++){
        double sampleVal = sample.playOnce();
        sample_total += abs(sampleVal);
        high_sample = MAX(high_sample, abs(sampleVal));
        if (fft.process(sampleVal)){
            oct.calculate(fft.magnitudes);
        }
    }
    float this_vol = high_sample;//sample_total / (double)sample.length;
    
    float vals[87];
    int biggestID = 0;
    for (int i=0; i<oct.nAverages; i++){
        float curve = ofMap(i, 0, 87, 0.5, 1.5);
        
        vals[i] = oct.peaks[i] * curve;
        if (vals[i] > vals[biggestID]){
            biggestID = i;
        }
    }
    
    pixelH = ofMap(biggestID,0, oct.nAverages-30, 0, 255, true);
    
    //fuck with the volume to try to get everything to wind up around a target
    float target_vol = 0.98;// 0.25;
    volume_equalize_adjust = target_vol / this_vol;
    
    cout<<sampleFileName<<":"<<biggestID<<endl;
    cout<<"vol: "<<this_vol<<endl;
    cout<<"new vol adjust: "<<volume_equalize_adjust<<endl;
    
}

void Sound::updateAudio(float originalPlaybackPrc){
    //millisTimer += ofGetElapsedTimeMillis()-lastMillisTime;
    
    float playbackPrc = originalPlaybackPrc + startPrcShift;
    if (playbackPrc > 1.0f)    playbackPrc -= 1.0f;
    
    //figure out what cycle we're on
    for (int i=0; i<cycles.size(); i++){
        if (playbackPrc > cycles[i].startPrc){
            curCycle = i;
        }
    }
    curSamplePrc = ofMap(playbackPrc, cycles[curCycle].startPrc, cycles[curCycle].endPrc, 0, 1);
    
    
    sample.setPosition(curSamplePrc);
    double sampleVal = sample.playOnce();
    
    //get the value
    baseAudioValue = sampleVal;
    audioValue = baseAudioValue * masterVolume * volume_equalize_adjust;
    
    //bring it to 0 if something would cancel it
    if (!cycles[curCycle].is_active){
        audioValue = 0;
    }
    if (!isActive){
        audioValue = 0;
    }
    
    //fft shit
    if (fft.process(audioValue)){
        oct.calculate(fft.magnitudes);
    }
    
    //check the visual component
    ampLeftToMovePixelPos -= abs(audioValue);
}

void Sound::draw(int orderPos, float totalTimelineDuration){
    float boxW = 800;
    float boxH = 70;
    
    ofVec2f offset(20, 60+orderPos*(boxH+10));
    ofPushMatrix();
    ofTranslate(offset.x, offset.y);
    
    
    
    int alphaVal = isActive ? 255 : 120;
    
    //break the box into the number of repeats
    float cycleW = boxW/totalCycles;
    float thisX = 0;
    for (int i=0; i<cycles.size(); i++){
        
        float thisW = cycleW * cycles[i].duration;
        //background
        ofFill();
        if (cycles[i].is_active){
            ofSetColor(bgColorNorm, alphaVal);
        }else{
            ofSetColor(bgColorOff, alphaVal);
        }
        ofDrawRectangle(thisX, 0, thisW, boxH);
        
        //show progress it in if it is the current period
        if (curCycle == i){
            if (isSelected){
                ofSetColor(selectedColor, alphaVal);
            }else{
                ofSetColor(normalColor, alphaVal);
            }
            ofFill();
            float thisH = boxH;
            if (!cycles[i].is_active){
                thisH *= 0.5;
            }
            ofDrawRectangle(thisX, boxH-thisH, thisW*curSamplePrc, thisH);
        }
        
        //show if it is selected
        if (cycles[i].isSelected){
            ofFill();
            ofSetColor(cycleSelectedColor);
            ofDrawRectangle(thisX, 0, thisW, boxH);
        }
        
        //wave form
        ///32767.0 is a number from the maxim class
        ofSetColor(50);
        ofSetLineWidth(1);
        float waveStep = (float)sample.length/thisW;
        float ampHeight = boxH * 0.5;
        for (int w=1; w<thisW; w++){
            int indexA = (int)(waveStep*(w-1));
            float valA = (sample.temp[indexA]/32767.0) * ampHeight * masterVolume * volume_equalize_adjust;
            int indexB = (int)(waveStep*w);
            float valB = (sample.temp[indexB]/32767.0) * ampHeight * masterVolume * volume_equalize_adjust;
            ofDrawLine(thisX+w-1, boxH/2 + valA, thisX+w, boxH/2 + valB);
        }
        
        //draw the outline
        ofNoFill();
        ofSetColor(0);
        ofSetLineWidth(isSelected ? 2 : 1);
        ofDrawRectangle(thisX, 0, thisW, boxH);
        
        //show the playback speed for this cycle
        //float playbackPrc = ((sampleDuration*totalCycles)/totalTimelineDuration)/(float)cycles[i].duration;
        float playbackPrc = getPlaybackPrcForCycle(i, totalTimelineDuration);
        ofSetColor(0);
        ofDrawBitmapString(ofToString((int)(playbackPrc*100))+"%", thisX+5, 15);
        
        //increase the X for the next cycle
        thisX += thisW;
    }
    
    //mark the start point
    float startArrowX = clickBox.width*startPrcShift;
    float startArrowH = 10;
    float startArrowW = 5;
    ofFill();
    ofSetColor(0);
    ofDrawTriangle(startArrowX-startArrowW, 0, startArrowX+startArrowW, 0, startArrowX, startArrowH);
    
    //file name
    ofSetColor(0);
    string nameText = sampleFileName;
    if (!isActive)      nameText += "(muted)";
    ofDrawBitmapString(nameText, 5, boxH-5);
    
    ofPopMatrix();
    
    clickBox.x = offset.x;
    clickBox.y = offset.y;
    clickBox.width = boxW;
    clickBox.height = boxH;
}

float Sound::getPlaybackPrcForCycle(int cycle_id, float totalTimelineDuration){
    return ((sampleDuration*totalCycles)/totalTimelineDuration)/(float)cycles[cycle_id].duration;
}

void Sound::updateExternalDisplayInfo(){
    numPixelSteps = 0;
    while (ampLeftToMovePixelPos < 0){
        ampLeftToMovePixelPos += ampToMovePixel;
        numPixelSteps++;
    }
    
    float vals[87];
    int biggestID = 0;
    for (int i=0; i<oct.nAverages; i++){
        float curve = ofMap(i, 0, 87, 0.5, 1.5);
        
        vals[i] = oct.peaks[i] * curve;
        if (vals[i] > vals[biggestID]){
            biggestID = i;
        }
    }
    
    //sometimes the fft gets fucky and returns the same value over and over, so we need to reset it.
    if (numPixelSteps>0){
        if (biggestID == biggestFFTLastFrame){
            framesWithTheSameFFT++;
            //cout<<"same frames "<<framesWithTheSameFFT<<endl;
            if (framesWithTheSameFFT > 40){
                cout<<"reset fft because of the same results over and over"<<endl;
                fft.setup(512*2, 512, 256);
                oct.setup(44100, 512*2, 10);
            }
        }else{
            framesWithTheSameFFT = 0;
        }
        //make sure it is changing sometimes
        biggestFFTLastFrame = biggestID;
    }
    
    pixelCurColor.setHsb(pixelH, pixelS, (int)ofMap(biggestID,0, oct.nAverages-30, 0, 255, true));
    
    //pixelCurColor.setHsb((int)ofMap(biggestID,0, oct.nAverages-30, 0, 255, true), pixelS, pixelB);
}



void Sound::addCycle(){
    Cycle newCycle;
    newCycle.setup();
    cycles.push_back(newCycle);
    updateCycles();
}

void Sound::removeCycle(){
    if (cycles.size() == 1){
        cout<<"can't go less than one cycle"<<endl;
        return;
    }
    cycles.erase(cycles.begin() + cycles.size()-1);
    updateCycles();
}

void Sound::combineCycles(int id){
    if (id >= cycles.size()-1){
        cout<<"can't combine last cycle"<<endl;
        return;
    }
    
    //figure out the duration of the next cycle
    int durationToAdd = cycles[id+1].duration;
    //remove that one
    cycles.erase(cycles.begin()+id+1);
    //add it to the other
    cycles[id].duration += durationToAdd;
    
    updateCycles();
}

void Sound::breakCycle(int id){
    if (cycles[id].duration == 1){
        cout<<"can't break cycle with duration 1"<<endl;
        return;
    }
    
    //attempt to split the cycle in half (rounding of course)
    int takeAway = cycles[id].duration/2;
    cycles[id].duration -= takeAway;
    
    Cycle newCycle;
    newCycle.setup();
    newCycle.duration = takeAway;
    cycles.insert(cycles.begin()+id, newCycle);
    
    updateCycles();
}

//goes through each cycle and gives them their perectenage of the overall loop
void Sound::updateCycles(){
    totalCycles = 0;
    for (int i=0; i<cycles.size(); i++){
        totalCycles += cycles[i].duration;
    }
    
    float prcPerClick = 1.0 / (float)totalCycles;
    
    float curVal = 0;
    for (int i=0; i<cycles.size(); i++){
        cycles[i].startPrc = curVal;
        curVal += cycles[i].duration * prcPerClick;
        cycles[i].endPrc = curVal;
    }
}

void Sound::scrollSelectedCycle(int dir){
    int curSelected = -1;
    for (int i=0; i<cycles.size(); i++){
        if (cycles[i].isSelected){
            curSelected = i;
            cycles[i].isSelected = false;
        }
    }
    
    curSelected += dir;
    if (curSelected<0)              curSelected = cycles.size()-1;
    if (curSelected>=cycles.size()) curSelected = 0;
    cycles[curSelected].isSelected = true;
}

bool Sound::mousePressed(int x, int y, int button){
    
    if (clickBox.inside(x, y)){
        
        //if it was already selected, maybe we select a cycle
        if (isSelected){
            float prc = (x-clickBox.x)/clickBox.width;
            for (int i=0; i<cycles.size(); i++){
                if (prc >= cycles[i].startPrc && prc < cycles[i].endPrc){
                    cycles[i].isSelected = true;
                }else{
                    cycles[i].isSelected = false;
                }
            }
        }
        
        
        isSelected = true;
    }else{
        deselect();
    }
}

void Sound::keyPress(int key, bool shiftIsHeld, bool commandIsHeld){
    if (isSelected){
        
        if (key == ' '){
            isActive = !isActive;
        }
        
        if (key == '['){
            removeCycle();
        }
        if (key == ']'){
            addCycle();
        }
        
        //up arrow
        if (key == 357 && commandIsHeld){
            masterVolume += 0.1;
            masterVolume = MIN(10, masterVolume);
        }
        //down arrow
        if (key == 359 && commandIsHeld){
            masterVolume -= 0.1;
            masterVolume = MAX(0, masterVolume);
        }
        
        //left arrow
        if (key == 356 && !commandIsHeld){
            scrollSelectedCycle(-1);
        }
        //right arrow
        if (key == 358  && !commandIsHeld){
            scrollSelectedCycle(1);
        }
        
        //left arrow
        if (key == 356 && commandIsHeld){
            shiftStartPoint(-1);
        }
        //right arrow
        if (key == 358  && commandIsHeld){
            shiftStartPoint(1);
        }
        
        //cycle specific commands
        for (int i=cycles.size()-1; i>=0; i--){
            if (cycles[i].isSelected){
                if (key == 'z' && !commandIsHeld){
                    cycles[i].is_active = !cycles[i].is_active;
                }
                if (key == 'Z'){
                    muteAllExcept(i);
                }
                if (key == 'z' && commandIsHeld){
                    invertAllMute();
                }
                
                if (key == 'c'){
                    combineCycles(i);
                    return;
                }
                if (key == 'b'){
                    breakCycle(i);
                    return;
                }
            }
        }
        
        //delete
        if (key == 127){
            cout<<"time to die"<<endl;
            killMe = true;
        }
        
        
    }
}

void Sound::deselect(){
    isSelected = false;
    for (int i=0; i<cycles.size(); i++){
        cycles[i].isSelected = false;
    }
}

void Sound::invertAllMute(){
    for (int i=0; i<cycles.size(); i++){
        cycles[i].is_active = !cycles[i].is_active;
    }
}
void Sound::muteAllExcept(int cycleToLeaveOn){
    for (int i=0; i<cycles.size(); i++){
        cycles[i].is_active = i == cycleToLeaveOn;
    }
    
}

void Sound::shiftStartPoint(int dir){
    startPrcShift += dir * (1.0f/(float)totalCycles);
    if (startPrcShift >= 1.0f)   startPrcShift -= 1.0f;
    if (startPrcShift < 0.0f)   startPrcShift += 1.0f;
    cout<<"shift "<<startPrcShift<<endl;
}

void Sound::cleanUp(){
    //don't leak memory pls
    sample.clear();
}
