/*
 * iperf, Copyright (c) 2014, The Regents of the University of
 * California, through Lawrence Berkeley National Laboratory (subject
 * to receipt of any required approvals from the U.S. Dept. of
 * Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE.  This software is owned by the U.S. Department of Energy.
 * As such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * This code is distributed under a BSD style license, see the LICENSE
 * file for complete information.
 */
#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
//#include <sys/select.h>
//#include <sys/uio.h>
#include <lwip/inet.h>   // <arpa/inet.h>

#include "iperf.h"
#include "iperf_api.h"
#include "iperf_util.h"
#include "iperf_locale.h"
#include "net.h"
#include "timer.h"
#include "iperf_timer.h"


int
iperf_create_streams(struct iperf_test *test)
{
    int i, s;
    struct iperf_stream *sp = NULL;

    for (i = 0; i < test->num_streams; ++i) {

        if ((s = test->protocol->_connect(test)) < 0)
            return -1;

	if (test->sender)
	    FD_SET(s, &test->write_set);
	else
	    FD_SET(s, &test->read_set);
	if (s > test->max_fd) test->max_fd = s;

        sp = iperf_new_stream(test, s);
        if (!sp)
            return -1;

        /* Perform the new stream callback */
        if (test->on_new_stream)
            test->on_new_stream(sp);
    }

    return 0;
}

static void
test_timer_proc(TimerClientData client_data, struct timeval *nowP)
{
#if IPERF3_DEBUG
    printf("[iperf_test][test_timer_proc], duration timeout!");
#endif

    struct iperf_test *test = client_data.p;

    test->timer = NULL;
    test->done = 1;
}

static void
client_stats_timer_proc(TimerClientData client_data, struct timeval *nowP)
{
    struct iperf_test *test = client_data.p;

    if (test->done)
        return;
    if (test->stats_callback)
	test->stats_callback(test);
}

static void
client_reporter_timer_proc(TimerClientData client_data, struct timeval *nowP)
{
    struct iperf_test *test = client_data.p;

    if (test->done)
        return;
    if (test->reporter_callback)
	test->reporter_callback(test);
}

static int
create_client_timers(struct iperf_test * test)
{
    struct timeval now;
    TimerClientData cd;

#if IPERF3_DEBUG
    printf("[iperf_test][create_client_timers], duration = %d, stats_timer = %d, reporter_timer = %d\n", test->duration, (int)test->stats_interval, (int)test->reporter_interval);
#endif

    if (gettimeofday(&now, NULL) < 0) {
	i_errno = IEINITTEST;
	return -1;
    }
    cd.p = test;
    test->timer = test->stats_timer = test->reporter_timer = NULL;

    if (test->duration != 0) {
	test->done = 0;
        test->timer = tmr_create(&now, test_timer_proc, cd, ( test->duration + test->omit ) * SEC_TO_US, 0);
        if (test->timer == NULL) {
            i_errno = IEINITTEST;
            return -1;
	}
    } 
    if (test->stats_interval != 0) {
        test->stats_timer = tmr_create(&now, client_stats_timer_proc, cd, test->stats_interval * SEC_TO_US, 1);
        if (test->stats_timer == NULL) {
            i_errno = IEINITTEST;
            return -1;
	}
    }
    if (test->reporter_interval != 0) {
        test->reporter_timer = tmr_create(&now, client_reporter_timer_proc, cd, test->reporter_interval * SEC_TO_US, 1);
        if (test->reporter_timer == NULL) {
            i_errno = IEINITTEST;
            return -1;
	}
    }
    return 0;
}

static void
client_omit_timer_proc(TimerClientData client_data, struct timeval *nowP)
{
    struct iperf_test *test = client_data.p;

    test->omit_timer = NULL;
    test->omitting = 0;
    iperf_reset_stats(test);
    if (test->verbose && !test->json_output && test->reporter_interval == 0)
        iperf_printf(test, "%s", report_omit_done);

    /* Reset the timers. */
    if (test->stats_timer != NULL)
        tmr_reset(nowP, test->stats_timer);
    if (test->reporter_timer != NULL)
        tmr_reset(nowP, test->reporter_timer);
}

