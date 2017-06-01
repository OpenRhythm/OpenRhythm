#pragma once
#include <yaml-cpp/yaml.h>

namespace ORCore {

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

        std::string getName()
        {
            return m_name;
        }

        std::string getDescription()
        {
            return m_description;
        }

        // Used to set the value from the config file
        void setConfigValue(T value)
        {
            m_valueConfigFile = value;
            m_setFromConfigFile = true;
        }

        // Get the currently used/set value
        T getValue()
        {
            return m_value;
        }

        // Get the value to save in the config file
        T getConfigValue()
        {

            return m_value;
        }

    private:
        std::string m_name;
        std::string m_description;
        T m_value;
        T m_valueDefault;

        bool m_setFromCommandLine = false,
             m_setFromConfigFile  = false,
             m_setFromInterface   = false;
    };

} // namespace ORGame
