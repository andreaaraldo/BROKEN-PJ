#!/usr/bin/perl

# Usage:
#	<script_name>.pl <tree_filename> <levels> <arity>
# The tree file should be the output of tree_generator_flexible.pl

$tree_filename = shift;
$l = shift; # levels of the tree
$d = shift; # arity of the tree (how many child each non-leaf node has)

$distance_table_file = "/tmp/distance_table.txt";

$N = ($d**$l -1)/($d-1); # number of nodes



use Cwd;
my $dir=getcwd;
$command = "mv -f $distance_table_file $distance_table_file.old";
system($command);

@nodes=0..$N-1;
foreach(@nodes)
{
	$node=$_;
	$command="cat $tree_filename | $dir/dijkstra.pl $node >> $distance_table_file";
	system($command);
}

$command = "$dir/redundant_tree_metrics.r $distance_table_file $l $d";
system($command);
