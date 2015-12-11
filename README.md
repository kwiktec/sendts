# Kwikflix UDP send
## kwik-udp-send

### Features

*Sending ts file(s) as a ts udp stream

*If there is no files to send, it sens the null packet

*Works with real-time process/threads priorities to provide stability of the stream

*Works with fifo files

### Build

Just run make

### Command-line options

* -i address - ip address to send packets

* -p port - port to send packets

* -b bitrate - bitrate of stream

* -d path_to_diectory - send all files from a directory, sorted my modified time, the directory is scanned in real-time, you can add files on fly

* -f path_to file - send one file, you have to select either a file or a directory

* --ts_in_cache number, -s number - commons cache size

* --accumul_ts number, -a number - size of filled cache part

* --ttl number, -t number - set up ttl

* --pri number, -P number - set up the process/thread priority

* --ts_in_udp number, -u number - the number of ts packets in one udp packet

* -c - don't stop file reading if it read zero bytes, use it for FIFO files

* -m - print debugging messages on the screen

* -l file_name - save debugging messages to a file 