static int
create_client_omit_timer(struct iperf_test * test)
{
    struct timeval now;
    TimerClientData cd;

    if (test->omit == 0) {
	test->omit_timer = NULL;
        test->omitting = 0;
    } else {
	if (gettimeofday(&now, NULL) < 0) {
	    i_errno = IEINITTEST;
	    return -1;
	}
	test->omitting = 1;
	cd.p = test;
	test->omit_timer = tmr_create(&now, client_omit_timer_proc, cd, test->omit * SEC_TO_US, 0);
	if (test->omit_timer == NULL) {
	    i_errno = IEINITTEST;
	    return -1;
	}
    }
    return 0;
}

int
iperf_handle_message_client(struct iperf_test *test)
{
    int rval;
    int32_t err;

#if IPERF3_DEBUG
    printf("[iperf_test][handle_message_client]begin.\n");
#endif

    /*!!! Why is this read() and not Nread()? */
#if IPERF3_DEBUG
    printf("[iperf_test][handle_message_client]ctrl_sck = %d, state = %d .\n", 
        (int)test->ctrl_sck, (int)test->state);
#endif

    if ((rval = read(test->ctrl_sck, (char*) &test->state, sizeof(signed char))) <= 0) {
        if (rval == 0) {
            i_errno = IECTRLCLOSE;
#if IPERF3_DEBUG
            printf("[iperf_test][handle_message_client]ERROR: IECTRLCLOSE.\n");
#endif
            return -1;
        } else {
#if IPERF3_DEBUG
            printf("[iperf_test][handle_message_client]ERROR: IERECVMESSAGE.\n");
#endif
            i_errno = IERECVMESSAGE;
            return -1;
        }
    }

#if IPERF3_DEBUG
    printf("[iperf_test][handle_message_client]check test state = %d\n", (int)test->state);
#endif

    switch (test->state) {

        case PARAM_EXCHANGE:
            if (iperf_exchange_parameters(test) < 0){
#if IPERF3_DEBUG
                    printf("[iperf_test][handle_message_client]PARAM_EXCHANGE ERROR: 1.\n");
#endif
                    return -1;
                }
            if (test->on_connect)
                test->on_connect(test);
            break;

        case CREATE_STREAMS:
            if (iperf_create_streams(test) < 0){
#if IPERF3_DEBUG
                    printf("[iperf_test][handle_message_client]CREATE_STREAMS ERROR: 1.\n");
#endif
                    return -1;
                }
            break;

        case TEST_START:
            if (iperf_init_test(test) < 0){
#if IPERF3_DEBUG
                    printf("[iperf_test][handle_message_client]TEST_START ERROR: 1.\n");
#endif
                    return -1;
                }
            if (create_client_timers(test) < 0){
#if IPERF3_DEBUG
                    printf("[iperf_test][handle_message_client]TEST_START ERROR: 2.\n");
#endif
                    return -1;
                }
            if (create_client_omit_timer(test) < 0){
#if IPERF3_DEBUG
                    printf("[iperf_test][handle_message_client]TEST_START ERROR: 3.\n");
#endif
                    return -1;
                }
	    if (!test->reverse)
		if (iperf_create_send_timers(test) < 0){
#if IPERF3_DEBUG
                    printf("[iperf_test][handle_message_client]TEST_START ERROR: 4.\n");
#endif
                    return -1;
                }
            break;

        case TEST_RUNNING:
            break;
        case EXCHANGE_RESULTS:
            if (iperf_exchange_results(test) < 0){
#if IPERF3_DEBUG
                    printf("[iperf_test][handle_message_client]EXCHANGE_RESULTS ERROR: 1.\n");
#endif
                    return -1;
                }
            break;
        case DISPLAY_RESULTS:
            if (test->on_test_finish)
                test->on_test_finish(test);
            iperf_client_end(test);
            break;
        case IPERF_DONE:
            break;
        case SERVER_TERMINATE:
            i_errno = IESERVERTERM;

	    /*
	     * Temporarily be in DISPLAY_RESULTS phase so we can get
	     * ending summary statistics.
	     */
	    signed char oldstate = test->state;
	    cpu_util(test->cpu_util);
	    test->state = DISPLAY_RESULTS;
	    test->reporter_callback(test);
	    test->state = oldstate;
#if IPERF3_DEBUG
            printf("[iperf_test][handle_message_client]SERVER_TERMINATE ERROR: 1.\n");
#endif
            return -1;
        case ACCESS_DENIED:
            i_errno = IEACCESSDENIED;
#if IPERF3_DEBUG
            printf("[iperf_test][handle_message_client]ACCESS_DENIED ERROR: 1.\n");
#endif
            return -1;
        case SERVER_ERROR:
            if (Nread(test->ctrl_sck, (char*) &err, sizeof(err), Ptcp) < 0) {
                i_errno = IECTRLREAD;
#if IPERF3_DEBUG
                printf("[iperf_test][handle_message_client]SERVER_ERROR ERROR: 1.\n");
#endif
                return -1;
            }
	    i_errno = ntohl(err);
            if (Nread(test->ctrl_sck, (char*) &err, sizeof(err), Ptcp) < 0) {
                i_errno = IECTRLREAD;
#if IPERF3_DEBUG
                printf("[iperf_test][handle_message_client]SERVER_ERROR ERROR: 2.\n");
#endif
                return -1;
            }
            errno = ntohl(err);
#if IPERF3_DEBUG
            printf("[iperf_test][handle_message_client]SERVER_ERROR ERROR: 3.\n");
#endif
            return -1;

        default:
            i_errno = IEMESSAGE;
#if IPERF3_DEBUG
            printf("[iperf_test][handle_message_client]IEMESSAGE ERROR: 1.\n");
#endif
            return -1;
    }

    return 0;
}



