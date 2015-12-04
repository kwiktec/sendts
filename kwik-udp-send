/*
Copyright by Michael Kormeev 2015
*/
#define MULTICAST

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#define TS_PACKET_SIZE 188	
#define PKT_ACCUMUL_NUM 10000
char                *dir;
unsigned char*      send_buf;
pthread_mutex_t     c_mutex = PTHREAD_MUTEX_INITIALIZER;
unsigned int        snd_pkt_num;       //the size of ts packets, that sended per one operation
unsigned int        packet_size;       //the size of udp packet, that sended per one operation
unsigned char       *cache_buf;        //the cache buffer
unsigned int        pkt_full;          //the quantity of all packets in the chache buffer
unsigned int        pkt_num = 0;       //the quanitity of filled packets in the chache buffer
unsigned int        start_pkt = 0;     //the number of the first packet in cache buffer where is the buffer is filled 
unsigned int        last_pkt = 0;      //the number of the last packet in cache buffer where is the buffer is filled 
unsigned int        file_fin = 0;      //file reading finished
int                 bNoMoreFile = 0;
int                 transport_fd = 0;
int                 sockfd;
struct timespec     nano_sleep_packet;
struct sockaddr_in  addr;

void process_file(char *tsfile){
     unsigned char temp_buf[TS_PACKET_SIZE];
     unsigned int len;
     transport_fd = open(tsfile, O_RDONLY);
     printf("processing: %s\r\n", tsfile);
     if(transport_fd < 0) { 
       fprintf(stderr, "couldn't open file: %s\n", tsfile);
       return;   
     }
     unsigned int read_bytes = 0;  //number of bytes that read from file
     for(;;){         
         if(pkt_num + 1 >= pkt_full){
            //the chache buffer is full
            printf("cache is full\r\n");
            nanosleep(&nano_sleep_packet, 0);
            continue;
         }
         len = read(transport_fd, temp_buf, TS_PACKET_SIZE);
         if(len == 0){
            //file reading completed
            close(transport_fd);
            printf("processed: %s\r\n", tsfile);
            printf("pkt_num: %d\r\n", pkt_num);
            break;            
         }
         if(len < TS_PACKET_SIZE){
            fprintf(stderr, "read < TS_PACKET_SIZE while reading: %s\n", tsfile);            
         }
         read_bytes += len;
         pthread_mutex_lock( &c_mutex );
         unsigned char *src_buf = cache_buf + last_pkt * TS_PACKET_SIZE;
         memcpy(src_buf, temp_buf, TS_PACKET_SIZE);
         last_pkt++;
         pkt_num++;
         if(last_pkt == pkt_full)last_pkt = 0;
         pthread_mutex_unlock( &c_mutex );        
         nanosleep(&nano_sleep_packet, 0);
     }
     

}
void *reading_thread( void *ptr ){
    DIR            *FD;
    struct stat     statbuf;
    struct dirent  *in_file;
    struct tm      *tmd;
    time_t         min_time = 0;                   //minimal played time
    time_t         srch_min_time = 0;              //minimal time in the current iteration of directory scanning
    char   *dir_buf = malloc(strlen(dir) + 100);
    char   *cur_buf = malloc(strlen(dir) + 100);   //name buffer for the current file
    /* Scanning the in directory */
    if (NULL == (FD = opendir (dir))) 
    {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        return 1;
     }
     for(;;){
          while ((in_file = readdir(FD))) 
          {
             /* On linux/Unix we don't want current and parent directories
             * On windows machine too, thanks Greg Hewgill
             */
             if(strcmp(in_file->d_name, ".") == 0)continue;
             if(strcmp(in_file->d_name, "..") == 0)continue;           
             sprintf(dir_buf, "%s%s", dir, in_file->d_name);
             stat(dir_buf, &statbuf);
             if(srch_min_time == 0 || (statbuf.st_mtime < srch_min_time && statbuf.st_mtime > min_time)){
                srch_min_time = statbuf.st_mtime;                
                strcpy(cur_buf, dir_buf);
             }
          }
          if(min_time == srch_min_time){
             bNoMoreFile = 1;
             printf("no files in directory\r\n");
             sleep(1);
             continue;
          }
          bNoMoreFile = 0;
          //играем наименьший по времени файл
          process_file(cur_buf);
          min_time = srch_min_time;
     }
}
long long int usecDiff(struct timespec* time_stop, struct timespec* time_start)
{
	long long int temp = 0;
	long long int utemp = 0;
		   
	if (time_stop && time_start) {
		if (time_stop->tv_nsec >= time_start->tv_nsec) {
			utemp = time_stop->tv_nsec - time_start->tv_nsec;    
			temp = time_stop->tv_sec - time_start->tv_sec;
		} else {
			utemp = time_stop->tv_nsec + 1000000000 - time_start->tv_nsec;       
			temp = time_stop->tv_sec - 1 - time_start->tv_sec;
		}
		if (temp >= 0 && utemp >= 0) {
			temp = (temp * 1000000000) + utemp;
        	} else {
			fprintf(stderr, "start time %ld.%ld is after stop time %ld.%ld\n", time_start->tv_sec, time_start->tv_nsec, time_stop->tv_sec, time_stop->tv_nsec);
			temp = -1;
		}
	} else {
		fprintf(stderr, "memory is garbaged?\n");
		temp = -1;
	}
        return temp / 1000;
}
void SendEmptyPacket(){
     int i;
     unsigned char *buf = send_buf;
     for(i=0; i<snd_pkt_num; i++){
         //memset(buf, 0x00, TS_PACKET_SIZE);
         buf[0] = 0x47;
         ((unsigned short*)(buf+1))[0] = 0x1fff;
         buf[1] = 0x1F;
         buf[2] = 0xFF;
         buf[3] = 0x10;
         buf += TS_PACKET_SIZE;
     }
    // printf("sending empty paket\r\n");
     int sent = sendto(sockfd, send_buf, packet_size, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
     if(sent <= 0)perror("send(): error ");
}

void SendPacket(){
     unsigned long long int packet_size2 = 0;
     int i;
     unsigned char *dst_buf = send_buf;
     for(i=0; i<snd_pkt_num; i++){
         pthread_mutex_lock( &c_mutex );
         unsigned char *src_buf = cache_buf + start_pkt * TS_PACKET_SIZE;
         memcpy(dst_buf, src_buf, TS_PACKET_SIZE);
         printf("sending paket: %d\r\n", pkt_num);
         packet_size2 += TS_PACKET_SIZE;
         pkt_num--;
         start_pkt++;
         if(start_pkt == pkt_full){
            start_pkt = 0;
         }
         if(pkt_num <= 0)break;
         pthread_mutex_unlock( &c_mutex );
         dst_buf += TS_PACKET_SIZE;         
     }
     int sent = sendto(sockfd, send_buf, packet_size2, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
     if(sent <= 0)perror("send(): error ");
}
int main (int argc, char *argv[]) {     
     pthread_t thread1, thread2;
     unsigned long long int packet_time = 0;
     unsigned long long int real_time = 0;    
     unsigned int bitrate;
     struct timespec time_start;
     struct timespec time_stop;
     memset(&addr, 0, sizeof(addr));
     memset(&time_start, 0, sizeof(time_start));
     memset(&time_stop, 0, sizeof(time_stop));
     memset(&nano_sleep_packet, 0, sizeof(nano_sleep_packet));

     if(argc < 4 ) {
	fprintf(stderr, "Usage: %s directory ipaddr port bitrate\n", argv[0]);
	fprintf(stderr, "ts_packet_per_ip_packet default is 7\n");
	fprintf(stderr, "bit rate refers to transport stream bit rate\n");
	fprintf(stderr, "zero bitrate is 100.000.000 bps\n");
	return 0;
    } else {
	dir = argv[1];
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv[2]);
	addr.sin_port = htons(atoi(argv[3]));
	bitrate = atoi(argv[4]);
	if (bitrate <= 0) {
	    bitrate = 100000000;
	}
	if (argc >= 6) {
            snd_pkt_num = strtoul(argv[6], 0, 0);
	    packet_size = snd_pkt_num * TS_PACKET_SIZE;
	} else {
	    packet_size = 7 * TS_PACKET_SIZE;
            snd_pkt_num = 7;
	}
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
	perror("socket(): error ");
	return 0;
    }
     pkt_full = 30000;
     cache_buf = malloc(pkt_full * TS_PACKET_SIZE);
     send_buf = malloc(packet_size);


     int rt = pthread_create( &thread1, NULL, reading_thread, (void*) 2);
     if(rt)
     {
         fprintf(stderr,"Error - pthread_create(reading_thread) return code: %d\n",rt);
         exit(EXIT_FAILURE);
     }
     int   bFirst = 1;
     nano_sleep_packet.tv_nsec = 665778; /* 1 packet at 100mbps*/
     clock_gettime(CLOCK_MONOTONIC, &time_start);
     for(;;){      
         clock_gettime(CLOCK_MONOTONIC, &time_stop);
         real_time = usecDiff(&time_stop, &time_start);
         while(real_time * bitrate > packet_time * 1000000){
               if((pkt_num >= PKT_ACCUMUL_NUM || bNoMoreFile == 1) && pkt_num > 0){
                  SendPacket();
                  packet_time += packet_size * 8;
                  continue;
               }                       
               SendEmptyPacket();
               packet_time += packet_size * 8;
         }//while(real_time * bitrate > packet_time * 1000000)
         nanosleep(&nano_sleep_packet, 0);
     }//for(;;)
     close(sockfd);
     free(send_buf);
     return 0; 
}
