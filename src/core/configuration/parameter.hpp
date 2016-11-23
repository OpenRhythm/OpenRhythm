#pragma once
#include <yaml-cpp/yaml.h>

namespace ORCore {

    template<typename T>
    class Parameter {
    public:
        Parameter(T valueDefault,
                std::string nameVisible,
                std::string description,
                std::string cliName,
                std::string cliNameShort)
        :   m_nameVisible(nameVisible), m_description(description),
            m_cliName(cliName), m_cliNameShort(cliNameShort),
            m_valueDefault(valueDefault) { };
        ~Parameter() {};

        std::string getName() {
            return m_nameVisible;
        }
        std::string getDescription() {
            return m_description;
        }
        std::string getCliName() {
            return m_cliName;
        }
        std::string getCliNameShort() {
            return m_cliNameShort;
        }

        // Used to set the value from the command line
        void setCliValue(T value) {
            m_valueCommandLine = value;
            m_setFromCommandLine = true;
        }

        // Used to set the value from the config file
        void setConfigValue(T value) {
            m_valueConfigFile = value;
            m_setFromConfigFile = true;
        }

        // Used to set the value from the interface
        void setInterfaceValue(T value) {
            m_valueInterface = value;
            m_setFromInterface = true;
        }


        // Get the currently used/set value
        T getValue() {
            if (m_setFromInterface)
                return m_valueInterface;

            if (m_setFromCommandLine)
                return m_valueCommandLine;

            if (m_setFromConfigFile)
                return m_valueConfigFile;

            return m_valueDefault;
        }

        // Get the value to save in the config file
        T getConfigValue() {
            if (m_setFromInterface)
                return m_valueInterface;

            if (m_setFromConfigFile)
                return m_valueConfigFile;

            return m_valueDefault;
        }

        friend YAML::Emitter &operator<<(YAML::Emitter& output, Parameter &P) {
            return output << P.getConfigValue();
        }

    private:
        std::string m_nameVisible;
        std::string m_description;

        std::string m_cliName;
        std::string m_cliNameShort;

        // All values, in "overriding order"
        T   m_valueDefault;
        T   m_valueCommandLine;
        T   m_valueConfigFile;
        T   m_valueInterface;

        bool m_setFromCommandLine = false,
             m_setFromConfigFile  = false,
             m_setFromInterface   = false;
    };

} // namespace ORGame
