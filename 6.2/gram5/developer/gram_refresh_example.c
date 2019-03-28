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

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s JOB-CONTACT\n", argv[0]);
        rc = 1;

        goto out;
    }

    printf("Refreshing Credential for GRAM Job: %s\n", argv[1]);

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
     * Refresh the credential of the job running at the contact named
     * by the first command-line argument to this program. We'll use the
     * process's default credential by passing in GSS_C_NO_CREDENTIAL.
     */
    rc = globus_gram_client_job_refresh_credentials(
            argv[1], GSS_C_NO_CREDENTIAL);
    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Unable to refresh credential for job %s because %s (Error %d)\n",
                argv[1], globus_gram_client_error_string(rc), rc);
    }
    else
    {
        printf("Refresh successful\n");
    }
    /*
     * Deactivating the module allows it to free memory and close network
     * connections.
     */
    rc = globus_module_deactivate(GLOBUS_GRAM_CLIENT_MODULE);
out:
    return rc;
}
/* End of gram_refresh_example.c */
