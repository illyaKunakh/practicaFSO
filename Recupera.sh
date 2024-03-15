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

# Comprovem si s'ha especificat l'opció -D (DryRun)
if [ "$1" == "-D" ]; then
    # Informem que estem en mode DryRun i sortim
    echo "DryRun mode activat"
    exit 0
fi

# Obtenim l'arxiu comprimit i el directori de destí
fitxer_tgz="$1"
directori_desti="${@: -1}"

# Resta de paràmetres són els fitxers a recuperar
fitxers_a_recuperar="${@:2:$(($#-2))}"

# Comprovem si l'arxiu comprimit existeix
if [ ! -f "$fitxer_tgz" ]; then
    echo "Error: L'arxiu comprimit especificat no existeix."
    exit 1
fi

# Comprovem si el directori de destí existeix
if [ ! -d "$directori_desti" ]; then
    echo "Error: El directori de destinació especificat no existeix."
    exit 1
fi

# Creem un directori temporal
tmp_dir=$(mktemp -d)
tar -xzf "$fitxer_tgz" -C "$tmp_dir"
ls "$tmp_dir"
# Descomprimim l'arxiu a un directori temporal
#if [ "$1" != "-D" ]; then
 #   tar -xzf "$fitxer_tgz" -C "$tmp_dir"
#    if [ $? -ne 0 ]; then
 #       echo "Error: No s'ha pogut descomprimir l'arxiu."
  #      exit 1
  #  fi
#fi
# Descomprimim l'arxiu
#tar -xzvf "$fitxer_tgz" -C "$directori_desti"

# Recuperem els fitxers especificats de l'arxiu comprimit
if [ "$1" != "-D" ]; then
    fitxers_a_recuperar=("${@:2:$(($#-2))}")
    for fitxer in "${fitxers_a_recuperar[@]}"; do
        if [ -e "$tmp_dir/$fitxer" ]; then
            if [ "$1" == "-D" ]; then
                dryrun_echo "Descomprimint $fitxer de $fitxer_tgz a $directori_desti"
            else
                cp -r "$tmp_dir/$fitxer" "$directori_desti"
            fi
        else
            echo "El fitxer $fitxer no es troba a l'arxiu comprimit."
        fi
    done
fi

# Recorrem els arxius recuperats i ens assegurem que afegim duplicats
for file in "${fitxers_a_recuperar[@]}"; do
    # Si existeix un arxiu amb el mateix nom pero amb data afegida, el mantenim
    if [ -e "$directori_desti/$file" ]; then
        mv "$directori_desti/$file" "$directori_desti/${file}$(date +%Y%m%d%H%M%S)"
    fi
done

# Comprovem si la descompressió ha tingut èxit
if [ $? -eq 0 ]; then
    echo "Recuperació finalitzada amb èxit."
else
    echo "Error: S'ha produït un error durant la recuperació."
fi

exit 0