#include "ofApp.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

//--------------------------------------------------------------
void ofApp::setup(){

    currentVideo = 0;
    videoWidth = 482;  // try to grab at this size.
    videoHeight = 480;
    // ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLogLevel(OF_LOG_NOTICE);
    ofSetFrameRate(30);
    ofSetBackgroundColor(0);

/// LED
    ledCtrl = make_unique<ofxLedController>(0, "");

/// GPIO BUTTON
    gpioPinNum = "21";
    exportGPIO(gpioPinNum);

/// GL
    grabImg.allocate(videoWidth, videoHeight, OF_IMAGE_COLOR_ALPHA);

    fbo.allocate(videoWidth, videoHeight, GL_RGBA);
    fbo.begin(); ofClear(0, 0, 0, 0); fbo.end();

/// VIDEO
    folderPath = "/mnt/usb";
    if (!videosDir.doesDirectoryExist(folderPath, false)){
        ofLogError("No folder " + folderPath);
        std::exit(0);
    }
    videosDir.open(folderPath);
    if (videosDir.listDir() == 0) {
        ofLogError("No videos in " + folderPath);
        std::exit(0);
    }
    videosDir.sort();

    std::smatch base_match;
    regex video_check("(.+)\\.mp4");

    for(int i = 0; i < (int)videosDir.size(); i++) {
        string pth = videosDir.getPath(i);
        std::regex_match(pth, base_match, video_check);
        if (base_match.size() == 0)
            continue;

        files.emplace_back(pth);
        ofLogVerbose("adding video " + pth);
    }

    ofLogVerbose("Total videos number: " + ofToString(files.size()));

    if (files.size()>0)
    {
        videoPlayer.load(files[currentVideo].path());
        videoPlayer.setUseTexture(true);
        videoPlayer.play();
    } else {
        ofLogError("NO VIDEOS");
        ofExit();
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    if (getGPIOValue(gpioPinNum))
        nextVideo();

    videoPlayer.update();
    // videoPlayer.getTexture().readToPixels(grabImg.getPixels());
    fbo.readToPixels(grabImg.getPixels());
    ledCtrl->updatePixels(grabImg.getPixels());
    ledCtrl->sendUdp();
}

//--------------------------------------------------------------
void ofApp::draw(){
//    ofSetColor(0);
//    ofDrawRectangle(0, 0, videoWidth, videoHeight);
    fbo.begin();
    if (fadeFramesNum > 0) {
        int fade = 255 - 255 * fadeFramesNum/MAX_FRAMES_FADE;
        ofSetColor(255, 255, 255, fade);
        fadeFramesNum--;
    } else {
        ofClear(0, 0, 0, 0);
        ofSetColor(255,255,255,255);
    }

    videoPlayer.draw(4, 0, videoWidth, videoHeight);
    fbo.end();
    ofSetColor(255,255,255,255);
    fbo.draw(0,0);

    // ledCtrl->draw();
}

void ofApp::nextVideo(){
    if (fadeFramesNum != 0)
        return;

    videoPlayer.stop();
    if (++currentVideo >= files.size())
        currentVideo = 0;

    videoPlayer.load(files[currentVideo].path());
    videoPlayer.setUseTexture(true);
    videoPlayer.play();

    fadeFramesNum = 60;
}


bool ofApp::getGPIOValue(string& pinNum) {
    string val;
    string getval_str = "/sys/class/gpio/gpio" + pinNum + "/value";
    ifstream getvalgpio(getval_str.c_str());
//    if (getvalgpio < 0){
    if (getvalgpio < 0){
        ofLogError("OPERATION FAILED: Unable to get value of GPIO" + pinNum +" .\n");
        return false;
    }

    getvalgpio >> val ;
    getvalgpio.close();

    if(val != "0") {
        return true;
    }
    else {
        return false;
    }

    return false;
}

int ofApp::exportGPIO(string& pinNum)
{
    string export_str = "/sys/class/gpio/export";
    ofstream exportgpio(export_str.c_str());
    if (exportgpio < 0){
        cout << " OPERATION FAILED: Unable to export GPIO"<< pinNum <<" ."<< endl;
        return -1;
    }

    exportgpio << pinNum;
    exportgpio.close();
    return 0;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'l')
        ledCtrl->load("");
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    nextVideo();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
