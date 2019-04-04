sub submit
{
    my $self = shift;
    my $description = $self->{JobDescription};
    my $now = time();
    my $jobid;
    my $fh;
    my $pending_time;
    my $active_time;
    my $done_time;
    my $failed_time = 0;

    if ($description->max_wall_time() != int($description->max_wall_time()))
    {
        return Globus::GRAM::Error::INVALID_MAX_WALL_TIME;
    }
    elsif ($description->max_queue_time() !=
        int($description->max_queue_time()))
    {
        $self->respond({GT3_FAILURE_MESSAGE => "Invalid max_queue_time"});

        return Globus::GRAM::Error::INVALID_ATTR;
    }
    $self->{sequence}++;
    $pending_time = $now;
    $active_time = $pending_time + int($description->max_queue_time);
    $done_time = $active_time + int($description->max_wall_time);

    $jobid = sprintf("%.63s", "$$".$self->{sequence}.".$now");

    if (!open($fh, ">>$job_dir/fakejob.log"))
    {
        $self->respond({GT3_FAILURE_MESSAGE => "Unable to write job file"});
        return Globus::GRAM::Error::INVALID_SCRIPT_STATUS;
    }
    print $fh "$jobid;$pending_time;$active_time;$done_time;$failed_time\n";
    close($fh);

    return { JOB_STATE => Globus::GRAM::JobState::PENDING,
             JOB_ID => $jobid };
}
