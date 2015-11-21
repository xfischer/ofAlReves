//
//  ofxTerrain.cpp
//  displacementMap
//
//  Created by Xavier Fischer on 11/02/2015.
//
//
#include "ofMain.h"
#include "ofxTerrain.h"
#include "ofApp.h"


void ofxTerrain::setup() {
	setup(PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y, 5, 30);
}

//--------------------------------------------------------------
void ofxTerrain::setup(int width, int height, int resolution, float velocity){

	defaultColor = ofFloatColor(1,1,1,0);//ofFloatColor(0,0,1,0); 

	planeWidth = width;
	planeHeight = height;
	planeResolution = resolution;
	planeVelocity = velocity;
	meshSize = ofVec2f(ceilf(planeWidth/planeResolution), ceilf(planeHeight/planeResolution));
	sumDeltaX = 0;
	//heightMap.reserve(width*height);

	// ary[i][j] is then rewritten as ary[i*sizeY+j]

	planeZscale = 50.;

	noiseScale = 0.06;
	noiseSeed = 0.1;
	noiseAmp = 1;

	noiseScale2 = 0.13;
	noiseSeed2 = 0.3;
	noiseAmp2 = 0.15;
	drawYLines = false;

    mode = 3;
    speedRate = 0;
    
	setupLineMesh(meshSize.x, meshSize.y, planeResolution);

	


}

//--------------------------------------------------------------
ofFloatColor ofxTerrain::getColor(float a){
	a = a*4-1;
	return ofFloatColor(defaultColor.r,defaultColor.g,defaultColor.b,a); //a);
}

//--------------------------------------------------------------
void ofxTerrain::setupLineMesh(int width, int height, int segmentLength){

	mesh.setMode(OF_PRIMITIVE_POINTS);
	//
	mesh.setMode(OF_PRIMITIVE_LINES);
	//mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
	mesh.clear();
	mesh.enableColors();
	mesh.enableIndices();
	mesh.disableNormals();

	// setup height map
	for(int x=0; x<width; x++) {
		for(int y=0; y<height; y++) {

			if (mode == 0 || mode == 2) {
				float noiseValue = genNoise(x, y);
				heightMap.push_back(noiseValue);
			}
			else
			{
				float noiseValue = 1-noiseAmp2+genNoise2(x, y); // background noise only
				heightMap.push_back(noiseValue);
			}
		}
	}

	// setup vertices
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			float Y = y * segmentLength;
			float X = x * segmentLength;
			float Z = heightMap[indexFromXY(x, y, height)];
			mesh.addVertex(ofVec3f(X,Y,Z * planeZscale));
			mesh.addColor(getColor(Z));
		}
	}


	// setup indexes
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {

			// -
			if (x + 1 != width){
				mesh.addIndex(indexFromXY(x, y, height));
				mesh.addIndex(indexFromXY(x+1, y, height));
			}

			// |
			if (drawYLines) {
				if (y + 1 != height){
					mesh.addIndex(indexFromXY(x, y, height));
					mesh.addIndex(indexFromXY(x, y+1, height));
				}
			}
		}
	}
}

//--------------------------------------------------------------
float ofxTerrain::genNoise(int x, int y){
	float noiseValue = noiseAmp * ofNoise(x * noiseScale, y * noiseScale, noiseSeed);
	noiseValue += genNoise2(x,y);
	return noiseValue;
}

//--------------------------------------------------------------
float ofxTerrain::genNoise2(int x, int y){
	float noiseValue = ofNoise(x * noiseScale2, y * noiseScale2, noiseSeed2);
	if (noiseValue>0.9) {
		noiseValue *= noiseAmp2;
	} else {
		noiseValue = 0;
	}
	return noiseValue;
}

