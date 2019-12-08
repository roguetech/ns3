#!/bin/sh

INTERVALS="0.008 0.005333333 0.0016 0.0008 0.0004"
TRIALS="1"
DISTANCES="50"

SIMTIME=20 #Default in wifi-example-sim.cc: The transmission Time approximately equals to the Simulation Running Time

echo WiFi Experiment Example

pCheck=`which sqlite3`
if [ -z "$pCheck" ]
then
  echo "ERROR: This script requires sqlite3 (wifi-example-sim does not)."
  exit 255
fi

pCheck=`which gnuplot`
if [ -z "$pCheck" ]
then
  echo "ERROR: This script requires gnuplot (wifi-example-sim does not)."
  exit 255
fi

pCheck=`which sed`
if [ -z "$pCheck" ]
then
  echo "ERROR: This script requires sed (wifi-example-sim does not)."
  exit 255
fi

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:bin/

if [ -e ../../DBQA2.db ]
then
  echo "Kill data.db? (y/n)"
  read ANS
  if [ "$ANS" = "yes" -o "$ANS" = "y" ]
  then
    echo Deleting database
    rm ../../DBQA2.db
  fi
fi

for trial in $TRIALS
do
  for distance in $DISTANCES
  do
    for interval in $INTERVALS
    do
       echo Trial $trial, distance $distance, interval $interval
       ../../waf --run "WiFi-QA2 --distance=$distance --interval=$interval --run=run-$distance-$trial-$interval"
    done
  done
done

#
#Another SQL command which just collects raw numbers of frames receved.
#
#CMD="select Experiments.input,avg(Singletons.value) \
#    from Singletons,Experiments \
#    where Singletons.run = Experiments.run AND \
#          Singletons.name='wifi-rx-frames' \
#    group by Experiments.input \
#    order by abs(Experiments.input) ASC;"

mv ../../DBQA2.db .

CMD="select exp.input, tx.value, rx.value, delay.value \
    from Singletons rx, Singletons tx, Experiments exp, Singletons delay \
    where rx.run = tx.run AND \
          rx.run = exp.run AND \
          delay.run = exp.run AND \
          rx.variable='receiver-rx-packets' AND \
          tx.variable='sender-tx-packets' AND \
          delay.variable='delay-average' \
    group by exp.input \
    order by abs(exp.input) ASC;"
 


sqlite3 -noheader DBQA2.db "$CMD" > wifi-default.temp
sed -i "s/|/ /g" wifi-default.temp
awk '{print $1 " " $3*8*1000/20/1000/1000}' wifi-default.temp >QA2-throughput.data
awk '{print $1 " " $4/1000000}' wifi-default.temp >QA2-delay.data
awk '{print $1 " " ($2-$3)/$2*100}' wifi-default.temp >QA2-loss.data

gnuplot wifi-example.gnuplot
#rm wifi-default.temp
echo "Done; data in wifi-default.data, plot in wifi-default.eps"
