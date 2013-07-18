#!/usr/bin/perl

# Usage: 
# <script_name>.pl <node>

my $starting_node = shift;

# To activate or deactivate debug messages
$debug_msg=0;

#####################
# <aa> This script take in stdin a topology. The format of this topology must
# be the one returned by tree_generator.pl . 
# </aa>


print "Topology:\n" if $debug_msg;

#<aa><> means stdin </aa>
foreach (<>)
{
		if ( /node\[(.*)\].*node\[(.*)\]/)
		{
			setLink($1,$2);
			setLink($2,$1);
			print "$1--$2\n" if $debug_msg;
		}
}

dijkstra($starting_node);
printCost_tabular();
print "\n";

#subroutines

# <aa> Invocation form:
#			dijkstra(<origin>)
# The method calculate the minimum path and the distance between the node 
# <origin> and every other node </aa>
sub dijkstra{
    my ($origin) = @_;

    $node{$origin}{cost}=0;
    $node{$origin}{pred}=-1;
    @queue = ($origin);

    while (@queue){
       $x = pop @queue;
       foreach $y (keys %{$node{$x}->{neighbors}}){
	   if ($node{$y}{cost}>$node{$x}{cost}+$node{$x}{neighbors}{$y}){
	       $node{$y}{cost} = $node{$x}{cost}+$node{$x}{neighbors}{$y};
	       $node{$y}{pred} = $x;
	       push @queue, $y;
	   }
       }
	
    }
}
sub setLink{
    my ($s,$d) = @_;

    if (!defined $node{$s}) 
    {
		$node{$s}{pred}=-1;
		$node{$s}{cost}=10000;
    }	
    $node{$s}{neighbors}{$d}=1;
}

sub printPaths{
    foreach $n (sort keys %node){
	$current = $n;
	print $current;

	while ($node{$current}{pred} != -1){
	    print "<-", $node{$current}{pred};
	    $current = $node{$current}{pred};
	}
	print "\n"  if $debug_msg;

    }
}


sub printCost{
	print "Printing costs:\n"  if $debug_msg;
    my ($c,%table);
    foreach $n (sort keys %node){
	$c = $node{$n}{cost};
	print "node=@node, n=$n, cost=$c\n" if $debug_msg;
	push @{$table{$c}}, $n;
    }

    $,=":";
    foreach $cost( sort{$a<=>$b} keys %table){
	print $cost,"\t",@{$table{$cost}},"\n";
    }
}

sub printCost_tabular{
	print "Printing costs:\n"  if $debug_msg;
    my ($c,%table);
    foreach $n (sort keys %node){
	$c = $node{$n}{cost};
	print "$c ";
    }
}