//--------------------------------------------------------------
void ofxTerrain::meshAppendX(int segmentLength, int numSegments){

	int newMeshSizeX = meshSize.x + numSegments;

	// setup height map
	//noiseScale = ofMap(ofGetMouseX(), 0 , ofGetWidth(), 0, 0.5);
	for(int x=meshSize.x; x<newMeshSizeX; x++) {
		for(int y=0; y<meshSize.y; y++) {
			switch (mode) {
			case 0:
				{
					float noiseValue = genNoise(x, y);
					heightMap.push_back(noiseValue);
				}
				break;
			case 1:
			case 3:
				{
					float noiseValue = 1-noiseAmp2+genNoise2(x, y); // background noise only
					heightMap.push_back(noiseValue);
				}
				break;
			}
		}
	}

	// setup vertices
	for(int x=meshSize.x; x<newMeshSizeX; x++) {
		for(int y=0; y<meshSize.y; y++) {

			float Y = y * segmentLength;
			float X = x * segmentLength;
			float Z = heightMap[indexFromXY(x, y, meshSize.y)];
			mesh.addVertex(ofVec3f(X,Y,Z * planeZscale));
			mesh.addColor(getColor(Z));
		}
	}

	// setup indexes
	for(int x=meshSize.x-1; x<newMeshSizeX; x++) {
		for (int y = 0; y < meshSize.y; y++) {

			float Y = y * segmentLength;

			// -
			if (x + 1 != newMeshSizeX){
				mesh.addIndex(indexFromXY(x, y, meshSize.y));
				mesh.addIndex(indexFromXY(x+1, y, meshSize.y));
			}

			// |
			if (drawYLines) {
				if (y + 1 != meshSize.y){
					mesh.addIndex(indexFromXY(x, y, meshSize.y));
					mesh.addIndex(indexFromXY(x, y+1, meshSize.y));
				}
			}
		}
	}

	meshSize.x += numSegments;

}

//--------------------------------------------------------------
void ofxTerrain::addHole(int x, int y){

	float nx = ofMap(x, 0, 1280, meshSize.x - planeWidth/planeResolution, meshSize.x);
	holes.push_back(ofVec2f(nx,y/planeResolution));
	holesAmp.push_back(0);
}

//--------------------------------------------------------------
void ofxTerrain::updateHoles(){

	for(int i = 0; i< holes.size(); i++) {

		ofVec2f pos = holes[i];
		float amp = holesAmp[i];
		float ampTween = ofxTween::map(amp, 0, VASA_HOLE_MAX_AMP, 0, VASA_HOLE_MAX_AMP, false, tweenEasing, ofxTween::easeInOut);

		if (amp> VASA_HOLE_MAX_AMP) continue;

		//   (coef d'amplitude)   x  exp^(  (Distance x coef d'ouverture) ^2)
		int xFrom = pos.x;
		int yFrom = pos.y;
		float holeRadius = VASA_HOLE_RADIUS;
		float radius = 2* holeRadius*holeRadius;

		float xMin = ofClamp(xFrom-radius, 0, meshSize.x);
		float xMax = ofClamp(xFrom+radius,0, meshSize.x);
		float xCenter = ofMap(0.5, 0, 1, xMin, xMax);

		float yMin = ofClamp(yFrom-radius, 0, meshSize.y);
		float yMax = ofClamp(yFrom+radius,0, meshSize.y);
		float yCenter = ofMap(0.5, 0, 1, yMin, yMax);

		for (int x = xMin; x < xMax; x++) {
			for (int y = yMin; y < yMax; y++) {
				int idx = indexFromXY(x, y, meshSize.y);
				ofVec3f curVertex = mesh.getVertex(idx);

				float val = ampTween * exp(- ( pow((float)x-xCenter,2)/radius + pow((float)y-yCenter,2)/radius));
				curVertex.z -= val;


				mesh.setVertex(idx, curVertex);
				mesh.setColor(idx, getColor((curVertex.z/planeZscale)));
			}
		}

		holesAmp[i] += 0.05;

	}



}

//--------------------------------------------------------------
void ofxTerrain::addHill(int x, int y, float radius){

	float nx = ofMap(x, 0, 1280, meshSize.x - planeWidth/planeResolution, meshSize.x);
	hills.push_back(ofVec2f(nx,y/planeResolution));
	hillsAmp.push_back(0);
	hillsRadius.push_back(radius);
}

//--------------------------------------------------------------
void ofxTerrain::updateHills(){

	for(int i = 0; i< hills.size(); i++) {

		ofVec2f pos = hills[i];
		float amp = hillsAmp[i];
		float ampTween = ofxTween::map(amp, 0, VASA_HILL_MAX_AMP, 0, VASA_HILL_MAX_AMP, true, tweenEasing, ofxTween::easeInOut);

		if (amp> VASA_HILL_MAX_AMP) continue;

		//   (coef d'amplitude)   x  exp^(  (Distance x coef d'ouverture) ^2)
		int xFrom = pos.x;
		int yFrom = pos.y;
		float hillRadius = hillsRadius[i];
		float radius = hillRadius*hillRadius;

		float xMin = ofClamp(xFrom-hillRadius, 0, meshSize.x);
		float xMax = ofClamp(xFrom+hillRadius,0, meshSize.x);
		float xCenter = ofMap(0.5, 0, 1, xMin, xMax);

		float yMin = ofClamp(yFrom-hillRadius, 0, meshSize.y);
		float yMax = ofClamp(yFrom+hillRadius,0, meshSize.y);
		float yCenter = ofMap(0.5, 0, 1, yMin, yMax);

		for (int x = xMin; x < xMax; x++) {
			for (int y = yMin; y < yMax; y++) {
				int idx = indexFromXY(x, y, meshSize.y);
				ofVec3f curVertex = mesh.getVertex(idx);

				//float val = ampTween * exp(- ( pow((float)x-xCenter,2)/radius + pow((float)y-yCenter,2)/radius));
				float sqrVal = radius - pow((float)x-xCenter,2) - pow((float)y-yCenter,2);
				if (sqrVal>0) {

					curVertex.z += amp * sqrt(sqrVal)/4;
				}




				mesh.setVertex(idx, curVertex);
				mesh.setColor(idx, getColor((curVertex.z/planeZscale)));
			}
		}

		hillsAmp[i] += 0.01;

	}



}

