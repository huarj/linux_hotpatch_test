# linux_hotpatch_test
#for study linux hotpatch implementation

2019-06-27
1、main.c
   you need compile the main.c file to the executing which named 'mytest'. This is main process and you need run in your linux system.
   
2、jmp.c 
   It is a hotpatch tool which can change the function implementation witch a new function.
   usage like this: ./jmp "31282" "4195712" "4195729",
   -- .jmp： target from jmp.c compile
   -- 31282: mytest's process pid
   -- 4195712: function address of 'myprint' in mytest
   -- 4195729: function address of 'new_myprint' in mytest
