
cur_dir=$(cd $( dirname ${BASH_SOURCE[0]} ) && pwd )

root_dir=$cur_dir

cd $root_dir

redo=1

data_root_dir=/media/xiaosa/xiaosa/学习/深度学习/目标检测/一阶段网络/YOLO/MobileNet-YOLO-master/data/VOC0712/  # Modify the path with your folder having Images and Labels directories.

dataset_name="VOC_PERSON"  # This will be directory holding train and test LMDBs

mapfile=labelmap_voc_person.prototxt  # Labelmap file for you dataset.

# Modify the below things if necessary.
anno_type="detection"
label_type="xml"
db="lmdb"
min_dim=0
max_dim=0
width=0
height=0


extra_cmd="--encode-type=jpg --encoded"

# Modify the caffe installation path in below line. /home/vkchcp0013/Documents/mobile-ssd/caffe/scripts/create_annoset.py

if [ $redo ] 
then
  extra_cmd="$extra_cmd --redo"
fi
for subset in   test train
 do
  python2 /media/xiaosa/xiaosa/学习/深度学习/目标检测/一阶段网络/YOLO/MobileNet-YOLO-master/scripts/create_annoset.py --anno-type=$anno_type --shuffle  --label-type=$label_type --label-map-file=$mapfile --min-dim=$min_dim --max-dim=$max_dim --resize-width=$width --resize-height=$height --check-label $extra_cmd $data_root_dir $data_root_dir/$subset.txt $data_root_dir/$dataset_name/$db/$dataset_name"_"$subset"_"$db examples/$dataset_name 
done

