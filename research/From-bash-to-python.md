- Create a pipe, wait for 5 seconds, and kill the pipe.

Bash implemetation is below. 
```bash
#!/usr/bin/env bash
file_name="/tmp/foo.log"
touch "$file_name"
{tail -f "$file_name" | grep bar ;} & PPID=$!
sleep 5
pkill -P $PPID # kill the process including its children
wait
```

Python implemetation is below.
```python
import os
import time
import subprocess
import psutil
def kill(ppid):
    """
    kill ppid and all its descendant
    """
    process = psutil.Process(ppid)
    for pid in process.children(recursive=True):
        pid.kill()
    ppid.kill()

file_name = "/tmp/foo.log"
if not os.path.exists(file_name):
    os.mknod(file_name)
cmd = "tail -f {}".format(file_name) + " | grep bar"
process = subprocess.Popen(cmd, shell=True)
time.sleep(5)

kill(process.pid)
# or you can use shell-like pkill
#os.system("pkill -P {}".format(process.pid))
process.wait()
```
