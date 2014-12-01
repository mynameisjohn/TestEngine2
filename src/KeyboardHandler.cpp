#include <KeyboardHandler.h>

KeyboardHandler::KeyboardHandler()
: keyState(KEYCOUNT)
{}

int KeyboardHandler::getKeyState(int k){
	return keyState[k & 0xFF];
}

void KeyboardHandler::handleKey(int k, bool down){
	k &= 0xFF;
	keyState[k] = down;//!keyState[k];
}

void KeyboardHandler::clear(){
	for (std::vector<bool>::iterator it=keyState.begin(); it!=keyState.end(); it++)
		*it = false;
}
