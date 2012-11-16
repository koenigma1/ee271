#!/usr/bin/python
import os
import sys
import time
import re

# Some ideas
# tar -czv -f hw_ppm.tar.gz *hw.ppm
# mutt -a ppm.tar.gz ronaldv@stanford.edu < results.log (tee to file or something)
# check size?
# tar log files...

def run(dir):
  '''
    run the tests in the directory
  '''
  files = [os.path.join(dir, f) for f in os.listdir(dir) if f.endswith('.dat')]
  for file in files:
    base = os.path.splitext(file)[0]
    dat = base + '.dat'
    # save the output in the current directory
    ppm = os.path.split(base)[1] + '_hw.ppm'
    log = os.path.split(base)[1] + '_sim.log'
    os.system('make run RUN="+testname=%s"' % (dat))
    os.system('mv sv_out.ppm %s' % (ppm))
    os.system('mv run_bb.log %s' % (log))
    os.system('tar -czv -f %s.tar.gz %s' % (log,log))
    os.system('rm -f %s' % (log,log))

  # do a separate loop so that the results appear at the bottom
  os.system('echo RESULTS > results.log')
  for file in files:
    base = os.path.splitext(file)[0]
    ref = base + '_ref.ppm'
    ppm = os.path.split(base)[1] + '_hw.ppm'
    print '*** checking results for %s' % base,
    status = os.system('echo *** checking results for %s >> results.log' % base)
    status = os.system('diff %s %s > /dev/null' % (ppm, ref))
    if status == 0:
			print '*** PASSED'
    		os.system('*** PASSED  >> results.log')
    else:
			print '*** FAILED'
    		os.system('*** FAILED >> results.log')


if __name__ == '__main__':
  os.system('make clean comp')
  run(sys.argv[1])
  os.system('tar -czv -f hw_ppm.tar.gz *hw.ppm') 
  os.system('mutt -s "test_results" -a ppm.tar.gz ronaldv@stanford.edu < results.log') 
  os.system('mutt -s "test_results" -a ppm.tar.gz makoenig@stanford.edu < results.log') 
