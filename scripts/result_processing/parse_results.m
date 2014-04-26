% result processing
global severe_debug = true;

per_seed_results = false;
out_folder="~/Dropbox/shared_with_servers/icn14_runs/";

priceratio_list={2,5,10};
possible_decisions={"lce", "fix0.1", "prob_cache", "fix0.01","costprob0.1","costprob0.01","fix1", "fix0",\
			 "costprob0","never","costprob0.02","fix0.0001", "costprob0.0002"};
decision_list={"fix0.01","costprob0.02"}; % The decision plocies that I want to plot
id_rep_list=1:5; # list of seeds
alpha_list = [1];
csize_list = {"1000"};
csize_to_write_list = {"1000"};

resultdir="~/software/ccnsim/results";
metric_list = {"p_hit", "total_cost", "per_request_cost", "hdistance", "expensive_link_utilization",\
						"client_requests", "decision_ratio"};

network="one_cache_scenario";
forwarding_="nrr";
replacement_="lru";
ctlg_="10\\^5"; 
ctlg_to_write_="1e5";

i = 1;

disp("YOU SHOULD CHECK IF THE FILE EXISTS");
############################################
##### PARSE FILES ##########################
############################################
for idx_csize = 1:length(csize_list)
	csize_ = csize_list{idx_csize};
	csize_to_write = csize_to_write_list{ idx_csize};
	for alpha_ = alpha_list
		for priceratio_idx = 1:length(priceratio_list)
			for decision_idx = 1:length(decision_list)
				for id_rep_ = id_rep_list
					priceratio_ = priceratio_list{priceratio_idx};
					decision_ = decision_list{decision_idx};

					filename = strcat(resultdir,"/",network,"/F-",forwarding_,"/D-",decision_,"/R-",replacement_,"/alpha-",num2str(alpha_),"/ctlg-",ctlg_,"/cachesize-",num2str(csize_),"/priceratio-",num2str(priceratio_),"/ccn-id",num2str(id_rep_),".sca");

					filename_list{i} = filename;

					decision{i} = decision_;
					forwarding{i} = forwarding_;
					replacement{i} = replacement_;
					alpha(i) = alpha_;
					ctlg{i} = ctlg_;
					csize{i} = csize_;
					priceratio{i} = priceratio_;
					id_rep(i) = id_rep_;

					string_to_search="p_hit\\[0\\] ";
					command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
					[status, output] = system(command,1);
					p_hit{i} = str2num(output);
					string_to_search="total_cost ";
					command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
					[status, output] = system(command,1);
					total_cost{i} = str2num(output);

					string_to_search="hdistance ";
					command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
					[status, output] = system(command,1);
					hdistance{i} = str2num(output);

					string_to_search="downloads\\[0\\] ";
					command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
					[status, output] = system(command,1);
					client_requests{i} = str2num(output);


					string_to_search="repo_load\\[3\\] ";
					command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
					[status, output] = system(command,1);
					cheap_link_load{i} = str2num(output);

					string_to_search="repo_load\\[4\\] ";
					command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
					[status, output] = system(command,1);
					expensive_link_load{i} = str2num(output);

					string_to_search="decision_yes\\[0\\] ";
					command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
					[status, output] = system(command,1);
					decision_yes{i} = str2num(output);

					string_to_search="decision_no\\[0\\] ";
					command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
					[status, output] = system(command,1);
					decision_no{i} = str2num(output);
					


					# CHECK RESULTS{
						if ( size(p_hit{i})!=[1 1] || size(total_cost{i})!=[1 1] || \
							 size(hdistance{i} )!=[1 1] \
								|| size(client_requests{i})!=[1 1] || size(cheap_link_load{i})!=[1 1] \
								|| size(expensive_link_load{i})!=[1 1] )

							priceratio_
							decision_
							disp(["p_hit=", num2str(p_hit{i}), "; total_cost=", \
									num2str(total_cost{i}), "; hdistance=",num2str(hdistance{i} ), \
									"; client_requests=",num2str(client_requests{i} )] );
							command
							error("Parsing error");
						endif
					# }CHECK RESULTS

					i++;
				endfor
			endfor
		endfor
	endfor %alpha for
