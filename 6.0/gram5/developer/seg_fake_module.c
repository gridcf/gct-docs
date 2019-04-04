/* Include Headers */
#include "globus_common.h"
#include "globus_scheduler_event_generator.h"
#include "globus_scheduler_event_generator_app.h"


/* Module Specific Data */
/* Fake Job Info */
typedef struct
{
    char   jobid[64];
    time_t pending;
    time_t active;
    time_t failed;
    time_t done;
}
fake_job_info_t;


/* A statically-initialized empty job info which is used to initialize
 * dynamically allocated fake_job_info_t structs
 */
static fake_job_info_t fake_job_info_initializer;


/* LRM Parser State */
/**
 * State of the FAKE log file parser.
 */
static struct 
{
    /** Timestamp of when to start generating events from */
    time_t                              start_timestamp;
    /** Log file path */
    char *                              log_path;
    /** Log file pointer */
    FILE *                              log;
    /** List of job info containing future info we might need to
      * turn into job state changes
      */
    globus_list_t *                     jobs;
    /**
     * shutdown mutex
     */
    globus_mutex_t                      mutex;
    /**
     * shutdown condition
     */
    globus_cond_t                       cond;
    /**
     * shutdown flag
     */
    globus_bool_t                       shutdown_called;
    /**
     * callback count
     */
    int                                 callback_count;
} globus_l_fake_state;


/* Module Specific Prototypes */
static
int
globus_l_fake_module_activate(void);

static
int
globus_l_fake_module_deactivate(void);

static
void
globus_l_fake_read_callback(void *user_arg);

static
int
globus_l_fake_find_by_job_id(void * datum, void * arg);

static
int
globus_l_fake_compare_events(void * low_datum, void * high_datum, void * relation_args);


/* Extension Module Descriptor */

GlobusExtensionDefineModule(globus_seg_fake) =
{
    "globus_seg_fake",
    globus_l_fake_module_activate,
    globus_l_fake_module_deactivate,
    NULL,
    NULL,
    NULL
};


/* Module Activation */

static
int
globus_l_fake_module_activate(void)
{

    /* Declare Variables */
    char *                              config_path = NULL;
    char *                              log_dir;
    int                                 rc;
    globus_result_t                     result = GLOBUS_SUCCESS;


    /* Activate Dependencies */
    rc = globus_module_activate(GLOBUS_COMMON_MODULE);
    if (rc != GLOBUS_SUCCESS)
    {
        fprintf(stderr, "Fatal error activating GLOBUS_COMMON_MODULE\n");
    
        result = GLOBUS_FAILURE;
        goto activation_failure;
    }


    /* Prepare Shutdown Handler */
    rc = globus_mutex_init(&globus_l_fake_state.mutex, NULL);
    if (rc != GLOBUS_SUCCESS)
    {
        result = GLOBUS_FAILURE;
        goto mutex_init_failed;
    }
    
    rc = globus_cond_init(&globus_l_fake_state.cond, NULL);
    if (rc != GLOBUS_SUCCESS)
    {
        result = GLOBUS_FAILURE;
        goto cond_init_failed;
    }
    globus_l_fake_state.shutdown_called = GLOBUS_FALSE;
    globus_l_fake_state.callback_count = 0;


    /* Read Configuration */
    result = globus_scheduler_event_generator_get_timestamp(
            &globus_l_fake_state.start_timestamp);
    if (result != GLOBUS_SUCCESS)
    {
        goto get_timestamp_failed;
    }

    result = globus_eval_path(
            "${sysconfdir}/globus/globus-fake.conf",
            &config_path);
    if (result != GLOBUS_SUCCESS || config_path == NULL)
    {
        goto get_config_path_failed;
    }
    result = globus_common_get_attribute_from_config_file(
            "",
            config_path,
            "log_path",
            &log_dir);
    
    /* This default must match fake.pm's default for things to work */
    if (result != GLOBUS_SUCCESS)
    {
        result = globus_eval_path("${localstatedir}/fake", &log_dir);
    }
    
    if (result != GLOBUS_SUCCESS)
    {
        goto get_log_dir_failed;
    }
    
    globus_l_fake_state.log_path =
        globus_common_create_string("%s/fakejob.log", log_dir);
    if (globus_l_fake_state.log_path == NULL)
    {
        result = GLOBUS_FAILURE;
    
        goto get_log_path_failed;
    }


    /* Register Event */
    result = globus_callback_register_oneshot(
            NULL,
            NULL,
            globus_l_fake_read_callback,
            &globus_l_fake_state);
    if (result != GLOBUS_SUCCESS)
    {
        goto register_oneshot_failed;
    }
    globus_l_fake_state.callback_count++;
    


/* Cleanup on Failure */
register_oneshot_failed:
get_log_path_failed:
    if (result != GLOBUS_SUCCESS)
    {
        free(globus_l_fake_state.log_path);
    }
    free(log_dir);
get_log_dir_failed:
    free(config_path);
get_config_path_failed:
get_timestamp_failed:
    if (result != GLOBUS_SUCCESS)
    {
malloc_state_failed:
        globus_cond_destroy(&globus_l_fake_state.cond);
cond_init_failed:
        globus_mutex_destroy(&globus_l_fake_state.mutex);
mutex_init_failed:
        globus_module_deactivate(GLOBUS_COMMON_MODULE);
    }
activation_failure:

    return result;
} /* globus_l_fake_module_activate() */


