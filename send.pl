use IO::Socket::INET;
 
my ($socket,$client_socket);
$socket = new IO::Socket::INET (
    PeerHost => '192.168.1.53',
    PeerPort => '23',
    Proto => 'tcp',
) or die "ERROR in Socket Creation : $!\n";
 
print "TCP Connection Success.\n";
 
# send command
my $data = "227,89,35,55,142,237,48,109,91,82,45,95,145,249,133,79,14,72,48,95,37,135,50,109,39,74,48,95,119,230,132,79,4,64,35,18,137,174,124,6,11,253,43,26,145,182,132,14,19,5,51,34,153,190,140,22,27,13,59,42,161,198,148,30,35,21,67,50,169,206,121,18,22,7,53,51,169,205,154,19,22,7,52,50,169,205,155,70,75,162\r\n";
my $array = split(",", $data);
$socket->send($data);
print $data;
sleep (1);
$socket->recv($data,1024);
print "Received from Server : $array\n";
$socket->close();
print "socket closed\n";