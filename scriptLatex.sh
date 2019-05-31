#!/bin/bash 

#to choose the proper script, digit the word after bash scriptLatex.sh


#Useful commands
#cat provvisorio.txt | awk '{print $3}' | sed ':l; N; s/\n/,/; tl' ; the last exchange \n (a capo) with ,
#cat provvisorio.txt | sed 's/\\\\/ /'   without the g it replaces only one occurrence
#										 per row, otherwise use sed 's/\\\\/ /g'
#cat provvisorio.txt | awk '{print $1, $3}'; prints the first and the third column	
#bash scriptLatex.sh extractImaging | sed ':l; N; s/\n/ /; tl' | sed 's/\\\\/\\\\ \n /g'; #to "fix" the a capo
#to properly align all use this command, the first eliminates all \n , then substitute \\ with \\ \n 
#bash scriptLatex.sh extractImaging | sed ':l; N; s/\n/ /; tl' | sed 's/\\\\/\\\\ \n /g'


#this script is for add another column in latex (to remove a column, you can use the button remove column of texstudio)
 # THEN TO PROPERLY ALIGN THEM, USE ALIGN COLUMN OF TEXSTUDIO
 #use provvisorio files to do the job
 if [ $1 = addfinal ] 
 then
 for i in `seq 1 5`;  #to execute,  bash scriptLatex.sh addfinal
 do 
     head -n $i provvisorio.txt | awk '{print $1, $2, $3, $4, $5, $6}' |  #add or remove the colums you need
	 tail -n +$i;
	head -n $i provvisorio2.txt | tail -n +$i;#usually the second file has only one column
	 echo \\\\;  #to do the backslash \\

done 	
fi									
#head -n  5 prints the first 5 lines, tail -n +5 prints the desired line
#use -n +K to output lines starting with the Kth; 


#this is for adding the i column of provvisorio2 as a j column of provvisorio
 if [ $1 = addmiddle ] 
 then
for i in `seq 1 9`;  #to execute,  bash scriptLatex.sh
 do 
     head -n $i provvisorio.txt | awk '{print $1, $2}' |  #add or remove the colums you need
	 tail -n +$i; 
	 head -n $i provvisorio2.txt | awk '{print $1}' | tail -n +$i;
	 echo -n '& ';  # echo -n to not go a capo
	
	head -n $i provvisorio.txt | awk '{print $3, $4, $5, $6, $7}' |  #add or remove the colums you need
	 tail -n +$i;
done 				
fi

#this is to put what there is in a text file in an array for c++, this is an example.
 if [ $1 = array ] 
 then
for i in `seq 2 6`;  #to execute,  bash scriptLatex.sh array
 do 
	echo \{ ;
	for j in `seq 0 6`;
	do
	num=$(expr $i + 6 \* $j); # This was due to how the txt file was 
	
     head -n $num provvisorio2.txt | awk '{print $1}' |  #add or remove the colums you need
	 tail -n +$num;
	 echo -n ,; 
	done
	echo \},;
		
done | 	sed ':l; N; s/\n/ /; tl' | 	sed 's/,\},/\},\n /g'	#all these commands are executed after the cycle.
fi															#this are there to put the printed table in terminal right

if [ $1 = array2 ] 
 then
for i in `seq 1 7`;  #to execute,  bash scriptLatex.sh array2
 do 
	echo \{ ;
	  head -n $i provvisorio.txt | awk '{print $2}' | tail -n +$i;
	 echo -n , ; 
	 head -n $i provvisorio.txt | awk '{print $5}' | tail -n +$i;
	 echo -n , ; 
	 head -n $i provvisorio.txt | awk '{print $8}' | tail -n +$i;
	 echo -n , ; 
	 head -n $i provvisorio.txt | awk '{print $11}' | tail -n +$i;
	 echo -n , ; 
	 head -n $i provvisorio.txt | awk '{print $14}' | tail -n +$i;
	 echo \},;
	  
		
done | 	sed ':l; N; s/\n/ /; tl' | 	sed 's/\},/\},\n /g' | sed 's/-/0 /g'	#all these commands are executed after the cycle.
fi


