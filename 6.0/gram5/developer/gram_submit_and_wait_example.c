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
    globus_gram_protocol_job_state_t state;
};

/*
 * Job State Callback Function
 *
 * This function is called when the job manager sends job states.
 */
static
void
example_callback(void * callback_arg, char * job_contact, int state,
        int errorcode)
{
    struct monitor_t * monitor = callback_arg;

    globus_mutex_lock(&monitor->mutex);

    printf("Old Job State: %d\nNew Job State: %d\n", monitor->state, state);

    monitor->state = state;

    if (state == GLOBUS_GRAM_PROTOCOL_JOB_STATE_FAILED ||
        state == GLOBUS_GRAM_PROTOCOL_JOB_STATE_DONE)
    {
        globus_cond_signal(&monitor->cond);
    }
    globus_mutex_unlock(&monitor->mutex);
}

int
main(int argc, char *argv[])
{
    int rc;
    char * callback_contact = NULL;
    char * job_contact = NULL;
    struct monitor_t monitor;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s RESOURCE-MANAGER-CONTACT RSL\n", argv[0]);
        rc = 1;

        goto out;
    }

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
        fprintf(stderr, "Error initializing mutex\n");
        goto deactivate;
    }
    rc = globus_cond_init(&monitor.cond, NULL);
    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Error initializing condition variable\n");
        goto destroy_mutex;
    }

    monitor.state = GLOBUS_GRAM_PROTOCOL_JOB_STATE_UNSUBMITTED;

    globus_mutex_lock(&monitor.mutex);

    /*
     * Allow GRAM state change callbacks 
     */
    rc = globus_gram_client_callback_allow(
            example_callback, &monitor, &callback_contact);
    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Error allowing callbacks because %s (Error %d)\n",
                globus_gram_client_error_string(rc), rc);
        goto destroy_cond;
    }
    /*
     * Submit the job request to the service passed as our first command-line
     * option. 
     */
    rc = globus_gram_client_job_request(
            argv[1], argv[2],
            GLOBUS_GRAM_PROTOCOL_JOB_STATE_FAILED|
            GLOBUS_GRAM_PROTOCOL_JOB_STATE_DONE,
            callback_contact, &job_contact);

    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Unable to submit job to %s because %s (Error %d)\n",
                argv[1], globus_gram_client_error_string(rc), rc);
        /* Job submit failed. Short circuit the while loop below by setting
         * the job state to failed
         */
        monitor.state = GLOBUS_GRAM_PROTOCOL_JOB_STATE_FAILED;
    }
    else
    {
        /* Display job contact string */
        printf("Job submit successful: %s\n", job_contact);
    }

    /* Wait for job state callback to let us know the job has completed */
    while (monitor.state != GLOBUS_GRAM_PROTOCOL_JOB_STATE_DONE &&
           monitor.state != GLOBUS_GRAM_PROTOCOL_JOB_STATE_FAILED)
    {
        globus_cond_wait(&monitor.cond, &monitor.mutex);
    }
    rc = globus_gram_client_callback_disallow(callback_contact);
    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Error disabling callbacks because %s (Error %d)\n",
                globus_gram_client_error_string(rc), rc);
    }
    globus_mutex_unlock(&monitor.mutex);

    if (job_contact != NULL)
    {
        free(job_contact);
    }

destroy_cond:
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
/* End of gram_submit_and_wait_example.c */
