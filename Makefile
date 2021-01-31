all: order_gen order_monitor order_band 

order_gen: order_gen.c
	gcc order_gen.c -o order_gen 

order_monitor: order_monitor.c
	gcc order_monitor.c -o order_monitor -lpthread

order_band: order_band.c
	gcc order_band.c -o order_band

clean:
	rm order_gen order_monitor order_band
