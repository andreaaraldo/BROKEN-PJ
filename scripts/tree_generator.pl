#!/usr/bin/perl
use Getopt::Long;
$l = shift;
$d = shift;
$N = ($d**$l -1)/($d-1);
$p_uncle = 1;
$p_broth = 1;

my $clients = '';
GetOptions('clients'=> \$clients);

@T = 0..$N-1;
push @queue, shift @T;
$father{0} = -1;
while (@queue){
    $current = shift @queue;
    $i =0;
    while ($i<$d && @T){
	$next = shift @T;
	push @{$childs{$current}}, $next;
	$father{$next} = $current;
	push @queue, $next;
	$i++;
    }
    push @leafs, $current if $i ==0;
}


foreach $n (0..$N-1){
    #Get my brothers
    $f = $father{$n};
    @father_sons= @{$childs{$f}}; #@cs are the father's sons

    foreach $b (@father_sons){
	push @{$brothers{$n}}, $b if $b != $n;
    }
    #Get the uncles of my sons
    @my_sons = @{$childs{$n}};
    foreach $s (@my_sons){
	@{$uncles{$s}} = @{$brothers{$n}};
    }
}

#&generate_clients() if $clients;
&generate_core();
print "//Uncle-nephew redundancy\n";
#&redundancy_level();
print "//Brothers redundancy\n";
&redundancy_peer();


sub generate_core(){
    print qq|//Tree core|,"\n";
    my $i;
    for $i( sort{$a<=>$b} keys %childs ) { 

	foreach $j ( @{ $childs{$i} }){
	    print "node[$i].face++ <--> { delay = 1ms; } <-->node[$j].face++;\n";
	}

    }
}

sub redundancy_peer(){
    $n=1;
    while ($n < $N-1){
	foreach $b (@{$brothers{$n}}){
	    print "node[$n].face++ <--> { delay = 1ms; } <-->node[$b].face++;\n" if rand() < $p_broth;
	}
	$n+=$d;
    }
}

sub redundancy_level(){
    foreach $n (0..$N-1){
	foreach $u (@{$uncles{$n}}){
	    #print $u,"\n";
	    print "node[$n].face++ <--> { delay = 1ms; } <-->node[$u].face++;\n" if rand() < $p_uncle;
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
    





