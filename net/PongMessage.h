#pragma once
#include "Fish.h"
#ifndef PONG_MESSAGE_H

class PongMessage
{
public:
	PongMessage() {  fish = gamelib::Fish("Neemo", "mathews"); }
	const char* Type;
	bool isHappy;
	int eventType;
	std::vector<std::string> names;
	std::vector<int> ages;
	gamelib::Fish fish;
};

#endif