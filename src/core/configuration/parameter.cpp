#include "parameter.hpp"


template<typename T>
Parameter<T>::Parameter(
        std::string nameVisible, std::string description,
        std::string cliName, char cliNameShort,
        T valueDefault)
:   m_nameVisible(nameVisible), m_description(description),
    m_cliName(cliName), m_cliNameShort(cliNameShort),
    m_valueDefault(valueDefault) {

}

template<typename T>
void Parameter<T>::setCliValue(T value) {
    m_valueCliOverride = value;
    m_cliOverriden = true;
}

template<typename T>
void Parameter<T>::setConfigValue(T value) {
    m_valueConfigFile = value;
    m_configFileOverriden = true;
}

template<typename T>
T Parameter<T>::getConfigValue() {
    return m_configFileOverriden ?
          m_valueConfigFile
        : m_valueDefault;
}

template<typename T>
T Parameter<T>::getValue() {
    return m_cliOverriden ?
          m_valueCliOverride
        : m_configFileOverriden ?
              m_valueConfigFile
            : m_valueDefault;
}
