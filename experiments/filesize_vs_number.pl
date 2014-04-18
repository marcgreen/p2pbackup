#!/usr/bin/perl
# TODO I haven't been able to actually run this code at all. It may be buggy.
# You may want to review it manually to make sure there aren't logic errors.

use warnings;
use strict;
use 5.014;

use LWP::Simple; # GET request
use Expect::Simple; # Interact with BTBackup shell
use JSON;

# notes for dan:
# I've been installing things on 192.168.202.230, so use that for cloning.
#
# 'cpanm' is installed there, which is a nice way to download/install perl
# modules. Run `sudo cpanm Module::Name` in a terminal to install modules.
# I forget if I've install JSON, so if not, run `sudo cpanm JSON`
#

# Globals
my $BTSync_api_url = 'http://mqp:btsync@127.0.0.1:8888/api';
my $Client_exe = 'btbackup';
my $Btbackup_dir = '/home/user/.btbackup';
my $Output_file = 'instance-output';
my $Log_file = 'instance-log';

# TODO Git test branch should raise minimum obligated storage to 10gb
my $Git_test_branch = 'filesize_vs_number';
my $Git_original_branch = 'master';
my $Interface = 'eth0';
my $Bandwidth = 100_000; # Kilobits/sec

if ($#ARGV != 1) die "Usage: $0 file-to-back-up number-of-times-to-back-it-up-per-node";
my ($file, $num_files) = @ARGV;

# Switch to relevant test scenario branch
git_checkout_branch($Git_test_branch);

# Make the bandwidth 100Mb for every node (realistic setting unneccesary)
# TODO don't constrain upload bandwidth b/c that will just make the test take longer,
#   and it doesn't effect the results
constrain_bandwidth($Bandwidth, $Bandwidth);

# Run the instance! Will send log to the syslog server
run_instance($file, $num_files);

# Remove bandwidth constraints
unconstrain_bandwidth();

# Switch back to original git branch
git_checkout_branch($Git_original_branch);

say "Test scenario completed. Logs sent to syslog server.";

sub run_instance {
    my ($filename, $num_files) = @_;

    # Delete .btbackup/ to start clean
    delete_btbackup_dir();

    # Run BTBackup
    my $console = new Expect::Simple { 
	Cmd => $Client_exe, # Look up Expect::Simple on CPAN to see how to pass in cmd line arguments
	Prompt => '>',
	DisconnectCmd => 'exit', };

    print $console->before; # Prints everything ./btbackup output to STDOUT before the most recent prompt

    say "Backing up $filename $num_files times. " .
	"Output of BTBackup written to '$Output_file'.\n" .
	"Log written to '$Log_file'.\n";

    # Backup the file(s)
    say "Issuing backup command(s).";
    $console->send("backup $filename") for 1..$num_files;

    # Wait until backup is done to start recovery
    my $upload_started = 0;
    while (!$upload_started || get_data_left('upload') != 0) {
	if (!$upload_started) {
	    say "Waiting for upload to start.";
	    while (get_data_left('upload') == 0) {}
	    $upload_started = 1;
	    say "Upload started, now waiting for it to end.";
	}
    }
    say "Upload finished.";

    # Log output of backup
    open (my $output_fh, '>>' $Output_file) or die "Err opening output file: $!";
    say $output_fh "\n";
    print $output_fh $console->before;

    # Start downloading file
    say "\n\nRecovering file(s)";
    # TODO need to quit network and rejoin with command line argument
    # See Expect::Simple docs for how to quit running process and pass in arguments
    # $console->send("recover ???");
    print $output_fh $console->before;
    close $output_fh;

    open (my $log_fh, '>>', $Log_file)or die "Error opening log flie: $!";
    # Log speed until download finishes
    my $download_started = 0;
    while (!$download_started || get_data_left('download') != 0) {
	if (!$download_started) {
	    say "Waiting for download to start..";
	    while (get_data_left('download') == 0) {}
	    $download_started = 1;
	    say "Download started, now waiting for it to end.";
	}

	# Get download speed
	my ($time, $meridian, $rxps) = `sar -n DEV 1 1 | grep $Interface | grep -v Average | awk '{print $1, $2, $6;}'`;
	print $log_fh "$time $meridian $rxps\n";
	print "$time $meridian $rxps\n";
    }
    close $log_fh;
    say "Download finished.";

    # Send data to log server
    # TODO

    # Clean up
    delete_btbackup_dir();
}

# constrain_bandwith(downlink, uplink), specified in Kb/s
sub constrain_bandwidth {
    die unless $#_ == 1; # both args required
    my ($downlink, $uplink) = @_;

    system("wondershaper $Interface $downlink $uplink");
    say "Bandwidth constrained to [$downlink down / $uplink up] on $Interface";
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

sub get_data_left {
    my $direction = pop;
    die if ($direction ne 'upload' || $direction ne 'download');
    $data_left = 0;

    for my $folder (get_folders()) {
	my @peers = get_folder_peers($folder{'secret'});
	for my $peer (@peers) {
	    $data_left += $peer{$direction};
	}
    }

    return $data_left;
}

sub get_folders {
    my $json = get("${BTSync_api_url}?method=get_folders");
    return decode_json($json);
}

sub get_folder_peers {
    my $secret = pop;
    my $json = get("${BTSync_api_url}?method=get_folder_peers" .
	"&secret=$secret");
    return decode_json($json);
}