/* iperf_connect -- client to server connection function */
int
iperf_connect(struct iperf_test *test)
{
#if IPERF3_DEBUG
    printf("[iperf_test]iperf_connect(), start.\n");
#endif
    //iperf_printf(test, "[iperf_test]%s \n", "iperf_printf test...");

    //FD_ZERO(&test->read_set);
    //FD_ZERO(&test->write_set);
    memset((void*)&test->read_set, 0, sizeof(test->read_set));
    memset((void*)&test->write_set, 0, sizeof(test->write_set));

#if IPERF3_DEBUG
    printf("[iperf_test]iperf_connect(), make_cookie.\n");
#endif
    make_cookie(test->cookie);

    /* Create and connect the control channel */
#if IPERF3_DEBUG
    printf("[iperf_test]create and connect the control channel.\n");
    printf("[iperf_test]test->ctrl_sck = %d.\n", test->ctrl_sck);
    printf("[iperf_test]test->settings->domain = %d.\n", test->settings->domain);
    printf("[iperf_test]Ptcp = %d.\n", Ptcp);
    printf("[iperf_test]test->bind_address = %s.\n", test->bind_address);
    printf("[iperf_test]test->server_hostname = %s.\n", test->server_hostname);
    printf("[iperf_test]test->server_port = %d.\n", test->server_port);
#endif
    if (test->ctrl_sck < 0)
    {
        test->ctrl_sck = netdial(test->settings->domain, Ptcp, test->bind_address, test->server_hostname, test->server_port);
    }
#if IPERF3_DEBUG
    printf("[iperf_test]test->ctrl_sck = %d.\n", test->ctrl_sck);
#endif

    if (test->ctrl_sck < 0)
    {
        i_errno = IECONNECT;
#if IPERF3_DEBUG
        printf("[iperf_test]ERROR:i_errno = %d, IECONNECT.\n", i_errno);
#endif
        return -1;
    }
#if IPERF3_DEBUG
    printf("[iperf_test]test->ctrl_sck = %d, test->cookie = %s.\n", test->ctrl_sck, test->cookie);
    printf("[iperf_test]COOKIE_SIZE = %d, Ptcp = %d.\n", COOKIE_SIZE, Ptcp);
#endif

    if (Nwrite(test->ctrl_sck, test->cookie, COOKIE_SIZE, Ptcp) < 0)
    {
        i_errno = IESENDCOOKIE;
#if IPERF3_DEBUG
        printf("[iperf_test]i_errno = %d, IESENDCOOKIE.\n", i_errno);
#endif
        return -1;
    }

    FD_SET(test->ctrl_sck, &test->read_set);
#if IPERF3_DEBUG
    printf("[iperf_test]test->ctrl_sck = %d, test->max_fd = %d.\n", test->ctrl_sck, test->max_fd);
#endif

    if (test->ctrl_sck > test->max_fd)
    {
        test->max_fd = test->ctrl_sck;
    }

    return 0;
}


