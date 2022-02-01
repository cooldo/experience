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

In bash programming, the pipe is arranged as a [Job](http://mywiki.wooledge.org/BashGuide/JobControl), which is implemented as "process group".

In order to kill this pipe, you need to kill the process group with PGID(process group id)

```bash
$ tail -f test.log | tee | grep foo | grep bar & PID=$!
$ PGID=$(ps -o pgid= $PID | grep -o [0-9]*)
$ kill -- -$PGID # don't forget to prefix with a -
```

* * *
You can also kill a pipe only by **PID** in bash.
```bash
$ set -m # set Job control enabled, you can use set -o to check
$ tail -f test.log | tee | grep foo | grep bar & PID=$!
$ kill -- -$PID
```
Here I only use PID to kill the whole process group. I guess the reason is in job control mode,  bash would help you to find the PGID by PID, thus makes **kill** work.
* * *
