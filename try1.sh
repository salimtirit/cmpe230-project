make
for currentCount in {1..11}
do
    time ./mylang2ir ./myCases/inputs/testcase$currentCount.my
    lli ./myCases/inputs/testcase$currentCount.ll > ./myCases/inputs/testcase$currentCount.txt
    diff -s myCases/outputs/testcase$currentCount.txt myCases/inputs/testcase$currentCount.txt
    #
done
