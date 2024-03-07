#Recupera [-D] fitxer_tgz fitxer1 fitxer2 ... directori_destí

#  L'script ha de ser capaç de recuperar els fitxers que se li passen per
# paràmetre des d’un arxiu comprimit (format .tgz o .tar.gz). Si hi ha
# fitxers repetits, on hem afegit una data, aquest fitxer o directori també
# s’ha de recuperar, per tant tindrem dos fitxers amb el mateix nom, un
# sense data i un amb data de modificació del fitxer.

#  Si un fitxer és un directori, la recuperació ha de ser recursiva, incloent
# tots els fitxers i subdirectoris, mantenint l'estructura de directoris
# original, així com els permisos i les dates de creació, modificació i
# accés de tots els fitxers.

#  Opció -D: Si hi ha aquesta opció l’script només informarà del que faria,
# però sense recuperar cap fitxer ni directori (DryRun).