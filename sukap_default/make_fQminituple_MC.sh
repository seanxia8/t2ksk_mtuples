#!/bin/bash

datetoday="20230201_MRt0"
MC_PATH="/disk02/sklb3/t2k-sk.mc.release/t2kmc_19b/root"
OUT_PATH="/disk02/sklb3/t2kmc/${datetoday}"
mkdir -p "${OUT_PATH}"

channel=("numu_x_numu" "numubar_x_numubar" "nue_x_nue" "nuebar_x_nuebar" "numu_x_nue" "numubar_x_nuebar")
Horns=("rhc" "fhc")
numode=("antinu-mode" "nu-mode")
flxfile=("/disk01/usr3/cvilela/T2K-SK/Flux_OA2019/tuned13av7p1/run1-10b_ndto9/sk_tuned13av7p1_13anom_run1-10b_numode_fine.root" "/disk01/usr3/cvilela/T2K-SK/Flux_OA2019/tuned13av7p1/run5c-9d_ndto9/sk_tuned13av7p1_13anom_run5c-9d_antinumode_fine.root")

#use_n50_cut=$1
NameForN50=("MRwoN50" "MRwN50")

logdir="./logMC"
mkdir -p "$logdir"

for use_n50_cut in "${!NameForN50[@]}"; do
    for horn in "${!Horns[@]}"; do
	for n in "${!channel[@]}"; do
	    nchannel=$(( n+1 )) # channel 0 is for data	
            infiles="${MC_PATH}/${Horns[$horn]}/${channel[$n]}/t2ksk19b.fqv4r0.${Horns[$horn]}.${channel[$n]}.0[0-9][0-9][0-9].00[0-9].root"
	    outdir="${OUT_PATH}/${NameForN50[$use_n50_cut]}/${Horns[$horn]}"
	    mkdir -p "${outdir}"
	    time ./build_fQminituple "${MC_PATH}/${Horns[$horn]}/${channel[$n]}/t2ksk19b.fqv4r0.${Horns[$horn]}.${channel[$n]}."'*'".root" "${nchannel}" "${flxfile[$horn]}" "" "" "$use_n50_cut" "${outdir}/t2ksk19b.fqv4r0b.${Horns[$horn]}.${channel[$n]}.root" >& "${logdir}/t2ksk19b.fqv4r0b.${Horns[$horn]}.${channel[$n]}.log"
	done
    done
done
