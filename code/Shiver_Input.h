/* date = June 17th 2024 4:42 am */

#ifndef _SHIVER__INPUT_H
#define _SHIVER__INPUT_H

#include "Intrinsics.h"
#include "util/Math.h"

enum KeyCodeID 
{ 
    KEY_NONE,
    KEY_LEFT_MOUSE,
    KEY_MIDDLE_MOUSE,
    KEY_RIGHT_MOUSE,
    
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
    KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
    KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
    
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    
    KEY_SPACE,
    KEY_TICK,
    KEY_MINUS,
    KEY_EQUAL,
    KEY_LEFT_BRACKET,
    KEY_RIGHT_BRACKET,
    KEY_SEMICOLON,
    KEY_QUOTE,
    KEY_COMMA,
    KEY_PERIOD,
    KEY_FORWARD_SLASH,
    KEY_BACKWARD_SLASH,
    KEY_TAB,
    KEY_ESCAPE,
    KEY_PAUSE,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_BACKSPACE,
    KEY_RETURN,
    KEY_DELETE,
    KEY_INSERT,
    KEY_HOME,
    KEY_END,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,
    KEY_CAPS_LOCK,
    KEY_NUM_LOCK,
    KEY_SCROLL_LOCK,
    KEY_MENU,
    KEY_SHIFT,
    KEY_CONTROL,
    KEY_ALT,
    KEY_COMMAND,
    
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, 
    KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
    
    KEY_NUMPAD_0,
    KEY_NUMPAD_1,
    KEY_NUMPAD_2,
    KEY_NUMPAD_3,
    KEY_NUMPAD_4,
    KEY_NUMPAD_5,
    KEY_NUMPAD_6,
    KEY_NUMPAD_7,
    KEY_NUMPAD_8,
    KEY_NUMPAD_9,
    
    KEY_NUMPAD_STAR,
    KEY_NUMPAD_PLUS,
    KEY_NUMPAD_MINUS,
    KEY_NUMPAD_DOT,
    KEY_NUMPAD_SLASH,
    
    KEY_COUNT = 256,
};

enum KeyBindings 
{
    BINDING_NONE,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    ATTACK,
    BINDING_COUNT,
};

struct Key 
{
    bool JustPressed;
    bool JustReleased;
    bool IsDown;
    uint8 HalfTransitionCount;
};

struct Keymapping 
{
    KeyCodeID MainKey;
    KeyCodeID AltKey;
};

struct KeyboardInput 
{
    ivec2 LastMouse;
    ivec2 CurrentMouse;
    ivec2 MouseDiff;
    
    Key Keys[KEY_COUNT];
    Keymapping Bindings[BINDING_COUNT];
};

struct Input 
{
    KeyboardInput Keyboard;
};

internal inline bool
IsKeyDown(KeyCodeID KeyCode, Input *GameInput) 
{
    Key Key = GameInput->Keyboard.Keys[KeyCode];
    if(Key.IsDown) 
    {return(1);
    }
    return(0);
}

internal inline bool
IsGameKeyDown(KeyBindings InputType, Input *GameInput) 
{
    KeyCodeID Keycode = GameInput->Keyboard.Bindings[InputType].MainKey;
    Key Key = GameInput->Keyboard.Keys[Keycode];
    if(Key.IsDown) 
    {
        return(1);
    }
    return(0);
}
#endif //_SHIVER__INPUT_H
