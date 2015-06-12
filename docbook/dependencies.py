#! /usr/bin/python

import os
import sys
import re

deps = {}
parsed = set()
fnlist = sys.argv[1:]

cur = ""

while len(fnlist) > 0:
    n = os.path.abspath(fnlist.pop())

    if n in parsed:
        continue

    if n not in deps:
        deps[n] = set()

    parsed.add(n)

    if n.endswith(".xml"):
        cur = n
        deps[n].add(n.replace(".xml",".txt"))
        fnlist.append(n.replace(".xml",".txt"))
        continue

    f = open(n)
    d = f.read()
    f.close()
    m = set([(os.path.abspath(os.path.join(os.path.dirname(n), p)))
            for p in re.findall(r"include::([^[]*)\[\]", d)])

    deps[cur].update(m)
    fnlist.extend(m.difference(parsed))

for d in deps:
    if d.endswith(".xml"):
        print os.path.relpath(d), ":", " \\\n    ".join([os.path.relpath(p) for p in deps[d]])