int
iperf_client_end(struct iperf_test *test)
{
    struct iperf_stream *sp;

    /* Close all stream sockets */
    SLIST_FOREACH(sp, &test->streams, streams) {
        close(sp->socket);
    }

    /* show final summary */
    test->reporter_callback(test);

    if (iperf_set_send_state(test, IPERF_DONE) != 0)
        return -1;

    return 0;
}


// TODO
static jmp_buf sigend_jmp_buf;

static void
sigend_handler(int sig)
{
    longjmp(sigend_jmp_buf, 1);
}


int
iperf_run_client(struct iperf_test * test)
{
    int startup;
    int result = 0;
    fd_set read_set, write_set;
    struct timeval now;
    struct timeval* timeout = NULL;
    struct iperf_stream *sp;


    /* Termination signals. */
    iperf_catch_sigend(sigend_handler);
    if (setjmp(sigend_jmp_buf))
	    iperf_got_sigend(test);

#if IPERF3_DEBUG
    printf("[iperf_test]iperf_run_client, start.\n");
    printf("[iperf_test][run_client]test->affinity = %d \n", test->affinity);
    printf("[iperf_test][run_client]test->json_output = %d \n", test->json_output);
    printf("[iperf_test][run_client]test->verbose = %d \n", test->verbose);
#endif

    if (test->affinity != -1)
	if (iperf_setaffinity(test->affinity) != 0)
	    return -1;

    if (test->json_output)
	if (iperf_json_start(test) < 0)
	    return -1;


    if (test->json_output)
    {
	    cJSON_AddItemToObject(test->json_start, "version", cJSON_CreateString(version));
	    cJSON_AddItemToObject(test->json_start, "system_info", cJSON_CreateString(get_system_info()));
    }
    else if (test->verbose)
    {

	    //iperf_printf(test, "%s\n", version);
	    //iperf_printf(test, "%s", "");
	    //fflush(stdout);
	    system("uname -a");

        /* Start the client and connect to the server */
#if IPERF3_DEBUG
        printf("[iperf_test][run_client]start the client and connect to the server.\n");
#endif
        if (iperf_connect(test) < 0)
            return -1;

        /* Begin calculating CPU utilization */
#if IPERF3_DEBUG
        printf("[iperf_test][run_client]begin calculating CPU utilization.\n");
#endif
        cpu_util(NULL);

        startup = 1;

#if IPERF3_DEBUG
        printf("[iperf_test][run_client]test->state = %d.\n", (int)test->state);
#endif

        while (test->state != IPERF_DONE)
        {
	        memcpy(&read_set, &test->read_set, sizeof(fd_set));
	        memcpy(&write_set, &test->write_set, sizeof(fd_set));
	        (void) gettimeofday(&now, NULL);
	        timeout = tmr_timeout(&now);

#if IPERF3_DEBUG
            printf("[iperf_test][run_client]select, max_fd= %d, timeout = %d.\n", (int)test->max_fd + 1, (int)timeout->tv_sec);
#endif

	        result = select(test->max_fd + 1, &read_set, &write_set, NULL, timeout);

#if IPERF3_DEBUG
            printf("[iperf_test][run_client]select_result = %d.\n", (int)result);
#endif
	        if (result < 0 && errno != EINTR)
            {
          	    i_errno = IESELECT;
	            return -1;
	        }

	        if (result > 0)
            {
	            if (FD_ISSET(test->ctrl_sck, &read_set))
                {
         	        if (iperf_handle_message_client(test) < 0)
                    {
		                return -1;
		            }
		            FD_CLR(test->ctrl_sck, &read_set);
	            }
	        }

            if (test->state == TEST_RUNNING)
            {
                /* Is this our first time really running? */
                if (startup)
                {
                    startup = 0;

                    // Set non-blocking for non-UDP tests
                    if (test->protocol->id != Pudp)
                    {
                        SLIST_FOREACH(sp, &test->streams, streams)
                        {
                            setnonblocking(sp->socket, 1);
                        }
                    }
                }

	            if (test->reverse)
                {
		            // Reverse mode. Client receives.
		            if (iperf_recv(test, &read_set) < 0)
		                return -1;
	            }
                else
                {
#if IPERF3_DEBUG
                    printf("[iperf_test][run_client]Client sends.\n");
#endif
		            // Regular mode. Client sends.
		            if (iperf_send(test, &write_set) < 0)
		                return -1;
	            }

                /* Run the timers. */
                (void) gettimeofday(&now, NULL);
                tmr_run(&now);

	            /* Is the test done yet? */
	            if ((!test->omitting) &&
	                ((test->duration != 0 && test->done) ||
	                 (test->settings->bytes != 0 && test->bytes_sent >= test->settings->bytes) ||
	                 (test->settings->blocks != 0 && test->blocks_sent >= test->settings->blocks)))
                {
		            // Set non-blocking for non-UDP tests
		            if (test->protocol->id != Pudp)
                    {
		                SLIST_FOREACH(sp, &test->streams, streams)
                        {
			                setnonblocking(sp->socket, 0);
		                }
		            }

		            /* Yes, done!  Send TEST_END. */
		            test->done = 1;
		            cpu_util(test->cpu_util);
		            test->stats_callback(test);
		            if (iperf_set_send_state(test, TEST_END) != 0)
		                return -1;
	            }
	        }

        	// If we're in reverse mode, continue draining the data
	        // connection(s) even if test is over.  This prevents a
	        // deadlock where the server side fills up its pipe(s)
	        // and gets blocked, so it can't receive state changes
	        // from the client side.
	        else if (test->reverse && test->state == TEST_END)
            {
	            if (iperf_recv(test, &read_set) < 0)
		        return -1;
	        }

        } 


        if (test->json_output)
        {
	        if (iperf_json_finish(test) < 0)
	            return -1;
        }
        else
        {
	        //iperf_printf(test, "\n");
	        //iperf_printf(test, "%s", report_done);
        }
    }

/////////////////////////////////
    if((int)((int)&sp + (int)&timeout + (int)&now + (int)&read_set + (int)&write_set + (int)&result + (int)&startup) == 0)
    {
        sp = NULL;
    }
/////////////////////////////////

    return 0;
}

