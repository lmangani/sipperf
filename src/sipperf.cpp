#include <pthread.h>
#include <string.h>
#include <string>
#include <stdint.h>
#include <unistd.h>
#include <re.h>
#include <vector>

#include "sipua.hpp"
#include "stack.hpp"


struct tmr base_timer;

static int count = 0;
static std::vector<SIPUE*> ues;

static void cleanup()
{

    for (SIPUE* a : ues)
    {
        delete a;
    }

    close_sip_stacks(false);
}

static void timer_fn(void* arg)
{
//    printf("Called on timer!\n");
    count++;
    if (count < 10000)
    {
        tmr_start(&base_timer, 10, timer_fn, NULL);
        SIPUE* a = new SIPUE("sip:127.0.0.1",
                             "sip:1234@127.0.0.1",
                             "1234@127.0.0.1",
                             "secret");
        a->register_ue();
        ues.push_back(a);
    }
    else
    {
        cleanup();
    }
}

int main(int argc, char *argv[])
{
    int err; /* errno return values */

    /* initialize libre state */
    err = libre_init();
    fd_setsize(50000);
    if (err) {
        re_fprintf(stderr, "re init failed: %s\n", strerror(err));
    }

    re_init_timer_heap();
    
    tmr_init(&base_timer);
    tmr_start(&base_timer, 0, timer_fn, NULL);

    create_sip_stacks(20);
    re_main(NULL);
    printf("End of loop\n");
    
    free_sip_stacks();
    libre_close();

    /* check for memory leaks */
    //tmr_debug();
    mem_debug();

    return 0;
}