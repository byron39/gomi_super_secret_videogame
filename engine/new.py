import os
import sys


if sys.argv[1]:
    src = "touch src/" + sys.argv[1] + ".cpp"
    os.system(src)
    include = "touch include/" + sys.argv[1] + ".hpp"
    os.system(include)

