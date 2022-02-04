## how to parsing command output
In bash scripting, you may get the output from one command,  parse this output, and filter out the string you want.
You can **grep** the output like below.
```bash
command | grep -E "foo|bar"
```
If parsing is complicated, you may use a named pipe.

In the following case, command **top** generates output periodically, I redirect its output to a pipe.
In another side, use **tail** to receive data from the pipe, and parsing the data in the while loop.

```bash
#!/usr/bin/env bash
trap 'rm -f pipe' EXIT
mkfifo pipe
{ top -b > pipe ;} &  P_PID=$! # don't need to get the background pid
while IFS= read -r line; do
    # do the string parsing here
    echo $line
done < <(tail -n +1 -f pipe)
wait 
```

How to handle this in python?

You can import subprocess module in python.
```bash
import subprocess
import fcntl
import os
import selectors
import sys 

process = subprocess.Popen(["top", "-b"], stdout=subprocess.PIPE)

handle = process.stdout.fileno()
flags = fcntl.fcntl(handle, fcntl.F_GETFL)
# see "man 2 select"
fcntl.fcntl(handle, fcntl.F_SETFL, flags | os.O_NONBLOCK)

sel = selectors.DefaultSelector()
sel.register(process.stdout, selectors.EVENT_READ)

while process.poll() is None:
    try:
        sel.select()
    except KeyboardInterrupt:
        sys.exit(0)

    while True:
        out = process.stdout.readline()
        if len(out):
            output = out.decode('utf-8').replace('\n','')
            # do the string parsing here
            print(output)
            continue
        break
sys.exit(0)
```

The above is just like the shell implemetaoin, we can make it more python style.
```python
import subprocess
import fcntl
import os
import selectors
import sys

def pipe_read(process):
    """
    Read all available output from a subprocess
    :param process: subprocess.Popen instance
    """
    while True:
        # Copy available output
        out = process.stdout.readline()
        if len(out):
            output = out.decode('utf-8').replace('\n','')
            print(output)
            continue
        return

def run_cmd(cmd, working_dir=None, env={}):
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE,stderr=subprocess.STDOUT, cwd=working_dir, env=env)

    # Make process.stdout non-blocking
    # see man 2 select
    handle = process.stdout.fileno()
    flags = fcntl.fcntl(handle, fcntl.F_GETFL)
    fcntl.fcntl(handle, fcntl.F_SETFL, flags | os.O_NONBLOCK)

    output_buffer = []
    sel = selectors.DefaultSelector()
    sel.register(process.stdout, selectors.EVENT_READ)

    while process.poll() is None:
        try:
            sel.select()
        except KeyboardInterrupt:
            sys.exit(0)
        pipe_read(process)

run_cmd(cmd=["top", "-b"])
sys.exit(0)
```
