filename = "~/temp/distance.txt";
arity = 2;
levels = 3;

distance_ = read.table(filename, header = FALSE, sep = " ", skip = 0);
distance <- as.matrix(distance_[, 1: dim(distance_)[2]-1 ], 
                      rownames.force=NA, colnames.force=NA);
colnames(distance) = NULL;

leaf_number = arity^(levels-1);

only_leaf_subset = distance[ (dim(distance)[1] -leaf_number+1) : dim(distance)[1] , ];

# Compute the average distance between the leafs and all the other nodes
average_distance = mean(only_leaf_subset);

distance_from_root = only_leaf_subset[,1];
distance_from_root_matrix = t ( tcrossprod(rep(1,dim(distance)[1]),distance_from_root) );
distance_compared_with_distance_to_root = only_leaf_subset - distance_from_root_matrix;

