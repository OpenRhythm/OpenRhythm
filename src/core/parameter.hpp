#pragma once
#include <yaml-cpp/yaml.h>

namespace ORCore {

    // Tbh this template is pretty pointless now.
    // What this will likely be replaced with is a config class
    // Which uses std::any to store several types of values
    // This should be FAR better imo than using a string for the same thing as i've seen others do.
    template<typename T>
    class Parameter
    {
    public:
        Parameter(T valueDefault, std::string name, std::string description)
        :m_name(name),
        m_description(description),
        m_value(valueDefault),
        m_valueDefault(valueDefault)
        {
        };

        std::string get_name()
        {
            return m_name;
        }

        std::string get_description()
        {
            return m_description;
        }

        // Used to set the value from the config file
        void set_value(T value)
        {
            m_valueConfigFile = value;
        }

        // Get the currently used/set value
        T get_value()
        {
            return m_value;
        }

    private:
        std::string m_name;
        std::string m_description;
        T m_value;
        T m_valueDefault;
    };

} // namespace ORGame
