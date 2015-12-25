#!/bin/bash

DESTDIR=/mnt/

dd if=/dev/full of=disk0 bs=1M count=4
mkfs.ext2 disk0

mount disk0 ${DESTDIR} -o loop

mkdir -p "${DESTDIR}/usr/bin"
mkdir -p "${DESTDIR}/usr/lib"
mkdir -p "${DESTDIR}/usr/sbin"
mkdir -p "${DESTDIR}/etc"
mkdir -p "${DESTDIR}/bin"
mkdir -p "${DESTDIR}/dev"
mkdir -p "${DESTDIR}/lib"
mkdir -p "${DESTDIR}/proc"
mkdir -p "${DESTDIR}/sbin"
mkdir -p "${DESTDIR}/tmp"

echo -n "fluxhost" > "${DESTDIR}/etc/hostname"
echo -n "fluxdomain" > "${DESTDIR}/etc/domainname"
#
# Install init services
#
mkdir -p "${DESTDIR}/usr/system/services/"
cp src/init/units/shell.service "${DESTDIR}/usr/system/services/"
mkdir -p "${DESTDIR}/usr/system/mounts/"
cp src/init/units/proc.mount "${DESTDIR}/usr/system/mounts/"
cp src/init/units/tmp.mount "${DESTDIR}/usr/system/mounts/"
mkdir -p "${DESTDIR}/var/lib/network/routes/"
cp src/init/units/local.route "${DESTDIR}/var/lib/network/routes/"
mkdir -p "${DESTDIR}/var/lib/network/interfaces/"
cp src/init/units/eth0.interface "${DESTDIR}/var/lib/network/interfaces/"

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
ps
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