/* Module Deactivation */
static
int
globus_l_fake_module_deactivate(void)
{
    
    /* Shutdown Handling */
    globus_mutex_lock(&globus_l_fake_state.mutex);
    globus_l_fake_state.shutdown_called = GLOBUS_TRUE;
    while (globus_l_fake_state.callback_count > 0)
    {
        globus_cond_wait(&globus_l_fake_state.cond, &globus_l_fake_state.mutex);
    }
    globus_mutex_unlock(&globus_l_fake_state.mutex);

    
    /* Cleanup State */
    globus_mutex_destroy(&globus_l_fake_state.mutex);
    globus_cond_destroy(&globus_l_fake_state.cond);
    if (globus_l_fake_state.log_path)
    {
        free(globus_l_fake_state.log_path);
    }
    if (globus_l_fake_state.log)
    {
        fclose(globus_l_fake_state.log);
    }
    while (!globus_list_empty(globus_l_fake_state.jobs))
    {
        fake_job_info_t *info;
        
        info = globus_list_remove(
                &globus_l_fake_state.jobs,
                globus_l_fake_state.jobs);
        free(info);
    }
    
    globus_module_deactivate(GLOBUS_COMMON_MODULE);
    
    return GLOBUS_SUCCESS;

} /* globus_l_fake_module_deactivate() */


/* Process Events */

