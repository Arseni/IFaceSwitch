
#ifndef BUTTONS_H
#define BUTTONS_H

typedef enum
{
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_SEL
}tButton;

typedef void (* tButtonCallback) (tButton btn);

#endif
