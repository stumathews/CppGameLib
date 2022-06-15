#pragma once
#include "pch.h"
#include <memory>
#include <string>

class Room;

class Movement
{
public:
	Movement(float durationMs, std::string movementTargetId, int maxPixels = 20, bool debug = false);
	~Movement();
	bool IsComplete();
	void Update(float deltaMs);
	float TakePixelsToMove();
	float GetPixelsTraveled();
	float GetPixelsStillToTravel();
	void SpendPixels();

	// This is the who the movement is to/for etc
	std::string GetMovementTargetId();

	// This is just a way to identify a movement 
	static int id;
private:
	float pixelsPerMs;
	float pixelsTraveled;
	float durationMs;
	float pixelsToMove;
	float totalTargetMovePixels;
	bool isComplete;
	std::string movementTargetId;
	bool debug;

};

