% metric_vs_priceratio
function y = metric_vs_x_variable (input_data)
	global severe_debug;

	% Unroll input data
	out_folder = input_data.out_folder;

	x_variable_name = input_data.x_variable_name;
	x_variable_values = input_data.x_variable_values;
	

	possible_decisions = input_data.possible_decisions;
	decision_list = input_data.decision_list; % The decision plocies that I want to plot
	id_rep_list = input_data.id_rep_list; # list of seeds
	alpha_list = input_data.alpha_list;
	csize_list = input_data.csize_list;
	csize_to_write_list = input_data.csize_to_write_list;

	resultdir = input_data.resultdir;
	metric_list = input_data.metric_list;

	network = input_data.network;
	forwarding_ = input_data.forwarding_;
	replacement_ = input_data.replacement_;
	ctlg_ = input_data.ctlg_; 
	ctlg_to_write_ = input_data.ctlg_to_write_;

	fixed_variable_names_additional = input_data.fixed_variable_names_additional;
	fixed_variable_values_additional = input_data.fixed_variable_values_additional;

	filename_list = input_data.parsed.filename_list;
	decision = input_data.parsed.decision;
	xi = input_data.parsed.xi;
	forwarding = input_data.parsed.forwarding;
	replacement = input_data.parsed.replacement;
	alpha = input_data.parsed.alpha;
	ctlg = input_data.parsed.ctlg;
	csize = input_data.parsed.csize;
	priceratio = input_data.parsed.priceratio;
	id_rep = input_data.parsed.id_rep;
	p_hit = input_data.parsed.p_hit;
	total_cost = input_data.parsed.total_cost;
	hdistance = input_data.parsed.hdistance;
	client_requests = input_data.parsed.client_requests;
	cheap_link_load = input_data.parsed.cheap_link_load;
	expensive_link_load = input_data.parsed.expensive_link_load;
	decision_yes = input_data.parsed.decision_yes;
	decision_no = input_data.parsed.decision_no;

	% CHECK_INPUT_DATA{
		if severe_debug
			if length(fixed_variable_names_additional) != 2 || length(fixed_variable_values_additional) != 2
				fixed_variable_names_additional
				fixed_variable_values_additional
				error("Only two additional fixed variables are admitted for now");
			endif
		endif
	% }CHECK_INPUT_DATA



	############################################
	##### MATRIX CONSTRUCTION ##################
	############################################
	for idx_csize = 1:length(csize_list)
		csize_ = csize_list{idx_csize};
		csize_to_write = csize_to_write_list{ idx_csize};

			seed_id = 1;

			# Initialize the matrix_over_seed{
				for idx_metric = 1:length(metric_list)
					matrix_over_seed_list{idx_metric} = [];
				endfor
			# }Initialize the matrix_over_seed


			column_names{1} = x_variable_name;
			for idx_decision = 1:length(possible_decisions)
				column_names{ idx_decision+1 } = possible_decisions{ idx_decision };
			endfor

			for id_rep_ = id_rep_list
				# Compute the x_variable_column, using whatever decision_ 
				decision_ = decision_list{1};
				idx_pr =  strcmp(decision, decision_ ) & ( id_rep == id_rep_ ) & strcmp(csize, csize_);
				for idx_fixed_variable_additional = 1:length(fixed_variable_names_additional)
					% CHECK{
						if severe_debug
							if ! isscalar(fixed_variable_values_additional(idx_fixed_variable_additional) )
								fixed_values = \
									fixed_variable_values_additional(idx_fixed_variable_additional)
								fixed_variable_name = \
									fixed_variable_names_additional(idx_fixed_variable_additional)
								error("The fixed value MUST be a scalar");								
							endif
						endif
					% }CHECK
					value = fixed_variable_values_additional(idx_fixed_variable_additional){1};
					idx_pr = idx_pr \
						& eval(fixed_variable_names_additional(idx_fixed_variable_additional) )\
						== value;
				endfor

				x_variable_column = x_variable_values;

				% CHECK{
					if severe_debug
						original_data = cell2mat(eval(x_variable_name) ); 
						extracted_column = original_data(idx_pr);
						if x_variable_column != extracted_column
							x_variable_column
							extracted_column
							error("x_variable_column and x_variable_column MUST match");
						endif
					endif
				% }CHECK

				# For each seed, the first column of each matrix must be the x_variable_column
				for idx_metric = 1:length(metric_list)
					metric_matrix_list{idx_metric} = x_variable_column';
				endfor

				for decision_idx = 1:length(possible_decisions)
					decision_ = possible_decisions{decision_idx};

					if any(strcmp(decision_,decision_list) )
						% I want to plot this decision policy
