% result processing
global severe_debug = true;

out_folder="~/Dropbox/shared_with_servers/icn14_runs/";

priceratio_list=[10];
possible_decisions={"lce", "fix0.1", "prob_cache", "fix0.01","costprob0.1","costprob0.01","fix1", "fix0",\
			 "costprob0","never","costprob0.02","fix0.0001", "costprob0.0002"};
decision_list={"costprob0.01"}; % The decision plocies that I want to plot
xi_list = [0.25, 0.5, 1, 2, 4, 8, 16, 32, 64];
weights_list={"0.333_0.333_0.334"};
id_rep_list=1:20; # list of seeds
alpha_list = [1];
csize_list = {"1e3"};
csize_to_write_list = {"1e3"};

resultdir="~/software/ccnsim/results";
metric_list = {"p_hit", "total_cost", "per_request_cost", "hdistance", "expensive_link_utilization",\
						"client_requests", "decision_ratio"};

network="one_cache_scenario_3_links";
forwarding_="nrr";
replacement_="lru";
ctlg_="1e5"; 
ctlg_to_write_="1e5";

fixed_variable_names_additional = {"priceratio", "alpha"};
x_variable_name = "xi";


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
				for xi_ = xi_list
					for idx_weight = 1:length(weights_list)
						weights_ = weights_list{idx_weight};
						for id_rep_ = id_rep_list

							selection_tuple.priceratio = priceratio_list(priceratio_idx);
							selection_tuple.decision = decision_list{decision_idx};
							selection_tuple.xi = xi_;
							selection_tuple.forwarding = forwarding_;
							selection_tuple.replacement = replacement_;
							selection_tuple.alpha = alpha_;
							selection_tuple.ctlg = ctlg_;
							selection_tuple.csize = csize_;
							selection_tuple.id_rep = id_rep_;
							selection_tuple.network = network;
							selection_tuple.weights = weights_;

							parsed_ = select(selection_tuple, resultdir);

							parsed(i) = parsed_;

							i++;
						endfor % seed loop
					endfor % weights loop
				endfor % xi loop
			endfor
		endfor
	endfor %alpha for
endfor %csize for


##################################
### PREPARE DATA FOR PLOTTING ####
######### FUNCTION ###############
input_data.out_folder = out_folder;

input_data.priceratio_list = priceratio_list;
input_data.possible_decisions = possible_decisions;
input_data.decision_list = decision_list; % The decision plocies that I want to plot
input_data.id_rep_list = id_rep_list; # list of seeds
input_data.alpha_list = alpha_list;
input_data.xi_list = alpha_list;
input_data.csize_list = csize_list;
input_data.csize_to_write_list = csize_to_write_list;

input_data.resultdir = resultdir;
input_data.metric_list = metric_list;

input_data.network = network;
input_data.forwarding_ = forwarding_;
input_data.replacement_ = replacement_;
input_data.ctlg_ = ctlg_; 
input_data.ctlg_to_write_ = ctlg_to_write_;

input_data.fixed_variable_names_additional = fixed_variable_names_additional;
for idx_fixed_variable_additional = 1:length(fixed_variable_names_additional)
	input_data.fixed_variable_values_additional{idx_fixed_variable_additional} = \
			eval( [input_data.fixed_variable_names_additional{idx_fixed_variable_additional},"_list"] );
endfor

input_data.x_variable_name = x_variable_name;
input_data.x_variable_values = eval( [input_data.x_variable_name,"_list"] ) ;

input_data.parsed = parsed;


metric_vs_x_variable(input_data);
