#!/usr/bin/perl
#L- are the total level
#i- is the actual level of the onion (0..L-1)
#p- connectivity probability
#degree- average conn. degree

$L=3;
$c=0;
$degree =3;

#print out round levels connection
for $i(0..$L-1){
    $nodes[$i] = int(rand(10))+1;
    #$nodes[$i] = 5*2**$i-1;
    print "//Level $i has $nodes[$i] nodes, namely:\n";

    for $n($c..$c+($nodes[$i]-1) -1){
	&connect($n,$n+1);
    }
    &connect($c+$nodes[$i]-1,$c);

    $c+=$nodes[$i];
}


#Start printing inter-round connections 
$c_old=0;
$c = $nodes[0];

for $i(1..$L-1){

    print "//Level $i and ",$i-1," connections\n";
    $p = $degree/($nodes[$i]+$nodes[$i-1]);
    print "//With $degree/($nodes[$i]+",$nodes[$i-1],")= $p conn. probability\n";

    for $ne($c..$c+($nodes[$i]-1)){
	for $ni($c_old..$c_old+($nodes[$i-1]-1)){
	    if (rand(1)<$p){
		&connect($ne,$ni);
	    }
	}
    } 
    
    print "\n";
    $c_old=$c;
    $c+=$nodes[$i];
}



#node[1].face++ <-->  { delay = 24.34ms; } <--> node[0].face++;
sub connect(){
    ($s,$d)=@_;
    print "node[$s].face++ <--> { delay = 1ms } <--> node[$d].face++;\n"
}
