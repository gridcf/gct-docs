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
    globus_bool_t done;
};

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
    monitor->done = GLOBUS_TRUE;
    globus_cond_signal(&monitor->cond);
    if (operation_failure_code == GLOBUS_SUCCESS)
    {
        printf("Submitted job %s\n",
            job_contact ? job_contact : "UNKNOWN");
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
    globus_gram_client_attr_t attr;
    struct monitor_t monitor;

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s RESOURCE-MANAGER-CONTACT RSL-SPEC...\n",
                argv[0]);
        rc = 1;

        goto out;
    }

    printf("Submiting job to %s with full proxy\n", argv[1]);

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
    monitor.done = GLOBUS_FALSE;

    /* Initialize attribute so that we can set the delegation attribute */
    rc = globus_gram_client_attr_init(&attr);

    /* Set the proxy attribute */
    rc = globus_gram_client_attr_set_delegation_mode(
        attr,
        GLOBUS_IO_SECURE_DELEGATION_MODE_FULL_PROXY);

    /* Submit the job rsl from argv[2]
     */
    globus_mutex_lock(&monitor.mutex);
    /* When the job has been submitted, the example_submit_callback
     * will be called, either from another thread or from a 
     * globus_cond_wait in a nonthreaded build
     */
    rc = globus_gram_client_register_job_request(
            argv[1], argv[2], 0, NULL, attr, example_submit_callback,
            &monitor);
    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Unable to submit job %s because %s (Error %d)\n",
                argv[2], globus_gram_client_error_string(rc), rc);
    }

    /* Wait until the example_submit_callback function has been called for
     * the job submission
     */
    while (!monitor.done)
    {
        globus_cond_wait(&monitor.cond, &monitor.mutex);
    }
    globus_mutex_unlock(&monitor.mutex);

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
/* End of gram_attr_example.c */