static
void
globus_l_fake_read_callback(void * arg)
{

    /* Declare Variables */
    char                                jobid[64];
    globus_list_t                       *l, *events;
    fake_job_info_t                     *info;
    globus_reltime_t                    delay = {0};
    time_t                              now;
    unsigned long                       pending_time, active_time, done_time, failed_time;
    globus_scheduler_event_t            *e;
    time_t                              last_timestamp;
    globus_result_t                     result = GLOBUS_SUCCESS;


    /* Check for Shutdown */
    globus_mutex_lock(&globus_l_fake_state.mutex);
    if (globus_l_fake_state.shutdown_called)
    {
        result = GLOBUS_FAILURE;
        goto error;
    }


    /* Open Log */
    if (globus_l_fake_state.log == NULL)
    {
        globus_l_fake_state.log = fopen(globus_l_fake_state.log_path, "r");
        if (globus_l_fake_state.log != NULL)
        {
            /* Enable line buffering */
            setvbuf(globus_l_fake_state.log, NULL, _IOLBF, 0);
        }
    }
    if (globus_l_fake_state.log == NULL)
    {
        result = GLOBUS_FAILURE;
        GlobusTimeReltimeSet(delay, 30, 0);
        goto reregister;
    }


    /* Read Log */
    
    /* previous read might have hit EOF, so clear it before trying to read */
    clearerr(globus_l_fake_state.log);
    
    /* Read any new job info from the log */
    while (fscanf(globus_l_fake_state.log, "%63[^;];%ld;%ld;%ld;%ld\n",
                jobid,
                &pending_time,
                &active_time,
                &done_time,
                &failed_time) == 5)
    {
        l = globus_list_search_pred(globus_l_fake_state.jobs, globus_l_fake_find_by_job_id, jobid);
        if (l != NULL)
        {
            info = globus_list_first(l);
            /* If there's a second entry for the same job, it was cancelled, so
             * clear done/failed timestamps and copy them below
             */
            info->done = info->failed = 0;
        }
        else
        {
            /* First time we've seen this job, set jobid and insert*/
            info = malloc(sizeof(fake_job_info_t));
            *info = fake_job_info_initializer;
            strcpy(info->jobid, jobid);
            globus_list_insert(&globus_l_fake_state.jobs, info);
        }
        /* set timestamps */
        info->pending = pending_time;
        info->active = active_time;
        info->done = done_time;
        info->failed = failed_time;
    }


    /* Create Events */
    /* Create set of events that we'll emit this time through: jobs which will
     * changed state since our last event update
     */
    now = time(NULL);
    
    events = NULL;
    for (l = globus_l_fake_state.jobs; l != NULL; l = globus_list_rest(l))
    {
        info = globus_list_first(l);
    
        if (info->pending >= globus_l_fake_state.start_timestamp &&
            info->pending < now)
        {
            e = malloc(sizeof(globus_scheduler_event_t));
            e->event_type = GLOBUS_SCHEDULER_EVENT_PENDING;
            e->job_id = info->jobid;
            e->timestamp = info->pending;
            e->exit_code = 0;
            e->failure_code = 0;
            e->raw_event = NULL;
    
            info->pending = 0;
    
            globus_list_insert(&events, e);
        }
        if (info->active >= globus_l_fake_state.start_timestamp &&
            info->active < now)
        {
            e = malloc(sizeof(globus_scheduler_event_t));
            e->event_type = GLOBUS_SCHEDULER_EVENT_ACTIVE;
            e->job_id = info->jobid;
            e->timestamp = info->active;
            e->exit_code = 0;
            e->failure_code = 0;
            e->raw_event = NULL;
    
            info->active = 0;
    
            globus_list_insert(&events, e);
        }
        if (info->done != 0 &&
            info->done >= globus_l_fake_state.start_timestamp &&
            info->done < now)
        {
            e = malloc(sizeof(globus_scheduler_event_t));
            e->event_type = GLOBUS_SCHEDULER_EVENT_DONE;
            e->job_id = info->jobid;
            e->timestamp = info->done;
            e->exit_code = 0;
            e->failure_code = 0;
            e->raw_event = NULL;
    
            info->done = 0;
    
            globus_list_insert(&events, e);
        }
        if (info->failed != 0 &&
            info->failed >= globus_l_fake_state.start_timestamp &&
            info->failed < now)
        {
            e = malloc(sizeof(globus_scheduler_event_t));
            e->event_type = GLOBUS_SCHEDULER_EVENT_FAILED;
            e->job_id = info->jobid;
            e->timestamp = info->failed;
            e->exit_code = 0;
            e->failure_code = GLOBUS_GRAM_PROTOCOL_ERROR_USER_CANCELLED;
            e->raw_event = NULL;
    
            info->failed = 0;
    
            globus_list_insert(&events, e);
        }
    }


    /* Emit Events */
    /* Sort the events so that they're in timestamp order */
    events = globus_list_sort_destructive(events, globus_l_fake_compare_events, NULL);
    
    /* Emit events in proper order */
    while (! globus_list_empty(events))
    {
        e = globus_list_remove(&events, events);
        last_timestamp = e->timestamp;
    
        switch (e->event_type)
        {
            case GLOBUS_SCHEDULER_EVENT_PENDING:
                globus_scheduler_event_pending(e->timestamp, e->job_id);
                break;
            case GLOBUS_SCHEDULER_EVENT_ACTIVE:
                globus_scheduler_event_active(e->timestamp, e->job_id);
                break;
            case GLOBUS_SCHEDULER_EVENT_FAILED:
                globus_scheduler_event_failed(e->timestamp, e->job_id, e->failure_code);
                break;
            case GLOBUS_SCHEDULER_EVENT_DONE:
                globus_scheduler_event_done(e->timestamp, e->job_id, e->exit_code);
                break;
        }
        /* If this is a terminal event, we can remove the job from the list */
        if (e->event_type == GLOBUS_SCHEDULER_EVENT_FAILED ||
            e->event_type == GLOBUS_SCHEDULER_EVENT_DONE)
        {
            l = globus_list_search_pred(globus_l_fake_state.jobs, globus_l_fake_find_by_job_id, e->job_id);
            info = globus_list_remove(&globus_l_fake_state.jobs, l);
            free(info);
        }
    
        free(e);
    }
    globus_l_fake_state.start_timestamp = last_timestamp;
    


    /* Reregister Callback */
    GlobusTimeReltimeSet(delay, 1, 0);
    reregister:
    result = globus_callback_register_oneshot(
            NULL,
            &delay,
            globus_l_fake_read_callback,
            &globus_l_fake_state);
    if (result != GLOBUS_SUCCESS)
    {
        goto error;
    }
    globus_mutex_unlock(&globus_l_fake_state.mutex);
    return;
    


    /* Error Handling */
    error:
    if (globus_l_fake_state.shutdown_called)
    {
        globus_l_fake_state.callback_count--;
    
        if (globus_l_fake_state.callback_count == 0)
        {
            globus_cond_signal(&globus_l_fake_state.cond);
        }
    }
    else
    {
        fprintf(stderr,
                "FATAL: Unable to register callback. FAKE SEG exiting\n");
        exit(EXIT_FAILURE);
    }
    globus_mutex_unlock(&globus_l_fake_state.mutex);
    
    return;
    
    

} /* globus_l_fake_read_callback() */


