#include "ofxCrossedLines.h"

unsigned durationStart = 4400;
unsigned durationEnd = 5350;
float m = 25; // margin at end
float sm = 5; // margin at start
float direction = 1;

//--------------------------------------------------------------
void ofxCrossedLines::setup(){
	numLine = -1;
	direction = 1;
	start = 0;
	end = 1;
//	tween.setParameters(easingsine, ofxTween::easeInOut, 0,1,durationStart,0);
	
}

//--------------------------------------------------------------
void ofxCrossedLines::update(){
	if (numLine == -1) return;

	if (ofGetElapsedTimef()>=endTime && numLine != 4)
	{
		nextMode();
	}
}

void ofxCrossedLines::nextMode() {

	numLine += direction;
	if (numLine == 5) {
		if (direction > 0) {
			cout << "NumLine : " << "start fadeout" << endl;
			direction = -1;
			numLine = 4;
		} else {
			cout << "NumLine : " << "end fadeout" << endl;
			numLine = -1;
			direction = 1;
		}
	}
	if (numLine == -1) direction = 1;

	initTime = ofGetElapsedTimef();
	float duration = direction > 0 ? durationStart : durationEnd;
	endTime = initTime + duration;
	start = direction > 0 ? 0 : 1;
	end = direction > 0 ? 1 : 0;
	

//	cout << "NumLine : " << ofToString(numLine) << ", duration: " << ofToString(duration) << endl;
}

//--------------------------------------------------------------
void ofxCrossedLines::draw(){
	auto now = ofGetElapsedTimef();
	
    ofEnableAlphaBlending();
    ofBackground(255,255,255,255);

	ofFill();
	ofSetColor(ofColor::red, 255);
	ofSetLineWidth(0);
	for (int i = 0; i<=numLine; i++)
	{
		if (i == numLine) {
			float y = ofxeasing::map(now, initTime, endTime, start, end, &ofxeasing::sine::easeInOut);
			drawLine(i, y);
		} else {
			drawLine(i);
		}
	}


	ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void ofxCrossedLines::drawLine(int lineIndex, float interpolation){

	float xStart = 0, yStart = 0, xEnd = 0, yEnd = 0;
	float w = ofGetWidth(), h = ofGetHeight();

	bool endOnYAxis = false;

	if (starMode) {
		switch (lineIndex)
		{
		case 0: 
			xStart = 0; xEnd = w;
			yStart = 0; yEnd = h * 0.4;
			break;	
		case 1: 
			xStart = 0; xEnd = w;
			yStart = h * 0.2; yEnd = h * 0.9;
			break;

		case 2: 
			endOnYAxis = true;
			xStart = 0; xEnd = w * 0.65;
			yStart = h * 0.43; yEnd = h;
			break;

		case 3: 
			endOnYAxis = true;
			xStart = 0; xEnd = w * 0.25;
			yStart = h * 0.65; yEnd = h;
			break;
		}
	} else {
		switch (lineIndex)
		{
		case 0: 
			xStart = w; xEnd = 0;
			yStart = h * 0.125; yEnd = h * 0.175;
			break;	
		case 1: 
			xStart = 0; xEnd = w;
			yStart = h * 0.25; yEnd = h * 0.5;
			break;
		case 2: 
			xStart = w; xEnd = 0;
			yStart = h * 0.6; yEnd = h * 0.75;
			break;
		case 3: 
			xStart = 0; xEnd = w;
			yStart = h * 0.8; yEnd = h * 0.9;
			break;
		}
	}

	//ofSetPolyMode(OF_POLY_WINDING_ABS_GEQ_TWO);
	float marginLerp = ofLerp(sm,m,interpolation);
	float marginLerpBase = ofLerp(0,sm,interpolation);
	ofBeginShape();
	ofVertex(xStart,yStart+sm);
	ofVertex(xStart,yStart-sm);
	if (endOnYAxis) {
		ofVertex(ofLerp(xStart, xEnd+marginLerp, interpolation),   ofLerp(yStart-marginLerpBase, yEnd, interpolation));
		ofVertex(ofLerp(xStart, xEnd-marginLerp, interpolation),   ofLerp(yStart+marginLerpBase, yEnd, interpolation));
	} else {
		ofVertex(ofLerp(xStart, xEnd, interpolation),   ofLerp(yStart-marginLerpBase, yEnd-marginLerp, interpolation));
		ofVertex(ofLerp(xStart, xEnd, interpolation),   ofLerp(yStart+marginLerpBase, yEnd+marginLerp, interpolation));
	}
	ofEndShape(true);
	/*ofTriangle(xStart, yStart,
	ofLerp(xStart, xEnd, interpolation), ofLerp(yStart-m, yEnd-m, interpolation), 
	ofLerp(xStart, xEnd, interpolation), ofLerp(yStart+m, yEnd+m, interpolation));*/
}

//--------------------------------------------------------------
void ofxCrossedLines::keyPressed(int key){
	switch (key)
	{
	case 'r': setup(); break;
	case ' ': nextMode(); break;
	default:
		break;
	}
}