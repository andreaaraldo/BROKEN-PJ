#!/usr/bin/perl

# Usage: 
# <script_name>.pl <levels> <arity> --clients <number_of_clients>

use Getopt::Long;

$l = shift; # levels of the tree
$d = shift; # arity of the tree (how many child each non-leaf node has)
$p_sibling = shift;
$p_uncle = shift;


$N = ($d**$l -1)/($d-1); # number of nodes

# To activate or deactivate debug messages
$debug_msg =0;

$seed = 1;
# srand($seed);
srand();

my $clients = '';
GetOptions('clients'=> \$clients);

@T = 0..$N-1; # identifiers of the node


#################################################################
########## Finding the links ####################################
#################################################################

# We will insert in @queue the node to which we are going to assign its children
push @queue, shift @T; # Initialize @queue with the root node (node 0)

@uncles = ();

$level = 0;
$nodes_before_the_current_level = 0;

while (@queue)
{
    $current = shift @queue; # pop the first element of @queue
    
    if ($current != 0){
	    $reminder = ($current % $d);
	    $leftmost_sibling = $current - $reminder + 1;
	    $leftmost_sibling = $current - $d + 1  if ($reminder == 0);
	    print "\nThe leftmost sibling is ",$leftmost_sibling if $debug_msg;
	    @uncles = $leftmost_sibling..($leftmost_sibling+$d-1);
    }
    
    print "\nThe brothers of ",$current," are ", @uncles,"\n" if $debug_msg;
    
    # Initialize siblings to a void vector
    @siblings = ();

    # The children of $current node are the next d nodes
    $i =0;
    while ($i<$d && @T)
    {
	$next = shift @T; # Take one by one the d nodes following $current

	# $next is a child of $current, i.e. there is a link between $current 
	# and $next. In other words, $next is a neighbor of current
	push @{$neighbors{$current}}, $next;

	push @queue, $next; # Remember that we have yet to assign children to $next

	# Consider each already visited sibling, and associate it to $next with  a 
	# probability p_sibling
	foreach(@siblings){
		$probability = $p_sibling;
		push @{ $neighbors{$_} }, $next if binary_decision();
	}

	# Consider each uncle, and associate it to $next with  a probability p_uncle
	foreach(@uncles){
		$probability = $p_uncle;
		if ( 	binary_decision() && 
			# Adding a link between $next and $_ is not possible if they are
			# already neighbors (i.e. $_ is the father of $next)
			!grep( /^$next$/, @{ $neighbors{$_} } ) 
		)
		{	push @{ $neighbors{$_} }, $next; }
	}

	push @siblings, $next;
	$i++;
    }

    print "\nchildren of ",$current,":",@siblings,"\n" if $debug_msg;

    # If $i==0, when selected $current @T is void. This measns that we already encountered
    # all the nodes ==> every node has already its own parent ==> 
    # there are no more nodes that can be child of $current ==> $current is a leaf
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
    print "\nQuel est votre projet d'études (UFR, titre du diplôme, stage,...) ? ";
    my $k=0;
    print "num_clients = ", $#leafs+1,";\n";
    print "node_clients = \"";
    $"=",";
    print "@leafs\";\n";
}
    
sub binary_decision(){
    # $probability must be defined
    return 1 if( rand() le $probability );
    return 0;
}

sub get_level(){
    # $node_id must be defined
    return int( log( $node_id ) / log($d) );
}

