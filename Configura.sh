# Configura dia hora script parametres

#  Configureu la còpia de seguretat per ser executada automàticament
# de forma periòdica, utilitzant la comanda crontab. Aquesta no s’ha
# explicat a classe, heu de buscar informació de com s’usa.

#  El script ha de ser programat per executar la comanda especificada
# amb els seus paràmetres, cada dia a l'hora especificada també com a
# paràmetre.

# Comprova que s'han passat els arguments correctes
if [ $# -lt 4 ]; then
   echo "Us: $0 day hour script params"
    exit 1
fi

# Extreu els arguments
day=$1
hour=$2
script=$3
params=${@:4}

# Comprueba rang de dia
if [ $day -lt 1 ] -o [ $day -gt 31 ]; then
    echo "Error: Dia incorrecte"
    exit 1s

# Comprova rand d'hora
if [ $hour -lt 0 ] -o [ $hour -gt 23 ]; then
    echo "Error: Hora incorrecta"
    exit 1
fi

comanda="* $hour $day * * $script $params"

# Afegeix la comanda al crontab
(crontab -l; echo "$comanda") | crontab -

echo "Script executat correctament, la comanda s'executarà cada dia $day a les $hour hores."
crontab -l

exit 0
