for i in {1,4,5,7,9,10,11,13,14,17,18,19};
#for i in 1;
do
  ./abc_opt ./data/final_cases_publish_1009/case$i/io_info.txt ./data/final_cases_publish_1009/case$i/iogen cir/circuit$i.blif opt/citcuit$i.v > acc/report_$i.txt 
  python src/cal_size.py opt/citcuit$i.v
done