#!/bin/bash
echo "Compilando"
make
M=100
echo "Comprobando que la multiplicación de matrices es correcta usando matrices de $M x $M"
n_tests=10
it=1
while [ $it -le $n_tests ]
do
  ./generate-matrix $M >sample.txt
  for n_thread in 1 2 4 8 16 32
  do
    echo "Corriendo test para $n_thread hilos"
    if [ "$(diff -w <(./simple <sample.txt) <(./multiplication $n_thread 0 1 <sample.txt))" != "" ]
    then
      echo "Error en la multiplicacion"
      exit -1
    fi
    echo "Test superado"
  done
  ((it++))
done

echo "Por ahora terminemos el programa aqui"
rm sample.txt
make clean
exit 0

N=5000
echo "Obteniendo tiempos de ejecuación para 2 matrices de $N x $N"
iterations=10
i=1
while [ $i -le $iterations ]
do
  echo "Iteración $i"
  echo "Generando un caso de prueba"
  ./generate-matrix $N >sample.txt
  for n_thread in 1 2 4 8 16 32
  do
    echo "Para $n_thread threads"
    ./multiplication $n_thread 1 0 <sample.txt >>"./times-obtained/$n_thread-threads.txt"
  done
  ((i++))
done

rm sample.txt
make clean
