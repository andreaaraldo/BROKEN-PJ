#!/usr/bin/perl
$rows=  shift || 4;
$cols=  shift || 4;
#$cols= shift ||;


# || 4;
#row connections
foreach $i(0..$rows-1){
    foreach $j($cols*$i..$cols*$i + $cols-1-1){
	printlink($j,$j+1);

    }
    printlink($cols*$i+$cols-1,$cols*$i) unless $cols==1;
}

#column connections
foreach $i(0..$cols-1){
    for ($j=$i;$j<$i+$cols*($rows-1);$j+=$cols){
	printlink ($j,$j+$cols);
    }
    printlink ($j,$i) unless $rows==1;
}


sub printlink {
    my ($i,$j)=@_;
    print "node[$i].face++ <--> { delay = 1ms; } <--> node[$j].face++;\n";
}
