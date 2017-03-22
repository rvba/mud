set(
	SRC
      	lpcap.h
     	lpcode.h
      	lpprint.h
      	lptree.h
     	lpvm.c
      	re.lua
	lpcap.c 
       	lpcode.c 
      	lpprint.c
      	lptree.c 
      	lptypes.h 
       	lpvm.h 
)


add_library(lpeg ${LIB_TYPE} ${SRC})
