  long do_compute(double A, double B, int arr_len);
  int blockingwrite(int fd, char buffer[], int totalbufflen);
  int asyncwrite(aio_context_t& ctx, struct iocb& cb, struct iocb *cbs[], char buffer[], int totalbufflen);
  int prepareBuffer(char buffer[], int totalbufflen, int idx);
  int asyncsetup(int& fd, aio_context_t& ctx, struct iocb& cb, struct iocb *cbs[]);
  int asynccheckcompletion(aio_context_t& ctx, struct io_event events[]);