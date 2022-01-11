# WARNING :
# before use, make sure your .config file is empty.
#
# Indeed .config values have higher priority, so if not empty its values will
# be used instead, which might end up in non-working files, or worse unintentional leaks ...

checkMkdir()
{
    local dir="$1"

    if [[ ! -d "$dir" ]];then
        mkdir $dir
    fi
}

deployGame()
{
    # deployGame $SAVEDATA_PATH $DEST_PATH
    local save="$1"
    local dest="$2"

    checkMkdir $dest

    if [[ ! -d "$save" ]];then
        echo "$save missing!"
    else
        eval "cp \"$save/\"* $dest -R"
        mv dist/VBOOTPBP.BIN dist/RECOVERY.BIN
        cp $MENU_PATH/* $dest -R
        mv $dest/VBOOT.PBP $dest/VBOOTPBP.BIN
        cp dist/* $dest
    fi
}

compressGame()
{
    # compressGame $NAME
    local name="$1"

    gameFolderPath="${RELEASE_DIR}/$name"

    7z a "${gameFolderPath}.7z" ./$gameFolderPath
}



SAVEDATAS=./contrib/PSP/SAVEDATA
MENU_PATH=./yMenu


RELEASE_DIR=release

FIRMWARES=(161 167 169 180 200)
FIRMWARES_NICE=(161 167 169 18X 200_201_202)



Games=(MotorStorm_Arctic_Edge Everybody_Tennis Super_Collapse_3 Urbanix Mad_Blocker_Alpha Gravity_Crash_Portable Monster_Hunter Uno)
gamesNbLen=0


IT=0
# Motorstorm
MOTORSTORM[$((IT++))]=UCUS987430000
MOTORSTORM[$((IT++))]=UCJS101040000
MOTORSTORM[$((IT++))]=UCES012500000
MOTORSTORM[$((IT++))]=UCKS451240000
GamesNb[$((gamesNbLen++))]=$IT

IT=0
# Everybody's tennis
TENNIS[$((IT++))]=UCJS10101
TENNIS[$((IT++))]=UCES01420
TENNIS[$((IT++))]=UCUS98701
TENNIS[$((IT++))]=UCAS40307
GamesNb[$((gamesNbLen++))]=$IT


IT=0
#Super Collapse 3
SUPER_COLLAPSE[$((IT++))]=ULUS10287DATA04
SUPER_COLLAPSE[$((IT++))]=ULES01027DATA00
GamesNb[$((gamesNbLen++))]=$IT


IT=0
#Urbanix
URBANIX[$((IT++))]=NPUZ00077
URBANIX[$((IT++))]=NPEZ00176
URBANIX[$((IT++))]=NPJH00082
GamesNb[$((gamesNbLen++))]=$IT

IT=0
#Mad Blocker Alpha
MAD_BLOCKER[$((IT++))]=NPUZ00210CPMEM
MAD_BLOCKER[$((IT++))]=NPEZ00327CPMEM
GamesNb[$((gamesNbLen++))]=$IT

IT=0
#Gravity Crash
GRAVITY_CRASH[$((IT++))]=NPUG80321DATA00
GRAVITY_CRASH[$((IT++))]=NPJG00044DATA00
GRAVITY_CRASH[$((IT++))]=NPHG00038DATA00
GRAVITY_CRASH[$((IT++))]=NPEG00020DATA00
GamesNb[$((gamesNbLen++))]=$IT

IT=0
#Monster Hunter Freedom Unite (US/EU)
#Monster hunter Portable 2ndG (JP)
#Monster hunter Freedom 2 (EU)
#Monster Hunter Portalble 3rd (JP)
MONSTER_HUNTER[$((IT++))]=ULES01213
MONSTER_HUNTER[$((IT++))]=ULJM05500
MONSTER_HUNTER[$((IT++))]=ULUS10391
MONSTER_HUNTER[$((IT++))]=ULJM05800QST
MONSTER_HUNTER[$((IT++))]=ULES00851
GamesNb[$((gamesNbLen++))]=$IT

IT=0
#Uno
UNO[$((IT++))]=NPEH00020DATA00
UNO[$((IT++))]=NPUH10027DATA00
UNO[$((IT++))]=NPJH00016DATA00
GamesNb[$((gamesNbLen++))]=$IT



IT=0
GAMES_161[$((IT++))]=${MOTORSTORM[@]}
GAMES_161[$((IT++))]=${TENNIS[@]}

IT=0
GAMES_167[$((IT++))]=${SUPER_COLLAPSE[@]}

IT=0
GAMES_180[$((IT++))]=${URBANIX[@]}
GAMES_180[$((IT++))]=${MAD_BLOCKER[@]}
GAMES_180[$((IT++))]=${GRAVITY_CRASH[@]}
GAMES_180[$((IT++))]=${MONSTER_HUNTER[@]}


IT=0
GAMES_200[$((IT++))]=${UNO[@]}


GAMES[161]=${GAMES_161[@]}
GAMES[167]=${GAMES_167[@]}
GAMES[180]=${GAMES_180[@]}
GAMES[200]=${GAMES_200[@]}


gamesIt=0
gameIdx=0
((maxNb=${GamesNb[$gameIdx]}))
for fw_v in ${!GAMES[*]}
do
    for SAVEDATA in ${GAMES[$fw_v]}
    do
        if (($gamesIt >= $maxNb)); then

            compressGame "${Games[$gameIdx]}"

            ((gameIdx=gameIdx+1))
            ((maxNb=maxNb+${GamesNb[$gameIdx]}))
        fi


        for FW_IDX in ${!FIRMWARES[*]}
        do
            FW_I=${FIRMWARES[$FW_IDX]}

            if (( FW_I <= fw_v )); then
                #echo "FW_I: $FW_I, fw_v: $fw_v, SAVEDATA: $SAVEDATA"


                checkMkdir $RELEASE_DIR
                GAMEID=${SAVEDATA:0:9}

                PARAMS="GAME=$GAMEID FIRMWARE=$FW_I RELEASE=1"
                gameFolder="${Games[$gameIdx]}"
                gameFolderPath="${RELEASE_DIR}/$gameFolder"

                checkMkdir $gameFolderPath

                FW_NICE_I="${FIRMWARES_NICE[$FW_IDX]}"

                gameFolderFwPath="${gameFolderPath}/${FW_NICE_I}"


                checkMkdir $gameFolderFwPath


                

                make $PARAMS clean
                make $PARAMS


                if [ $GAMEID == UCES01420 ]; then
                    subFolds=(tenniseu-no_spanish tenniseu-no_italian)


                    for idx in ${!subFolds[*]}
                    do
                        subFold=${subFolds[$idx]}

                        saveDataPath[$idx]="${SAVEDATAS}/${subFold}/${SAVEDATA}"
                        gamePath[$idx]="${gameFolderFwPath}/${subFold}/${SAVEDATA}"

                        checkMkdir "${gameFolderFwPath}/${subFold}"
                        checkMkdir ${gamePath[$idx]}

                        deployGame ${saveDataPath[$idx]} ${gamePath[$idx]}
                    done


                elif [ $GAMEID == NPHG00038 ]; then

                    mv dist/H.BIN dist/H2.BIN


                    NPEG00020DATA01

                    saveDataPath0="${SAVEDATAS}/Gravity Crash HK/NPEG00020DATA01"
                    saveDataPath1="${SAVEDATAS}/Gravity Crash HK/${SAVEDATA}"

                    subFold="${gameFolderFwPath}/Gravity_Crash_HK"
                    gamePath0="${subFold}/NPEG00020DATA01"
                    gamePath1="${subFold}/${SAVEDATA}"

                    checkMkdir $subFold
                    checkMkdir $gamePath0
                    checkMkdir $gamePath1

                    eval "cp \"$saveDataPath1/\"* $gamePath1 -R"
                    deployGame "$saveDataPath0" $gamePath0

                else
                    gameIdPath="${gameFolderFwPath}/${SAVEDATA}"

                    saveDataPath="${SAVEDATAS}/${SAVEDATA}"


                    deployGame $saveDataPath $gameIdPath
                fi


                make $PARAMS clean



                #echo "gameFolderPath: $gameFolderPath, gameIdPath: $gameIdPath, gameFolderFwPath: $gameFolderFwPath"
            fi
        done

        ((gamesIt=gamesIt+1))
    done
done

compressGame "${Games[$gameIdx]}"


7z a ./release.7z ./release/*.7z




