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

while getopts "kD" opt; do
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
arxiu_sortida="${arxiu_sortida}.tgz"
#Es capten tots els arguments menys l'ultim
arxius=("${@:1:$#-1}")


if [ "$opcio_d" = false ]; then
    # Si fitxer de sortida existeix, el descomprimim
    if [ -f "$arxiu_sortida" ]; then
        existeix_tgz=true
        tar -xzf "$arxiu_sortida" -C "$dir_temp"
    else 
        existeix_tgz=false
    fi
fi

if [ "$opcio_k" = true ] && [ $existeix_tgz = false ]; then
    echo "Error: No s'ha trobat el fitxer $arxiu_sortida, es procedeix a fer una insercio normal (no -k)"
    opcio_k=false
fi
# Tractem els arxius un a un
for arxiu in "${arxius[@]}"; do
    # Si l'argument origen es un arxiu o directori
    if [ -f "$arxiu" -o -d "$arxiu" ]; then
        if [ "$opcio_k" = true ] && ! find "$dir_temp" -name "$(basename "$arxiu")" -print -quit | grep -q '^'; then
            echo "Error: No s'ha trobat l'arxiu $arxiu a $arxiu_sortida, es procedeix a fer una insercio normal (no -k)"
            opcio_k=false
        fi
        # Si no es pasen opcions com argument 
        if [ "$opcio_k" = false ] && [ "$opcio_d" = false ]; then
            # Es guarda el directori pare d'"arxiu"
            mkdir -p "$dir_temp/$(dirname "$arxiu")"
            cp -r "$arxiu" "$dir_temp/$arxiu" 
        # Si -k
        elif [ "$opcio_k" = true ]; then
            if [ "$opcio_d" = true ]; then
                echo "S'afegeix l'extensio .tgz a l'arxiu de sortida"
                echo "Si aquest arxiu comprimit  $arxiu_sortida ja existeix, es descomprimiria el seu contingut a $dir_temp"
                echo "Es contena la data de ultima modificacio de $arxiu tal que l'arxiu amb la data seria $arxiu.$(date -r "$arxiu" +"%Y%m%d") "
                echo "Es crea un directori en una ruta que combina $dir_temp i el directori pare de $arxiu.$(date -r "$arxiu" +"%Y%m%d")"
                echo "Es fa una copia recursiva de $arxiu.$(date -r "$arxiu" +"%Y%m%d") a $dir_temp/$arxiu.$(date -r "$arxiu" +"%Y%m%d")"
            else
                # Es concatena la data al arxiu
                data_mod=$(date -r "$arxiu" +"%Y%m%d")
                nou="$arxiu.$data_mod"
                mkdir -p "$dir_temp/$(dirname "$nou")"
                cp -r "$arxiu" "$dir_temp/$nou"
            fi
        # Si -D
        elif [ "$opcio_d" = true ]; then
            echo "S'afegeix l'extensio .tgz a l'arxiu de sortida"
            echo "Si aquest arxiu comprimit  $arxiu_sortida ja existeix, es descomprimiria el seu contingut a $dir_temp"
            echo "Es crea un directori en una ruta que combina $dir_temp i el directori pare de $arxiu"
            echo "Es fa una copia recursiva de $arxiu a $dir_temp/$arxiu"
            echo "Es comprimeix l'arxiu"
            echo "S'hauria afegit $arxiu a $arxiu_sortida"
        fi
    else
        echo "Error: Arxiu o directori incorrecte: $arxiu"
        exit 1
    fi
done

if [ "$opcio_d" = false ]; then
# Es comprimeix l'arxiu
tar -czvf "$arxiu_sortida" -C "$dir_temp" .
fi
rm -r "$dir_temp"
