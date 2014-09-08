sub poll
{
    my $self = shift;
    my $description = $self->{JobDescription};
    my $state;
    my $pid;
    my $now;
    my $fh;
    my $pending_time = 0;
    my $active_time;
    my $done_time;
    my $failed_time;
    my $seqno;

    my $jobid = $description->jobid();

    if(!defined $jobid)
    {
        $self->log("poll: job id undefined!");
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

    $now = time();

    if ($pending_time == 0)
    {
        # not found
        $state = Globus::GRAM::JobState::FAILED;
    }
    elsif (int($failed_time) != 0)
    {
        $state = Globus::GRAM::JobState::FAILED;
    }
    elsif ($now < int($active_time))
    {
        $state = Globus::GRAM::JobState::PENDING;
        return
    }
    elsif ($now < int($done_time))
    {
        $state = Globus::GRAM::JobState::ACTIVE;
    }
    else
    {
        $state = Globus::GRAM::JobState::DONE;
    }

    return { JOB_STATE => $state };
}
