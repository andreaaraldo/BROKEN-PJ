#!/usr/bin/perl
use Getopt::Long;
$N = shift;
$d = shift;

my $clients = '';
GetOptions('clients'=> \$clients);

@T = 0..$N-1;
push @queue, shift @T;
while (@queue){
    $current = shift @queue;
    $i =0;
    while ($i<$d && @T){
	$next = shift @T;
	push @{$neighbors{$current}}, $next;
	push @queue, $next;
	$i++;
    }
    push @leafs, $current if $i ==0;
}

&generate_core();
&generate_clients() if $clients;


sub generate_core(){
    print qq|//Tree core|,"\n";
    my $i;
    for $i( sort{$a<=>$b} keys %neighbors ) { 

	foreach $j ( @{ $neighbors{$i} }){
	    print "node[$i].face++ <--> { delay = 1ms; } <-->node[$j].face++;\n";
	}

    }
}

sub generate_clients(){
    print "\n";
    print qq|//Clients at leaves|, "\n";
    my $k=0;
    for $l (@leafs){
	#node[i].face++ <--> { delay = 1ms; } <--> client[i].client_port;
	print "node[$l].face++<--> { delay = 1ms; } <-->client[$k].client_port;\n";
	$k++;
    }
}
    





