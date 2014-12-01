#ifndef EVENT_REGISTER_H
#define EVENT_REGISTER_H

#include <glm/glm.hpp>
#include <KeyboardHandler.h>

using glm::vec2;

class EventRegister{
public:
	EventRegister();
	void toggleLMB();
	void toggleRMB();
	void handleKey(int k, bool down);
	void handleMotion(vec2 ms);
	void clearKeyState();
	int getKeyState(int k);
	vec2 getWorldMouse();
private:
	bool lmb, rmb;
	vec2 worldMouse;
	KeyboardHandler keyHandler;
	friend class Player;
};

#endif
