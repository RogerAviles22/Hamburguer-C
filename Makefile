all: order_gen order_monitor  

order_gen: order_gen.c
	gcc order_gen.c -o order_gen 

order_monitor: order_monitor.c
	gcc order_monitor.c -o order_monitor 

clean:
	rm order_gen order_monitor
