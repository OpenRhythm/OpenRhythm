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
        WindowSize  = 1 << 3,
        KeyUp        = 1 << 4,
        KeyDown      = 1 << 5,
        EventAll = Quit | MouseMove | WindowClose | WindowSize | KeyUp | KeyDown
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

    // This is functionally works similar to boost::any but customized to the event system.
    // Basically the idea is to use polymorphism and templates to store any type of data
    // in the same object. We use this idea to deliver many different types of event structs
    // within a single object.
    // Also since this has a lot of template stuff in it, I decided to put the entire implementation
    // in the header.
    struct Event
    {
        EventType type;
        float time;
        template<class Type> friend
        Type event_cast(const Event&);

        Event(Event&& other)
        : ptr(std::move(other.ptr))
        {
            time = other.time;
            type = other.type;
            other.time = 0.0;
            other.type = EventNone;
        }

        Event(const Event& other) {
            if (other.ptr) {
                ptr = other.ptr->clone();
            }
            time = other.time;
            type = other.type;
        }

        Event& operator=(Event&& other) {
            ptr = std::move(other.ptr);
            time = other.time;
            type = other.type;
            other.time = 0.0;
            other.type = EventNone;
            return *this;
        }

        Event& operator=(const Event& other) {
            ptr =  std::move(Event(other).ptr);
            return *this;
        }

        Event()
        : type(EventNone), time(0.0), ptr(nullptr)
        {}

        template<typename T>
        Event(EventType eType, float eTime, const T& event)
        : type(eType), time(eTime), ptr(new Concrete<T>(event))
        {
        }
    private:
        struct Placeholder
        {
            virtual ~Placeholder() {};
            virtual std::unique_ptr<Placeholder> clone() = 0;
        };

        template<typename T>
        struct Concrete : public Placeholder
        {
            Concrete(T&& x)
            : value(std::move(x))
            {}

            Concrete(const T& x)
            : value(x)
            {}


            virtual std::unique_ptr<Placeholder> clone()
            {
                return std::make_unique<Concrete>(value);
            }

            T value;
        };

        std::unique_ptr<Placeholder> ptr;
    };

    // Used to get the event data out of the event.
    template<typename T>
    T event_cast(const Event& val) {
        return static_cast<Event::Concrete<T>*>(val.ptr.get())->value;
    }

    struct Listener
    {
        int id;
        EventType mask;
        std::function<bool (const Event&)> handler;
        // more to come
    };

    class EventManager
    {
    public:
        void add_listener(Listener &listener);
        void remove_listener(Listener &listener);
        void broadcast_event(const Event &event);
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

} // namespace ORCore
