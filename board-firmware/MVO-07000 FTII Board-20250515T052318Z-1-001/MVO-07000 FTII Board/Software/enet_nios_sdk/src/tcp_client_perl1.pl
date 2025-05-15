#!perl

# tcp_client_perl1
#use socket;
use IO::Socket;
my ($host, $port, $kidpid, $handle, $line);

$host = "169.254.96.254";
$port = "20000";


$no_of_attempts = 1;

again:
#for($xx = 0; $xx < 20000; $xx++){$junk = $xx;} #delay
if ($no_of_attempts == 100001) {goto ext;}

$handle = IO::Socket::INET->new(
		Proto => "tcp",
		PeerAddr => $host,
		PeerPort => $port,
		)
	    #or die "cannot connect";
		or $no_of_attempts++, goto again;

print "It took $no_of_attempts try(s) to get a connection, but we got one!!!\n";

$handle->autoflush(1);  # So output gets there right away.
print STDERR "[Connected to $host:$port]\n";

# Split the program into 2 processes, identical twins.
die "can't fork: $!" unless defined($kidpid = fork());
#$kidpid = fork();

# The if{} block only runs in the parent process.
if ($kidpid)
	{
	# Copy the socket to the standard output.
	while (defined ($line = <$handle>))
		{
		print STDOUT $line;
		}
	kill ("TERM", $kidpid);  # Send SIGTERM to child
	} 
# The else{} block only runs in the child process.
else
	{
	# copy standard input to the socket
	#while (defined ($line = <STDIN>))
	#	{
	#	print "This was the line just entered ${line} \n";
	#	print $handle $line;
	#	print "Just sent  ${line} to server\n";
	#	}
	
	#print ("OK to enter something \n");
	while ($line = <STDIN>)
		{
		#print "line = ${line} \n";
		print $handle $line;

		#$jctr++;
		#print "hello again ${jctr} \n";
		#for($xx = 0; $xx < 2000; $xx++){$junk = $xx;} #delay
		}
	
	}


ext:
if ($no_of_attempts == 100001) {print "Unable to connect, tried 100000 times\n\n";}
printf("Goodbye!!! from perl\n");

# end of file
