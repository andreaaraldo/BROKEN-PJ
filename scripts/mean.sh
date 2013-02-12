for i in 0 `seq 0.1 0.1 0.9` 1 ;do 
    printf "$i\t";grep "$1" $2/beta0.9/co$i/CCN-ID*.sca | awk '{sum+=$4}END{print sum/NR}';
done
