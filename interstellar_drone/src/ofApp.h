#pragma once

#include "ofMain.h"

#include "ofxMaxim.h"

#include "Node.hpp"

#include "ShipProjectile.hpp"
#include "DisplayWindow.hpp"



class ofApp : public ofBaseApp{

public:
    void setup();
    
    void loadFiles();
    void makeNode();
    
    void update();
    void fire(int type, int x, int y, float angle);
    
    void draw();
    void drawHealth();
    
    void assessHealth();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    //void audioIn(float * input, int bufferSize, int nChannels);
    void audioOut(float * output, int bufferSize, int nChannels);
    
    //void addSound(string filePath, string fileName);
    //void addSoundTest(string filePath, string fileName);
    
    void scrollSelectedSound(int dir);
    
    //alternate window
    shared_ptr<DisplayWindow> displayWindow;
    
    //sound streams
    ofSoundStream soundStreamOut, soundStreamIn;
    
    //maxim stuff
    int		bufferSize;
    int		sampleRate;
    
    maxiOsc timer;
    
    
    //marking playback
    float timeline_duration;
    double playbackPrc;
    
    double basePlaybackPrc;
    
    //sound nodes
    vector<Node *> nodes;
    
    vector<ofFile> files;
    
    //samples
    float masterVolume;
    
    //input
    //bool shiftIsHeld, commandIsHeld;
    
    //player stuff
    Ship ship;
    vector<ShipProjectile> ship_projectiles;
    int selected_projectile;
    
    //for assessing the health
    #define NUM_HEALTH_BANDS 3
    #define NUM_CYCLES 12          //MUST MATCH THE NODES
    CycleInfo health_info[NUM_HEALTH_BANDS][NUM_CYCLES];
    float health_band_total_vol[NUM_HEALTH_BANDS];
    
    
};
