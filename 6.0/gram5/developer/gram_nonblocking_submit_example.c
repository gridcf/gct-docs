/*
 * These headers contain declarations for the globus_module functions
 * and GRAM Client API functions
 */
#include "globus_common.h"
#include "globus_gram_client.h"

#include <stdio.h>

struct monitor_t
{
    globus_mutex_t mutex;
    globus_cond_t cond;
    int submit_pending;
    int successful_submits;
};

#define CONCURRENT_SUBMITS 5

static
void
example_submit_callback(
    void * user_callback_arg,
    globus_gram_protocol_error_t operation_failure_code,
    const char * job_contact,
    globus_gram_protocol_job_state_t job_state,
    globus_gram_protocol_error_t job_failure_code)
{
    struct monitor_t * monitor = user_callback_arg;

    globus_mutex_lock(&monitor->mutex);
    monitor->submit_pending--;
    if (monitor->submit_pending < CONCURRENT_SUBMITS)
    {
        globus_cond_signal(&monitor->cond);
    }
    printf("Submitted job %s\n",
            job_contact ? job_contact : "UNKNOWN");
    if (operation_failure_code == GLOBUS_SUCCESS)
    {
        monitor->successful_submits++;
    }
    else
    {
        printf("submit failed because %s (Error %d)\n",
                globus_gram_client_error_string(operation_failure_code),
                operation_failure_code);
    }
    globus_mutex_unlock(&monitor->mutex);
}

int
main(int argc, char *argv[])
{
    int rc;
    int i;
    struct monitor_t monitor;

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s RESOURCE-MANAGER-CONTACT RSL-SPEC...\n",
                argv[0]);
        rc = 1;

        goto out;
    }

    printf("Submiting %d jobs to %s\n", argc-2, argv[1]);

    /*
     * Always activate the GLOBUS_GRAM_CLIENT_MODULE prior to using any
     * functions from the GRAM Client API or behavior is undefined.
     */
    rc = globus_module_activate(GLOBUS_GRAM_CLIENT_MODULE);
    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Error activating %s because %s (Error %d)\n",
                GLOBUS_GRAM_CLIENT_MODULE->module_name,
                globus_gram_client_error_string(rc),
                rc);
        goto out;
    }

    rc = globus_mutex_init(&monitor.mutex, NULL);
    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Error initializing mutex %d\n", rc);

        goto deactivate;
    }

    rc = globus_cond_init(&monitor.cond, NULL);
    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Error initializing condition variable %d\n", rc);

        goto destroy_mutex;
    }
    monitor.submit_pending = 0;

    /* Submits jobs from argv[2] until end of the argv array. At most
     * CONCURRENT_SUBMITS will be pending at any given time.
     */
    globus_mutex_lock(&monitor.mutex);
    for (i = 2; i < argc; i++)
    {
        /* This throttles the number of concurrent job submissions */
        while (monitor.submit_pending >= CONCURRENT_SUBMITS)
        {
            globus_cond_wait(&monitor.cond, &monitor.mutex);
        }

        /* When the job has been submitted, the example_submit_callback
         * will be called, either from another thread or from a 
         * globus_cond_wait in a nonthreaded build
         */
        rc = globus_gram_client_register_job_request(
                argv[1], argv[i], 0, NULL, NULL, example_submit_callback,
                &monitor);
        if (rc != GLOBUS_SUCCESS)
        {
            fprintf(stderr, "Unable to submit job %s because %s (Error %d)\n",
                    argv[i], globus_gram_client_error_string(rc), rc);
        }
        else
        {
            monitor.submit_pending++;
        }
    }

    /* Wait until the example_submit_callback function has been called for
     * each job submission
     */
    while (monitor.submit_pending > 0)
    {
        globus_cond_wait(&monitor.cond, &monitor.mutex);
    }
    globus_mutex_unlock(&monitor.mutex);

    printf("Submitted %d jobs (%d successfully)\n",
            argc-2, monitor.successful_submits);

    globus_cond_destroy(&monitor.cond);
destroy_mutex:
    globus_mutex_destroy(&monitor.mutex);
deactivate:
    /*
     * Deactivating the module allows it to free memory and close network
     * connections.
     */
    rc = globus_module_deactivate(GLOBUS_GRAM_CLIENT_MODULE);
out:
    return rc;
}
/* End of gram_nonblocking_submit_example.c */
