// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include <unordered_map>
#include <typeindex>
#include <memory>

namespace ORCore
{

    class Resolver
    {
    public:

        template<typename T>
        static void set(T& service)
        {

            auto key = std::type_index(typeid(T));
            auto value = new Concrete<T>(&service);

            // Construct the unique_ptr in place:
            // TODO - Can use unordered_map::try_emplace in C++17
            auto result = m_services.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(key),
                std::forward_as_tuple(value));

            // if the key already exists we need to overwrite the value
            if (!result.second)
            {
                m_services[key] = std::unique_ptr<Placeholder>(value);
            }
        }

        template<typename T>
        static T& get()
        {
            // We will not catch the std::out_of_range exception, and now we can return a reference.
            Placeholder* ptr = m_services.at(std::type_index(typeid(T))).get();
            return *(static_cast<Concrete<T>*>(ptr)->ptr);
        }

    private:
        struct Placeholder
        {
            virtual ~Placeholder() {};
        };

        template<typename T>
        struct Concrete : public Placeholder
        {
            Concrete(T* x)
            : ptr(x)
            {}

            T* ptr;
        };
        static std::unordered_map<std::type_index, std::unique_ptr<Placeholder>> m_services;
    };

}