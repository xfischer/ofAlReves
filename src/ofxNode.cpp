//
//  ofxNode.cpp
//  ofAlReves
//
//  Created by Xavier Fischer on 13/03/2017.
//
//

#include "ofxNode.h"


// ------ constructors ------

ofxNode::ofxNode(float theX, float theY, bool isLocked) : ofVec3f(theX, theY)
, minX(std::numeric_limits<float>::min())
, maxX(std::numeric_limits<float>::max())
, minY(std::numeric_limits<float>::min())
, maxY(std::numeric_limits<float>::max())
, minZ(std::numeric_limits<float>::min())
, maxZ(std::numeric_limits<float>::max())
, velocity(new ofVec3f(0))
, previousVelocity(ofVec3f(0))
, maxVelocity(OFXNODE_VELOCITY)
, damping(OFXNODE_DAMPING)
, radius(OFXNODE_RADIUS)
, strength(OFXNODE_STRENGTH)
, ramp(OFXNODE_RAMP)
, locked(isLocked)
{}



// ---------------- implementation
// ------ calculate attraction ------
void ofxNode::attract(vector<ofxNode*> & theNodes) {
	// attraction or repulsion part
	for (int i = 0; i < theNodes.size(); i++)
	{
		ofxNode *otherNode = theNodes[i];
		// not with itself
		if (otherNode->id == id)
			continue;
		
		this->attract(otherNode);
	}
}
void ofxNode::attract(vector<ofxNode*> & theNodes, const ofVec3f& repulsionCenter, float repulsionStrength, float repulsionRadius) {
	// attraction or repulsion part
	for (int i = 0; i < theNodes.size(); i++) {
		ofxNode *otherNode = theNodes[i];
		// not with itself
		if (otherNode->id == id)
			continue;
		
		this->attract(otherNode);
	}
	
	repulsionForce = ofVec3f(0);
	if (!this->locked) {
		float d = this->distance(repulsionCenter);
		if (d > 0 && d < repulsionRadius) {
			float s = pow(d / repulsionRadius, 1 / ramp);
			float f = s * 9.f * repulsionStrength * (1.f / (s + 1.f) + ((s - 3.f) / 4.f)) / d;
			repulsionForce = (ofVec3f)*this - repulsionCenter;
			repulsionForce *= f;
			this->velocity->x += repulsionForce.x;
			this->velocity->y += repulsionForce.y;
			this->velocity->z += repulsionForce.z;
		}
	}
}

void ofxNode::attract(ofxNode* theNode) {
	float d = this->distance((ofVec3f)(*theNode));
	
	if (d > 0 && d < radius) {
		float s = pow(d / radius, 1 / ramp);
		float f = s * 9.f * strength * (1.f / (s + 1.f) + ((s - 3.f) / 4.f)) / d;
		ofVec3f df = (ofVec3f)*this - (ofVec3f)(*theNode);
		df *= f;
		
		if (theNode->locked) {
			this->velocity->x -= df.x;
			this->velocity->y -= df.y;
			this->velocity->z -= df.z;
		}
		else {
			theNode->velocity->x += df.x;
			theNode->velocity->y += df.y;
			theNode->velocity->z += df.z;
		}
		
	}
}


void ofxNode::setPosition(float theX, float theY)
{
	x = theX;
	y = theY;
}



void ofxNode::update(float gravity, bool theLockX, bool theLockY, bool theLockZ)
{
	if (!locked) {
		this->velocity->y += gravity;
	}
	
	update(theLockX, theLockY, theLockZ);
}
void ofxNode::update(bool theLockX, bool theLockY, bool theLockZ)
{
	
	velocity->limit(maxVelocity);
	
	
	//float da = velocity->angle(previousVelocity);
	//if (!isnan(da)) {
	//	da = abs(180-da);
	//	////da = PApplet.pow(da, 4);
	//	if (da < 1 || (da >85 && da <89))
	//	{
	//		cout << da << endl;
	
	//		da = 0;
	//		////PApplet.println(id + ", " + da);
	//		*(velocity) *= 0;
	//	}
	//}
	
	if (!theLockX) x += velocity->x;
	if (!theLockY) y += velocity->y;
	if (!theLockZ) z += velocity->z;
	
	if (x < minX) {
		x = minX - (x - minX);
		velocity->x = -velocity->x;
	}
	if (x > maxX) {
		x = maxX - (x - maxX);
		velocity->x = -velocity->x;
	}
	
	if (y < minY) {
		y = minY - (y - minY);
		velocity->y = -velocity->y;
	}
	if (y > maxY) {
		y = maxY - (y - maxY);
		velocity->y = -velocity->y;
	}
	
	if (z < minZ) {
		z = minZ - (z - minZ);
		velocity->z = -velocity->z;
	}
	if (z > maxZ) {
		z = maxZ - (z - maxZ);
		velocity->z = -velocity->z;
	}
	
	// x = PApplet.constrain(x, minX, maxX);
	// y = PApplet.constrain(y, minY, maxY);
	// z = PApplet.constrain(z, minZ, maxZ);
	
	velocity->x = velocity->x * (1 - damping);
	velocity->y = velocity->y * (1 - damping);
	velocity->z = velocity->z * (1 - damping);
	
	previousVelocity = *velocity;
}

void ofxNode::setBoundary(float theMinX, float theMinY, float theMinZ,
								 float theMaxX, float theMaxY, float theMaxZ) {
	minX = theMinX;
	maxX = theMaxX;
	minY = theMinY;
	maxY = theMaxY;
	minZ = theMinZ;
	maxZ = theMaxZ;
}

void ofxNode::setBoundary(float theMinX, float theMinY, float theMaxX,
								 float theMaxY) {
	minX = theMinX;
	maxX = theMaxX;
	minY = theMinY;
	maxY = theMaxY;
}