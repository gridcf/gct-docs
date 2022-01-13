/*
 * These headers contain declarations for the globus_module functions
 * and GRAM Client API functions
 */
#include "globus_common.h"
#include "globus_gram_client.h"
#include "globus_gram_protocol.h"

#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
    int rc;
    globus_hashtable_t extensions = NULL;
    globus_gram_protocol_extension_t * extension_value;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s RESOURCE-MANAGER-CONTACT\n", argv[0]);
        rc = 1;

        goto out;
    }

    printf("Checking version of GRAM resource: %s\n", argv[1]);

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
     * Contact the service passed as our first command-line option and perform
     * a version check. If successful,
     * this function will return GLOBUS_SUCCESS, otherwise an integer
     * error code. Old versions of the job manager will return 
     * GLOBUS_GRAM_PROTOCOL_ERROR_HTTP_UNPACK_FAILED as they do not support
     * the version operation.
     */
    rc = globus_gram_client_get_jobmanager_version(argv[1], &extensions);
    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Unable to get service version from %s because %s "
                "(Error %d)\n",
                argv[1], globus_gram_client_error_string(rc), rc);
    }
    else
    {
        /* The version information is returned in the extensions hash table */
        extension_value = globus_hashtable_lookup(
                &extensions,
                "toolkit-version");

        if (extension_value == NULL)
        {
            printf("Unknown toolkit version\n");
        }
        else
        {
            printf("Toolkit Version: %s\n", extension_value->value);
        }

        extension_value = globus_hashtable_lookup(
                &extensions,
                "version");
        if (extension_value == NULL)
        {
            printf("Unknown package version\n");
        }
        else
        {
            printf("Package Version: %s\n", extension_value->value);
        }
        /* Free the extensions hash and its values */
        globus_gram_protocol_hash_destroy(&extensions);
    }

    /*
     * Deactivating the module allows it to free memory and close network
     * connections.
     */
    rc = globus_module_deactivate(GLOBUS_GRAM_CLIENT_MODULE);
out:
    return rc;
}
/* End of gram_version_example.c */
