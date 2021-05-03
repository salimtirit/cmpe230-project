make
for currentCount in {0..18}
do
   ./mylang2ir ./inputs/testcase$currentCount.my
   lli inputs/testcase$currentCount.ll > inputs/testcase$currentCount.txt
   diff -s outputs/testcase$currentCount.txt inputs/testcase$currentCount.txt
    #
done
