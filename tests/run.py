#!/usr/bin/python
import os
import sys
import time
import re

def run(dir):
  '''
    run the tests in the directory
  '''
  files = [os.path.join(dir, f) for f in os.listdir(dir) if f.endswith('.dat')]
  for file in files:
    base = os.path.splitext(file)[0]
    dat = base + '.dat'
    # save the output in the current directory
    ppm = os.path.split(base)[1] + '_golden.ppm'
    os.system('./rast_gold %s %s' % (ppm, dat))

  # do a separate loop so that the results appear at the bottom
  for file in files:
    base = os.path.splitext(file)[0]
    ref = base + '_ref.ppm'
    ppm = os.path.split(base)[1] + '_golden.ppm'
    print '*** checking results for %s' % base
    os.system('diff %s %s' % (ppm, ref))


if __name__ == '__main__':
  os.system('make clean comp_gold')
  run(sys.argv[1])
