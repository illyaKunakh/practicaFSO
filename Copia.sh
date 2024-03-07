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
