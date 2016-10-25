#include "parameter.hpp"


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
