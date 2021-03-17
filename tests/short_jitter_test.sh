#!/bin/bash

# cleanup d'un test précédent
rm -f received_file

# On lance le simulateur de lien avec 10% de pertes et un délais de 50ms
#../link_sim -p 1341 -P 2456 -l 10 -d 50 -R  &> link.log &
../link_sim -p 1341 -P 2456 -d 50 -j 20 -R &> link.log &
link_pid=$!

# On lance le receiver et capture sa sortie standard
../receiver :: 2456 1>received_file 2> receiver.log&
receiver_pid=$!

cleanup()
{
    echo cleanup
    kill -9 $receiver_pid
    kill -9 $link_pid
    exit 0
}
trap cleanup SIGINT  # Kill les process en arrière plan en cas de ^-C

# On démarre le transfert
if ! ../sender :: 1341 < test.txt 2> sender.log ; then
  echo "Crash du sender!"
  cat sender.log
  err=1  # On enregistre l'erreur
fi

<<<<<<< HEAD
sleep 30 # On attend 30 seconde que le receiver finisse
=======
echo "sleep ..."
sleep 10 # On attend 30 seconde que le receiver finisse
>>>>>>> c8cb6ca70274b567367f04e05783f0aa7838e24f

if kill -0 $receiver_pid &> /dev/null ; then
  echo "Le receiver ne s'est pas arreté à la fin du transfert!"
  kill -9 $receiver_pid
  err=1
else  # On teste la valeur de retour du receiver
  if ! wait $receiver_pid ; then
    echo "Crash du receiver!"
    cat receiver.log
    err=1
  fi
fi

# On arrête le simulateur de lien
kill -9 $link_pid &2> /dev/null

# On vérifie que le transfert s'est bien déroulé
if [[ "$(md5sum test.txt | awk '{print $1}')" != "$(md5sum received_file | awk '{print $1}')" ]]; then
  echo "Le transfert a corrompu le fichier!"
  echo "Diff binaire des deux fichiers: (attendu vs produit)"
  diff -C 9 <(od -Ax -t x1z  test.txt) <(od -Ax -t x1z received_file)
  exit 1
else
  echo "Le transfert est réussi!"
  exit ${err:-0}  # En cas d'erreurs avant, on renvoie le code d'erreur
fi
