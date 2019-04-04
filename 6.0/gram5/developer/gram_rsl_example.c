/*
 * These headers contain declarations for the globus_module,
 * the GRAM Client, RSL, and protocol functions
 */
#include "globus_common.h"
#include "globus_gram_client.h"
#include "globus_rsl.h"
#include "globus_gram_protocol.h"

#include <stdio.h>
#include <strings.h>

static
int
example_rsl_attribute_match(void * datum, void * arg)
{
    const char * relation_attribute = globus_rsl_relation_get_attribute(datum);
    const char * attribute = arg;

    /* RSL attributes are case-insensitive */
    return (relation_attribute &&
            strcasecmp(relation_attribute, attribute) == 0);
}

int
main(int argc, char *argv[])
{
    int rc;
    globus_rsl_t *rsl, *environment_relation;
    globus_rsl_value_t *new_env_pair = NULL;
    globus_list_t *environment_relation_node;
    char * rsl_string;
    char * job_contact;

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

    /* Parse the RSL string into a syntax tree */
    rsl = globus_rsl_parse(argv[2]);
    if (rsl == NULL)
    {
        rc = 1;
        fprintf(stderr, "Error parsing RSL string\n");
        goto deactivate;
    }

    /* Create the new environment variable pair that we'll insert
     * into the RSL. We'll start by making an empty sequence
     */
    new_env_pair = globus_rsl_value_make_sequence(NULL);
    if (new_env_pair == NULL)
    {
        fprintf(stderr, "Error creating value sequence\n");
        rc = 1;

        goto free_rsl;
    }
    /* Then insert the name-value pair in reverse order */
    rc = globus_list_insert(
            globus_rsl_value_sequence_get_list_ref(new_env_pair),
            globus_rsl_value_make_literal(
                strdup("itsvalue")));
    if (rc != GLOBUS_SUCCESS)
    {
        goto free_env_pair;
    }

    rc = globus_list_insert(
            globus_rsl_value_sequence_get_list_ref(new_env_pair),
            globus_rsl_value_make_literal(
                strdup("EXAMPLE_ENVIRONMENT_VARIABLE")));
    if (rc != GLOBUS_SUCCESS)
    {
        goto free_env_pair;
    }
    /* Now, check to see if the RSL already contains an environment 
     * attribute.
     */
    environment_relation_node = globus_list_search_pred(
            globus_rsl_boolean_get_operand_list(rsl),
            example_rsl_attribute_match,
            GLOBUS_GRAM_PROTOCOL_ENVIRONMENT_PARAM);

    if (environment_relation_node == NULL)
    {
        /* Not present yet, create a new relation and insert it into
         * the RSL.
         */
        environment_relation = globus_rsl_make_relation(
                GLOBUS_RSL_EQ,
                strdup(GLOBUS_GRAM_PROTOCOL_ENVIRONMENT_PARAM),
                globus_rsl_value_make_sequence(NULL));
        rc = globus_list_insert(
                globus_rsl_boolean_get_operand_list_ref(rsl),
                environment_relation);
        if (rc != GLOBUS_SUCCESS)
        {
            globus_rsl_free_recursive(environment_relation);
            goto free_env_pair;
        }
    }
    else
    {
        /* Pull the environment relation out of the node returned from the
         * search function
         */
        environment_relation = globus_list_first(environment_relation_node);
    }

    /* Add the new environment binding to the value sequence associated with
     * the environment relation
     */
    rc = globus_list_insert(
        globus_rsl_value_sequence_get_list_ref(
                globus_rsl_relation_get_value_sequence(environment_relation)),
        new_env_pair);
    if (rc != GLOBUS_SUCCESS)
    {
        goto free_env_pair;
    }
    new_env_pair = NULL;

    /* Convert the RSL parse tree to a string */
    rsl_string = globus_rsl_unparse(rsl);

    /*
     * Submit the augmented RSL to the service passed as our first command-line
     * option. If successful, this function will return GLOBUS_SUCCESS,
     * otherwise an integer error code.
     */
    rc = globus_gram_client_job_request(
            argv[1],
            rsl_string,
            0,
            NULL,
            &job_contact);
    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Unable to submit job to %s because %s (Error %d)\n",
                argv[1], globus_gram_client_error_string(rc), rc);
    }
    else
    {
        printf("Job submitted successfully: %s\n", job_contact);
    }

    free(rsl_string);

    if (job_contact)
    {
        free(job_contact);
    }
free_env_pair:
    if (new_env_pair != NULL)
    {
        globus_rsl_value_free_recursive(new_env_pair);
    }
free_rsl:
    globus_rsl_free_recursive(rsl);
deactivate:
    /*
     * Deactivating the module allows it to free memory and close network
     * connections.
     */
    rc = globus_module_deactivate(GLOBUS_GRAM_CLIENT_MODULE);
out:
    return rc;
}
/* End of gram_rsl_example.c */
