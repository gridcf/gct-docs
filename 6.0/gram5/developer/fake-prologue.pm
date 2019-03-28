use Globus::GRAM::Error;
use Globus::GRAM::JobState;
use Globus::GRAM::JobManager;
use Globus::Core::Paths;
use Globus::Core::Config;
use File::Path;
use strict;
use warnings;

package Globus::GRAM::JobManager::fake;

our @ISA = ('Globus::GRAM::JobManager');
