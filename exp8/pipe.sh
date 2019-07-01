# 创建管道文件
if [ -e "myfifo" ]
then
    rm ./myfifo
    mkfifo ./myfifo
else
    mkfifo ./myfifo
fi

clear

# 在死循环中不停的读取管道内容，读完之后根据读到的内容进行操作，然后写回管道中
#resp=""
while true
do
    r=$(tr -d '\0' < myfifo)
    case "$r" in
    1)
        resp=`ls` ;;
    2)
        resp=`date`  ;;
    3)
        resp=`uname -a`  ;;
    *)
        resp="你的输入有误" ;;
    esac

    echo $resp > myfifo
    echo -e "Sent [ "$resp" ] to fifo\n"

done
