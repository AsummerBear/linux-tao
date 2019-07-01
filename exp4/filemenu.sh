until 
echo 1 list directory
echo 2 change directory
echo 3 edit file
echo 4 delete file
echo 5 exit menu
read choice
false
do
	case $choice in
	1) ls
		;;
	2) read dir
	   	cd $dir
	   	pwd
	   	;;
	3) read file
	  	vim $file
	  	;;
	4) read file
	  	rm $file
	  	;;
	q|Q|5) echo "good bye"
	  	exit
		;;
	*) echo "illegal option"
	  	;;
	esac
done
