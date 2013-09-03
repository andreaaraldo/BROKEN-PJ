#!/usr/bin/perl

#Getting parameters
use Getopt::Long;
$l = shift;
$d = shift;

$N = ($d**$l -1)/($d-1);

my $net_name = 'tree';
my $p_uncle = 0;
my $p_bro = 0;

GetOptions('bro:f'=> \$p_bro,
           'unc:f'=> \$p_uncle,
           'name:s'=> \$net_name);




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

&generate_header();
&generate_core();
print "//Uncle-nephew redundancy\n";

&redundancy_level();
print "//Brothers redundancy\n";
&redundancy_peer();
&header_end();
&generate_levels();


sub generate_core(){
    print qq|//Tree core|,"\n";
    my $i;
    for $i( sort{$a<=>$b} keys %childs ) { 

	foreach $j ( @{ $childs{$i} }){
	    print "\tnode[$i].face++ <--> { delay = 1ms; } <-->node[$j].face++;\n";
	}

    }
}

sub redundancy_peer(){
    foreach $n(1..$N-1){
	foreach $b (@{$brothers{$n}}){
	    print "\tnode[$n].face++ <--> { delay = 1ms; } <-->node[$b].face++;\n" if rand() < $p_bro && $b>$n;
	}
    }
}

sub redundancy_level(){
    foreach $n (0..$N-1){
	foreach $u (@{$uncles{$n}}){
	    #print $u,"\n";
	    print "\tnode[$n].face++ <--> { delay = 1ms; } <-->node[$u].face++;\n" if rand() < $p_uncle;
	}
    }
}

sub generate_header(){
    my $num_clients = $#leafs+1;
    my $head_net = $net_name."_network";
    $"=",";
    print <<NED
package networks;
network $head_net extends base_network{
    parameters:
        //Number of ccn nodes
    	n = $N;
	//Number of repositories
	node_repos = "0";
	num_repos = 1;
	replicas = 1;
        //Number of clients
	num_clients = $num_clients;
	node_clients = "@leafs";
connections allowunconnected:
NED
}
    

sub header_end(){
    print "}\n";
}


sub generate_levels(){
    my %l,$c;
    $c = 0;
    $level{$c} = 0;
    $nl = 1;

    foreach $i (1..$l-1){
	$nl = $d*$nl;
	for $j ($c+1..$c+$nl){
	    $level{$j} = $i;
	}
	$c = $c+$nl;
    }
    $file_ini ="levels_tree_".$l."_".$d.".ini"; 
    open (INI,">$file_ini");
    foreach $m (sort {$a<=> $b}keys %level){
	print INI "**.node[$m].level=", "  ", $level{$m},"\n";
    }
    close INI;
}





