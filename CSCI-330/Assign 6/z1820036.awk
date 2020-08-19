#Tristen Anderson 
#z1820036
#CSCI 330 Assignment 6
#Purpose: read through an input file and find sales data from the year 2014
# and process totals per month, station and the total

#! /bin/awk -f

BEGIN {	
print "Sales Data for year 2014"	#print out a header and initialize variables to zero
print "========================"
count = 0
sumX = 0
sumY = 0
sumZ = 0
}

{
if ($1 == 2014){
	
	sum = 0				#set record sum to zero for each record
	for(i = 4; i<=NF; i++)		#get the sum of all fields after but including $4
		sum += $i	

	if ($3 == "X")			#if $3 is X add the record sum to the volume of station X
		sumX += sum
	if ($3 == "Y")			# " " station Y
		sumY += sum		
	if ($3 == "Z")			# " " station Z
		sumZ += sum
	count++				#add one to the records processed for each record frok 2014
	printf ("%6s\t%7.2f\n", $2 ":", sum)		#print the sum for each record along with its month
	}
}

END {
print "========================"			#print some footnotes about station volume
print "Station Volume for 2014 is:"
printf ("%6s\t%7.2f\n", "X:", sumX)
printf ("%6s\t%7.2f\n", "Y:", sumY)
printf ("%6s\t%7.2f\n", "Z:", sumZ)
print "========================"			# print the total volume 
printf ("%6s\t%7.2f\n", "Total:", sumX+sumY+sumZ)
print count " records processed"
}
