our($job_dir, $fake_seg_dir);

BEGIN
{
    my $config = new Globus::Core::Config(
        '${sysconfdir}/globus/globus-fake.conf');
    
    $job_dir = $fake_seg_dir = "";

    if ($config)
    {
        $job_dir = $config->get_attribute("log_path") || "";
    }
    if ($job_dir eq '')
    {
        $job_dir = Globus::Core::Paths::eval_path('${localstatedir}/fake');
    }
}
