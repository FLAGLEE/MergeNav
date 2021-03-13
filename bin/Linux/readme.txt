# MergeNav V2.0
	## -s
		stream type
		read stream_path\out_path\out_file_prefix from stream.txt
		e.g.    ./MergeNav -s

	## -f
		file type
		read stream_path\out_path\out_file_prefix from file.txt
		e.g.	./MergeNav -f

	## -p
		self-define output file prefix
		e.g.	./MergeNav -p WHUSGG
			out:		WHUSGG.20p
			default:	brdcDOY0.20p

	## -o
		output path
		e.g.	./MergeNav -o ./Result/

	## other
		input file path



# e.g. 
	## read from cmd
		./MergeNav -p LQZcmdflie -o ./Result/ ./RTCM3/HH0012.2020330binRTCM3 ./RTCM3/HH0013.2020330binRTCM3 ./RTCM3/HH0014.2020330binRTCM3 
		./MergeNav -p LQZcmdflie -o ./Result/ ./RTCM3/HH0012.2020330binRTCM3 ./RTCM3/HH0013.2020330binRTCM3 ./RTCM3/HH0014.2020330binRTCM3 ./RTCM3/HH0015.2020330binRTCM3 ./RTCM3/HH0016.2020330binRTCM3 ./RTCM3/HH0017.2020330binRTCM3 ./RTCM3/HH0018.2020330binRTCM3 ./RTCM3/HH0019.2020330binRTCM3 ./RTCM3/HH0020.2020330binRTCM3 ./RTCM3/HH0021.2020330binRTCM3

	## read from stream file
		./MergeNav -s
		./MergeNav -s -p LQZstreamfile
		./MergeNav -s -p LQZstreamfile -o -o ./Result/

	## read from file file
		./MergeNav -f
		./MergeNav -s -p LQZfilefile
		./MergeNav -s -p LQZfilefile -o -o ./Result/


# ver log
	## 1.0
	## 1.1
		- add file auto stop
		- add linux version
	## 1.2
		- fix one file bug
	## 2.0
		- fix thread bug, now can calculate 1 to 10 files, More files have not been tested.
