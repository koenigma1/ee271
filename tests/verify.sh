#!/bin/bash
if [ $# -lt "2" ]; then
	echo "usage:"
	echo "verify <TEST_TAG> <vector_directory> <GENESIS_PARAMS> --include_long"
	echo "exit"
fi

# FILL VARIABLE 
TEST_TAG=$1
VECT_DIR=$2
GENESIS_PARAM=$3
VECTORS=(`ls ${VECT_DIR}/*.dat -1 | tr '\n' ' '`)
SHORT_VECTORS=(`ls ${VECT_DIR}/*00_sv.dat -1 | tr '\n' ' '` `ls ${VECT_DIR}/*short.dat -1 | tr '\n' ' '`) 
declare -A FILE_NAME
declare -A BASE 
declare -A PPM 
declare -A PPM_REF 
declare -A LOG 
for VECTOR in ${VECTORS[@]}; do
	FILE_NAME[$VECTOR]=`echo ${VECTOR##*/}`
	BASE[$VECTOR]=`echo ${FILE_NAME[$VECTOR]%.*}`
	PPM[$VECTOR]="${TEST_TAG}_${BASE[$VECTOR]}_hw.ppm"
	PPM_REF[$VECTOR]=${VECT_DIR}/${BASE[$VECTOR]}_ref.ppm
	LOG[$VECTOR]="${TEST_TAG}_${BASE[$VECTOR]}_sim.log"  
done

# Build 
echo "Building with parameters: "
echo $GENESIS_PARAM
make genesis_clean comp GEN_PARAM="${GENESIS_PARAM}" >& ${TEST_TAG}_verify_comp.log 
if [ "$?" -eq "0" ]; then
	echo "*** SUCCESS"
else
	echo "ERROR"
	echo "See ${TEST_TAG}_verify_comp.log for details"
	exit -1
fi

# Perform tests
echo "TEST_TAG: ${TEST_TAG}"
echo "TEST_TAG: ${TEST_TAG}" > ${TEST_TAG}.log
if [ "$4" == "--include_long" ]; then
	TEST_VECTORS=${VECTORS[@]}
else
	TEST_VECTORS=${SHORT_VECTORS[@]}
fi
echo "Starting Simulation" 
echo "Starting Simulation" >> ${TEST_TAG}.log
for VECTOR in ${TEST_VECTORS[@]}; do
	echo -n "Simulating: ${FILE_NAME[$VECTOR]} "
	echo -n "Simulating: ${FILE_NAME[$VECTOR]}" >> ${TEST_TAG}.log
	make run RUN="+testname=$VECTOR" >& ${LOG[$VECTOR]}
	SIM_RES=$?
	mv sv_out.ppm ${PPM[$VECTOR]}
	gzip -f ${LOG[$VECTOR]}
	if [ "$SIM_RES" -eq "0" ]; then
		echo " *** DONE" 
		echo " *** DONE" >> ${TEST_TAG}.log 
	else
		echo "ERROR"
		echo "ERROR" >> ${TEST_TAG}.log
		echo "See ${LOG[$VECTOR]}.gz for details"
		exit -1
	fi
done

# Verify
echo "Starting Verification"
echo "Starting Verification" >> ${TEST_TAG}.log
for VECTOR in ${TEST_VECTORS[@]}; do
	echo -n "Verifying: ${FILE_NAME[$VECTOR]} "
	echo -n "Verifying: ${FILE_NAME[$VECTOR]}" >> ${TEST_TAG}.log
	diff ${PPM_REF[$VECTOR]} ${PPM[$VECTOR]} >> ${TEST_TAG}.log
	if [ "$?" -eq "0" ]; then 
		echo "*** PASSED" 
		echo "*** PASSED" >> ${TEST_TAG}.log
	else
		echo " *** FAILED"
		echo " *** FAILED" >> ${TEST_TAG}.log
		echo "Differences between ${PPM_REF[$VECTOR]} ${PPM[$VECTOR]}" >> ${TEST_TAG}.log
	fi
done

# Package
echo "Packaging Results"
if [ -d "results_${TEST_TAG}" ]; then
	rm -rf results_${TEST_TAG}
fi
mkdir results_${TEST_TAG}
mv ${TEST_TAG}* results_${TEST_TAG} > /dev/null
tar -czv -f results_$TEST_TAG.tar.gz results_${TEST_TAG} > /dev/null

echo "ALL DONE"
#rm -rf results_${TEST_TAG}


