# Ars Boretskii OC

## UNIX like, multitask, i80386 PM + MMU, well-commented.

Well commented kernel based on micro architecture.
This helps you to write your own OC.
Main goal to give you small well-commended features that you can reuse in your OC.
Currently all tasks work in kernel mode.

### Content

- scheduler (/kernel/sched)
- initial ram disk (/initrd)
- shell task (/initrd/sh.c)
- tty character device driver (/kernel/dev/tty.c)
- abstract file operations (/kernel/vfs/file.c)
- interprocess communication (/kernel/ipc)
- architecture support (/arch)
- small c lib with lists support (/lib)

### Info

Target processor: i80386 (Protected mode).

### Dependencies

```
$ apt-get install gcc
$ apt-get install qemu
```

### Build and run

```
$ mkdir bin
$ sudo make
```

### Shell commands

Process list:
```
Ars Boretskii kernel 1.0
# ps
```

Exit from current process (returns you to kernel console):
```
Ars Boretskii kernel 1.0
# exit
```

Kill process:
```
Ars Boretskii kernel 1.0
# kill 1
```

Exec elf on initrd:
```
Ars Boretskii kernel 1.0
# exec <name>
```

Clear screen:
```
Ars Boretskii kernel 1.0
# clear
```

Example:
```
Ars Boretskii kernel 1.0
# ps
# clear
# exec sh.elf
# ps
# kill 4
# exit
```

### Contributors
- Ilya Starchenko (thanks for bochs)

### Wish list

- User mode & TSS (ring3)
- Extend initrd to support more that 4Kb (reseach needed why qemu cut it)
- Forth interpreter (as next initrd elf)
