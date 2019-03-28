sub cancel
{
    my $self = shift;
    my $description = $self->{JobDescription};
    my $pgid;
    my $jobid = $description->jobid();
    my $fh;
    my $pending_time = 0;
    my $active_time;
    my $done_time;
    my $failed_time ;
    my $now = time();

    if(!defined $jobid)
    {
        $self->log("cancel: no jobid defined!");
        return { JOB_STATE => Globus::GRAM::JobState::FAILED };
    }

    open($fh, "<$job_dir/fakejob.log");

    # Multiple matches might occur if the job is cancelled, so we keep looping
    # until EOF
    while (<$fh>)
    {
        chomp;

        my @fields = split(/;/);

        if ($fields[0] ne $jobid)
        {
            next;
        }
        
        $pending_time = $fields[1];
        $active_time = $fields[2];
        $done_time = $fields[3];
        $failed_time = $fields[4];
    }
    close($fh);

    $self->log("cancel job " . $jobid);
    if ($now < int($done_time) && int($failed_time) == 0)
    {
        $failed_time = $now;
        $done_time = 0;
        if (!open($fh, ">>$job_dir/fakejob.log"))
        {
            $self->respond({GT3_FAILURE_MESSAGE => "Unable to write job file"});
            return Globus::GRAM::Error::INVALID_SCRIPT_STATUS;
        }
        print $fh "$jobid;$pending_time;$active_time;$done_time;$failed_time\n";
        close($fh);
    }

    return { JOB_STATE => Globus::GRAM::JobState::FAILED };
}
