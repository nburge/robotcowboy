/*
 * Copyright (c) 2012 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/robotcowboy for documentation
 *
 */
#include "AppCore.h"

#ifndef TARGET_OF_IPHONE
	// for mapping special keys
	#ifndef __APPLE__
		#include <GL/glut.h>
	#else
		#include <GLUT/glut.h>
	#endif
#endif

#include "Scene.h"
#include "Global.h"

AppCore::AppCore() ://App& parent) : parent(parent),
	global(Global::instance()) {

	mouseButton = 1;
	bMousePressed = false;
	currentKey = 0;
	bKeyPressed = false;
	
	bAltPressed = false;
	bShiftPressed = false;
	bControlPressed = false;

	Global::instance().core = this;
}

//--------------------------------------------------------------
void AppCore::setup(const int numOutChannels, const int numInChannels,
				    const int sampleRate, const int ticksPerBuffer) {

	// setup of
	global.resetGraphics();
	//ofSetLogLevel(OF_LOG_VERBOSE);
	//ofSetLogLevel("Pd", OF_LOG_WARNING);
	//ofSetLogLevel("ofxLua", OF_LOG_VERBOSE);
	
	#ifdef TARGET_OF_IPHONE
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
	#ifndef TARGET_OF_IPHONE
		ofSetDataPathRoot(ofFilePath::getAbsolutePath(ofFilePath::getCurrentWorkingDirectory()+"/../../../data"));
	#endif
	global.scenePath = ofToDataPath("")+"/scenes/";
	ofLogVerbose() << "AppCore: scene path is " << global.scenePath;
	
	// load scenes
	sceneManager.add(new Scene("TestOsc"));
	sceneManager.add(new Scene("TestInput"));
	sceneManager.add(new Scene("PhysicsChain"));
	sceneManager.add(new Scene("PhysicsPolygon"));
	//sceneManager.setup(false);	// setup all the scenes on the fly
	ofSetLogLevel("ofxSceneManager", OF_LOG_VERBOSE); // lets see whats going on inside
	sceneManager.gotoScene(0, true);
	sceneManager.run(true);
}

//--------------------------------------------------------------
void AppCore::update() {
	
	sceneManager.update();
	
	// check for waiting osc messages
	global.audioEngine.update();
	global.osc.update();
	global.physics.update();
	global.gui.update();
}

//--------------------------------------------------------------
void AppCore::draw() {

	sceneManager.draw();
	
	if(global.scriptEngine.errorOcurred()) {
		ofSetColor(0);
		ofxBitmapString(11, 11) << global.scriptEngine.getErrorMessage();
		ofSetColor(255);
		ofxBitmapString(10, 10) << global.scriptEngine.getErrorMessage();
	}
	
	global.gui.console.draw();
	global.gui.drawFps();
	global.gui.draw();
}

//--------------------------------------------------------------
void AppCore::exit() {
	sceneManager.clear();
	global.clear();
}

//--------------------------------------------------------------
void AppCore::keyPressed(int key) {

	// read modifier keys
	#ifndef TARGET_OF_IPHONE
		#ifdef TARGET_WIN32
			bAltPressed = (bool) ((GetKeyState(VK_MENU) & 0x80) > 0);
			bShiftPressed = (bool) ((GetKeyState(VK_SHIFT) & 0x80) > 0);
			bControlPressed = (bool) ((GetKeyState(VK_CONTROL) & 0x80) > 0);
		#else
			bAltPressed = (bool) (glutGetModifiers() & GLUT_ACTIVE_ALT);
			bShiftPressed = (bool) (glutGetModifiers() & GLUT_ACTIVE_SHIFT);
			bControlPressed = (bool) (glutGetModifiers() & GLUT_ACTIVE_CTRL);
		#endif
	#endif
	if(bAltPressed) {
		switch(key) {
			case 'f':
				//ofToggleFullscreen();
				break;
			default:
				break;
		}
		return;
	}

	currentKey = key;
	bKeyPressed = true;

	switch(key) {
	
		case 'r':
			global.scriptEngine.reloadScript();
			global.audioEngine.reloadPatch();
			sceneManager.getCurrentScene()->setup();
			break;
			
		default:
			break;
	}
	
	sceneManager.keyPressed(key);
}

//--------------------------------------------------------------
void AppCore::keyReleased(int key) {
	bKeyPressed = false;
	sceneManager.keyReleased(key);
}

//--------------------------------------------------------------
void AppCore::mouseMoved(int x, int y) {
	sceneManager.mouseMoved(x, y);
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
void AppCore::mouseDragged(int x, int y, int button) {
	sceneManager.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void AppCore::audioReceived(float * input, int bufferSize, int nChannels) {
	global.audioEngine.pd.audioIn(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void AppCore::audioRequested(float * output, int bufferSize, int nChannels) {
	global.audioEngine.pd.audioOut(output, bufferSize, nChannels);
}
