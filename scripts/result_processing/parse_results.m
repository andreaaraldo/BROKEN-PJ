# result processing

priceratio_list={1,2,5};
decision_list={"lce", "fix0.1"}

resultdir="~/software/ccnsim/results";
network="simple_scenario";
forwarding="nrr";
decision="lce";
replacement="lru";
alpha=1.2;
ctlg="10\\^4";
csize=10;
id_rep=0;

database = [];
i = 1;

for priceratio_idx = 1:length(priceratio_list)
	for decision_idx = 1:length(decision_list)
		priceratio = priceratio_list{priceratio_idx}
		decision = decision_list{decision_idx}

		filename = strcat(resultdir,"/",network,"/F-",forwarding,"/D-",decision,"/R-",replacement,"/alpha-",num2str(alpha),"/ctlg-",ctlg,"/cachesize-",num2str(csize),"/priceratio-",num2str(priceratio),"/ccn-id",num2str(id_rep),".sca");

		disp("YOU SHOULD CHECK IF THE FILE EXISTS");

		string_to_search="p_hit ";
		command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
		[status, output] = system(command,1);
		p_hit{i} = str2num(output);
		p_hit{i}

		string_to_search="total_cost ";
		command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
		[status, output] = system(command,1);
		total_cost{i} = str2num(output);
		total_cost{i}

		string_to_search="hdistance ";
		command = ["grep ","\"",string_to_search,"\""," ",filename," | awk \'{print $4}\' "];
		[status, output] = system(command,1);
		hdistance{i} = str2num(output);
		hdistance{i}

		single_entry.p_hit = p_hit;
		single_entry.total_cost = total_cost;
		single_entry.hdistance = hdistance;

		single_entry
		i++;
	endfor
endfor
