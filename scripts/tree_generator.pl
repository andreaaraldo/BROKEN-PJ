#!/usr/bin/perl
use Getopt::Long;
$l = shift;
$d = shift;
$N = ($d**$l -1)/($d-1);

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

&generate_clients() if $clients;
&generate_core();


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
    my $k=0;
    print "num_clients = ", $#leafs+1,";\n";
    print "node_clients = \"";
    $"=",";
    print "@leafs\";\n";
}
    





