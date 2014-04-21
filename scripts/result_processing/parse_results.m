# result processing

priceratio_list={1,2,3,4,5,6,7,8,9,10};
decision_list={"lce", "fix0.1", "prob_cache", "fix0.01","costprob0.1","costprob0.01"};
id_rep_list=1:5; # list of seeds

resultdir="~/software/ccnsim/results";
network="simple_scenario";
forwarding_="nrr";
replacement_="lru";
alpha_=0.8;
ctlg_="10\\^3"; 
ctlg_to_write_="1e3";
csize_=10;

i = 1;

disp("YOU SHOULD CHECK IF THE FILE EXISTS");


for priceratio_idx = 1:length(priceratio_list)
	for decision_idx = 1:length(decision_list)
		for id_rep_ = id_rep_list
			priceratio_ = priceratio_list{priceratio_idx};
			decision_ = decision_list{decision_idx};

			filename = strcat(resultdir,"/",network,"/F-",forwarding_,"/D-",decision_,"/R-",replacement_,"/alpha-",num2str(alpha_),"/ctlg-",ctlg_,"/cachesize-",num2str(csize_),"/priceratio-",num2str(priceratio_),"/ccn-id",num2str(id_rep_),".sca");

			decision{i} = decision_;
			forwarding{i} = forwarding_;
			replacement{i} = replacement_;
			alpha{i} = alpha_;
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

			# CHECK RESULTS{
				if length(p_hit{i})==0 || length(total_cost{i})==0 || length(hdistance{i})==0
					priceratio_
					decision_
					disp(["p_hit=", num2str(p_hit{i}), "; total_cost=", \
							num2str(total_cost{i}), "; hdistance=",num2str(hdistance{i} ) ] );
					error("Persing error");
				endif
			# }CHECK RESULTS

			i++;
		endfor
	endfor
endfor


######### Matrix construction

for id_rep_ = id_rep_list
	# Compute the priceratio_column, using whatever decision_ 
	decision_ = decision_list{1};
	idx =  strcmp(decision, decision_ ) & ( id_rep == id_rep_ ) ;
	priceratio_column = cell2mat( priceratio(idx) );

	p_hit_matrix = priceratio_column';
	total_cost_matrix = priceratio_column';
	hdistance_matrix = priceratio_column';

	for decision_idx = 1:length(decision_list)
		decision_ = decision_list{decision_idx};
		idx =  strcmp(decision, decision_ ) & id_rep == id_rep_;
		priceratio_column_for_check = cell2mat( priceratio(idx) );
		if any(priceratio_column_for_check !=  priceratio_column)
			error("price ratio is erroneous");
		endif

		p_hit_column = cell2mat( p_hit(idx) );
		total_cost_column = cell2mat( total_cost(idx) );
		hdistance_column = cell2mat( hdistance(idx) );

		# CHECK DIMENSIONS{
			if size(p_hit_matrix,1) != length(p_hit_column)
				p_hit_matrix
				p_hit_column
				priceratio_column_for_check
				decision_
				error("Length of p_hit_column MUST bne equal to the row number of p_hit_matrix");
			endif
		# }CHECK DIMENSIONS
	
		p_hit_matrix = [p_hit_matrix, p_hit_column'];
		total_cost_matrix = [total_cost_matrix, total_cost_column'];
		hdistance_matrix = [hdistance_matrix, hdistance_column'];
	endfor

	######### Enjoliver
	column_names{1} = {"priceratio"};
	for idx_decision = 1:length(decision_list)
		column_names{ idx_decision+1 } = decision_list{ idx_decision };
	endfor

	fixed_variables = { network; forwarding_; replacement_; alpha_; ctlg_; csize_; id_rep_ };
	fixed_variable_names = {"network"; "forwarding"; "replacement"; "alpha"; "ctlg";"csize"; "id_rep"};

	comment="";

	metric="p_hit";
	matrix = p_hit_matrix;
	out_filename = ["~/Dropbox/shared_with_servers/icn14_runs/",metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,"-seed_",num2str(id_rep_),".dat"];

	print_table(out_filename, matrix, column_names, fixed_variables,
													fixed_variable_names, comment);

	metric="total_cost";
	matrix = total_cost_matrix;
	out_filename = ["~/Dropbox/shared_with_servers/icn14_runs/",metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,"-seed_",num2str(id_rep_),".dat"];

	print_table(out_filename, matrix, column_names, fixed_variables,
													fixed_variable_names, comment);

	metric="hdistance";
	matrix = hdistance_matrix;
	out_filename = ["~/Dropbox/shared_with_servers/icn14_runs/",metric,"-alpha_",num2str(alpha_),"-ctlg_",ctlg_to_write_,"-seed_",num2str(id_rep_),".dat"];

	print_table(out_filename, matrix, column_names, fixed_variables,
													fixed_variable_names, comment);
endfor # id_rep for
