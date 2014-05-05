% ciao
function parsed = select(selection_tuple, resultdir)
	global severe_debug
						priceratio_ = selection_tuple.priceratio;
						decision_ = selection_tuple.decision;
						xi_ = selection_tuple.xi;
						forwarding_ = selection_tuple.forwarding;
						replacement_ = selection_tuple.replacement;
						alpha_ = selection_tuple.alpha;
						ctlg_ = selection_tuple.ctlg;
						csize_ = selection_tuple.csize;
						id_rep_ = selection_tuple.id_rep;
						network = selection_tuple.network;
						weights_ = selection_tuple.weights;

						if strcmp( substr(decision_,1,3), "fix")
							decision_root_ = "fix";
							target_decision_probability_ = \
								num2str( strrep(decision_,"fix","") );

						elseif strcmp( substr(decision_,1,17), "costprobprodplain")
							decision_root_ = "costprobprodplain";
							target_decision_probability_ = \
								num2str( strrep(decision_,"costprobprodplain","") );

						elseif strcmp( substr(decision_,1,16), "costprobprodcorr")
							decision_root_ = "costprobprodcorr";
							target_decision_probability_ = \
								num2str( strrep(decision_,"costprobprodcorr","") );

						elseif strcmp( substr(decision_,1,17), "costprobcoinplain")
							decision_root_ = "costprobcoinplain";
							target_decision_probability_ = \
								num2str( strrep(decision_,"costprobcoinplain","") );

						elseif strcmp( substr(decision_,1,16), "costprobcoincorr")
							decision_root_ = "costprobcoincorr";
							target_decision_probability_ = \
								num2str( strrep(decision_,"costprobcoincorr","") );

						else
							decision_root_ = decision_;
							target_decision_probability_ = NaN;
						endif

						filename = strcat(resultdir,"/",network,"/F-",forwarding_,"/D-",decision_,"/xi-",xi_,"/R-",replacement_,"/alpha-",num2str(alpha_),"/ctlg-",ctlg_,"/cachesize-",num2str(csize_),"/weights-",weights_,"/priceratio-",num2str(priceratio_),"/ccn-id",num2str(id_rep_),".sca");


						parsed.filename_list = filename;

						parsed.decision = decision_;
						parsed.decision_root = decision_root_;
						parsed.target_decision_probability = target_decision_probability_;
						parsed.xi = xi_;
						parsed.forwarding = forwarding_;
						parsed.replacement = replacement_;
						parsed.alpha = alpha_;
						parsed.ctlg = ctlg_;
						parsed.csize = csize_;
						parsed.priceratio = priceratio_;
						parsed.id_rep = id_rep_;

						string_to_search="p_hit\\[0\\] ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.p_hit = str2num(output);

						string_to_search="total_cost ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.total_cost = str2num(output);

						string_to_search="hdistance ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.hdistance = str2num(output);

						string_to_search="downloads\\[0\\] ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.client_requests = str2num(output);


						string_to_search="repo_load\\[4\\] ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.free_link_load = str2num(output);

						string_to_search="repo_load\\[5\\] ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.cheap_link_load = str2num(output);

						string_to_search="repo_load\\[6\\] ";
						command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
						[status, output] = system(command,1);
						parsed.expensive_link_load = str2num(output);

			if strcmp(decision_root_, "costprob")
				selection_tuple_of_fixed_counterpart = selection_tuple;
				selection_tuple_of_fixed_counterpart.decision =\
						 ["fix",target_decision_probability_];
				selection_tuple_of_fixed_counterpart.xi = 1;
				fixed_counterpart_parsed = select(selection_tuple_of_fixed_counterpart,\
						resultdir);
				parsed.cost_savings = (fixed_counterpart_parsed.total_cost - parsed.total_cost)/\
											fixed_counterpart_parsed.total_cost;
			else
				parsed.cost_savings = NaN;
			endif


			if severe_debug
				if (size(parsed.free_link_load) != [1,1] && size(parsed.cheap_link_load) != [1,1] &&  size(parsed.expensive_link_load) != [1,1] )
					free_link_load = parsed.free_link_load
					cheap_link_load = parsed.cheap_link_load
					expensive_link_load = parsed.expensive_link_load
					error("Error in the link load computation");
				end
			endif

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
			

	# CHECK RESULTS{
								if ( size(parsed.p_hit)!=[1 1] || size(parsed.total_cost)!=[1 1] || \
									 size(parsed.hdistance )!=[1 1] \
										|| size(parsed.client_requests)!=[1 1] || size(parsed.cheap_link_load)!=[1 1] \
										|| size(parsed.expensive_link_load)!=[1 1] )

									priceratio_
									decision_
									disp(["p_hit=", num2str(parsed.p_hit), "; total_cost=", \
											num2str(parsed.total_cost), "; hdistance=",num2str(parsed.hdistance ), \
											"; client_requests=",num2str(parsed.client_requests )] );
									command
									error("Parsing error");
								endif
	# }CHECK RESULTS
		
endfunction