# CANCELLARE?
#						idx =  strcmp(decision, decision_ ) & id_rep == id_rep_ & strcmp(csize, csize_);
#						for idx_fixed_variable_additional = 1:length(fixed_variable_names_additional)
#							idx = idx \
#								& cell2mat( eval(fixed_variable_names_additional(idx_fixed_variable_additional) ) )\
#								== fixed_variable_values_additional(idx_fixed_variable_additional)  \ 
#						endfor

						idx = idx_pr;
						# CHECK{
							if severe_debug
								original_data = eval(x_variable_name);
								x_variable_column_for_check = cell2mat( original_data(idx) );
								if x_variable_column_for_check !=  x_variable_column
									error("x_variable_column is erroneous");
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
									if size(metric_matrix_list{idx_metric},1) != length(column_list{idx_metric})
										matrix = metric_matrix_list{idx_metric}
										column = column_list{idx_metric}
										x_variable_column_for_check
										decision_
										error("Length of column MUST be equal to the row number of matrix");
									endif

									if size(metric_matrix_list{idx_metric},1) != length(x_variable_values) || length(column_list{idx_metric} ) != length(x_variable_values)
										fixed_variable_names_additional
										fixed_variable_values_additional
										x_variable_values
										column_list{idx_metric}
										metric_matrix_list{idx_metric}
										error("The number of rows in the matrix and the lenghth of x_variable_column must be equal to the number of price ratios");
									endif
								endif
							# }CHECK DIMENSIONS

						endfor

					else
						% I don't want to plot this decision policy => I will replace it with -1
						for idx_metric = 1:length(metric_list)
							column_list{idx_metric} = ones( 1, length(x_variable_values) ) * -1;
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
						if size(matrix_over_seed_list{idx_metric},1) != length(x_variable_values)
							fixed_variable_names_additional
							fixed_variable_values_additional
							x_variable_values
							matrix_over_seed_list{idx_metric}
							disp( ["metric=", metric_list{idx_metric} ] );
							error("The number of rows in the matrix must be equal to the number of price ratios");
						endif
					endif
					% }CHECK MATRIX
				endfor


				seed_id ++;
			endfor # id_rep for

			fixed_variables = { network; forwarding_; replacement_;	ctlg_; csize_; id_rep_list };

			fixed_variable_names = {"network"; "forwarding"; "replacement"; "ctlg";"csize"; "seed_list"};

			for idx_fixed_variable_additional = 1:length(fixed_variable_names_additional)
				fixed_variables{ length(fixed_variables) + idx_fixed_variable_additional, 1 } =\
							fixed_variable_values_additional{idx_fixed_variable_additional};
				fixed_variable_names{ length(fixed_variable_names) + idx_fixed_variable_additional, 1 } =\
							fixed_variable_names_additional{idx_fixed_variable_additional};
			endfor
			comment="";

			for idx_metric = 1:length(metric_list)
				% CHECK MATRIX{
					if severe_debug
						if size(matrix_over_seed_list{idx_metric},1) != length(x_variable_values) || size(matrix_over_seed_list{idx_metric},2) != length(column_names)
							fixed_variable_names_additional
							fixed_variable_values_additional
							x_variable_values
							matrix_over_seed = matrix_over_seed_list{idx_metric}
							metric = metric_list{idx_metric}
							error(["The number of rows in the matrix must be equal to the number of ",\
									"x variable.The number of columns must match the column names"]);
						endif
					endif
				% }CHECK MATRIX

				[mean_matrix, conf_matrix] = confidence_interval(matrix_over_seed_list{idx_metric});
				mean_matrix_list{idx_metric} = mean_matrix;
				conf_matrix_list{idx_metric} = conf_matrix;

				% CHECK MATRIX{
					if severe_debug
						if size(mean_matrix,1) != length(x_variable_values) || size(conf_matrix,1) != length(x_variable_values) || size(mean_matrix,2) != length(column_names) || size(conf_matrix,2) != length(column_names)
							fixed_variable_names_additional
							fixed_variable_values_additional
							x_variable_values
a							mean_matrix
							conf_matrix
							metric_list{idx_metric}
							error(["The number of rows in the matrix must be equal to the number of ",\
									"x variable.The number of columns must match the column names"]);
						endif
					endif
				% }CHECK MATRIX
			endfor


			for idx_metric = 1:length(metric_list)
				metric = metric_list{idx_metric};

				common_out_filename = [ out_folder, metric,"_vs_", x_variable_name];
				for idx_fixed_variable_additional = 1:length(fixed_variable_names_additional)
					value = fixed_variable_values_additional(idx_fixed_variable_additional){1};
					common_out_filename = [common_out_filename, "-",\
						fixed_variable_names_additional{idx_fixed_variable_additional}, "_", num2str(value)\
						];
				endfor
				common_out_filename = [common_out_filename, "-ctlg_",ctlg_to_write_,"-csize_",csize_ ];

				% Print mean matrix
				matrix = mean_matrix_list{idx_metric};
				out_filename = [common_out_filename,"-mean.dat"];
				print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);

				% Print confidence interval matrix
				matrix = conf_matrix_list{idx_metric};;
				out_filename = [common_out_filename,"-conf.dat"];
				print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);
			endfor

	endfor %csize
endfunction
