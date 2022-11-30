all: data/input_16.in data/input_64.in data/input_128.in data/input_256.in data/input_512.in data/input_1024.in data/input_2048.in data/input_4096.in data/input_8192.in data/input_16384.in  rmm
	 rmm

rmm: main.cpp header/single_thread.h header/multi_thread.h
	g++ -g -ggdb main.cpp -o rmm -I ./header -lpthread -mavx -mavx2

data/generate: data/generate.cpp
	g++ ./data/generate.cpp -o ./data/generate

data/input_16.in: data/generate
	./data/generate 16 

data/input_64.in: data/generate
	./data/generate 64 

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

run: data/input_16.in data/input_64.in data/input_128.in data/input_256.in data/input_512.in data/input_1024.in data/input_2048.in data/input_4096.in data/input_8192.in data/input_16384.in  rmm
	./rmm data/input_16.in
	./rmm data/input_64.in
	./rmm data/input_128.in
	./rmm data/input_256.in
	./rmm data/input_512.in
	./rmm data/input_1024.in
	./rmm data/input_2048.in
	./rmm data/input_4096.in
	./rmm data/input_8192.in
# /./rmm data/input_16384.in

test : data/input_16.in data/input_64.in data/input_128.in data/input_256.in data/input_512.in data/input_1024.in data/input_2048.in data/input_4096.in data/input_8192.in  rmm
	./rmm data/input_16.in
	./rmm data/input_64.in
	./rmm data/input_128.in
	./rmm data/input_256.in
	./rmm data/input_512.in
	./rmm data/input_1024.in
	./rmm data/input_2048.in
	./rmm data/input_4096.in
	./rmm data/input_8192.in
#
clean:
	rm rmm
