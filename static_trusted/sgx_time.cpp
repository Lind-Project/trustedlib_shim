#include "sgx_time.h"
#include "shim_t.h"

int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    int ret;

    if (ocall_clock_gettime(&ret, clock_id, (void *)tp, sizeof(struct timespec))
        != SGX_SUCCESS) {
        return -1;
    }

    return ret;
}