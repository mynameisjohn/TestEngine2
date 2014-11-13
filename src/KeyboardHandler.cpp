#include "KeyboardHandler.h"

KeyboardHandler::KeyboardHandler()
: keyState(KEYCOUNT)
{}

int KeyboardHandler::getKeyState(int k){
	return keyState[k & 0xFF];
}

void KeyboardHandler::handleKey(int k){
	k &= 0xFF;
	keyState[k] = !keyState[k];
}