/* Utility Functions */
/* Find By Job ID */
static
int
globus_l_fake_find_by_job_id(void * datum, void * arg)
{
    fake_job_info_t * info = datum;

    return (strcmp(info->jobid, arg) == 0);
} /* globus_l_fake_find_by_job_id() */


/* Sort Events */
static
int
globus_l_fake_compare_events(void * low_datum, void * high_datum, void * relation_args)
{
    globus_scheduler_event_t *low_event = low_datum, *high_event = high_datum;

    if (low_event->timestamp < high_event->timestamp)
    {
        return GLOBUS_TRUE;
    }
    else if (low_event->timestamp == high_event->timestamp)
    {
        if (low_event->event_type == GLOBUS_SCHEDULER_EVENT_PENDING)
        {
            return GLOBUS_TRUE;
        }
        else if (low_event->event_type == GLOBUS_SCHEDULER_EVENT_ACTIVE &&
                 high_event->event_type != GLOBUS_SCHEDULER_EVENT_PENDING)
        {
            return GLOBUS_TRUE;
        }
        else if (high_event->event_type != GLOBUS_SCHEDULER_EVENT_PENDING &&
                 high_event->event_type != GLOBUS_SCHEDULER_EVENT_ACTIVE)
        {
            return GLOBUS_TRUE;
        }
    }
    return GLOBUS_FALSE;
} /* globus_l_fake_compare_events() */
