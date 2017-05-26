#!/usr/bin/python2.7

import sys
import time

mComment = "// "
mFilename = sys.argv[1]
mAuthor = "1452334 Bonan Ruan"
mGenerateTime = time.ctime()

mFile = open(sys.argv[1], "w")
mFile.write(mComment + mFilename + "\n")
mFile.write(mComment + mAuthor + "\n")
mFile.write(mComment + mGenerateTime + "\n")
mFile.close()
