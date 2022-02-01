# How to kill a pipe ?

In bash programing, sometimes you start a pipe in the background as below and you want to kill this pipe when some conditions met.

```bash
tail -f test.log | tee | grep foo | grep bar &
```

You can only get the PID of the last subprocess, like below.

```bash
tail -f test.log | tee | grep foo | grep bar & PID=$!
```

You cannot just use *kill $PID* to kill this pipe.

```bash
$ tail -f test.log | tee | grep foo | grep bar & PID=$!
[1] 83362
$ ps
    PID TTY          TIME CMD
  81172 pts/4    00:00:00 bash
  83359 pts/4    00:00:00 tail
  83360 pts/4    00:00:00 tee
  83361 pts/4    00:00:00 grep
  83362 pts/4    00:00:00 grep
  83363 pts/4    00:00:00 ps
$ kill $PID
$ ps
    PID TTY          TIME CMD
  81172 pts/4    00:00:00 bash
  83359 pts/4    00:00:00 tail
  83360 pts/4    00:00:00 tee
  83361 pts/4    00:00:00 grep
  83366 pts/4    00:00:00 ps
```

When using **kill $PID**, only the last process (which is one of the grep) in the pipe has been killed. The other processes are still running.
So how to do it?
In bash programming, the pipe arranged as a [Job](http://mywiki.wooledge.org/BashGuide/JobControl), which is implemented as "process group".
In order to kill this pipe, you need to kill the process group.
First, you need to enable Job control in your script with **set -m** or **set -o monitor**.
In the terminal, the **set -m** option has already been set.

```bash
$ set -o | grep monitor
monitor        	on
```

Second, you can use **kill %%** to kill this job.

- %% or %+ to refer to the current job: the one most recently started in the background.

But in a script programming, you may have more than one background pipes.
The good way is to get last process's PID in each pipe, and use **kill -- -$PID** to kill it whenever you want.

- -$PID refers to the process group id(PGID) which gets from the $PID, don't forget to prefix with a **-**.

Therefore, you can do it like this.

```bash
$ tail -f test.log | tee | grep foo | grep bar & PID=$!
[1] 85397
$ ps
    PID TTY          TIME CMD
  84996 pts/5    00:00:00 bash
  85394 pts/5    00:00:00 tail
  85395 pts/5    00:00:00 tee
  85396 pts/5    00:00:00 grep
  85397 pts/5    00:00:00 grep
  85398 pts/5    00:00:00 ps
$ kill -- -$PID
$ ps
    PID TTY          TIME CMD
  84996 pts/5    00:00:00 bash
  85402 pts/5    00:00:00 ps
[1]+  Terminated              tail -f test.log | tee | grep --color=auto foo | grep --color=auto bar
$ 
```

You can see all the process inside the pipe has been killed.
* * *
Notice

When use sh (not bash), you can not use **-$PID** to get the group id of this pipe.
```sh
$ sh
$ tail -f test.log | tee | grep foo | grep bar & PID=$!
$ kill -- -$PID
sh: 2: kill: No such process
```
You need to find the real group id of this pipe and kill it.
```sh
$ ps -O pid,ppid,pgid,comm
    PID     PID    PPID    PGID COMMAND         S TTY          TIME COMMAND
  85648   85648    3240   85648 bash            S pts/4    00:00:00 -/bin/bash
  85781   85781   85648   85781 sh              S pts/4    00:00:00 sh
  85783   85783   85781   85783 tail            S pts/4    00:00:00 tail -f test.
  85784   85784   85781   85783 tee             S pts/4    00:00:00 tee
  85785   85785   85781   85783 grep            S pts/4    00:00:00 grep foo
  85786   85786   85781   85783 grep            S pts/4    00:00:00 grep bar
  85802   85802   85781   85802 ps              R pts/4    00:00:00 ps -O pid,ppi
$ kill -- -85783  # pgid is 85783
$ ps
    PID TTY          TIME CMD
  85648 pts/4    00:00:00 bash
  85781 pts/4    00:00:00 sh
  85804 pts/4    00:00:00 ps
[1] + Terminated                 tail -f test.log | tee | grep foo | grep bar

```
* * *