//--------------------------------------------------------------
int ofxTerrain::indexFromXY(int x, int y, int totalHeight){
	return x * totalHeight + y;
}

//--------------------------------------------------------------
void ofxTerrain::update(){

	switch (mode) {
	case 0:
	case 1:
	case 3:
		speedRate+=1/60.;
		double curDelta = speedRate*(planeVelocity);

		if (sumDeltaX > planeResolution){
			meshAppendX(planeResolution,1);
			sumDeltaX = 0;
		}

		sumDeltaX += curDelta-deltaX;
		deltaX = curDelta;

		break;
	}

	updateHoles();
	updateHills();
}

//--------------------------------------------------------------
void ofxTerrain::draw(){

	ofEnableDepthTest();

	ofClear(ofColor::fromHex(0x00008C));

	ofPushMatrix();
	ofTranslate(-deltaX+2.5*speedRate, 0);
	ofSetLineWidth(4);
	mesh.draw();

	ofPopMatrix();

	ofSetColor(ofColor::white); // draw text

	if (bSmallCursor){
		ofSetColor(ofColor::red);
		ofSetLineWidth(2);
		int lw = 2;
		ofLine(mouseX-lw, mouseY,mouseX+lw, mouseY);
		ofLine(mouseX, mouseY-lw,mouseX, mouseY+lw);
	}

	ofDisableDepthTest();
}

//--------------------------------------------------------------
void ofxTerrain::keyPressed(int key) {


	switch(key)
	{
	case ' ': { mode = (mode +1)  % 4;
			  } break;

	case 'h': addHole(800,400); break;
	case 'H': addHill(800,400,15); break;
	case 'j': addHole(ofGetMouseX(),ofGetMouseY()); break;
	case 'J': addHill(ofGetMouseX(),ofGetMouseY(),15); break;
	case 'k': addHill(ofGetMouseX(),ofGetMouseY(),5); break;
	case 'C' : bSmallCursor = !bSmallCursor; break;
	case 'B' : {
		/*ofxPostProcessing postTerrain;
		postTerrain.init(ofGetWidth(), ofGetHeight());
		postTerrain.createPass<FxaaPass>();
		postTerrain.createPass<FxaaPass>();
		postTerrain.createPass<BloomPass>();
		postList[VASA_MODE_TERRAIN] = postTerrain;
		ofxPostProcessing postDalles;
		postDalles.init(ofGetWidth(), ofGetHeight());
		postDalles.createPass<FxaaPass>();
		postDalles.createPass<FxaaPass>();
		postDalles.createPass<BloomPass>();
		postList[VASA_MODE_DALLES] = postDalles;*/
			   } break;
	case 'b' : {
		//ofxPostProcessing postTerrain;
		//postTerrain.init(ofGetWidth(), ofGetHeight());
		//postTerrain.createPass<FxaaPass>();
		//postTerrain.createPass<FxaaPass>();
		////postTerrain.createPass<BloomPass>();
		//postList[VASA_MODE_TERRAIN] = postTerrain;
		//ofxPostProcessing postDalles;
		//postDalles.init(ofGetWidth(), ofGetHeight());
		//postDalles.createPass<FxaaPass>();
		//postDalles.createPass<FxaaPass>();
		////postDalles.createPass<BloomPass>();
		//postList[VASA_MODE_DALLES] = postDalles;
			   } break;

	case 'd': {
		//
		ofApp *app = (ofApp *)ofxGetAppPtr();
		app->cam.setOrientation(ofVec3f(43.1984,0,0));
		app->cam.setPosition(-200, -200.867, 208.106);

			  } break;
            
        case 'm' : bSmallCursor = true; break;
        case 'M' : bSmallCursor=false; break;
            
        case 'r': setup(planeWidth, planeHeight, planeResolution, planeVelocity); break;

	}


}