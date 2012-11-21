#!/usr/bin/python
import os
import sys
import time
import re

# Some ideas
# tar -czv -f hw_ppm.tar.gz *hw.ppm
# mutt -a ppm.tar.gz ronaldv@stanford.edu < results.log (tee to file or something)
#check size?
# tar log files...

def run(dir, mod_FSM):
  '''
    run the tests in the directory
  '''
  files = [os.path.join(dir, f) for f in os.listdir(dir) if f.endswith('.dat')]
  #os.system('make clean comp GEN_PARAM="top_rast.rast.test_iterator.ModifiedFSM=%s"' % (mod_FSM))
  for file in files:
    base = os.path.splitext(file)[0]
    dat = base + '.dat'
    # save the output in the current directory
    ppm = os.path.split(base)[1] + '_hw.ppm'
    log = os.path.split(base)[1] + '_sim.log'
    os.system('make genesis_clean run RUN="+testname=%s" GEN_PARAM="top_rast.rast.PipesSamp=6 top_rast.rast.PipesHash=4 top_rast.rast.test_iterator.ModifiedFSM=%s"' % (dat,mod_FSM))
    os.system('mv sv_out.ppm %s' % (ppm))
    os.system('mv run_bb.log %s' % (log))
    os.system('tar -czv -f %s.tar.gz %s' % (log,log))
    os.system('rm -f %s' % (log))

  # do a separate loop so that the results appear at the bottom
  os.system('echo "RESULTS " > results.log')
  for file in files:
    base = os.path.splitext(file)[0]
    ref = base + '_ref.ppm'
    ppm = os.path.split(base)[1] + '_hw.ppm'
    print '*** checking results for %s' % base,
    status = os.system('echo "*** checking results for %s" >> results.log' % base)
    status = os.system('diff %s %s >& /dev/null' % (ppm, ref))
    if status == 0:
		print '*** PASSED'
    		os.system('echo "*** PASSED"  >> results.log')
    else:
		print '*** FAILED'
    		os.system('echo "*** FAILED" >> results.log')


if __name__ == '__main__':
  os.system('rm -f *_hw.ppm')
  run(sys.argv[1],'NO')
  os.system('tar -czv -f hw_ppm_FSM_NO.tar.gz *hw.ppm') 
  os.system('mutt -s "test_results mod_FSM=NO" -a hw_ppm.tar.gz -- ronaldv@stanford.edu < results.log') 
 # os.system('mutt -s "test_results mod_FSM=NO" -a hw_ppm.tar.gz -- makoenig@stanford.edu < results.log') 
  os.system('rm -f *_hw.ppm')
  run(sys.argv[1],'YES')
  os.system('tar -czv -f hw_ppm_FSM_YES.tar.gz *hw.ppm') 
  os.system('mutt -s "test_results mod_FSM=YES" -a hw_ppm.tar.gz -- ronaldv@stanford.edu < results.log') 
 # os.system('mutt -s "test_results mod_FSM=YES" -a hw_ppm.tar.gz -- makoenig@stanford.edu < results.log') 
