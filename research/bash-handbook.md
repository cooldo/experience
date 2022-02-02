# How to kill a pipe ?

In bash monogramming, sometimes you start a pipe in the background as below and you want to kill this pipe when some conditions met.

```bash
tail -f test.log | tee | grep foo | grep bar &
```

You can only get the PID of the last sub-process, like below.

```bash
tail -f test.log | tee | grep foo | grep bar & PID=$!
```

You cannot just use *kill $PID* to kill this pipe.

```bash
$ tail -f test.log | tee | grep foo | grep bar & PID=$!
[1] 101703
$ ps
    PID TTY          TIME CMD
 101578 pts/7    00:00:00 bash
 101700 pts/7    00:00:00 tail
 101701 pts/7    00:00:00 tee
 101702 pts/7    00:00:00 grep
 101703 pts/7    00:00:00 grep
 101705 pts/7    00:00:00 ps
$ kill $PID
$ ps
    PID TTY          TIME CMD
 101578 pts/7    00:00:00 bash
 101700 pts/7    00:00:00 tail
 101701 pts/7    00:00:00 tee
 101702 pts/7    00:00:00 grep
 101706 pts/7    00:00:00 ps
```

When using **kill $PID**, only the last process (which is one of the grep) in the pipe has been killed. The other processes are still running.

So how to do it?

The first way is that all the sub-processes in the pipe belong to the same PGID(process group id), if you kill the PGID, then you can kill the pipe.

```bash
$ ps -o pid,ppid,pgid,comm
    PID    PPID    PGID COMMAND
 101578    3240  101578 bash
 101700  101578  101700 tail
 101701  101578  101700 tee
 101702  101578  101700 grep
 101713  101578  101713 ps
```

Shell command **kill** can kill the PGID.

```bash
$ tail -f test.log | tee | grep foo | grep bar & PID=$!
$ PGID=$(ps -o pgid= $PID | grep -o [0-9]*) # get the pgid
$ kill -- -$PGID # kill the pgid
```

Nevertheless, you should not expect to get the PGID by log filter, it is not safe for different Unix system with different log format.

The above solution also has its drawback. When you put 2 background pipes in one script, they belong to the a same PGID, when you kill the PGID,  all the 2 pipes are killed.

```bash
#!/usr/bin/env bash
tail -f test.log | tee | grep foo | grep bar & PIPE1=$!
PGID1=$(ps -o pgid= $PIPE1 | grep -o [0-9]*)
tail -f test.log | tee | grep foo | grep bar & PIPE2=$!
PGID2=$(ps -o pgid= $PIPE2 | grep -o [0-9]*)
kill -- -$PGID1 # PGID1 and PGID2 are the same
```

The good way is to put the pipe inside a [command group](http://mywiki.wooledge.org/BashGuide/CompoundCommands#Command_grouping) , and we can get the parent pid of this command group.

Shell command **pkill** provides a way to kill all the child process by its parent pid.

```bash
$ { tail -f test.log | tee | grep foo | grep bar ;} & PID=$!
$ pkill -P $PID
```

* * *

When you enable [Job control](http://mywiki.wooledge.org/BashGuide/JobControl) in bash, you can use PGID to kill pipes, because the PGID of each pipe will be different.

```bash
#!/usr/bin/env bash
set -m # enable Job Control, use set -o to check 
tail -f test.log | tee | grep foo | grep bar & PIPE1=$!
PGID1=$(ps -o pgid= $PIPE1 | grep -o [0-9]*)
tail -f test.log | tee | grep foo | grep bar & PIPE2=$!
PGID2=$(ps -o pgid= $PIPE2 | grep -o [0-9]*)
kill -- -$PGID1 # PGID1 and PGID2 are the different, so only kill pipe1
```

You can also kill a pipe only by it's sub-process's pid by using job control.
Here I only use sub-process's pid to kill the pipe. I guess the reason is in job control
mode, bash would help you to find the PGID by PID, thus makes **kill** work.

```bash
$ set -m # set Job control enabled, you can use set -o to check
$ tail -f test.log | tee | grep foo | grep bar & PID=$!
$ kill -- -$PID
```

* * *