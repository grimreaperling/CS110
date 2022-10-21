#!/usr/bin/env python
import sys
 
def read_mapper_output(file):
    for line in file:
        yield line.strip().split(' ')
 
def main():
    data = read_mapper_output(sys.stdin)
    for vec in data:
        word = vec[0]
        count = sum(int(number) for number in vec[1:])
        print "%s %d" % (word, count)
 
if __name__ == "__main__":
    main()
