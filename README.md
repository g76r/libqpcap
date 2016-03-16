libqpcap
========

ABOUT
-----

libqpcap is a Qt-ish wrapper over libpcap (see http://www.tcpdump.org/).

It has been initialy done as a part of HTTP Passive Probe (see
http://httppp.g76r.eu).

LICENSE
-------

This software is made availlable to you under the terms of the GNU Affero
General Public License version 3, see LICENSE.txt file content.

BUILD INSTRUCTIONS
------------------

This is a regular qmake project, one should run qmake then make to compile.
Works on Linux and Windows. Should work on MacOS X either (not yet tested):

``` bash
qmake
make
```

Dependencies:
- on POSIX systems one should first install libpcap dev kit, e.g.
  "apt-get install libpcap-devel" or "yum install libpcap-devel" under Linux
- on Windows systems one should install WinPCAP devkit and place it under
  c:\WpdPack directory
- Qt 4.8

