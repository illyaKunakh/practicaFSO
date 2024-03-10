# Copia [-k] [-D] fitxer1 fitxer2 ... fitxer_tgz

#  L'script ha de ser capaç de crear un arxiu comprimit (format .tgz
# o .tar.gz) que contingui tots els fitxers i directoris especificats per
# paràmetre. Si l’arxiu comprimit existeix, ha d’afegir els nous fitxers i
# directoris al fitxer comprimit.

#  Si un fitxer introduït per paràmetre es correspon a un directori, la
# còpia ha de ser recursiva, incloent tots els fitxers i subdirectoris,
# mantenint l'estructura de directoris original, així com els permisos i les
# dates de creació, modificació i accés de tots els fitxers.

#  Opció -K: Si quan es vol afegir un nou fitxer a un arxiu comprimit, aquest
# ja existeix dins del l’arxiu comprimit, es vol que el script conservi els
# dos fitxers afegint al nom del fitxer que volem afegir la data de
# modificació del fitxer que afegim. Es a dir si el fitxer que afegim és:
# /home/milax/p1.c, i ja n’existeix un amb aquest nom, el nom del fitxer
# que emmagatzemarà serà /home/milax/p1.c.20240223, si la data de
# modificació de l’arxiu és el 23 de febrer del 2024.

#  Opció -D: Si hi ha aquesta opció l’script només informarà del que faria pel
# terminal, però sense crear ni modificar l’arxiu comprimit (DryRun).

#  L'script ha de ser flexible per permetre especificar tant rutes absolutes
# com rutes relatives per als fitxers i directoris tant d'entrada com de
# sortida.

utilitzacio() {
    echo "Us: $0 [-k] [-D] arxiu1 arxiu2 ... arxiu_tgz"
    echo "Opcions:"
    echo " -k: Afegeix un nou arxiu conservant l'existent"
    echo " -D: Mostra els canvis que es farien"
    exit 1
}

# Si hi han menys de 2 arguments, es recorda com s'utilitza el script
if [ $# -lt 2 ]; then
    utilitzacio
fi

opcio_k=false
opcio_d=false
dir_temp=$(mktemp -d)

while llegeix_opcio "kD" opt; do
    case $opt in
        k)
            opcio_k=true
            ;;
        D)
            opcio_d=true
            ;;
        *)
            utilitzacio
            ;;
    esac
done
shift $((OPTIND - 1))

#Es capta l'ultim argument
arxiu_sortida="${@: -1}"
#Es capten tots els arguments menys l'ultim
arxius=("${@:1:$#-1}")

# Comprovem si el fitxer de sortida es un .tgz
if [[ $arxiu_sortida != *.tgz ]]; then
    echo "El fixter comprimit indicat no té l'extensio adequada: $arxiu_sortida"
    exit 1
fi

# Tractem els arxius un a un
for arxiu in "${arxius[@]}"; do
    # Si l'argument origen es un arxiu o directori
    if [ -f "$arxiu" -o -d "$arxiu" ]; then
        cp -r "$arxiu" "$dir_temp"
        arxiu_temp="$dir_temp/$(basename "$arxiu")"
        # Si -k, es comprova si el fitxer ja existeix al tgz
        if [ "$opcio_k" = true ] && tar -tf "$arxiu_sortida" | grep -q "$(basename $arxiu_temp)"; then
            mod_date=$(date -r "$arxiu_temp" +%Y%m%d)
            nou_temp="$dir_temp/$(basename "$arxiu_temp")$mod_date"
            mv "$arxiu_temp" "$nou_temp"
            arxiu_temp="$nou_temp"
        fi
        # Si -D, es mostra el que es faria
        if [ "$opcio_d" = true ]; then
            echo "S'haria afegit $arxiu_temp a $arxiu_sortida"
        # Sino, inserta arxiu_temp al nou_arxiu_sortida
        else
            tar -rf "$arxiu_sortida" -C "$dir_temp" "$(basename "$arxiu_temp")"
        fi
    else
        echo "Error: Arxiu o directori incorrecte: $arxiu"
        exit 1
    fi
done

rm -r "$dir_temp"
