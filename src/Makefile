CC = g++ -Wno-deprecated -O2

# ferret -- main, general version of Ferret
ferret: mainferret.o ferretapp.o selectfiles.o resultstable.o documentview.o \
		outputreport.o xmlreport.o helpframe.o \
		tokenset.o tokenreader.o tupleset.o document.o documentlist.o
	$(CC) -o ferret \
		tokenset.o tokenreader.o tupleset.o document.o documentlist.o \
		mainferret.o ferretapp.o selectfiles.o resultstable.o documentview.o \
		outputreport.o xmlreport.o helpframe.o pdfreport.o \
		-l wxcode_gtk2u_pdfdoc-2.9 `wx-config --libs`
# for Mac
#		-lwxpdfdocud `wx-config --libs`

mainferret.o: mainferret.cpp mainferret.h \
		documentlist.h ferretapp.h
	$(CC) `wx-config --cxxflags` -c mainferret.cpp -o mainferret.o

# graphical and pdf functionality
ferretapp.o: ferretapp.cpp ferretapp.h \
		helpframe.h
	$(CC) `wx-config --cxxflags` -c ferretapp.cpp -o ferretapp.o
	
selectfiles.o: selectfiles.cpp selectfiles.h resultstable.h \
		tokenset.h tokenreader.h tupleset.h document.h documentlist.h
	$(CC) `wx-config --cxxflags` -c selectfiles.cpp -o selectfiles.o 

resultstable.o: resultstable.cpp resultstable.h \
		tokenset.h tokenreader.h tokenset.h document.h documentlist.h
	$(CC) `wx-config --cxxflags` -c resultstable.cpp -o resultstable.o

documentview.o: documentview.cpp documentview.h \
		tokenset.h document.h ferretapp.h resultstable.h
	$(CC) `wx-config --cxxflags` -c documentview.cpp -o documentview.o

helpframe.o: helpframe.cpp helpframe.h \
		ferretapp.h
	$(CC) `wx-config --cxxflags` -c helpframe.cpp -o helpframe.o

outputreport.o: outputreport.cpp outputreport.h
	$(CC) `wx-config --cxxflags` -c outputreport.cpp -o outputreport.o

pdfreport.o: pdfreport.cpp pdfreport.h resultstable.h outputreport.h \
		tokenset.h tokenreader.h document.h documentlist.h
	$(CC) `wx-config --cxxflags` -c pdfreport.cpp -o pdfreport.o

xmlreport.o: xmlreport.cpp xmlreport.h outputreport.h \
		tokenset.h tokenreader.h document.h documentlist.h
	$(CC) `wx-config --cxxflags` -c xmlreport.cpp -o xmlreport.o
	
# coreferret -- the main functions for Ferret dealing with documents
documentlist.o: documentlist.cpp documentlist.h \
		tupleset.h tokenset.h document.h
	$(CC) `wx-config --cxxflags` -c documentlist.cpp -o documentlist.o

document.o: document.cpp document.h \
		tokenset.h tokenreader.h
	$(CC) `wx-config --cxxflags` -c document.cpp -o document.o
	
tokenreader.o: tokenreader.cpp tokenreader.h \
		tokenset.h
	$(CC) `wx-config --cxxflags` -c tokenreader.cpp -o tokenreader.o

tupleset.o: tupleset.cpp tupleset.h \
		tokenset.h
	$(CC) `wx-config --cxxflags` -c tupleset.cpp -o tupleset.o
	
tokenset.o: tokenset.cpp tokenset.h
	$(CC) `wx-config --cxxflags` -c tokenset.cpp -o tokenset.o
	
# for Mac bundle
Ferret.app: ferret
	-mkdir Ferret.app
	-mkdir Ferret.app/Contents
	-mkdir Ferret.app/Contents/MacOS
	-mkdir Ferret.app/Contents/Resources
	-mkdir Ferret.app/Contents/Resources/English.lproj
	echo -n 'Ferret5.0' > Ferret.app/Contents/PkgInfo
	cp ferret Ferret.app/Contents/MacOS/Ferret

# testferret -- some simple checks that the basic operations are correct
testferret: testferret.o coreferret.o
	$(CC) -o testferret testferret.o coreferret.o `wx-config --libs`

testferret.o: testferret.cpp coreferret.h
	$(CC) `wx-config --cxxflags` -c testferret.cpp -o testferret.o

clean:
	rm *.o

