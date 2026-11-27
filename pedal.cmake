# Sources de la pédale, partagées par le VST3 autonome et le board.
# Chemins calculés depuis CE fichier et normalisés (sans ".."), pour que
# deux pédales qui listent le même fichier FMR produisent la même chaîne.
get_filename_component(_FMR "${CMAKE_CURRENT_LIST_DIR}/../FMR_Library" ABSOLUTE)

list(APPEND PEDAL_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/TimeCookerDSP.cpp
  ${CMAKE_CURRENT_LIST_DIR}/TimeCookerGUI.cpp
  ${_FMR}/FMR_Lib/FMR_DSP_float.cpp
  ${_FMR}/FMR_Modulation/FMR_Chorus.cpp
  ${_FMR}/FMR_Modulation/FMR_Rotary.cpp
  ${_FMR}/FMR_Modulation/FMR_Flanger.cpp
  ${_FMR}/FMR_Modulation/FMR_Phaser.cpp
  ${_FMR}/FMR_Modulation/FMR_Tremolo.cpp
  )
