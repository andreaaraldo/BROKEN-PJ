% result processing
global severe_debug = true;

per_seed_results = false;
out_folder="~/Dropbox/shared_with_servers/icn14_runs/";

priceratio_list={1,2,3,4,5,6,7,8,9,10};
decision_list={"lce", "fix0.1", "prob_cache", "fix0.01","costprob0.1","costprob0.01"};
id_rep_list=1:20; # list of seeds
alpha_list = [0, 0.8, 1.2];

resultdir="~/software/ccnsim/results";
network="simple_scenario";
forwarding_="nrr";
replacement_="lru";
ctlg_="10\\^3"; 
ctlg_to_write_="1e3";
csize_="10";
csize_to_write="10";

i = 1;

disp("YOU SHOULD CHECK IF THE FILE EXISTS");
############################################
##### PARSE FILES ##########################
############################################
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

				string_to_search="p_hit ";
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
				# CHECK RESULTS{
					if ( size(p_hit{i})!=[1 1] || size(total_cost{i})!=[1 1] || size(hdistance{i} )!=[1 1] \
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


############################################
##### MATRIX CONSTRUCTION ##################
############################################

for alpha_ = alpha_list
	seed_id = 1;

	p_hit_matrix_over_seed=[];
	total_cost_matrix_over_seed=[];
	per_request_cost_matrix_over_seed=[];
	hdistance_matrix_over_seed=[];
	expensive_link_utilization_matrix_over_seed=[];
	client_requests_matrix_over_seed=[];


	column_names{1} = {"priceratio"};
	for idx_decision = 1:length(decision_list)
		column_names{ idx_decision+1 } = decision_list{ idx_decision };
	endfor

	for id_rep_ = id_rep_list
		# Compute the priceratio_column, using whatever decision_ 
		decision_ = decision_list{1};
		idx_pr =  strcmp(decision, decision_ ) & ( id_rep == id_rep_ ) & (alpha == alpha_);
		priceratio_column = cell2mat( priceratio(idx_pr) );

		p_hit_matrix = priceratio_column';
		total_cost_matrix = priceratio_column';
		per_request_cost_matrix = priceratio_column';
		hdistance_matrix = priceratio_column';
		expensive_link_utilization_matrix = priceratio_column';
		client_requests_matrix = priceratio_column';

		for decision_idx = 1:length(decision_list)
			decision_ = decision_list{decision_idx};
			idx =  strcmp(decision, decision_ ) & id_rep == id_rep_  & (alpha == alpha_);

			if severe_debug
				priceratio_column_for_check = cell2mat( priceratio(idx) );
				if any(priceratio_column_for_check !=  priceratio_column)
					error("price ratio is erroneous");
				endif
			endif

			p_hit_column = cell2mat( p_hit(idx) );
			total_cost_column = cell2mat( total_cost(idx) );
			per_request_cost_column = cell2mat( total_cost(idx) ) ./ cell2mat( client_requests(idx) );
			hdistance_column = cell2mat( hdistance(idx) );
			expensive_link_utilization_column = cell2mat( expensive_link_load(idx) ) ./ \
							( cell2mat( expensive_link_load(idx) ) + cell2mat( cheap_link_load(idx) ) );
			client_requests_column = cell2mat( client_requests(idx) );

			# CHECK DIMENSIONS{
			if severe_debug
				if length(priceratio_column) != length(priceratio_list)
					disp("The files that are involved are");
					filename_list(idx_pr)
					priceratio_column
					error("Error in price ratio column");
				endif


				if size(p_hit_matrix,1) != length(p_hit_column)
					p_hit_matrix
					p_hit_column
					priceratio_column_for_check
					decision_
					error("Length of p_hit_column MUST be equal to the row number of p_hit_matrix");
				endif

				if size(p_hit_matrix,1) != length(priceratio_list) || length(priceratio_column) != length(priceratio_list)
					alpha_
					priceratio_list
					p_hit_column
					p_hit_matrix
					error("The number of rows in the matrix and the lenghth of priceratio_column must be equal to the number of price ratios");
				endif
			endif

			# }CHECK DIMENSIONS
	
			p_hit_matrix = [p_hit_matrix, p_hit_column'];
			total_cost_matrix = [total_cost_matrix, total_cost_column'];
			per_request_cost_matrix = [per_request_cost_matrix, per_request_cost_column'];
			hdistance_matrix = [hdistance_matrix, hdistance_column'];
			expensive_link_utilization_matrix = \
						[expensive_link_utilization_matrix, expensive_link_utilization_column'];
			client_requests_matrix = [client_requests_matrix, client_requests_column'];

		endfor % decision for

		% Going along the rows of p_hit_matrix_over_seed, the price_ratios are changing
		% Going along the comumns of p_hit_matrix_over_seed, the decision policies are changing
		% Going along the 3rd dimension of p_hit_matrix_over_seed, the seeds are changing
		p_hit_matrix_over_seed = cat(3,p_hit_matrix_over_seed, p_hit_matrix);
		total_cost_matrix_over_seed = cat(3,total_cost_matrix_over_seed, total_cost_matrix);
		per_request_cost_matrix_over_seed = \
					cat(3,per_request_cost_matrix_over_seed, per_request_cost_matrix);
		hdistance_matrix_over_seed = cat(3,hdistance_matrix_over_seed, hdistance_matrix);
		expensive_link_utilization_matrix_over_seed =\
					cat(3,expensive_link_utilization_matrix_over_seed,\
								 expensive_link_utilization_matrix);
		client_requests_matrix_over_seed = \
					cat(3,client_requests_matrix_over_seed, client_requests_matrix);


		% CHECK MATRIX{
		if severe_debug
			if size(p_hit_matrix_over_seed,1) != length(priceratio_list)
				alpha_
				priceratio_list
				p_hit_matrix_over_seed
				error("The number of rows in the matrix must be equal to the number of price ratios");
			endif
		endif
		% }CHECK MATRIX


		if per_seed_results
			fixed_variables = { network; forwarding_; replacement_; alpha_; ctlg_; csize_; id_rep_ };
			fixed_variable_names = {"network"; "forwarding"; "replacement"; "alpha"; "ctlg";"csize"; "id_rep"};
			comment="";

			metric="p_hit";
			matrix = p_hit_matrix;
			out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),\
							"-ctlg_",ctlg_to_write_,"-seed_",num2str(id_rep_),".dat"];

			print_table(out_filename, matrix, column_names, fixed_variables,
															fixed_variable_names, comment);

			metric="total_cost";
			matrix = total_cost_matrix;
			out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),\
							"-ctlg_",ctlg_to_write_,"-seed_",num2str(id_rep_),".dat"];

			print_table(out_filename, matrix, column_names, fixed_variables,
															fixed_variable_names, comment);


			metric="per_request_cost";
			matrix = per_request_cost_matrix;
			out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),\
							"-ctlg_",ctlg_to_write_,"-seed_",num2str(id_rep_),".dat"];

			print_table(out_filename, matrix, column_names, fixed_variables,
															fixed_variable_names, comment);



			metric="hdistance";
			matrix = hdistance_matrix;
			out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),\
							"-ctlg_",ctlg_to_write_,"-seed_",num2str(id_rep_),".dat"];

			print_table(out_filename, matrix, column_names, fixed_variables,
															fixed_variable_names, comment);



			metric="expensive_link_utilization";
			matrix = expensive_link_utilization_matrix;
			out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),\
							"-ctlg_",ctlg_to_write_,"-seed_",num2str(id_rep_),".dat"];

			print_table(out_filename, matrix, column_names, fixed_variables,
															fixed_variable_names, comment);


			metric="client_requests";
			matrix = client_requests_matrix;
			out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),\
							"-ctlg_",ctlg_to_write_,"-seed_",num2str(id_rep_),".dat"];

			print_table(out_filename, matrix, column_names, fixed_variables,
															fixed_variable_names, comment);

		endif % per_seed_results

		seed_id ++;
	endfor # id_rep for

	fixed_variables = { network; forwarding_; replacement_; alpha_; ctlg_; csize_; id_rep_list };
	fixed_variable_names = {"network"; "forwarding"; "replacement"; "alpha"; "ctlg";"csize"; "seed_list"};
	comment="";


	[p_hit_mean_matrix, p_hit_conf_matrix] = confidence_interval(p_hit_matrix_over_seed);
	[total_cost_mean_matrix, total_cost_conf_matrix] = confidence_interval(total_cost_matrix_over_seed);
	[per_request_cost_mean_matrix, per_request_cost_conf_matrix] = \
										confidence_interval(per_request_cost_matrix_over_seed);
	[hdistance_mean_matrix, hdistance_conf_matrix] = confidence_interval(hdistance_matrix_over_seed);
	[expensive_link_utilization_mean_matrix, expensive_link_utilization_conf_matrix] = \
										confidence_interval(expensive_link_utilization_matrix_over_seed);
	[client_requests_mean_matrix, client_requests_conf_matrix] = \
										confidence_interval(client_requests_matrix_over_seed);


	% CHECK MATRIX{
	if severe_debug
		if size(p_hit_mean_matrix,1) != length(priceratio_list)
				alpha_
				priceratio_list
				p_hit_mean_matrix
				p_hit_matrix_over_seed
				error("The number of rows in the matrix must be equal to the number of price ratios");
		endif
	endif
	% }CHECK MATRIX


	metric="p_hit-mean";
	matrix = p_hit_mean_matrix;
	out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,".dat"];
	print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);

	metric="total_cost-mean";
	matrix = total_cost_mean_matrix;
	out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,".dat"];
	print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);

	metric="per_request_cost-mean";
	matrix = per_request_cost_mean_matrix;
	out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,".dat"];
	print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);

	metric="hdistance-mean";
	matrix = hdistance_mean_matrix;
	out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,".dat"];
	print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);

	metric="expensive_link_utilization-mean";
	matrix = expensive_link_utilization_mean_matrix;
	out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,".dat"];
	print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);

	metric="client_requests-mean";
	matrix = client_requests_mean_matrix;
	out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,".dat"];
	print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);



	metric="p_hit-conf";
	matrix = p_hit_conf_matrix;
	out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,".dat"];
	print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);

	metric="total_cost-conf";
	matrix = total_cost_conf_matrix;
	out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,".dat"];
	print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);

	metric="per_request_cost-conf";
	matrix = per_request_cost_conf_matrix;
	out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,".dat"];
	print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);

	metric="hdistance-conf";
	matrix = hdistance_conf_matrix;
	out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,".dat"];
	print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);

	metric="expensive_link_utilization-conf";
	matrix = expensive_link_utilization_conf_matrix;
	out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,".dat"];
	print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);

	metric="client_requests-conf";
	matrix = client_requests_conf_matrix;
	out_filename = [out_folder,metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,".dat"];
	print_table(out_filename, matrix, column_names, fixed_variables,fixed_variable_names, comment);
endfor % alpha
