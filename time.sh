for i in {1,4,5,7,9,10,11,13,14,17,18,19};
#for i in {19..19};
do 
  (time ./lrg-sflip ./data/final_cases_publish_1009/case$i/io_info.txt ./data/final_cases_publish_1009/case$i/iogen cir/circuit$i.blif) > time/report_$i.txt 2>&1
  ./test ./data/final_cases_publish_1009/case$i/io_info.txt ./data/final_cases_publish_1009/case$i/iogen cir/circuit$i.blif > acc/report_$i.txt 
done