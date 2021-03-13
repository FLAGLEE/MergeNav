MergeNav V1.2
fix one file and mul file

## -s
	stream type
	read configure from stream.txt
	e.g.    MergeNav.exe -s

## -f
	file type
	read configure from file.txt
	e.g.	MergeNav.exe -f

## -p
	self-define output file prefix
	e.g.	MergeNav.exe -p WHUSGG
		out:	WHUSGG.20p
		default:	brdcDOY0.20p

## -o
	output path
	e.g.	MergeNav.exe -o D:/data/fusion_nav_data/IGSN/

## other
	file path



e.g. 
MergeNav.exe -p LQZ3310 -o ./Result/ ./RTCM3/HH0012.2020330binRTCM3 ./RTCM3/HH0013.2020330binRTCM3 