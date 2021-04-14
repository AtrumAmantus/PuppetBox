#ifdef _USE_GLFW

#pragma once

#include <GLFW/glfw3.h>

#include "AbstractInputProcessor.h"
#include "KeyCode.h"
#include "Logger.h"
#include "WindowProperties.h"

#define MAX_KEY_CODES		120
#define MAX_MOUSE_CODES		8

class KeyRegister
{
public:
	static const int SIZE = MAX_KEY_CODES;
public:
	KeyRegister() : lastIndex(-1), keys{} {};
	void registerKey(const int key) { keys[++lastIndex] = key; };
	const short int* getKeys() const { return keys; };
private:
	short int lastIndex;		//Tracks last registered index for registering key codes
	short int keys[SIZE];		//Stores key codes sequentially for fast iteration
};

class GlfwInputProcessor : public AbstractInputProcessor
{
public:
	GlfwInputProcessor(GLFWwindow& window) : window_(window) {};
	void init(WindowProperties* windowProperties)
	{
		windowProperties_ = windowProperties;
		initKeyboardState();
	}
	void loadCurrentState()
	{
		if (windowProperties_ != nullptr)
		{
			window.windowClose = glfwWindowShouldClose(&window_);

			// Store previous state
			for (int i = 0; i < KEY_LAST + 1; ++i)
			{
				keyboard.previousKeyState[i] = keyboard.keyState[i];
			}

			// Get current state for all registerd keys
			const short int* keyMap = keyRegister.getKeys();

			for (int i = 0; i < keyRegister.SIZE; ++i)
			{
				keyboard.keyState[keyMap[i]] = (glfwGetKey(&window_, keyMap[i]) != GLFW_RELEASE);
			}

			mouse.x = windowProperties_->mouse.x;
			mouse.y = windowProperties_->mouse.y;
			mouse.deltaX = windowProperties_->mouse.deltaX;
			mouse.deltaY = windowProperties_->mouse.deltaY;
			window.width = windowProperties_->width;
			window.height = windowProperties_->height;
			window.newWidth = windowProperties_->newWidth;
			window.newHeight = windowProperties_->newHeight;
			windowProperties_->resetInputs();
		}
		else
		{
			LOGGER_ERROR("GlfwInputProcess was not initialized with init()");
		}
	}
private:
	WindowProperties* windowProperties_ = nullptr;
	KeyRegister keyRegister{};
	GLFWwindow& window_;
private:
	void initKeyboardState()
	{
		/* Printable keys */
		keyRegister.registerKey(KEY_SPACE);
		keyRegister.registerKey(KEY_APOSTROPHE);	/* ' */
		keyRegister.registerKey(KEY_COMMA);			/* , */
		keyRegister.registerKey(KEY_MINUS);			/* - */
		keyRegister.registerKey(KEY_PERIOD);		/* . */
		keyRegister.registerKey(KEY_SLASH);			/* / */
		keyRegister.registerKey(KEY_0);
		keyRegister.registerKey(KEY_1);
		keyRegister.registerKey(KEY_2);
		keyRegister.registerKey(KEY_3);
		keyRegister.registerKey(KEY_4);
		keyRegister.registerKey(KEY_5);
		keyRegister.registerKey(KEY_6);
		keyRegister.registerKey(KEY_7);
		keyRegister.registerKey(KEY_8);
		keyRegister.registerKey(KEY_9);
		keyRegister.registerKey(KEY_SEMICOLON);		/* ; */
		keyRegister.registerKey(KEY_EQUAL);			/* = */
		keyRegister.registerKey(KEY_A);
		keyRegister.registerKey(KEY_B);
		keyRegister.registerKey(KEY_C);
		keyRegister.registerKey(KEY_D);
		keyRegister.registerKey(KEY_E);
		keyRegister.registerKey(KEY_F);
		keyRegister.registerKey(KEY_G);
		keyRegister.registerKey(KEY_H);
		keyRegister.registerKey(KEY_I);
		keyRegister.registerKey(KEY_J);
		keyRegister.registerKey(KEY_K);
		keyRegister.registerKey(KEY_L);
		keyRegister.registerKey(KEY_M);
		keyRegister.registerKey(KEY_N);
		keyRegister.registerKey(KEY_O);
		keyRegister.registerKey(KEY_P);
		keyRegister.registerKey(KEY_Q);
		keyRegister.registerKey(KEY_R);
		keyRegister.registerKey(KEY_S);
		keyRegister.registerKey(KEY_T);
		keyRegister.registerKey(KEY_U);
		keyRegister.registerKey(KEY_V);
		keyRegister.registerKey(KEY_W);
		keyRegister.registerKey(KEY_X);
		keyRegister.registerKey(KEY_Y);
		keyRegister.registerKey(KEY_Z);
		keyRegister.registerKey(KEY_LEFT_BRACKET);  /* [ */
		keyRegister.registerKey(KEY_BACKSLASH);		/* \ */
		keyRegister.registerKey(KEY_RIGHT_BRACKET);	/* ] */
		keyRegister.registerKey(KEY_GRAVE_ACCENT);	/* ` */
		keyRegister.registerKey(KEY_WORLD_1);		/* non-US #1 */
		keyRegister.registerKey(KEY_WORLD_2);		/* non-US #2 */

		/* Function keys */
		keyRegister.registerKey(KEY_ESCAPE);
		keyRegister.registerKey(KEY_ENTER);
		keyRegister.registerKey(KEY_TAB);
		keyRegister.registerKey(KEY_BACKSPACE);
		keyRegister.registerKey(KEY_INSERT);
		keyRegister.registerKey(KEY_DELETE);
		keyRegister.registerKey(KEY_RIGHT);
		keyRegister.registerKey(KEY_LEFT);
		keyRegister.registerKey(KEY_DOWN);
		keyRegister.registerKey(KEY_UP);
		keyRegister.registerKey(KEY_PAGE_UP);
		keyRegister.registerKey(KEY_PAGE_DOWN);
		keyRegister.registerKey(KEY_HOME);
		keyRegister.registerKey(KEY_END);
		keyRegister.registerKey(KEY_CAPS_LOCK);
		keyRegister.registerKey(KEY_SCROLL_LOCK);
		keyRegister.registerKey(KEY_NUM_LOCK);
		keyRegister.registerKey(KEY_PRINT_SCREEN);
		keyRegister.registerKey(KEY_PAUSE);
		keyRegister.registerKey(KEY_F1);
		keyRegister.registerKey(KEY_F2);
		keyRegister.registerKey(KEY_F3);
		keyRegister.registerKey(KEY_F4);
		keyRegister.registerKey(KEY_F5);
		keyRegister.registerKey(KEY_F6);
		keyRegister.registerKey(KEY_F7);
		keyRegister.registerKey(KEY_F8);
		keyRegister.registerKey(KEY_F9);
		keyRegister.registerKey(KEY_F10);
		keyRegister.registerKey(KEY_F11);
		keyRegister.registerKey(KEY_F12);
		keyRegister.registerKey(KEY_F13);
		keyRegister.registerKey(KEY_F14);
		keyRegister.registerKey(KEY_F15);
		keyRegister.registerKey(KEY_F16);
		keyRegister.registerKey(KEY_F17);
		keyRegister.registerKey(KEY_F18);
		keyRegister.registerKey(KEY_F19);
		keyRegister.registerKey(KEY_F20);
		keyRegister.registerKey(KEY_F21);
		keyRegister.registerKey(KEY_F22);
		keyRegister.registerKey(KEY_F23);
		keyRegister.registerKey(KEY_F24);
		keyRegister.registerKey(KEY_F25);
		keyRegister.registerKey(KEY_KP_0);
		keyRegister.registerKey(KEY_KP_1);
		keyRegister.registerKey(KEY_KP_2);
		keyRegister.registerKey(KEY_KP_3);
		keyRegister.registerKey(KEY_KP_4);
		keyRegister.registerKey(KEY_KP_5);
		keyRegister.registerKey(KEY_KP_6);
		keyRegister.registerKey(KEY_KP_7);
		keyRegister.registerKey(KEY_KP_8);
		keyRegister.registerKey(KEY_KP_9);
		keyRegister.registerKey(KEY_KP_DECIMAL);
		keyRegister.registerKey(KEY_KP_DIVIDE);
		keyRegister.registerKey(KEY_KP_MULTIPLY);
		keyRegister.registerKey(KEY_KP_SUBTRACT);
		keyRegister.registerKey(KEY_KP_ADD);
		keyRegister.registerKey(KEY_KP_ENTER);
		keyRegister.registerKey(KEY_KP_EQUAL);
		keyRegister.registerKey(KEY_LEFT_SHIFT);
		keyRegister.registerKey(KEY_LEFT_CONTROL);
		keyRegister.registerKey(KEY_LEFT_ALT);
		keyRegister.registerKey(KEY_LEFT_SUPER);
		keyRegister.registerKey(KEY_RIGHT_SHIFT);
		keyRegister.registerKey(KEY_RIGHT_CONTROL);
		keyRegister.registerKey(KEY_RIGHT_ALT);
		keyRegister.registerKey(KEY_RIGHT_SUPER);
		keyRegister.registerKey(KEY_MENU);
	}
};

#endif //_USE_GLFW