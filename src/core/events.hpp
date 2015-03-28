#pragma once
#include <functional>
#include <vector>
#include <type_traits>

#include "SDL.h"
#undef main

#include "keycode.hpp"

namespace MgCore
{
    enum class EventType;

    using underlying = std::underlying_type<EventType>::type;

    static EventType operator|(const EventType& left, const EventType& right)
    {
        return static_cast<EventType>(static_cast<underlying>(left) | static_cast<underlying>(right));
    }

    static EventType operator&(const EventType& left, const EventType& right)
    {
        return static_cast<EventType>(static_cast<underlying>(left) & static_cast<underlying>(right));
    }

    enum class EventType
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

    class Events
    {
    private:
        std::vector<Listener*> m_listeners;
        Event m_event;
    public:
        Events();
        void add_listener(Listener &listener);
        void remove_listener(Listener &listener);
        void broadcast_event(Event &event);
        void process();
    };
}