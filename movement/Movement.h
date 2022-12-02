#pragma once
#include "pch.h"
#include <memory>
#include <string>
#include <Direction.h>
#include "IMovement.h"

class Room;

/// <summary>
/// A movement is defined as a movement in a specified direction, over a time period in which it must reach its destination
/// </summary>
class Movement : public gamelib::IMovement
{
public:
	
	/// <summary>
	///  A movement is defined as a movement in a specified direction, over a time period in which it must reach its destination.
	/// </summary>
	/// <param name="durationMs">How long should this movement take overall to comeplete</param>
	/// <param name="direction">Direction of the movements</param>
	/// <param name="maxPixels">The total distance that the movement should move once its complete, this linearly interpolated over the duration of the move</param>
	/// <param name="debug">log movement info</param>
	/// <returns></returns>
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


public:
	// Inherited via IMovement
	virtual gamelib::Direction GetDirection() override;


	// Inherited via IMovement
	virtual unsigned short GetPixelsToMove() override;

};

