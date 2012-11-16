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
    log = os.path.split(base)[1] + '_sim.log'
    os.system('make run RUN="+testname=%s"' % (dat))
    os.system('mv sv_out.ppm %s' % (ppm))
    os.system('mv run_bb.log %s' % (log))

  # do a separate loop so that the results appear at the bottom
  for file in files:
    base = os.path.splitext(file)[0]
    ref = base + '_ref.ppm'
    ppm = os.path.split(base)[1] + '_golden.ppm'
    print '*** checking results for %s' % base,
    status = os.system('diff %s %s > /dev/null' % (ppm, ref))
    if status == 0:
			print '*** PASSED'
    else:
			print '*** FAILED'


if __name__ == '__main__':
  os.system('make clean comp')
  run(sys.argv[1])
