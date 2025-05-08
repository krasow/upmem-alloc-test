all: host device

host:
	gcc -O3 --std=c99 -o trivial_checksum_example_host trivial_checksum_example_host.c -g `dpu-pkg-config --cflags --libs dpu`

device:
	dpu-upmem-dpurte-clang trivial_checksum_example.c -o trivial_checksum_example

clean:
	rm trivial_checksum_example_host trivial_checksum_example
