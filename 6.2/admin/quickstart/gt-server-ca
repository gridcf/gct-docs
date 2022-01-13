#!/usr/bin/env perl

use strict;
use Getopt::Long;

my $simplecahash;
my $passwd;
my $yes;
my $rc;
my $hostname;
my $help;

GetOptions( 'y!' => \$yes,
            'pass=s' => \$passwd,
            'help|h' => \$help);

if ($help)
{
   print "Usage:\n";
   print "$0 -y [-pass=<pass>] [-help]\n";
   print "   -y: Confirm script execution.  Required.\n";
   print "   -pass: Supply a password.  ~/.globus/.simplecapass will\n";
   print "          be used if it exists.  If the file does not exist\n";
   print "          and a password is not supplied, the script will prompt\n";
   print "          for a password.  The password must be four or more characters\n";
   print "   -help: Display this help message.\n";
   exit 0;
}

if(!$yes)
{
    print "This script will destroy any hostcerts you may have in\n";
    print "\$GLOBUS_LOCATION/etc.  If this is ok, \n";
    print "then run with the -y option.\n";

    exit 8;
}

my $passwdok = 0;
open(PASSFILE, "<$ENV{HOME}/.globus/.simplecapass");
if ( $? eq 0 )
{
   $passwd = <PASSFILE>;
   chomp $passwd;
   if ( length($passwd) >= 4 )
   {
      $passwdok = 1;
      print "Using password from $ENV{HOME}/.globus/.simplecapass\n";
   }
}
close(PASSFILE);

while(!$passwdok)
{
    print "Please enter a password of at least four characters for the CA: ";
    system("stty -echo");
    $passwd = <STDIN>;
    chomp($passwd);
    system("stty echo");

    if (length($passwd) < 4)
    {
        print "\nERROR: Password must be at least four characters\n";
        next;
    }

    print "\nConfirm password:";
    system("stty -echo");
    my $passwd_conf = <STDIN>;
    chomp($passwd_conf);
    system("stty echo");
    print "\n";

    if($passwd ne $passwd_conf)
    {
        print "ERROR: Passwords do not match\n";
    } else
    {
        $passwdok = 1;
    }
}

my $oldumask = umask;
umask 022;
if ( ! -d "$ENV{HOME}/.globus" )
{
   system("mkdir $ENV{HOME}/.globus");
   if ( $? ne 0 )
   {
       print "ERROR:  Unable to create $ENV{HOME}/.globus: $!";
       exit 4;
   }
}

umask 0377;
open(PASSFILE, ">$ENV{HOME}/.globus/.simplecapass");
if ( $? ne 0)
{
   print "ERROR: Unable to open $ENV{HOME}/.globus/.simplecapass for writing\n";
   exit 6;
}
print PASSFILE "$passwd";
close PASSFILE;

umask $oldumask;
       
my $logfile="gt-server-ca.log";
system("touch $logfile");

#
# all the envs should be set up now so we install the CA stuff
#
print "Creating a new simpleCA, logging to $logfile...";
$rc = system("/usr/bin/grid-ca-create -force -pass $passwd -noint > $logfile 2>&1");
if($rc != 0)
{
    print "grid-ca-create failed.  See $logfile for details.\n";
    exit 2;
}

my ($glob) = glob("/etc/grid-security/certificates/*.0");
$glob =~ s#/etc/grid-security/certificates/(.{8})\.0#$1#;
$simplecahash = $glob;

print "\nYour CA hash is: $simplecahash\n";
print "It is located at /etc/grid-security/certificates/${simplecahash}.0\n";
#
#  now request the cert
# 
$hostname = `globus-hostname`;
chomp $hostname;
   
system("grid-default-ca -ca $simplecahash >> $logfile 2>> $logfile");

system("grid-cert-request -ca $simplecahash -host $hostname -force >> $logfile 2>> $logfile");
if ( $? ne 0 )
{
    print "There was an error requesting a host certificate for $hostname.\n";
    print "Please see $logfile for details.\n";
    exit 10;
}

system("grid-ca-sign -in /etc/grid-security/hostcert_request.pem -out /etc/grid-security/hostcert.pem -passin pass:$passwd -force >> $logfile");
if ( $? ne 0 )
{
    print "There was an signing the request for $hostname.\n";
    print "Please see $logfile for details.\n";
    exit 11;
}

# Need -f when copying the key, else it errors out on overwriting a
# file with such strict permissions
#system("cp $GL/etc/hostcert.pem $GL/etc/containercert.pem");
#system("cp -f $GL/etc/hostkey.pem $GL/etc/containerkey.pem");

print "Your host DN is ";
system("grid-cert-info -subject -file /etc/grid-security/hostcert.pem");
print "The hostcert is located at /etc/grid-security/hostcert.pem\n";

