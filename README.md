# Proyecto: Burguer-Making Machine V2 
Aplicación donde simula la venta de hamburguesas a travès de una máqina automatizada usando hilos como bandas.

## Compilación
Para compilar, hacemos uso de:
```
make
```
Se generarán 3 ejecutables
```
./order_monitor
```
```
./order_ger
```
```
./order_band
```
Con ellos simularemos el procesamiento de órdenes de hamburguesas. Primero se ejecuta order_monitor (server).

## Uso para order_monitor
Aquí se administra los recursos guardados de la cantidad de dispensadores de ingredientes, despacho de órdenes por bandas y muestra mensajes de alerta cuando haya dispensador vacío.
```
./order_monitor
```
## Uso para order_gen
Genera y envía y órdenes
```
./order_monitor
```
## Uso para order_band
Administra y suministra las bandas. Funcionalidad implementada pero falta que el server lo lea en paralelo.
```
./order_band <option> <number>
```
Tiene las siguientes acciones:
```
Mostrar mensaje de ayuda>> ./order_band -h
Mostrar el estado de las bandas>> ./order_band -s
Pausar banda indicando el número de la banda>> ./order_band -p [0-2]
Reanudar banda indicando el número de la banda>> ./order_band -r [0-2]
Dar mantenimiento a los dispensadores indicando el número de la banda> ./order_band -m [0-2] 
```

## Nota emergencia
En caso de "Error in binding.", ejecutar la siguiente lìnea:
```
ps -ef | grep order
```
Buscamos el procesos "order_monitor" y lo eliminamos con:
```
kill -9 <number_process>
```