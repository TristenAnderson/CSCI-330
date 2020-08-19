#! /bin/bash
#CSCI 330	Assignment 5
#Tristen Anderson	z1820036
#Purpose: Finds misspellings in a text file and flags them. Allows 
#the user to add words to memory, such that they are not flagged again.

if [ $# -lt 1 ]; then					#if number of arguments is under 1 then no file was given
	echo "No Input File Given"
	exit 1
fi

if [ ! -f "$1" ]; then					#check if it is a file
	echo "$* is not a file"
	exit 2
fi

if [ ! -r "$1" ]; then					#check for read permission
	echo "$* file is not readable"
	exit 3
fi

corrections=()							#initialize storage arrays
misspell=()
count=1
touch ~/.memory							#update memory file

for i in $(cat "$1" | aspell list)		#for every word in the misspell list
do
	if [ ! $(grep $i ~/.memory) ]; then		#if the word is not in memory
		
		echo -e "'$i' is misspelled. Press \"Enter\" to keep"
		read -p "this spelling, or type a correction here: " answer
		echo -e "\n"
		
		if [ "$answer" = "" ]; then			#if you dont wont to correct add it to memory
			echo "$i" >> ~/.memory
		else
			corrections[$count]="$answer"		# copy the corrections and misspells into arrays
			misspell[$count]="$i"
			count=$((count+1))
			
		fi
	fi	
done

echo -e "MISSPELLED:\tCORRECTIONS:\n"		# print labels 

for j in $(seq $count)						#print the corrections and misspells 
do
	echo -e ${misspell[$j]} "\t       " ${corrections[$j]}
done

#for each word that os spelled incorrectly the invoker is asked either 
# to insist on the spelling
# or tp provide a replacement