endfor %csize for


############################################
##### MATRIX CONSTRUCTION ##################
############################################
for idx_csize = 1:length(csize_list)
	csize_ = csize_list{idx_csize};
	csize_to_write = csize_to_write_list{ idx_csize};

	for alpha_ = alpha_list
		seed_id = 1;

		# Initialize the matrix_over_seed{
			for idx_metric = 1:length(metric_list)
				matrix_over_seed_list{idx_metric} = [];
			endfor
		# }Initialize the matrix_over_seed


		column_names{1} = "priceratio";
		for idx_decision = 1:length(possible_decisions)
			column_names{ idx_decision+1 } = possible_decisions{ idx_decision };
		endfor

		for id_rep_ = id_rep_list
			# Compute the priceratio_column, using whatever decision_ 
			decision_ = decision_list{1};
			idx_pr =  strcmp(decision, decision_ ) & ( id_rep == id_rep_ ) & (alpha == alpha_) \ 
							& strcmp(csize, csize_);
			priceratio_column = cell2mat( priceratio(idx_pr) );

			# For each seed, the first column of each matrix must be the price ratio
			for idx_metric = 1:length(metric_list)
				metric_matrix_list{idx_metric} = priceratio_column';
			endfor

			for decision_idx = 1:length(possible_decisions)
				decision_ = possible_decisions{decision_idx};

				if any(strcmp(decision_,decision_list) )
					% I want to plot this decision policy
					idx =  strcmp(decision, decision_ ) & id_rep == id_rep_  & (alpha == alpha_) \ 
								& strcmp(csize, csize_);

					# CHECK{
						if severe_debug
							priceratio_column_for_check = cell2mat( priceratio(idx) );
							if any(priceratio_column_for_check !=  priceratio_column)
								error("price ratio is erroneous");
							endif
						endif
					# }CHECK

					for idx_metric = 1:length(metric_list)
						column_list{idx_metric} = [];
		
						switch ( metric_list{idx_metric} )
							case "p_hit"
								column_list{idx_metric} = cell2mat( p_hit(idx) );

							case "total_cost"
								column_list{idx_metric} = cell2mat( total_cost(idx) );

							case "per_request_cost"
								column_list{idx_metric} = \
									cell2mat( total_cost(idx) ) ./ cell2mat( client_requests(idx) );

							case "hdistance"
								column_list{idx_metric} = cell2mat( hdistance(idx) );

							case "expensive_link_utilization"
								column_list{idx_metric} = cell2mat( expensive_link_load(idx) ) ./ \
										( cell2mat( expensive_link_load(idx) ) + \
										  cell2mat( cheap_link_load(idx)) 	);

							case "client_requests"
								column_list{idx_metric} = cell2mat( client_requests(idx) );

							case "decision_ratio"
								column_list{idx_metric} = cell2mat( decision_yes(idx) ) ./ \
										( cell2mat( decision_yes(idx) ) + cell2mat( decision_no(idx) ));

							otherwise
								error(["metric ",metric_name," is not valid"]);
						endswitch

						# CHECK DIMENSIONS{
							if severe_debug
								if length(priceratio_column) != length(priceratio_list)
									disp("The files that are involved are");
									filename_list(idx_pr)
									priceratio_column
									error("Error in price ratio column");
								endif


								if size(metric_matrix_list{idx_metric},1) != length(column_list{idx_metric})
									matrix = metric_matrix_list{idx_metric}
									column = column_list{idx_metric}
									priceratio_column_for_check
									decision_
									error("Length of column MUST be equal to the row number of matrix");
								endif

								if size(metric_matrix_list{idx_metric},1) != length(priceratio_list) || length(column_list{idx_metric} ) != length(priceratio_list)
									alpha_
									priceratio_list
									column_list{idx_metric}
									metric_matrix_list{idx_metric}
									error("The number of rows in the matrix and the lenghth of priceratio_column must be equal to the number of price ratios");
								endif
							endif
						# }CHECK DIMENSIONS

					endfor

				else
					% I don't want to plot this decision policy => I will replace it with -1
					for idx_metric = 1:length(metric_list)
						column_list{idx_metric} = ones( 1, length(priceratio_list) ) * -1;
					endfor
				endif

				for idx_metric = 1:length(metric_list)
					% Add the column corresponding to decision_ to the matrices	
					metric_matrix_list{idx_metric} = \
						[ metric_matrix_list{idx_metric}, column_list{idx_metric}' ];
					% The matrix above is related to one seed only
				endfor
			endfor % decision loop

			for idx_metric = 1:length(metric_list)
				% Going along the rows of p_hit_matrix_over_seed, the price_ratios are changing
				% Going along the comumns of p_hit_matrix_over_seed, the decision policies are changing
				% Going along the 3rd dimension of p_hit_matrix_over_seed, the seeds are changing
				matrix_over_seed_list{idx_metric} = cat( 3,\
						matrix_over_seed_list{idx_metric}, metric_matrix_list{idx_metric});

				% CHECK MATRIX{
				if severe_debug
					if size(matrix_over_seed_list{idx_metric},1) != length(priceratio_list)
						alpha_
						priceratio_list
						matrix_over_seed_list{idx_metric}
						disp( ["metric=", metric_list{idx_metric} ] );
						error("The number of rows in the matrix must be equal to the number of price ratios");
					endif
				endif
				% }CHECK MATRIX
			endfor


			seed_id ++;
		endfor # id_rep for

		fixed_variables = { network; forwarding_; replacement_; alpha_; ctlg_; csize_;\
						id_rep_list };
		fixed_variable_names = {"network"; "forwarding"; "replacement"; "alpha"; "ctlg";"csize";\
						 "seed_list"};
		comment="";

		for idx_metric = 1:length(metric_list)
			% CHECK MATRIX{
				if severe_debug
					if size(matrix_over_seed_list{idx_metric},1) != length(priceratio_list) || size(matrix_over_seed_list{idx_metric},2) != length(column_names)
						alpha_
						priceratio_list
						matrix_over_seed = matrix_over_seed_list{idx_metric}
						metric = metric_list{idx_metric}
						error(["The number of rows in the matrix must be equal to the number of ",\
								"price ratios.The number of columns must match the column names"]);
					endif
				endif
			% }CHECK MATRIX

			[mean_matrix, conf_matrix] = confidence_interval(matrix_over_seed_list{idx_metric});
			mean_matrix_list{idx_metric} = mean_matrix;
			conf_matrix_list{idx_metric} = conf_matrix;

			% CHECK MATRIX{
				if severe_debug
					if size(mean_matrix,1) != length(priceratio_list) || size(conf_matrix,1) != length(priceratio_list) || size(mean_matrix,2) != length(column_names) || size(conf_matrix,2) != length(column_names)
						alpha_
						priceratio_list
						mean_matrix
						conf_matrix
						metric_list{idx_metric}
						error(["The number of rows in the matrix must be equal to the number of ",\
								"price ratios.The number of columns must match the column names"]);
					endif
				endif
			% }CHECK MATRIX
		endfor


		for idx_metric = 1:length(metric_list)
			metric = metric_list{idx_metric};

			% Print mean matrix
			matrix = mean_matrix_list{idx_metric};
			out_filename = [out_folder,metric,"-mean-alpha_",num2str(alpha_),\
						"-ctlg_",ctlg_to_write_,"-csize_",csize_,".dat"];
			print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);

			% Print confidence interval matrix
			matrix = conf_matrix_list{idx_metric};;
			out_filename = [out_folder,metric,"-conf-alpha_",num2str(alpha_),\
						"-ctlg_",ctlg_to_write_,"-csize_",csize_,".dat"];
			print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);
		endfor

	endfor % alpha
endfor %csize
