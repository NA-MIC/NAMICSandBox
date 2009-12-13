#!/bin/bash

dataDir=./trainingShapes

resultDataDir=alignTrainingShapes

mkdir ${resultDataDir}

exeFile='./src/imageRegByPointSet/c/affine/r/imgRegByPtSet3DTest_uchar'

fixedImg=${dataDir}/02780302_label.nrrd

fixedImgIso=${dataDir}/02780302_label-iso.nrrd

unu resample -s = = x6.4 -i ${fixedImg} -o ${fixedImgIso}


for j in ${dataDir}/*_label.nrrd; do
    movingImgIso=`echo ${j} | sed s/.nrrd/-iso.nrrd/`

    unu resample -s = = x6.4 -i ${j} -o ${movingImgIso}

    regMovingImgIso=`echo ${j} | sed s/-iso/-reg-iso/`

    ${exeFile} ${fixedImgIso} ${movingImgIso} 5000 0.5 ${regMovingImgIso}

    regMovingImg=`echo ${regMovingImgIso} | sed s/-iso//`

    unu resample -s = = 26 -i ${regMovingImgIso} -o ${regMovingImg}

    mv ${regMovingImg} ${resultDataDir}/

    #rm movingImgIso
done




##################################################
#rm fixedImgIso
