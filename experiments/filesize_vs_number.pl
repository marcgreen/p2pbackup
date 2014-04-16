#!/usr/bin/perl

use warnings;
use strict;
use 5.014;

# Interact with BTBackup shell
use Expect::Simple;

# Globals
my $Client_exe = 'btbackup';
my $Git_original_branch = 'master';
my $Btbackup_dir = '/home/user/.btbackup';

# Git branch should raise minimum obligated storage to 10gb
my $Git_test_branch = 'filesize_vs_number';
my $Interface = 'eth0';
my $Bandwidth = 100_000; # Kilobits/sec

# Names of each file of specified size
my %Files = (10gbfile => '10gbfile',
	     1gbfile => '1gbfile', 
	     100mbfile => '100mbfile',
	     10mbfile => '10mbfile',
	     1mbfile => '1mbfile');

# Switch to relevant test scenario branch
git_checkout_branch($Git_test_branch);

# Make the bandwidth 100Mb for every node (realistic setting unneccesary)
constrain_bandwidth($Bandwidth, $Bandwidth);

# Run the instances! Each will send it's log to the syslog server
run_10g_instance();
run_1g_instance();
run_100m_instance();
run_10m_instance();
run_1m_instance();

# Remove bandwidth constraints
unconstrain_bandwidth();

# Switch back to original git branch
git_checkout_branch($Git_original_branch);

# constrain_bandwith(downlink, uplink), specified in Kb/s
sub constrain_bandwidth {
    die unless $#_ == 1; # both args required
    my ($downlink, $uplink) = @_;

    system("wondershaper $Interface $downlink $uplink");
    say "Bandwidth constrained to $downlink/$uplink on $Interface";
}

sub unconstrain_bandwidth {
    system("wondershaper clear $Interface");
    say "Bandwidth constraints cleared on $Interface";
}

sub git_checkout_branch {
    my $br = pop;

    say "Switching git branch to $br";
    system("git checkout $br");
}

sub delete_btbackup_dir {
    say "Deleting $Btbackup_dir";
    system("rm -r $Btbackup_dir");
}

sub run_10gb_instance {
    # Delete .btbackup/ to start clean
    delete_btbackup_dir();

    # Run BTBackup
    my $console = new Expect::Simple { 
	Cmd => $Client_exe,
	Prompt => '>',
	DisconnectCmd => 'exit', };

    print $console->before;

    # TODO sleep so all nodes get the chance to join network?
    # note: all nodes will be running each instance ASAP, and will
    # transition to the next instance immediately. is this what we want?

    say "Backing up 10gb file. Output of BTBackup follows:\n";
    $console->send("backup $Files{'10gbfile'}");
    print $console->before;

    # Take note of start time

    say "\n\nRecovering file:";
    $console->send("recover ???"); # TODO

    # TODO log download speed

    print $console->before;

    # Take note of end time

    # Send data to log server
    
    # Clean up
    delete_btbackup_dir();
}
