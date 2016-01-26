#include <stdio.h>

char *HelpMsg =  "sendts <options>\n"
                 "\nOptions:\n"
                 "-i <address> - ip address to send packets\n"
                 "-p <port> - port to send packets\n"
                 "-b <bitrate> - bitrate of stream\n"
                 "-d <path to directory> - send all files from a directory, sorted my modified time, the directory is scanned in real-time, you can add files on fly\n"
                 "-f <path to file> - send one file, you have to select either a file or a directory\n"	
                 "--ts_in_cache <number>, -s <number> - common cache size, you can use K to set the size in kilobytes and M in megabytes\n"	
                 "--accumul_ts <number>, -a <number> - size of filled cache part, you can use K to set the size in kilobytes and M in megabytes\n"
                 "--ttl <number>, -t <number> - set ttl\n"
                 "--pri <number>, -P <number> - set the process/thread priority\n"
                 "--ts_in_udp <number>, -u <number> - the number of ts packets in one udp packet\n"
                 "-c - don't stop file reading if it read zero bytes, use it for FIFO files\n"
                 "-m - print debugging messages on the screen\n"
                 "-l <file name> - save debugging messages to a file\n" 
                 "-D <seconds> - print the state of buffer in some delays (in seconds)\n"
                 "-F <file size> - set a minimal file size that we're waiting for, you can use K for kilobytes and M for megabytes\n" 
                 "-M - set a minimal file size, based on accumulation buffer size (accumuluation size + 25% of it) \n"
                 "-v[0-3] - verbosity level\n"
                 "\nVerbosity:\n"
                 "-v0 - prints nothing\n"
                 "-v1 - prints minimal information about processing\n"
                 "-v2 - prints information about buffer state with one second delay\n"
                 "-v3 - prints debugging messages\n";


void PrintHelp(){
     printf(HelpMsg);
}
