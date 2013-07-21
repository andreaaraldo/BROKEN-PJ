#!/usr/bin/perl

# Usage:
#	<script_name>.pl <tree_filename> <higher_node>

$tree_filename = shift;
$higher_node = shift;


use Cwd;
my $dir=getcwd;

@nodes=0..$higher_node;
foreach(@nodes)
{
	$node=$_;
	$command="cat $tree_filename | $dir/dijkstra.pl $node";
	system($command);
}
