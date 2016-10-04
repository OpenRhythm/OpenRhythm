#pragma once
#include <functional>
#include <vector>
#include <type_traits>

#include <SDL.h>
#undef main

#include "keycode.hpp"

namespace ORCore
{

    enum EventType
    {
        EventNone    = 0,
        Quit         = 1 << 0,
        MouseMove    = 1 << 1,
        WindowClose  = 1 << 2,
        WindowSized  = 1 << 3,
        KeyUp        = 1 << 4,
        KeyDown      = 1 << 5,
        EventAll = Quit | MouseMove | WindowClose | WindowSized | KeyUp | KeyDown
    };


    // enable the use of bitwise operators on flags

    struct MouseMoveEvent
    {
        int x;
        int y;
    };

    struct WindowSizeEvent
    {

        Uint32 id;
        int width;
        int height;
    };


    struct WindowCloseEvent
    {
        Uint32 id;
    };

    struct KeyUpEvent
    {
        KeyCode key;
        ModFlag mod;

    };

    struct KeyDownEvent
    {
        KeyCode key;
        ModFlag mod;

    };

    struct QuitEvent
    {
        bool quit;

    };

    union EventMan
    {
        MouseMoveEvent mouseMove;
        WindowSizeEvent windowSized;
        WindowCloseEvent windowClose;
        KeyUpEvent keyUp;
        KeyDownEvent keyDown;
        QuitEvent quit;
        EventMan(MouseMoveEvent mM) :mouseMove(mM) {};
        EventMan(WindowSizeEvent wS) :windowSized(wS) {};
        EventMan(WindowCloseEvent wC) :windowClose(wC) {};
        EventMan(KeyUpEvent kUp) :keyUp(kUp) {};
        EventMan(KeyDownEvent kDn) :keyDown(kDn) {};
        EventMan(QuitEvent q) :quit(q) {};
        EventMan() {};
    };

    struct Event
    {
        EventType type;
        float time;
        EventMan event;
    };

    struct Listener
    {
        int id;
        EventType mask;
        std::function<bool (Event&)> handler;
        // more to come
    };

    class EventManager
    {
    public:
        void add_listener(Listener &listener);
        void remove_listener(Listener &listener);
        void broadcast_event(Event &event);
    private:
        std::vector<Listener*> m_listeners;
    };

    class EventPumpSDL2
    {
    public:
        EventPumpSDL2(EventManager *events);
        void process();
    private:
        EventManager *m_events;
    };
}