#if 0
int
iperf_run_client(struct iperf_test * test)
{
    int startup;
    int result = 0;
    fd_set read_set, write_set;
    struct timeval now;
    struct timeval* timeout = NULL;
    struct iperf_stream *sp;

#if 0
    /* Termination signals. */
    iperf_catch_sigend(sigend_handler);
    if (setjmp(sigend_jmp_buf))
	iperf_got_sigend(test);
#endif

    if (test->affinity != -1)
	if (iperf_setaffinity(test->affinity) != 0)
	    return -1;

    if (test->json_output)
	if (iperf_json_start(test) < 0)
	    return -1;


    if (test->json_output)
    {
	    cJSON_AddItemToObject(test->json_start, "version", cJSON_CreateString(version));
	    cJSON_AddItemToObject(test->json_start, "system_info", cJSON_CreateString(get_system_info()));
    }
    else if (test->verbose)
    {
	    iperf_printf(test, "%s\n", version);
	    iperf_printf(test, "%s", "");
	    //fflush(stdout);
	    system("uname -a");

        /* Start the client and connect to the server */
        if (iperf_connect(test) < 0)
            return -1;

        /* Begin calculating CPU utilization */
        cpu_util(NULL);

        startup = 1;
        while (test->state != IPERF_DONE)
        {
	        memcpy(&read_set, &test->read_set, sizeof(fd_set));
	        memcpy(&write_set, &test->write_set, sizeof(fd_set));
	        (void) gettimeofday(&now, NULL);
	        timeout = tmr_timeout(&now);
	        result = select(test->max_fd + 1, &read_set, &write_set, NULL, timeout);
	        if (result < 0 && errno != EINTR)
            {
          	    i_errno = IESELECT;
	            return -1;
	        }
	        if (result > 0)
            {
	            if (FD_ISSET(test->ctrl_sck, &read_set))
                {
         	        if (iperf_handle_message_client(test) < 0)
                    {
		                return -1;
		            }
		            FD_CLR(test->ctrl_sck, &read_set);
	            }
	        }

            if (test->state == TEST_RUNNING)
            {
                /* Is this our first time really running? */
                if (startup)
                {
                    startup = 0;

                    // Set non-blocking for non-UDP tests
                    if (test->protocol->id != Pudp)
                    {
                        SLIST_FOREACH(sp, &test->streams, streams)
                        {
                            setnonblocking(sp->socket, 1);
                        }
                    }
                }

	            if (test->reverse)
                {
		            // Reverse mode. Client receives.
		            if (iperf_recv(test, &read_set) < 0)
		                return -1;
	            }
                else
                {
		            // Regular mode. Client sends.
		            if (iperf_send(test, &write_set) < 0)
		                return -1;
	            }

                /* Run the timers. */
                (void) gettimeofday(&now, NULL);
                tmr_run(&now);

	            /* Is the test done yet? */
	            if ((!test->omitting) &&
	                ((test->duration != 0 && test->done) ||
	                 (test->settings->bytes != 0 && test->bytes_sent >= test->settings->bytes) ||
	                 (test->settings->blocks != 0 && test->blocks_sent >= test->settings->blocks)))
                {
		            // Set non-blocking for non-UDP tests
		            if (test->protocol->id != Pudp)
                    {
		                SLIST_FOREACH(sp, &test->streams, streams)
                        {
			                setnonblocking(sp->socket, 0);
		                }
		            }

		            /* Yes, done!  Send TEST_END. */
		            test->done = 1;
		            cpu_util(test->cpu_util);
		            test->stats_callback(test);
		            if (iperf_set_send_state(test, TEST_END) != 0)
		                return -1;
	            }
	        }

        	// If we're in reverse mode, continue draining the data
	        // connection(s) even if test is over.  This prevents a
	        // deadlock where the server side fills up its pipe(s)
	        // and gets blocked, so it can't receive state changes
	        // from the client side.
	        else if (test->reverse && test->state == TEST_END)
            {
	            if (iperf_recv(test, &read_set) < 0)
		        return -1;
	        }
        }

        if (test->json_output)
        {
	        if (iperf_json_finish(test) < 0)
	            return -1;
        }
        else
        {
	        iperf_printf(test, "\n");
	        iperf_printf(test, "%s", report_done);
        }
    }
    return 0;
}
#endif 