# The following are specific for certain text file created by certain c++ functions for data elaboration/put latex tables
####
##### IMAGING


#this is for imaging, take as an example for future works
# extract data from a txt file in a "latex suitable way"
 if [ $1 = extractImaging ] 
 then
for i in `seq 2 6`;  #to execute,  bash scriptLatex.sh extractImaging
 do 
	for j in `seq 0 6`;
	do
	num=$(expr $i + 6 \* $j); # with expr you evaluate mathematically the expression
     head -n $num provvisorio2.txt | awk '{print $1}' |  #add or remove the colums you need
	 tail -n +$num;
	 echo -n ' $\pm$ '; 
	 head -n $num provvisorio2.txt | awk '{print $3}' | tail -n +$num;
	 echo -n '& ';  # echo -n to not go a capo
	done
	echo '\\';
		
done 				
fi
#if you need to remove a column, use removecolumn of texstudio


#associated to attenuationCoeffCalculus()
if [ $1 = attcoeff ] 
 then
for i in `seq 1 7`;  #to execute,  bash scriptLatex.sh attcoeff
 do 					# change the lines in seq for what you need
	 head -n $i provvisorio.txt | awk '{print $1}' |  tail -n +$i;
      echo -n '& ';
      head -n $i provvisorio.txt | awk '{print $2}' |  tail -n +$i; 
       echo -n ' $\pm$ '; 
	 head -n $i provvisorio.txt | awk '{print $4}' | tail -n +$i;
	 echo -n '& ';  # echo -n to not go a capo
	head -n $i provvisorio.txt | awk '{print $5}' | tail -n +$i;
	echo -n ' $\pm$ '; 
	 head -n $i provvisorio.txt | awk '{print $7}' | tail -n +$i;
		 echo -n '& ';  # echo -n to not go a capo
	 head -n $i provvisorio.txt | awk '{print $8}' |  tail -n +$i; 
       echo -n ' $\pm$ '; 
	 head -n $i provvisorio.txt | awk '{print $10}' | tail -n +$i;
	 echo -n '& ';
	  head -n $i provvisorio.txt | awk '{print $11}' |  tail -n +$i;
	  echo -n ' $\pm$ '; 
	 head -n $i provvisorio.txt | awk '{print $13}' | tail -n +$i;
		 echo -n '& ';  # echo -n to not go a capo
	 head -n $i provvisorio.txt | awk '{print $14}' |  tail -n +$i; 
       echo -n ' $\pm$ '; 
	 head -n $i provvisorio.txt | awk '{print $16}' | tail -n +$i;
	 echo ' \\';
done | 	sed ':l; N; s/\n/ /; tl' | 	sed 's/\\\\/\\\\ \n /g'	#all these commands are executed after the cycle.
	
fi







#####
##### GAMMA EXPERIMENT
#this is to put what there is in a text file in a tex file.
 if [ $1 = intrinsicefficiency ] 
 then
for i in `seq 2 4`;  #to execute,  bash scriptLatex.sh intrinsicefficiency
 do 
		
     head -n $i provvisorio.txt | awk '{print $1}' | tail -n +$i;
     echo -n '& ';
     head -n $i provvisorio.txt | awk '{print $2}' |  #add or remove the colums you need
	 tail -n +$i;
	 echo -n ' $\pm$ '; 
	 head -n $i provvisorio.txt | awk '{print $3}' | tail -n +$i;
	 echo -n '& ';  # echo -n to not go a capo
	head -n $i provvisorio.txt | awk '{print $4}' | tail -n +$i;
	echo -n ' $\pm$ '; 
	 head -n $i provvisorio.txt | awk '{print $5}' | tail -n +$i;
	echo '\\';
		
done | 	sed ':l; N; s/\n/ /; tl' | 	sed 's/\\\\/\\\\ \n /g'	#all these commands are executed after the cycle.
	echo 'HPGe';
