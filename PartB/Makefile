sim: data/input_128.in cmm_sim

server: data/input_128.in cmm_server

cmm_server: main.cu header/gpu_thread.h
	nvcc main.cu -o cmm_server -I ./header

cmm_sim: main.cu header/gpu_thread.h
	nvcc main.cu -o cmm_sim -I ./header --cudart shared -gencode arch=compute_70,code=compute_70

data/generate: data/generate.cpp
	g++ ./data/generate.cpp -o ./data/generate

data/input_128.in: data/generate
	./data/generate 128

data/input_256.in: data/generate
	./data/generate 256

data/input_512.in: data/generate
	./data/generate 512

data/input_1024.in: data/generate
	./data/generate 1024

data/input_2048.in: data/generate
	./data/generate 2048

data/input_4096.in: data/generate
	./data/generate 4096

data/input_8192.in: data/generate
	./data/generate 8192

data/input_16384.in: data/generate
	./data/generate 16384


run_sim: data/input_128.in cmm_sim
	./cmm_sim data/input_128.in

run_server: data/input_128.in cmm_server
	./cmm_server data/input_128.in

all : data/input_1024.in data/input_128.in data/input_256.in data/input_512.in data/input_2048.in data/input_4096.in data/input_8192.in data/input_16384.in cmm_server 
	./cmm_server data/input_128.in
	./cmm_server data/input_256.in
	./cmm_server data/input_512.in
	./cmm_server data/input_1024.in
	./cmm_server data/input_2048.in
	./cmm_server data/input_4096.in
	./cmm_server data/input_8192.in
	./cmm_server data/input_16384.in

counters : cmm_server  data/input_1024.in data/input_128.in data/input_256.in data/input_512.in data/input_2048.in data/input_4096.in data/input_8192.in  
	 ./cmm_server data/input_128.in
	 ./cmm_server data/input_256.in
	 ./cmm_server data/input_512.in
	 ./cmm_server data/input_1024.in
	 ./cmm_server data/input_2048.in
	 ./cmm_server data/input_4096.in
	 ./cmm_server data/input_8192.in
	
clean:
	rm -f cmm_sim cmm_server
