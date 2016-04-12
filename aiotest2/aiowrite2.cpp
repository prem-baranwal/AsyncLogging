#define _GNU_SOURCE       /* syscall() is not POSIX */
 
#include <cstdio>        /* for perror() */
#include <unistd.h>       /* for syscall() */
#include <sys/syscall.h>  /* for __NR_* definitions */
#include <linux/aio_abi.h>    /* for AIO types and constants */
#include <fcntl.h>        /* O_RDWR */
#include <string.h>       /* memset() */
#include <inttypes.h> /* uint64_t */
#include <sstream>
#include <chrono>
#include <iostream>
#include "aioheader.h"

  #define WRITE_BUFFER_LEN 512
  #define DIM_X 524288
  using namespace std;

  int totalWrites = 0;

  inline int io_setup(unsigned nr, aio_context_t *ctxp)
  {
    return syscall(__NR_io_setup, nr, ctxp);
  }
  inline int io_destroy(aio_context_t ctx) 
  {
    return syscall(__NR_io_destroy, ctx);
  }
  inline int io_submit(aio_context_t ctx, long nr,  struct iocb **iocbpp) 
  {
    return syscall(__NR_io_submit, ctx, nr, iocbpp);
  }
  inline int io_getevents(aio_context_t ctx, long min_nr, long max_nr,
        struct io_event *events, struct timespec *timeout)
  {
    return syscall(__NR_io_getevents, ctx, min_nr, max_nr, events, timeout);
  }

int main(int argc, char *argv[])
{
  if(argc < 2){
    cout<< "Usage: aiotest2 < 0(non-blocking) | 1(blocking)>"<<endl;
    exit(0);
  }
  bool bBlocking = atoi(argv[1]) == 0 ? false : true;
  cout<<bBlocking<<endl;

  int fd, ret = 0;
  fd = open("aio2.dat", O_CREAT | O_RDWR |  O_TRUNC | O_APPEND , 0664);
  if (fd < 0) {
      perror("open error");
      return -1;
  }

  aio_context_t ctx;
  struct iocb cb;
  struct iocb *cbs[1];
  if(false == bBlocking){
    asyncsetup(fd, ctx, cb, cbs);
  }

  chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();

  char data[WRITE_BUFFER_LEN];
  for(auto i=0; i < DIM_X; i++){
    //prepare buffer
    prepareBuffer(data, WRITE_BUFFER_LEN, i);
    if(true==bBlocking)
      ret = blockingwrite(fd, data, WRITE_BUFFER_LEN);
    else{
      ret = asyncwrite(ctx, cb, cbs, data, WRITE_BUFFER_LEN);
    }
  }

  if(false == bBlocking){
    struct io_event events[1];
    asynccheckcompletion(ctx, events);
  }

  chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
  cout<<"elapsed "<<chrono::duration_cast<chrono::seconds>( t2 - t1 ).count()<<" seconds"<<endl;
  return ret;
}

int asyncsetup(int& fd, aio_context_t& ctx, struct iocb& cb, struct iocb *cbs[])
{
  int ret = 0;
    //aio_context_t ctx;
    //struct iocb cb;
    //struct iocb *cbs[1];

    ctx = 0;
    ret = io_setup(128, &ctx);
    if (ret < 0) {
        perror("io_setup error");
        return -1;
    }

    /* setup I/O control block */
    memset(&cb, 0, sizeof(cb));
    cb.aio_fildes = fd;
    cb.aio_lio_opcode = IOCB_CMD_PWRITE;
  
    cbs[0] = &cb;

    return ret;
}

int asynccheckcompletion(aio_context_t& ctx, struct io_event events[])
{
    int ret = 0;

    cout<<"getting completion events for : "<<totalWrites<<endl;
    /* get the reply */
    ret = io_getevents(ctx, 20, 20, events, NULL);
    printf("return value: %d\n", ret);

    ret = io_destroy(ctx);
    if (ret < 0) {
        perror("io_destroy error");
        return -1;
    }
    return ret;
}

int asyncwrite(aio_context_t& ctx, struct iocb& cb, struct iocb *cbs[], char buffer[], int totalbufflen)
  {
    //aio_context_t ctx;
    //struct iocb cb;
    //struct iocb *cbs[1];
    
    //struct io_event events[1];
    int ret;
  
    // ctx = 0;
    // ret = io_setup(128, &ctx);
    // if (ret < 0) {
    //     perror("io_setup error");
    //     return -1;
    // }
  
    /* setup I/O control block */
    // memset(&cb, 0, sizeof(cb));
    // cb.aio_fildes = fd;
    // cb.aio_lio_opcode = IOCB_CMD_PWRITE;

    //cbs[0] = &cb;

    do_compute(0, 2048, 2048);

    /* command-specific options */
    cb.aio_buf = (uint64_t)buffer;
    cb.aio_offset = 0;
    cb.aio_nbytes = totalbufflen;
  
    ret = io_submit(ctx, 1, cbs);
    if (ret != 1) {
        if (ret < 0)
            perror("io_submit error");
        else
            fprintf(stderr, "could not sumbit IOs");
        return  -1;
    }
    totalWrites++;

    /* get the reply */
    // ret = io_getevents(ctx, 1, 1, events, NULL);
    // printf("return value: %d\n", ret);

    // ret = io_destroy(ctx);
    // if (ret < 0) {
    //     perror("io_destroy error");
    //     return -1;
    // }
  
    return ret;
  }

  int blockingwrite(int fd, char buffer[], int totalbufflen){
    do_compute(0, 2048, 2048);
    int ret = 0;
    ret = write(fd,(const void *)buffer, totalbufflen);

    return ret;
  }

  int prepareBuffer(char buffer[], int totalbufflen, int idx){
    //prepare data
    std::stringstream sstream;
    sstream << "["<<idx<<"] Some general string for processing data and writing asynchronously in a file\n";
    sstream << "We are trying to use linux kernel APIs for writing info into log file.\n";
    std::string toAppend = sstream.str(); 
    size_t bufflen = toAppend.size();
    if(bufflen < totalbufflen){
      memcpy(buffer, toAppend.c_str(), bufflen);
      memset(buffer+bufflen, 65, totalbufflen-bufflen-1);
    }
    
    buffer[totalbufflen-1]=0;

    return 0;
  }