for i in `seq 6 10`;
 do 
	
	 head -n $i provvisorio.txt | awk '{print $1}' | tail -n +$i;
     echo -n '& ';	
     head -n $i provvisorio.txt | awk '{print $2}' |  #add or remove the colums you need
	 tail -n +$i;
	 echo -n ' $\pm$ '; 
	 head -n $i provvisorio.txt | awk '{print $3}' | tail -n +$i;
	 echo -n '& ';  # echo -n to not go a capo
	head -n $i provvisorio.txt | awk '{print $4}' | tail -n +$i;
	echo -n ' $\pm$ '; 
	 head -n $i provvisorio.txt | awk '{print $5}' | tail -n +$i;
	echo '\\';
		
done | 	sed ':l; N; s/\n/ /; tl' | 	sed 's/\\\\/\\\\ \n /g'	#all these commands are executed after the cycle.

fi


#this is to put what there is in a text file in a tex file.
 if [ $1 = errorcounts ] 
 then
for i in `seq 1 18`;  #to execute,  bash scriptLatex.sh errorcounts
 do 
		
     head -n $i provvisorio.txt | awk '{print $1, $2, $3}' |  #add or remove the colums you need
	 tail -n +$i;
	 echo -n ' $\pm$ '; 
	 head -n $i provvisorio2.txt | awk '{print $2}' | tail -n +$i;
	 echo -n '& ';  # echo -n to not go a capo
	head -n $i provvisorio2.txt | awk '{print $3}' | tail -n +$i;
	echo -n ' $\pm$ '; 
	 head -n $i provvisorio2.txt | awk '{print $4}' | tail -n +$i;
	echo '\\';
		
done | 	sed ':l; N; s/\n/ /; tl' | 	sed 's/\\\\/\\\\ \n /g'	#all these commands are executed after the cycle.
	
fi

#This is to print on terminal the peaks from the latex file, I think I don't need this anymore
 if [ $1 = printpeaks ] 
 then
for i in `seq 1 14`;  #to execute,  bash scriptLatex.sh errorcounts
 do 
		
     head -n $i provvisorio.txt | awk '{print $1}' |  #add or remove the colums you need
	 tail -n +$i;
	 echo -n ', '; 
	 
		
done | 	sed ':l; N; s/\n//; tl' | sed 's/ ,/0,/g'	#all these commands are executed after the cycle.
	echo ;
fi


# associated to Efficiency()
 if [ $1 = efficiency ] 
 then
for i in `seq 1 6`;  #to execute,  bash scriptLatex.sh efficiency
 do 
		
     head -n $i provvisorio.txt | tail -n +$i;
	 head -n $i provvisorio2.txt | tail -n +$i;
	 
		
done | 	 sed 's/, },/},/g'	#all these commands are executed after the cycle.
	
fi

#associated to Activity()
if [ $1 = activity ] 
 then
for i in `seq 211 224`;  #to execute,  bash scriptLatex.sh activity
 do 					# change the lines in seq for what you need
	j=$(expr $i - 210);
     head -n $i provvisorio.txt | awk '{print $1}' |  tail -n +$i;
      echo -n '& ';
      head -n $i provvisorio.txt | awk '{print $2}' |  tail -n +$i; 
       echo -n ' $\pm$ '; 
	 head -n $i provvisorio.txt | awk '{print $3}' | tail -n +$i;
	 echo -n '& ';  # echo -n to not go a capo
	head -n $i provvisorio.txt | awk '{print $4}' | tail -n +$i;
	echo -n ' $\pm$ '; 
	 head -n $i provvisorio.txt | awk '{print $5}' | tail -n +$i;
		 echo -n '& ';  # echo -n to not go a capo
	 head -n $i provvisorio.txt | awk '{print $6}' |  tail -n +$i; 
       echo -n ' $\pm$ '; 
	 head -n $i provvisorio.txt | awk '{print $7}' | tail -n +$i;
	 echo -n '& ';
	  head -n $i provvisorio.txt | awk '{print $8}' |  tail -n +$i;
	echo ' & ';
	  head -n $j provvisorio3.txt | awk '{print $17}' |  tail -n +$j; 
	  echo ' \\';
		
done | 	sed ':l; N; s/\n/ /; tl' | 	sed 's/\\\\/\\\\ \n /g'	#all these commands are executed after the cycle.
	
fi

