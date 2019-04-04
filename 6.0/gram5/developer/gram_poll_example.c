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
    int status = 0;
    int failure_code = 0;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s JOB-CONTACT\n", argv[0]);
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
    /*
     * Check the job status of the job named by the first argument to
     * this program.
     */
    rc = globus_gram_client_job_status(argv[1], &status, &failure_code);
    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Unable to check job status because %s (Error %d)\n",
                globus_gram_client_error_string(rc), rc);
    }
    else
    {
        switch (status)
        {
            case GLOBUS_GRAM_PROTOCOL_JOB_STATE_UNSUBMITTED:
                printf("Unsubmitted\n");
                break;
            case GLOBUS_GRAM_PROTOCOL_JOB_STATE_STAGE_IN:
                printf("StageIn\n");
                break;
            case GLOBUS_GRAM_PROTOCOL_JOB_STATE_PENDING:
                printf("Pending\n");
                break;
            case GLOBUS_GRAM_PROTOCOL_JOB_STATE_ACTIVE:
                printf("Active\n");
                break;
            case GLOBUS_GRAM_PROTOCOL_JOB_STATE_SUSPENDED:
                printf("Suspended\n");
                break;
            case GLOBUS_GRAM_PROTOCOL_JOB_STATE_STAGE_OUT:
                printf("StageOut\n");
                break;
            case GLOBUS_GRAM_PROTOCOL_JOB_STATE_DONE:
                printf("Done\n");
                break;
            case GLOBUS_GRAM_PROTOCOL_JOB_STATE_FAILED:
                printf("Failed (%d)\n", failure_code);
                break;
            default:
                printf("Unknown job state\n");
                break;
        }
    }
    /*
     * Deactivating the module allows it to free memory and close network
     * connections.
     */
    rc = globus_module_deactivate(GLOBUS_GRAM_CLIENT_MODULE);
out:
    return rc;
}
/* End of gram_poll_example.c */
