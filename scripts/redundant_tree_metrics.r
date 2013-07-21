#!/usr/bin/Rscript

# Attention: this script is intended to be called by  redundant_tree_metrics.pl. Usage
#   <script_name> <distance_table_filename> <levels> <arity>

args = commandArgs(trailingOnly=TRUE);
if(length(args)!=3)
    stop("Missing arguments. Check the usage");

filename = args[1];
levels = as.integer(args[2] );
arity = as.integer(args[3] );


distance_ = read.table(filename, header = FALSE, sep = " ", skip = 0);
distance <- as.matrix(distance_[, 1: dim(distance_)[2]-1 ], 
                      rownames.force=NA, colnames.force=NA);
colnames(distance) = NULL;

leaf_number = arity^(levels-1);

only_leaf_subset = distance[ (dim(distance)[1] -leaf_number+1) : dim(distance)[1] , ];

# Compute the average distance between the leafs and all the other nodes
average_distance = mean(only_leaf_subset);

# Compute the average number of nodes closer than root
distance_from_root = only_leaf_subset[,1];
distance_from_root_matrix = t ( tcrossprod(rep(1,dim(distance)[1]),distance_from_root) );
distance_compared_with_distance_to_root = only_leaf_subset - distance_from_root_matrix;

closer_nodes = NULL;
for (i in 1:dim(distance_compared_with_distance_to_root)[1] ){
    row = distance_compared_with_distance_to_root[i,];
    closer_distances = subset(row, row<0);
    
    if(i==1){ closer_nodes = c(length(closer_distances) ); }
    else{ closer_nodes = cbind( closer_nodes, length(closer_distances) ); }
}

cat(average_distance)
cat("\n");
cat( mean(closer_nodes) )
cat("\n");