/*
 * Copyright (c) 2011 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/robotcowboy for documentation
 *
 */
#include "AppCore.h"

#include "App.h"
#include "Scene.h"
#include "Global.h"

AppCore::AppCore(App& parent) : parent(parent),
	global(Global::instance()), sceneManager(parent) {

	mouseButton = 1;
	bMousePressed = false;

	Global::instance().core = this;
}

//--------------------------------------------------------------
void AppCore::setup(const int numOutChannels, const int numInChannels,
				    const int sampleRate, const int ticksPerBuffer) {

	// setup of
	ofSetVerticalSync(true);
	//ofSetLogLevel(OF_LOG_VERBOSE);
	//ofSetLogLevel("Pd", OF_LOG_WARNING);
	//ofSetLogLevel("ofxLua", OF_LOG_VERBOSE);
	ofBackground(100, 100, 100);
	
	#ifdef TARGET_IOS
		ofSetDataPathRoot(ofToDataPath("data/", true));
	#endif
	//ofLogNotice() << "Data path: " << ofToDataPath("data", true);
	
	//ofSetDataPathRoot(ofFilePath::getAbsolutePath("data/"));
	ofLogNotice() << "Data path: " << ofToDataPath("");
	
	// setup global objects
	global.setup(numOutChannels, numInChannels,
				 sampleRate, ticksPerBuffer);
	
	// set log level from config file
	ofSetLogLevel(global.logLevel);
	
	// data path
	#ifndef TARGET_IOS
		ofSetDataPathRoot(ofFilePath::getAbsolutePath(ofFilePath::getCurrentWorkingDirectory()+"/../../../data"));
	#endif
	global.scenePath = ofToDataPath("")+"/scenes/";
	ofLogVerbose() << "AppCore: scene path is " << global.scenePath;
	
	// load scenes
	sceneManager.add(new Scene(parent, "Test2"));
	sceneManager.add(new Scene(parent, "Test3"));
	//sceneManager.setup(false);	// setup all the scenes on the fly
	ofSetLogLevel("ofxSceneManager", OF_LOG_VERBOSE); // lets see whats going on inside
	sceneManager.gotoScene(0, true);
	sceneManager.run(true);
	//parent.setSceneManager(&sceneManager);
}

//--------------------------------------------------------------
void AppCore::update() {
	
	sceneManager.update();
	
	// check for waiting osc messages
	global.osc.update();
}

//--------------------------------------------------------------
void AppCore::draw() {
	sceneManager.draw();
	
	if(global.scriptEngine.errorOcurred) {
		ofxBitmapString(10, 10) << global.scriptEngine.errorMsg;
	}
	
	global.gui.console.draw();
}

//--------------------------------------------------------------
void AppCore::exit() {
	sceneManager.clear();
	global.clear();
}

//--------------------------------------------------------------
void AppCore::keyPressed(int key) {

	switch(key) {
	
		case 'r':
			global.scriptEngine.reloadScript();
			sceneManager.getCurrentScene()->setup();
			break;
			
		default:
			break;
	}
	
	sceneManager.keyPressed(key);
}

//--------------------------------------------------------------
void AppCore::mousePressed(int x, int y, int button) {
	mouseButton = button;
	bMousePressed = true;
	sceneManager.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void AppCore::mouseReleased(int x, int y, int button) {
	bMousePressed = false;
	sceneManager.mouseReleased(x, y, button);
}

//--------------------------------------------------------------
void AppCore::audioReceived(float * input, int bufferSize, int nChannels) {
	global.audioEngine.pd.audioIn(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void AppCore::audioRequested(float * output, int bufferSize, int nChannels) {
	global.audioEngine.pd.audioOut(output, bufferSize, nChannels);
}
