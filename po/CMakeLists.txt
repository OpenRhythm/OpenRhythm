string(REPLACE ";" "\n"                  TRANSLATION_SOURCE "${ALL_SOURCE}")
string(REPLACE "${CMAKE_SOURCE_DIR}/" "" TRANSLATION_SOURCE "${TRANSLATION_SOURCE}")

GettextTranslate()
