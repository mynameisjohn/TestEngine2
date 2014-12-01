#include <EventRegister.h>

EventRegister::EventRegister()
: lmb(0), rmb(0), worldMouse(0,0), keyHandler()
{}

void EventRegister::toggleLMB(){
	lmb = !lmb;
}

void EventRegister::toggleRMB(){
	rmb = !rmb;
}

void EventRegister::handleKey(int k, bool down){
	keyHandler.handleKey(k, down);
}

void EventRegister::handleMotion(vec2 ms){
	worldMouse = ms;
}

void EventRegister::clearKeyState(){
	keyHandler.clear();
}

int EventRegister::getKeyState(int k){
	return keyHandler.getKeyState(k);
}

vec2 EventRegister::getWorldMouse(){
	return worldMouse;
}
