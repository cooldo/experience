When you create a pipe, and use tail -f to receive the pipe, it won't be output in real time.

```bash
mkfifo pipe
top -b > pipe &
tail -f pipe # tail will be blocked and there's no output
```

This is because the **tail** will seek for the end of the file, but a named pipe cannot seek. So **tail** doesn't know the end of file.

If you use a regular file as a pipe, this regular file can seek, then the **tail** command will output at once.

```bash
regular_file=$(mktemp)
top -b > $regular_file &
tail -f $regular_file # tail will output at once
```

You should use pipe as much as possible, here is the reason.

1. Unlike a regular file, named pipe is a special file
2. The data in a pipe is transient, unlike the content of a regular file
3. Pipes forbid random accessing, while regular files do allow this.
4. All of the above

The advantage over using files as a means of communication is, that processes are synchronized by pipes: a process writing to a pipe blocks if there is no reader, and a process reading from a pipe blocks if there is no writer. 

Then how to use pipe in this situation?

We can make tail to read from the first line, just like below.

```bash
mkfifo pipe
top -b > pipe &
tail -n +1 -f pipe # read from first line
```

More complicated case, **top** write to pipe in the background, another process reads from the pipe, and do some operation according to the received string. When press ctrl c, remove the pipe.

```bash
#!/usr/bin/env bash
trap 'rm -f pipe' EXIT
mkfifo pipe
{ top -b > pipe ;} &  P_PID=$! # don't need to get the background pid
while IFS= read -r line; do
    # do the string filter here
    echo $line
done < <(tail -n +1 -f pipe)
wait 
```

I have not reaped the background process when script stops, because when the read side of pipe stops, it broke the pipe, so the write side of pipe (which is in the background) will stop either.

The following subshell solutions also work.
```bash
{ top -b ;} > pipe &  # use command group, put pipe outside the group
(top -b > pipe) &     # use a subshell
(top -b ) > pipe &    # use s shubshell, put pipe outside the subshell
```
