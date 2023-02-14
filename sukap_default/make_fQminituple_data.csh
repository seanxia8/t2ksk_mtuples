#!/bin/tcsh -f

#set DATA_PATH=/disk01/sklb2/t2kdata/official_release/2020mar
#set OUT_PATH=/disk01/sklb2/t2kdata/official_release/minituples
set DATA_PATH=/disk01/sklb2/t2kdata/official_release/2022dec
set OUT_PATH=/disk01/sklb2/t2kdata/n50dt_check/minituples_20230201_MRt0
mkdir -p "${OUT_PATH}"

#time ./build_fQminituple ${DATA_PATH}/final'*'.root 0 "" "" "" ${OUT_PATH}/t2ksk.14c.fqv4r0.full_run1-9.data.minituple.backup.root >& t2kdata_run1-9.log
#time ./build_fQminituple ${DATA_PATH}/final'*'.root 0 "" "" "" ${OUT_PATH}/t2ksk.19b.fqv4r0b.full_run1-10.data.minituple.root >& t2kdata_run1-10.log
#time ./build_fQminituple ${DATA_PATH}/final'*'.root 0 "" "" "" 0 ${OUT_PATH}/t2ksk.19b.fqv4r0b.full_run1-10.data.minituple.test.20230112.root >& t2kdata_run1-10_test20230112.log
#time ./build_fQminituple ${DATA_PATH}/final'*'.root 0 "" "" "" 1 ${OUT_PATH}/t2ksk.19b.fqv4r0b.full_run1-10.data.minituple.test.newcut.20230112.root >& t2kdata_run1-10_testnewcut20230112.log
time ./build_fQminituple ${DATA_PATH}/final'*'.root 0 "" "" "" 0 ${OUT_PATH}/t2ksk.21a.fqv4r0c.full_run1-11.data.minituple.20230201.root >& t2kdata_run1-11_20230201_MRt0.log
time ./build_fQminituple ${DATA_PATH}/final'*'.root 0 "" "" "" 1 ${OUT_PATH}/t2ksk.21a.fqv4r0c.full_run1-11.data.minituple.newcut.20230201.root >& t2kdata_run1-11_newcut20230201_MRt0.log
#time ./build_fQminituple ${DATA_PATH}/final.run86.fc.apfit.ontiming.21againcorr.fQv4r0c.root 0 "" "" "" 0 ${OUT_PATH}/t2ksk.21a.fqv4r0c.full_run11.data.minituple.test.20230111.root >& t2kdata_run11_test20230111.log
#time ./build_fQminituple ${DATA_PATH}/final.run86.fc.apfit.ontiming.21againcorr.fQv4r0c.root 0 "" "" "" 1 ${OUT_PATH}/t2ksk.21a.fqv4r0c.full_run11.data.minituple.test.newcut.20230111.root >& t2kdata_run11_testnewcut20230111.log
