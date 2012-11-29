#!/bin/bash
# Script to find minimum precision on multipliers
MULTPREC="24"
while [ ${MULTPREC} -ge 0 ] ; do
	# SCRIPT TAG VECTOR_PATH GENESIS_PARAMETERS LONG_OR_SHORT
	tests/verify.sh MULTPREC_${MULTPREC} ../../vect "top_rast.rast.sampletest.MultPrec=${MULTPREC} top_rast.rast.sampletest.MultMod=1"
	MULTPREC=$[$MULTPREC - 1]
done
