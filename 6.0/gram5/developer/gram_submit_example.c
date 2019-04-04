/*
 * These headers contain declarations for the globus_module functions
 * and GRAM Client API functions
 */
#include "globus_common.h"
#include "globus_gram_client.h"

#include <stdio.h>

int
main(int argc, char *argv[])
{
    int rc;
    char * job_contact = NULL;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s RESOURCE-MANAGER-CONTACT RSL\n", argv[0]);
        rc = 1;

        goto out;
    }

    printf("Submitting job to GRAM resource: %s\n", argv[1]);

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
    /*
     * Submit the job request to the service passed as our first command-line
     * option. If successful, this function will return GLOBUS_SUCCESS,
     * otherwise an integer error code.
     */
    rc = globus_gram_client_job_request(
            argv[1], argv[2], 0, NULL, &job_contact);

    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Unable to submit job to %s because %s (Error %d)\n",
                argv[1], globus_gram_client_error_string(rc), rc);
        if (job_contact != NULL)
        {
            printf("Job Contact: %s\n", job_contact);
        }
    }
    else
    {
        /* Display job contact string */
        printf("Job submit successful: %s\n", job_contact);
    }

    if (job_contact != NULL)
    {
        free(job_contact);
    }
    /*
     * Deactivating the module allows it to free memory and close network
     * connections.
     */
    rc = globus_module_deactivate(GLOBUS_GRAM_CLIENT_MODULE);
out:
    return rc;
}
/* End of gram_submit_example.c */
