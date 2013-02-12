package topologies;
import modules.*;

network level3_network {

    parameters:
    	int n = 46;
	double degree = default(4);

	int c = default(8);

    submodules:
 	statistics: NetworkInspector{

	}

	request_generator[c]: request_generator{
	        @display("i=block/routing");
	    
	}

	global_generator: global_generator{
	        @display("i=block/routing");
	    
	}

    	content_distribution: content_distribution{
	    parameters:
		r =  default(1);
	
	}

        ccn_node[n]: ccn_node{
            parameters:
	        @display("i=block/broadcast_s;is=s");
            //    @display("p=50,50,ri,200,200");
        }


	routing: routing{
	}



connections allowunconnected:
	ccn_node[0].face++ <--> { delay = 0.699743613611879ms; } <--> ccn_node[1].face++;
	ccn_node[0].face++ <--> { delay = 1.55963394775673ms; } <--> ccn_node[2].face++;
	ccn_node[0].face++ <--> { delay = 1.88532911009081ms; } <--> ccn_node[3].face++;
	ccn_node[0].face++ <--> { delay = 2.01525285021424ms; } <--> ccn_node[4].face++;
	ccn_node[1].face++ <--> { delay = 1.5622819497653ms; } <--> ccn_node[2].face++;
	ccn_node[1].face++ <--> { delay = 1.59576807051388ms; } <--> ccn_node[3].face++;
	ccn_node[10].face++ <--> { delay = 19.7210147124792ms; } <--> ccn_node[11].face++;
	ccn_node[10].face++ <--> { delay = 2.94005274291922ms; } <--> ccn_node[12].face++;
	ccn_node[10].face++ <--> { delay = 18.001808302142ms; } <--> ccn_node[13].face++;
	ccn_node[10].face++ <--> { delay = 19.6400586232912ms; } <--> ccn_node[14].face++;
	ccn_node[10].face++ <--> { delay = 0.930875068217323ms; } <--> ccn_node[15].face++;
	ccn_node[10].face++ <--> { delay = 2.39033852037053ms; } <--> ccn_node[16].face++;
	ccn_node[10].face++ <--> { delay = 7.68872484627139ms; } <--> ccn_node[17].face++;
	ccn_node[10].face++ <--> { delay = 18.5226753489951ms; } <--> ccn_node[18].face++;
	ccn_node[10].face++ <--> { delay = 18.8519227076216ms; } <--> ccn_node[28].face++;
	ccn_node[10].face++ <--> { delay = 17.7437123502417ms; } <--> ccn_node[29].face++;
	ccn_node[10].face++ <--> { delay = 19.27296065188ms; } <--> ccn_node[30].face++;
	ccn_node[10].face++ <--> { delay = 2.84864363952102ms; } <--> ccn_node[31].face++;
	ccn_node[10].face++ <--> { delay = 4.67700418361009ms; } <--> ccn_node[32].face++;
	ccn_node[10].face++ <--> { delay = 9.73026753143365ms; } <--> ccn_node[33].face++;
	ccn_node[10].face++ <--> { delay = 12.8136884315574ms; } <--> ccn_node[35].face++;
	ccn_node[10].face++ <--> { delay = 19.8989532740876ms; } <--> ccn_node[36].face++;
	ccn_node[10].face++ <--> { delay = 17.355078362519ms; } <--> ccn_node[37].face++;
	ccn_node[10].face++ <--> { delay = 0.330056735798249ms; } <--> ccn_node[38].face++;
	ccn_node[10].face++ <--> { delay = 19.7129293095592ms; } <--> ccn_node[39].face++;
	ccn_node[10].face++ <--> { delay = 14.3486781889034ms; } <--> ccn_node[40].face++;
	ccn_node[11].face++ <--> { delay = 17.184736870328ms; } <--> ccn_node[12].face++;
	ccn_node[11].face++ <--> { delay = 3.39125510012522ms; } <--> ccn_node[13].face++;
	ccn_node[11].face++ <--> { delay = 0.141106154689448ms; } <--> ccn_node[14].face++;
	ccn_node[11].face++ <--> { delay = 19.491370482797ms; } <--> ccn_node[15].face++;
	ccn_node[11].face++ <--> { delay = 20.4998013750844ms; } <--> ccn_node[16].face++;
	ccn_node[11].face++ <--> { delay = 19.3209613118993ms; } <--> ccn_node[17].face++;
	ccn_node[11].face++ <--> { delay = 1.62733364571067ms; } <--> ccn_node[18].face++;
	ccn_node[11].face++ <--> { delay = 7.55145592209754ms; } <--> ccn_node[29].face++;
	ccn_node[11].face++ <--> { delay = 0.620583384672223ms; } <--> ccn_node[30].face++;
	ccn_node[11].face++ <--> { delay = 20.1014066858336ms; } <--> ccn_node[31].face++;
	ccn_node[11].face++ <--> { delay = 15.8327352787206ms; } <--> ccn_node[32].face++;
	ccn_node[11].face++ <--> { delay = 12.7112458174267ms; } <--> ccn_node[33].face++;
	ccn_node[11].face++ <--> { delay = 20.6843954593898ms; } <--> ccn_node[34].face++;
	ccn_node[11].face++ <--> { delay = 7.01068897472049ms; } <--> ccn_node[35].face++;
	ccn_node[11].face++ <--> { delay = 0.280723638773994ms; } <--> ccn_node[36].face++;
	ccn_node[11].face++ <--> { delay = 8.07512730237914ms; } <--> ccn_node[37].face++;
	ccn_node[11].face++ <--> { delay = 19.5602075478991ms; } <--> ccn_node[38].face++;
	ccn_node[11].face++ <--> { delay = 0.0312643364659312ms; } <--> ccn_node[39].face++;
	ccn_node[11].face++ <--> { delay = 29.1588945256645ms; } <--> ccn_node[4].face++;
	ccn_node[11].face++ <--> { delay = 31.5982426456463ms; } <--> ccn_node[44].face++;
	ccn_node[12].face++ <--> { delay = 4.92145703772741ms; } <--> ccn_node[16].face++;
	ccn_node[12].face++ <--> { delay = 8.92971964408383ms; } <--> ccn_node[17].face++;
	ccn_node[12].face++ <--> { delay = 15.8987822403206ms; } <--> ccn_node[18].face++;
	ccn_node[13].face++ <--> { delay = 19.1443875884519ms; } <--> ccn_node[16].face++;
	ccn_node[13].face++ <--> { delay = 18.98216578317ms; } <--> ccn_node[17].face++;
	ccn_node[13].face++ <--> { delay = 1.8616231104935ms; } <--> ccn_node[18].face++;
	ccn_node[13].face++ <--> { delay = 5.33819773994786ms; } <--> ccn_node[35].face++;
	ccn_node[13].face++ <--> { delay = 3.67107483667472ms; } <--> ccn_node[36].face++;
	ccn_node[13].face++ <--> { delay = 4.72610446037405ms; } <--> ccn_node[37].face++;
	ccn_node[14].face++ <--> { delay = 20.4333049261819ms; } <--> ccn_node[16].face++;
	ccn_node[14].face++ <--> { delay = 19.2944180048151ms; } <--> ccn_node[17].face++;
	ccn_node[14].face++ <--> { delay = 1.49060940405506ms; } <--> ccn_node[18].face++;
	ccn_node[15].face++ <--> { delay = 3.3211522299544ms; } <--> ccn_node[16].face++;
	ccn_node[15].face++ <--> { delay = 8.51978747368999ms; } <--> ccn_node[17].face++;
	ccn_node[15].face++ <--> { delay = 18.2394468044859ms; } <--> ccn_node[18].face++;
	ccn_node[16].face++ <--> { delay = 5.71561756404175ms; } <--> ccn_node[17].face++;
	ccn_node[16].face++ <--> { delay = 19.4442417506344ms; } <--> ccn_node[18].face++;
	ccn_node[16].face++ <--> { delay = 13.8309070724106ms; } <--> ccn_node[35].face++;
	ccn_node[16].face++ <--> { delay = 20.6504431819649ms; } <--> ccn_node[36].face++;
	ccn_node[16].face++ <--> { delay = 19.0194169040216ms; } <--> ccn_node[37].face++;
	ccn_node[16].face++ <--> { delay = 2.71257848103978ms; } <--> ccn_node[38].face++;
	ccn_node[16].face++ <--> { delay = 20.488148921334ms; } <--> ccn_node[39].face++;
	ccn_node[17].face++ <--> { delay = 18.6918595751372ms; } <--> ccn_node[18].face++;
	ccn_node[17].face++ <--> { delay = 13.8418587470974ms; } <--> ccn_node[35].face++;
	ccn_node[17].face++ <--> { delay = 19.3949343842529ms; } <--> ccn_node[36].face++;
	ccn_node[17].face++ <--> { delay = 20.3190003929401ms; } <--> ccn_node[37].face++;
	ccn_node[17].face++ <--> { delay = 7.93565659619452ms; } <--> ccn_node[38].face++;
	ccn_node[17].face++ <--> { delay = 19.3012314510989ms; } <--> ccn_node[39].face++;
	ccn_node[18].face++ <--> { delay = 1.64134190753599ms; } <--> ccn_node[39].face++;
	ccn_node[19].face++ <--> { delay = 18.6766341461069ms; } <--> ccn_node[10].face++;
	ccn_node[19].face++ <--> { delay = 1.35482147163357ms; } <--> ccn_node[11].face++;
	ccn_node[19].face++ <--> { delay = 19.5680477997134ms; } <--> ccn_node[16].face++;
	ccn_node[19].face++ <--> { delay = 18.7311080954632ms; } <--> ccn_node[17].face++;
	ccn_node[19].face++ <--> { delay = 0.281322687176047ms; } <--> ccn_node[18].face++;
	ccn_node[19].face++ <--> { delay = 4.86978736191705ms; } <--> ccn_node[7].face++;
	ccn_node[19].face++ <--> { delay = 9.72322027965186ms; } <--> ccn_node[8].face++;
	ccn_node[2].face++ <--> { delay = 30.9779065131767ms; } <--> ccn_node[11].face++;
	ccn_node[2].face++ <--> { delay = 32.6040058370753ms; } <--> ccn_node[18].face++;
	ccn_node[2].face++ <--> { delay = 3.15804073417278ms; } <--> ccn_node[3].face++;
	ccn_node[2].face++ <--> { delay = 2.34012884016252ms; } <--> ccn_node[4].face++;
	ccn_node[2].face++ <--> { delay = 2.0357355149515ms; } <--> ccn_node[41].face++;
	ccn_node[2].face++ <--> { delay = 7.15087408784109ms; } <--> ccn_node[42].face++;
	ccn_node[2].face++ <--> { delay = 1.51653740453145ms; } <--> ccn_node[43].face++;
	ccn_node[2].face++ <--> { delay = 1.47245906075377ms; } <--> ccn_node[44].face++;
	ccn_node[20].face++ <--> { delay = 2.16152721948152ms; } <--> ccn_node[2].face++;
	ccn_node[20].face++ <--> { delay = 2.3608930700131ms; } <--> ccn_node[21].face++;
	ccn_node[20].face++ <--> { delay = 4.62071213151615ms; } <--> ccn_node[3].face++;
	ccn_node[21].face++ <--> { delay = 0.944468343206374ms; } <--> ccn_node[2].face++;
	ccn_node[21].face++ <--> { delay = 2.37977630245223ms; } <--> ccn_node[3].face++;
	ccn_node[22].face++ <--> { delay = 20.8864181920654ms; } <--> ccn_node[10].face++;
	ccn_node[22].face++ <--> { delay = 1.52818775993091ms; } <--> ccn_node[11].face++;
	ccn_node[22].face++ <--> { delay = 4.88711410738906ms; } <--> ccn_node[13].face++;
	ccn_node[22].face++ <--> { delay = 21.5400443557056ms; } <--> ccn_node[16].face++;
	ccn_node[22].face++ <--> { delay = 19.993175944005ms; } <--> ccn_node[17].face++;
	ccn_node[22].face++ <--> { delay = 3.15550474307128ms; } <--> ccn_node[18].face++;
	ccn_node[22].face++ <--> { delay = 6.83381142072769ms; } <--> ccn_node[7].face++;
	ccn_node[22].face++ <--> { delay = 12.4363734730694ms; } <--> ccn_node[8].face++;
	ccn_node[22].face++ <--> { delay = 14.1420447163401ms; } <--> ccn_node[9].face++;
	ccn_node[23].face++ <--> { delay = 15.3017628980041ms; } <--> ccn_node[10].face++;
	ccn_node[23].face++ <--> { delay = 4.53202034122801ms; } <--> ccn_node[11].face++;
	ccn_node[23].face++ <--> { delay = 3.16256570179669ms; } <--> ccn_node[13].face++;
	ccn_node[23].face++ <--> { delay = 16.2512133607094ms; } <--> ccn_node[16].face++;
	ccn_node[23].face++ <--> { delay = 15.8275679640746ms; } <--> ccn_node[17].face++;
	ccn_node[23].face++ <--> { delay = 3.22091900635453ms; } <--> ccn_node[18].face++;
	ccn_node[23].face++ <--> { delay = 6.55702429142063ms; } <--> ccn_node[8].face++;
	ccn_node[24].face++ <--> { delay = 16.4971634786592ms; } <--> ccn_node[10].face++;
	ccn_node[24].face++ <--> { delay = 3.24743960868072ms; } <--> ccn_node[11].face++;
	ccn_node[24].face++ <--> { delay = 17.2525090468823ms; } <--> ccn_node[16].face++;
	ccn_node[24].face++ <--> { delay = 16.2545272881117ms; } <--> ccn_node[17].face++;
	ccn_node[24].face++ <--> { delay = 2.46048098729345ms; } <--> ccn_node[18].face++;
	ccn_node[24].face++ <--> { delay = 8.22107498325243ms; } <--> ccn_node[8].face++;
	ccn_node[25].face++ <--> { delay = 15.9481449750702ms; } <--> ccn_node[10].face++;
	ccn_node[25].face++ <--> { delay = 3.88956907223441ms; } <--> ccn_node[11].face++;
	ccn_node[25].face++ <--> { delay = 4.11276174689792ms; } <--> ccn_node[13].face++;
	ccn_node[25].face++ <--> { delay = 16.6349260061567ms; } <--> ccn_node[16].face++;
	ccn_node[25].face++ <--> { delay = 15.5239806328732ms; } <--> ccn_node[17].face++;
	ccn_node[25].face++ <--> { delay = 3.21198066343449ms; } <--> ccn_node[18].face++;
	ccn_node[26].face++ <--> { delay = 11.0914798639502ms; } <--> ccn_node[10].face++;
	ccn_node[26].face++ <--> { delay = 11.4027441257575ms; } <--> ccn_node[11].face++;
	ccn_node[26].face++ <--> { delay = 12.9243808535192ms; } <--> ccn_node[16].face++;
	ccn_node[26].face++ <--> { delay = 15.1947572412764ms; } <--> ccn_node[17].face++;
	ccn_node[26].face++ <--> { delay = 9.81518726240613ms; } <--> ccn_node[18].face++;
	ccn_node[26].face++ <--> { delay = 1.51577808645583ms; } <--> ccn_node[33].face++;
	ccn_node[27].face++ <--> { delay = 1.85801368545196ms; } <--> ccn_node[10].face++;
	ccn_node[27].face++ <--> { delay = 17.9391116191711ms; } <--> ccn_node[11].face++;
	ccn_node[27].face++ <--> { delay = 3.03748851968468ms; } <--> ccn_node[16].face++;
	ccn_node[27].face++ <--> { delay = 6.98824082413304ms; } <--> ccn_node[17].face++;
	ccn_node[27].face++ <--> { delay = 16.7787667562161ms; } <--> ccn_node[18].face++;
	ccn_node[28].face++ <--> { delay = 8.77880309119896ms; } <--> ccn_node[11].face++;
	ccn_node[28].face++ <--> { delay = 5.62820642196231ms; } <--> ccn_node[13].face++;
	ccn_node[28].face++ <--> { delay = 20.5757186821716ms; } <--> ccn_node[16].face++;
	ccn_node[28].face++ <--> { delay = 21.9693515292004ms; } <--> ccn_node[17].face++;
	ccn_node[28].face++ <--> { delay = 7.44426393180017ms; } <--> ccn_node[18].face++;
	ccn_node[29].face++ <--> { delay = 19.3571455209828ms; } <--> ccn_node[16].face++;
	ccn_node[29].face++ <--> { delay = 20.48516282975ms; } <--> ccn_node[17].face++;
	ccn_node[29].face++ <--> { delay = 6.10053908846644ms; } <--> ccn_node[18].face++;
	ccn_node[3].face++ <--> { delay = 27.8377559786444ms; } <--> ccn_node[11].face++;
	ccn_node[3].face++ <--> { delay = 38.4696116871562ms; } <--> ccn_node[17].face++;
	ccn_node[3].face++ <--> { delay = 29.4645463284345ms; } <--> ccn_node[18].face++;
	ccn_node[3].face++ <--> { delay = 1.71246693397065ms; } <--> ccn_node[4].face++;
	ccn_node[3].face++ <--> { delay = 6.31536032045267ms; } <--> ccn_node[42].face++;
	ccn_node[3].face++ <--> { delay = 4.58678251895811ms; } <--> ccn_node[43].face++;
	ccn_node[3].face++ <--> { delay = 3.87862930965681ms; } <--> ccn_node[44].face++;
	ccn_node[3].face++ <--> { delay = 4.78915796529137ms; } <--> ccn_node[45].face++;
	ccn_node[30].face++ <--> { delay = 20.1067873227153ms; } <--> ccn_node[16].face++;
	ccn_node[30].face++ <--> { delay = 19.0896785660613ms; } <--> ccn_node[17].face++;
	ccn_node[30].face++ <--> { delay = 1.00741082347545ms; } <--> ccn_node[18].face++;
	ccn_node[30].face++ <--> { delay = 0.635958566536213ms; } <--> ccn_node[39].face++;
	ccn_node[31].face++ <--> { delay = 0.714419239023442ms; } <--> ccn_node[16].face++;
	ccn_node[31].face++ <--> { delay = 5.03198520154489ms; } <--> ccn_node[17].face++;
	ccn_node[31].face++ <--> { delay = 19.0869253442257ms; } <--> ccn_node[18].face++;
	ccn_node[32].face++ <--> { delay = 4.70463733527364ms; } <--> ccn_node[16].face++;
	ccn_node[32].face++ <--> { delay = 5.62676654420176ms; } <--> ccn_node[17].face++;
	ccn_node[32].face++ <--> { delay = 14.8356694118214ms; } <--> ccn_node[18].face++;
	ccn_node[33].face++ <--> { delay = 11.6636906847763ms; } <--> ccn_node[16].face++;
	ccn_node[33].face++ <--> { delay = 14.3563007935942ms; } <--> ccn_node[17].face++;
	ccn_node[33].face++ <--> { delay = 11.1513687338088ms; } <--> ccn_node[18].face++;
	ccn_node[34].face++ <--> { delay = 0.381420811600469ms; } <--> ccn_node[16].face++;
	ccn_node[34].face++ <--> { delay = 5.46425584185052ms; } <--> ccn_node[17].face++;
	ccn_node[34].face++ <--> { delay = 19.6506122689107ms; } <--> ccn_node[18].face++;
	ccn_node[35].face++ <--> { delay = 5.71485509784267ms; } <--> ccn_node[18].face++;
	ccn_node[36].face++ <--> { delay = 1.90457181681882ms; } <--> ccn_node[18].face++;
	ccn_node[37].face++ <--> { delay = 6.58757021264865ms; } <--> ccn_node[18].face++;
	ccn_node[38].face++ <--> { delay = 18.3446142117741ms; } <--> ccn_node[18].face++;
	ccn_node[4].face++ <--> { delay = 30.7859883557325ms; } <--> ccn_node[18].face++;
	ccn_node[40].face++ <--> { delay = 3.66097104952236ms; } <--> ccn_node[13].face++;
	ccn_node[40].face++ <--> { delay = 5.97938876597481ms; } <--> ccn_node[14].face++;
	ccn_node[40].face++ <--> { delay = 15.5538373024986ms; } <--> ccn_node[16].face++;
	ccn_node[41].face++ <--> { delay = 3.60467312251354ms; } <--> ccn_node[3].face++;
	ccn_node[42].face++ <--> { delay = 28.8193415655172ms; } <--> ccn_node[11].face++;
	ccn_node[42].face++ <--> { delay = 30.4067905705861ms; } <--> ccn_node[18].face++;
	ccn_node[45].face++ <--> { delay = 33.9252207063578ms; } <--> ccn_node[18].face++;
	ccn_node[5].face++ <--> { delay = 15.5956600742354ms; } <--> ccn_node[10].face++;
	ccn_node[5].face++ <--> { delay = 6.00743024175476ms; } <--> ccn_node[11].face++;
	ccn_node[5].face++ <--> { delay = 12.7518948089011ms; } <--> ccn_node[12].face++;
	ccn_node[5].face++ <--> { delay = 2.87054830888084ms; } <--> ccn_node[13].face++;
	ccn_node[5].face++ <--> { delay = 5.87074289160728ms; } <--> ccn_node[14].face++;
	ccn_node[5].face++ <--> { delay = 15.1431230870802ms; } <--> ccn_node[15].face++;
	ccn_node[5].face++ <--> { delay = 16.9491450566271ms; } <--> ccn_node[16].face++;
	ccn_node[5].face++ <--> { delay = 17.5143459791053ms; } <--> ccn_node[17].face++;
	ccn_node[5].face++ <--> { delay = 4.38018878263173ms; } <--> ccn_node[18].face++;
	ccn_node[5].face++ <--> { delay = 1.83053301410572ms; } <--> ccn_node[6].face++;
	ccn_node[5].face++ <--> { delay = 4.71810812635976ms; } <--> ccn_node[7].face++;
	ccn_node[5].face++ <--> { delay = 5.75137875158028ms; } <--> ccn_node[8].face++;
	ccn_node[5].face++ <--> { delay = 9.67081135208023ms; } <--> ccn_node[9].face++;
	ccn_node[6].face++ <--> { delay = 17.0722673437741ms; } <--> ccn_node[10].face++;
	ccn_node[6].face++ <--> { delay = 4.27086377025817ms; } <--> ccn_node[11].face++;
	ccn_node[6].face++ <--> { delay = 18.2794162395011ms; } <--> ccn_node[16].face++;
	ccn_node[6].face++ <--> { delay = 18.3495988076097ms; } <--> ccn_node[17].face++;
	ccn_node[6].face++ <--> { delay = 2.65814072487666ms; } <--> ccn_node[18].face++;
	ccn_node[6].face++ <--> { delay = 7.44290219054975ms; } <--> ccn_node[8].face++;
	ccn_node[7].face++ <--> { delay = 14.0526484629278ms; } <--> ccn_node[10].face++;
	ccn_node[7].face++ <--> { delay = 5.72539765160039ms; } <--> ccn_node[11].face++;
	ccn_node[7].face++ <--> { delay = 11.6351669582945ms; } <--> ccn_node[12].face++;
	ccn_node[7].face++ <--> { delay = 5.1339999825284ms; } <--> ccn_node[13].face++;
	ccn_node[7].face++ <--> { delay = 5.66006296520887ms; } <--> ccn_node[14].face++;
	ccn_node[7].face++ <--> { delay = 13.8786751988483ms; } <--> ccn_node[15].face++;
	ccn_node[7].face++ <--> { delay = 14.7744081963979ms; } <--> ccn_node[16].face++;
	ccn_node[7].face++ <--> { delay = 13.9413353089448ms; } <--> ccn_node[17].face++;
	ccn_node[7].face++ <--> { delay = 4.79269864527852ms; } <--> ccn_node[18].face++;
	ccn_node[7].face++ <--> { delay = 2.01223970834797ms; } <--> ccn_node[23].face++;
	ccn_node[7].face++ <--> { delay = 2.47832477920296ms; } <--> ccn_node[24].face++;
	ccn_node[7].face++ <--> { delay = 1.89714357641037ms; } <--> ccn_node[25].face++;
	ccn_node[7].face++ <--> { delay = 7.55619251202906ms; } <--> ccn_node[26].face++;
	ccn_node[7].face++ <--> { delay = 12.2477964598261ms; } <--> ccn_node[27].face++;
	ccn_node[7].face++ <--> { delay = 9.56272458116584ms; } <--> ccn_node[28].face++;
	ccn_node[7].face++ <--> { delay = 7.91959880074888ms; } <--> ccn_node[29].face++;
	ccn_node[7].face++ <--> { delay = 31.7641191210355ms; } <--> ccn_node[3].face++;
	ccn_node[7].face++ <--> { delay = 5.3491117284948ms; } <--> ccn_node[30].face++;
	ccn_node[7].face++ <--> { delay = 14.379523395025ms; } <--> ccn_node[31].face++;
	ccn_node[7].face++ <--> { delay = 10.1122732690684ms; } <--> ccn_node[32].face++;
	ccn_node[7].face++ <--> { delay = 8.43819072387166ms; } <--> ccn_node[33].face++;
	ccn_node[7].face++ <--> { delay = 14.960010383025ms; } <--> ccn_node[34].face++;
	ccn_node[7].face++ <--> { delay = 2.09122114457625ms; } <--> ccn_node[35].face++;
	ccn_node[7].face++ <--> { delay = 5.87936801526091ms; } <--> ccn_node[36].face++;
	ccn_node[7].face++ <--> { delay = 8.07205861019873ms; } <--> ccn_node[37].face++;
	ccn_node[7].face++ <--> { delay = 13.9077012943739ms; } <--> ccn_node[38].face++;
	ccn_node[7].face++ <--> { delay = 5.71376896220664ms; } <--> ccn_node[39].face++;
	ccn_node[7].face++ <--> { delay = 6.43760504337832ms; } <--> ccn_node[8].face++;
	ccn_node[7].face++ <--> { delay = 7.31964781963753ms; } <--> ccn_node[9].face++;
	ccn_node[8].face++ <--> { delay = 10.0456672433203ms; } <--> ccn_node[10].face++;
	ccn_node[8].face++ <--> { delay = 11.0009242092306ms; } <--> ccn_node[11].face++;
	ccn_node[8].face++ <--> { delay = 7.13022347450063ms; } <--> ccn_node[12].face++;
	ccn_node[8].face++ <--> { delay = 8.46405012296202ms; } <--> ccn_node[13].face++;
	ccn_node[8].face++ <--> { delay = 10.8851212713146ms; } <--> ccn_node[14].face++;
	ccn_node[8].face++ <--> { delay = 9.5058815972022ms; } <--> ccn_node[15].face++;
	ccn_node[8].face++ <--> { delay = 11.6748469301052ms; } <--> ccn_node[16].face++;
	ccn_node[8].face++ <--> { delay = 13.5297009811618ms; } <--> ccn_node[17].face++;
	ccn_node[8].face++ <--> { delay = 9.49836499313979ms; } <--> ccn_node[18].face++;
	ccn_node[8].face++ <--> { delay = 8.01165821504847ms; } <--> ccn_node[25].face++;
	ccn_node[8].face++ <--> { delay = 1.79734274874247ms; } <--> ccn_node[26].face++;
	ccn_node[8].face++ <--> { delay = 8.65279966988776ms; } <--> ccn_node[27].face++;
	ccn_node[8].face++ <--> { delay = 8.90426431888901ms; } <--> ccn_node[28].face++;
	ccn_node[8].face++ <--> { delay = 7.70699136339725ms; } <--> ccn_node[29].face++;
	ccn_node[8].face++ <--> { delay = 10.4325406765224ms; } <--> ccn_node[30].face++;
	ccn_node[8].face++ <--> { delay = 11.582016414369ms; } <--> ccn_node[31].face++;
	ccn_node[8].face++ <--> { delay = 8.0588108384029ms; } <--> ccn_node[32].face++;
	ccn_node[8].face++ <--> { delay = 2.0304558691511ms; } <--> ccn_node[33].face++;
	ccn_node[8].face++ <--> { delay = 11.9855228230229ms; } <--> ccn_node[34].face++;
	ccn_node[8].face++ <--> { delay = 4.38860704167783ms; } <--> ccn_node[35].face++;
	ccn_node[8].face++ <--> { delay = 11.2430856502308ms; } <--> ccn_node[36].face++;
	ccn_node[8].face++ <--> { delay = 7.34711766697769ms; } <--> ccn_node[37].face++;
	ccn_node[8].face++ <--> { delay = 9.78885581940582ms; } <--> ccn_node[38].face++;
	ccn_node[8].face++ <--> { delay = 11.0037851811598ms; } <--> ccn_node[39].face++;
	ccn_node[8].face++ <--> { delay = 4.93325950050165ms; } <--> ccn_node[40].face++;
	ccn_node[8].face++ <--> { delay = 5.31257916564315ms; } <--> ccn_node[9].face++;
	ccn_node[9].face++ <--> { delay = 6.77495897647902ms; } <--> ccn_node[10].face++;
	ccn_node[9].face++ <--> { delay = 13.0319252204767ms; } <--> ccn_node[11].face++;
	ccn_node[9].face++ <--> { delay = 4.71855022803752ms; } <--> ccn_node[12].face++;
	ccn_node[9].face++ <--> { delay = 12.9594752263768ms; } <--> ccn_node[14].face++;
	ccn_node[9].face++ <--> { delay = 6.71050946590421ms; } <--> ccn_node[15].face++;
	ccn_node[9].face++ <--> { delay = 7.50910338728108ms; } <--> ccn_node[16].face++;
	ccn_node[9].face++ <--> { delay = 8.23946958515862ms; } <--> ccn_node[17].face++;
	ccn_node[9].face++ <--> { delay = 11.9373254278376ms; } <--> ccn_node[18].face++;
	ccn_node[9].face++ <--> { delay = 7.03404047235019ms; } <--> ccn_node[26].face++;
	ccn_node[9].face++ <--> { delay = 4.94290461777074ms; } <--> ccn_node[27].face++;
	ccn_node[9].face++ <--> { delay = 13.8282594395366ms; } <--> ccn_node[28].face++;
	ccn_node[9].face++ <--> { delay = 12.4154307252622ms; } <--> ccn_node[29].face++;
	ccn_node[9].face++ <--> { delay = 12.618408070217ms; } <--> ccn_node[30].face++;
	ccn_node[9].face++ <--> { delay = 3.03927743798862ms; } <--> ccn_node[32].face++;
	ccn_node[9].face++ <--> { delay = 7.72580873446228ms; } <--> ccn_node[34].face++;
	ccn_node[9].face++ <--> { delay = 6.65853544578616ms; } <--> ccn_node[38].face++;

}
