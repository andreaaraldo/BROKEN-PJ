% ciao
function parsed = select(selection_tuple, resultdir, optimization_result_folder)
	global severe_debug
						priceratio_ = selection_tuple.priceratio;
						decision_ = selection_tuple.decision;
						xi_ = selection_tuple.xi;
						forwarding_ = selection_tuple.forwarding;
						replacement_ = selection_tuple.replacement;
						alpha_ = selection_tuple.alpha;
						q_ = selection_tuple.q;
						ctlg_ = selection_tuple.ctlg;
						csize_ = selection_tuple.csize;
						id_rep_ = selection_tuple.id_rep;
						network = selection_tuple.network;
						weights_ = selection_tuple.weights;
						metric_list = selection_tuple.metric_list;

						decision_root_ = "";
						if strmatch( "fix", decision_ )
							decision_root_ = "fix";
							target_decision_probability_ = ...
								num2str( strrep(decision_,"fix","") )

						elseif strmatch("costopt", decision_)
							decision_root_ = "costopt";
							target_decision_probability_ = NaN;
						

						elseif length(decision_) >= 16

							if strmatch( "costprobprodplain", decision_)
								decision_root_ = "costprobprodplain";
								target_decision_probability_ = ...
									num2str( strrep(decision_,"costprobprodplain","") );

							elseif strmatch( "costprobprodcorr", decision_ )
								decision_root_ = "costprobprodcorr";
								target_decision_probability_ = ...
									num2str( strrep(decision_,"costprobprodcorr","") );

							elseif strmatch( "costprobcoinplain", decision_ )
								decision_root_ = "costprobcoinplain";
								target_decision_probability_ = ...
									num2str( strrep(decision_,"costprobcoinplain","") );

							elseif strmatch( "costprobcoincorr", decision_ )
								decision_root_ = "costprobcoincorr";
								target_decision_probability_ = ...
									num2str( strrep(decision_,"costprobcoincorr","") );

							elseif strmatch( "costprobtailperf", decision_ )
								decision_root_ = "costprobtailperf";
								target_decision_probability_ = NaN;

							elseif strmatch( "costprobtailsmart", decision_ )
								decision_root_ = "costprobtailsmart";
								target_decision_probability_ = ...
									num2str( strrep(decision_,"costprobtailsmart","") );
							endif
						else
							decision_root_ = decision_;
							target_decision_probability_ = NaN;
						endif

						% CHECK{
						if isequal(decision_root_,"")
							error(["Error in parsing the decision policy ",decision_]);
						endif
						% }CHECK

						destination_folder = ...
							strcat(resultdir,"/",network,"/q-",q_,...
							"/F-",forwarding_,...
							"/D-",decision_,"/xi-",xi_,"/R-",replacement_,...
							"/alpha-",alpha_,"/ctlg-",ctlg_,...
							"/cachesize-",num2str(csize_),"/weights-",weights_,...
							"/priceratio-",priceratio_);

						filename = strcat(destination_folder,"/ccn-id", ...
										num2str(id_rep_),".sca");

						if isequal(decision_,"costopt")
							% This file does not exists yet
							create_ccnsim_representation(selection_tuple,...
								destination_folder, optimization_result_folder);
						endif

						
						% CHECK{
							fid = fopen(filename, "r");
							if fid < 0
								filename
								error("the file does not exist");
							endif
							fclose(fid);
						% CHECK{

						parsed.filename_list = filename;

						parsed.decision = decision_;
						parsed.decision_root = decision_root_;
						parsed.target_decision_probability = target_decision_probability_;
						parsed.xi = xi_;
						parsed.forwarding = forwarding_;
						parsed.replacement = replacement_;
						parsed.alpha = alpha_;
						parsed.q = q_;
						parsed.ctlg = ctlg_;
						parsed.csize = csize_;
						parsed.priceratio = priceratio_;
						parsed.id_rep = id_rep_;
						parsed.weights = weights_;

						string_to_search="p_hit\\[0\\] ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.p_hit = str2num(output);

						string_to_search="total_cost ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.total_cost = str2num(output);

						disp("prima di prova")
						strcmp("hdistance",metric_list{:,:})
						error("dopo di prova")

					if strcmp("hdistance",metric_list{:,:})
						string_to_search="hdistance ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.hdistance = str2num(output);
					else
						parsed.hdistance = NaN;
					endif

					if strcmp("client_requests",metric_list{:,:})
						string_to_search="downloads\\[0\\] ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.client_requests = str2num(output);
					else
						parsed.client_requests = NaN;
					endif

					if strcmp("repo_load",metric_list{:,:})
						string_to_search="repo_load\\[4\\] ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.free_link_load = str2num(output);
						if size(parsed.free_link_load) == [0,0]
							parsed.free_link_load = 0;
						endif

						string_to_search="repo_load\\[5\\] ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.cheap_link_load = str2num(output);
						if size(parsed.cheap_link_load) == [0,0]
							parsed.cheap_link_load = 0;
						endif


						string_to_search="repo_load\\[6\\] ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.expensive_link_load = str2num(output);
						if size(parsed.expensive_link_load) == [0,0]
							parsed.expensive_link_load = 0;
						endif
					else
						parsed.free_link_load = NaN;
						parsed.cheap_link_load = NaN;
						parsed.expensive_link_load = NaN;
					endif

			if strcmp("cost_savings",metric_list{:,:}) && strmatch("costprob", decision_root_)
				selection_tuple_of_fixed_counterpart = selection_tuple;
				selection_tuple_of_fixed_counterpart.decision =...
						 ["fix",target_decision_probability_];
				selection_tuple_of_fixed_counterpart.xi = "1";
				fixed_counterpart_parsed = select(selection_tuple_of_fixed_counterpart,...
						resultdir, optimization_result_folder);
				parsed.cost_savings = (fixed_counterpart_parsed.total_cost - parsed.total_cost)/...
											fixed_counterpart_parsed.total_cost;
			else
				parsed.cost_savings = NaN;
			endif

	% COMPUTE COST FRACTION{
		parsed.cost_fraction = NaN;
		% Comparison with the no-cache scenario
		if strcmp("cost_fraction", metric_list{:,:} )
			if isequal("costopt", decision_) && strcmp(csize_, "0")
				parsed.cost_fraction = 1;
			elseif isequal("costopt", decision_) && !strcmp(csize_, "0")
				selection_tuple_of_counterpart = selection_tuple;
				selection_tuple_of_counterpart.csize = "0";
				counterpart_parsed = select(selection_tuple_of_counterpart,...
						resultdir, optimization_result_folder);
				parsed.cost_fraction = parsed.total_cost / counterpart_parsed.total_cost;
			elseif !isequal("never", decision_)
				selection_tuple_of_never_counterpart = selection_tuple;
				selection_tuple_of_never_counterpart.decision = "never";
				selection_tuple_of_never_counterpart.xi = "1";
				never_counterpart_parsed = select(selection_tuple_of_never_counterpart,...
						resultdir, optimization_result_folder);
				parsed.cost_fraction = parsed.total_cost / never_counterpart_parsed.total_cost;
			endif
		endif
	% }COMPUTE COST FRACTION

	% CHECK{
		if severe_debug
					if (size(parsed.free_link_load) != [1,1] || ...
						size(parsed.cheap_link_load) != [1,1] ...
						||  size(parsed.expensive_link_load) != [1,1] )

						cheap_link_load_size = size(parsed.cheap_link_load)
						free_link_load = parsed.free_link_load
						cheap_link_load = parsed.cheap_link_load
						expensive_link_load = parsed.expensive_link_load
						error("Error in the link load computation");
					endif
		endif
	% }CHECK

	% COMPUTE POTENTIAL_REDUCTION{
		% Comparison with costopt
		if strcmp("potential_reduction", metric_list{:,:} ) && !isequal("costopt", decision_) 
				selection_tuple_of_counterpart = selection_tuple;
				selection_tuple_of_counterpart.decision = "costopt";
				counterpart_parsed = select(selection_tuple_of_counterpart,...
						resultdir, optimization_result_folder);
				parsed.potential_reduction = ...
						parsed.cost_fraction - counterpart_parsed.cost_fraction;
		else
				parsed.potential_reduction = NaN;
		endif
	% }COMPUTE POTENTIAL_REDUCTION


	if strcmp("decision_ratio",metric_list{:,:})
						string_to_search="decision_yes\\[0\\] ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.decision_yes = str2num(output);

						string_to_search="decision_no\\[0\\] ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.decision_no = str2num(output);

						string_to_search="decision_ratio\\[0\\] ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.decision_ratio = str2num(output);
	else
						parsed.decision_yes = NaN;
						parsed.decision_no = NaN;
						parsed.decision_ratio = NaN;
	endif


	# CHECK RESULTS{
	if ( size(parsed.p_hit)!=[1 1] || size(parsed.total_cost)!=[1 1] || ...
				size(parsed.hdistance )!=[1 1] || size(parsed.client_requests)!=[1 1]...
				||size(parsed.cheap_link_load)!=[1 1] || ...
				size(parsed.expensive_link_load)!=[1 1] )

		p_hit = parsed.p_hit
		total_cost = parsed.total_cost
		hdistance = parsed.hdistance
		client_requests = parsed.client_requests
		cheap_link_load = parsed.cheap_link_load
		expensive_link_load = parsed.expensive_link_load
		priceratio_
		decision_
		command
		error("Parsing error");
	endif
	# }CHECK RESULTS
		
endfunction
