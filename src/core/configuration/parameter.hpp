#ifndef PARAMETER_HPP
#define PARAMETER_HPP

#include <yaml-cpp/yaml.h>

template<typename T>
class Parameter {
public:
    Parameter(std::string nameVisible, std::string description,
              std::string cliName, char cliNameShort,
              T valueDefault)
    :   m_nameVisible(nameVisible), m_description(description),
        m_cliName(cliName), m_cliNameShort(cliNameShort),
        m_valueDefault(valueDefault) { };
    ~Parameter() {};

    void setCliValue(T value);
    void setConfigValue(T value);

    T getValue();
    T getConfigValue();



private:
    std::string m_nameVisible;
    std::string m_description;

    std::string m_cliName;
    char        m_cliNameShort;

    // All values, in "overriding order"
    T   m_valueDefault;
    T   m_valueConfigFile;
    T   m_valueCliOverride;

    bool m_configFileOverriden;
    bool m_cliOverriden;
};


#endif // PARAMETER_HPP
