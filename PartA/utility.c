// Some utility functions and structures common to all the programs
#define _GNU_SOURCE
#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <inttypes.h>


/*
 * Generalized hardware cache events:
 *
 *       { L1-D, L1-I, LLC, ITLB, DTLB, BPU, NODE } x
 *       { read, write, prefetch } x
 *       { accesses, misses }
 */

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags)
{
    int ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                    group_fd, flags);
    return ret;
}

 // events 1. L1d cache miss 2. LLC miss 3.TLB miss 4.Page fault 
struct event_data{
    struct perf_event_attr current_event;
    int fd;
    long long count;
    // long long total;
}l1_cache,l1_total,llc,llc_total,data_TLB,data_TLB_total,inst_TLB,inst_TLB_total,page_faults,total_cycles;


int initialize_L1_cache_event(struct event_data event,int flag)
{
    memset(&event,0,sizeof(struct event_data)) ;

    struct perf_event_attr l1_cache_event = event.current_event;
    memset(&l1_cache_event,0,sizeof(struct perf_event_attr)) ;
    l1_cache_event.type = PERF_TYPE_HW_CACHE;
    l1_cache_event.size = sizeof(struct perf_event_attr) ;
    if(flag==1)
    l1_cache_event.config = PERF_COUNT_HW_CACHE_L1D |
               PERF_COUNT_HW_CACHE_OP_READ << 8 |
               PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16;
    else
    l1_cache_event.config = PERF_COUNT_HW_CACHE_L1D |
               PERF_COUNT_HW_CACHE_OP_READ << 8 |
               PERF_COUNT_HW_CACHE_RESULT_MISS << 16;
    // l1_cache_event.exclude_user = 1;
    l1_cache_event.disabled = 1;
    l1_cache_event.exclude_kernel = 1;
    l1_cache_event.exclude_hv = 1;

    event.fd = perf_event_open(&l1_cache_event, 0, -1, -1, 0);
    if (event.fd == -1) {
        fprintf(stderr, "Error opening leader %llx. There exists some problem with L1 cache flags\n", l1_cache_event.config);
        exit(EXIT_FAILURE);
    }
    ioctl(event.fd, PERF_EVENT_IOC_RESET, 0);
    return event.fd;
    // ioctl(event->fd, PERF_EVENT_IOC_ENABLE, 0);
}

int initialize_LLC_event(struct event_data event,int flag)
{
    memset(&event,0,sizeof(struct event_data)) ;

    struct perf_event_attr llc_cache_event = event.current_event;
    memset(&llc_cache_event,0,sizeof(struct perf_event_attr)) ;
    llc_cache_event.type = PERF_TYPE_HW_CACHE;
    llc_cache_event.size = sizeof(struct perf_event_attr) ;
    if(flag==1)
    llc_cache_event.config = PERF_COUNT_HW_CACHE_LL |
               PERF_COUNT_HW_CACHE_OP_READ << 8 |
               PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16;
    else
    llc_cache_event.config = PERF_COUNT_HW_CACHE_LL |
               PERF_COUNT_HW_CACHE_OP_READ << 8 |
               PERF_COUNT_HW_CACHE_RESULT_MISS << 16;

    llc_cache_event.disabled = 1;
    llc_cache_event.exclude_kernel = 1;
    llc_cache_event.exclude_hv = 1;

    event.fd = perf_event_open(&llc_cache_event, 0, -1, -1, 0);
    if (event.fd == -1) {
        fprintf(stderr, "Error opening leader %llx. There exists some problem with LLC flags\n", llc_cache_event.config);
        exit(EXIT_FAILURE);
    }
    ioctl(event.fd, PERF_EVENT_IOC_RESET, 0);
    return event.fd;
    // ioctl(event->fd, PERF_EVENT_IOC_ENABLE, 0);
}

int initialize_PF_event(struct event_data event)
{
    memset(&event,0,sizeof(struct event_data)) ;

    struct perf_event_attr pf_event = event.current_event;
    memset(&pf_event,0,sizeof(struct perf_event_attr)) ;
    pf_event.type = PERF_TYPE_SOFTWARE;
    pf_event.size = sizeof(struct perf_event_attr) ;
    pf_event.config = PERF_COUNT_SW_PAGE_FAULTS;

    pf_event.disabled = 1;
    pf_event.exclude_kernel = 1;
    pf_event.exclude_hv = 1;

    event.fd = perf_event_open(&pf_event, 0, -1, -1, 0);
    if (event.fd == -1) {
        fprintf(stderr, "Error opening leader %llx. There exists some problem with Page Fault flags\n", pf_event.config);
        exit(EXIT_FAILURE);
    }
    ioctl(event.fd, PERF_EVENT_IOC_RESET, 0);
    return event.fd;

}
