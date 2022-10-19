#pragma once
#include "pch.h"
#include <memory>
#include <string>
#include <Direction.h>

class Room;

class Movement
{
public:
	Movement(float durationMs, gamelib::Direction direction, int maxPixels = 20, bool debug = false);
	bool IsComplete();

	// Calculate the movement in pixels to move for the amount of time that has elapsed
	void Update(float deltaMs);
	unsigned int TakePixelsToMove();
	unsigned int PreviewPixelsToMove();
	unsigned int GetPixelsTraveled();
	unsigned int GetPixelsStillToTravel();
	void SpendPixels();
	gamelib::Direction direction;

	// This is just a way to identify a movement 
	static int id;
private:
	float pixelsPerMs;
	float msPerPixel;
	unsigned int pixelsTraveled;
	float durationMs;
	unsigned int pixelsToMove;
	unsigned int totalTargetMovePixels;
	bool isComplete;
	std::string movementTowardsTargetId;
	bool debug;

};

