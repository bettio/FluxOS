#!/bin/bash

DESTDIR=/mnt/

dd if=/dev/full of=disk0 bs=1M count=4
mkfs.ext2 disk0

mount disk0 ${DESTDIR} -o loop

mkdir -p "${DESTDIR}/usr/bin"
mkdir -p "${DESTDIR}/usr/lib"
mkdir -p "${DESTDIR}/usr/sbin"
mkdir -p "${DESTDIR}/bin"
mkdir -p "${DESTDIR}/dev"
mkdir -p "${DESTDIR}/lib"
mkdir -p "${DESTDIR}/proc"
mkdir -p "${DESTDIR}/sbin"

cp src/init/init "${DESTDIR}/sbin/"

cp src/fluxsh/fluxsh "${DESTDIR}/bin/"

while read coreutil
do
    cp "src/coreutils/${coreutil}" "${DESTDIR}/bin/"
done << EOF
basename
cat
chmod
chown
dirname
domainname
false
hostname
ln
ls
mkdir
mknod
mount
mv
pwd
readlink
reboot
rm
rmdir
touch
true
umount
uname
EOF

while read test
do
    cp "src/tests/${test}" "${DESTDIR}/bin/"
done << EOF
mmap
EOF

umount ${DESTDIR}
