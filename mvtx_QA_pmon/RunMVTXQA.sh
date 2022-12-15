echo "Now Running QA"

cycle=1

 rm NewRun.txt
# cd workdir
 rm OldRun.txt
 
  touch  NewRun.txt
  touch  OldRun.txt

  	
  rm FileCount.txt

  echo 0 >> FileCount.txt


while [ 1 -lt 2 ]
do
	
   cp NewRun.txt OldRun.txt
	
   echo "cycle = "  ${cycle} 


   cycle=$((cycle+1)) 
   

   ls -1 workdir/* > NewRun.txt


   echo "----------- Diff ---------------"

   #diff  NewRun.txt OldRun.txt
 
   diff NewRun.txt OldRun.txt | tail | awk '{print $2}' > Diff.txt


  # grep -Fxvf NewRun.txt OldRun.txt > Diff.txt

   echo "----------- DONE ---------------"

   #echo  `grep -Eo '[1-9]{1,4}' Diff.txt`  > RunInfo.txt

   

   if [ -s Diff.txt ]; then 
	root -b -l -q open_file.C
	#root -l -q open_file.C

   else

	   echo "No New Files in This Cycle"

   fi

   echo "Now Waiting for new files to be injected"



   sleep 10
    

done
  






