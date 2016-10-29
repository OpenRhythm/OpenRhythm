#ifndef PARAMETER_HPP
#define PARAMETER_HPP

#include <yaml-cpp/yaml.h>

template<typename T>
class Parameter {
public:
    Parameter(T valueDefault,
            std::string nameVisible, std::string description,
            std::string cliName, char cliNameShort)
    :   m_nameVisible(nameVisible), m_description(description),
        m_cliName(cliName), m_cliNameShort(cliNameShort),
        m_valueDefault(valueDefault) { };
    ~Parameter() {};

    void setCliValue(T value) {
        m_valueCliOverride = value;
        m_cliOverriden = true;
    }
    void setConfigValue(T value) {
        m_valueConfigFile = value;
        m_configFileOverriden = true;
    }

    T getValue() {
        return m_cliOverriden ?
              m_valueCliOverride
            : m_configFileOverriden ?
                  m_valueConfigFile
                : m_valueDefault;
    }
    T getConfigValue() {
        return m_configFileOverriden ?
              m_valueConfigFile
            : m_valueDefault;
    }



private:
    std::string m_nameVisible;
    std::string m_description;

    std::string m_cliName;
    char        m_cliNameShort;

    // All values, in "overriding order"
    T   m_valueDefault;
    T   m_valueConfigFile;
    T   m_valueCliOverride;

    bool m_configFileOverriden = false;
    bool m_cliOverriden = false;
};


#endif // PARAMETER_HPP
