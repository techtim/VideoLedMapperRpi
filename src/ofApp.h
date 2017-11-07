#pragma once

#include "ofMain.h"
#include "ofxLedController.h"

#define MAX_FRAMES_FADE 60

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);

    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void nextVideo();
    bool getGPIOValue(string& pinNum);

    int exportGPIO(string& pinNum);

    ofFbo fbo;
    ofImage grabImg;
    ofVideoPlayer videoPlayer;
    // vector<ofVideoPlayer> videos;

    // ofxOMXPlayer omxPlayer;
    // ofxOMXPlayerSettings settings;

    vector<ofFile> files;

    ofDirectory videosDir;

    unsigned int currentVideo, fadeFramesNum;
    int videoWidth, videoHeight;
    std::string folderPath;
    std::string gpioPinNum;

    unique_ptr<ofxLedController> ledCtrl;
};
