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

#!/bin/bash

directori_desti="${@: -1}"
fitxers_a_recuperar="${@:2:$#-2}" 

# Comprovem si el directori de destí existeix
if [ ! -d "$directori_desti" ]; then
    echo "Error: El directori de destí especificat no existeix."
    exit 1
fi

# Creem un directori temporal
directori_temp=$(mktemp -d) 

#Si el primer parametre es -D hem de fer DRY RUN
if [ "$1" = "-D" ] 
then
    echo mode DryRun 
    fitxer_tgz="$2"
    echo descomprimim, extreiem fitxers i directoris de "$fitxer_tgz" i els posem a "$directori_desti" 
else #Descomprimim
    fitxer_tgz="$1"
    # Comprovem si l'arxiu comprimit existeix
    if [ ! -f "$fitxer_tgz" ]; then
        echo "Error: L'arxiu comprimit especificat no existeix."
        exit 1
    fi

    tar -xzf "$1" -C "$directori_temp" #extreiem arxius i carpetes a la carptea temporal
    for i in "$fitxers_a_recuperar" 
    do
        recuperats=$(find "$directori_temp" -name "$i" -o -name "$i*" -type f)
        for j in $recuperats #copiem al directori_desti els fitxers/directoris
        do
            cp -r "$j" "$directori_desti"
        done
    done
    echo "Recuperació finalitzada"
fi
rm -rf $directori_temp #esborrem el directori temporal

exit 0


