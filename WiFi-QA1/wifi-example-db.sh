#!/bin/sh

DISTANCES="50"
TRIALS="1"

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

if [ -e ../../data.db ]
then
  echo "Kill data.db? (y/n)"
  read ANS
  if [ "$ANS" = "yes" -o "$ANS" = "y" ]
  then
    echo Deleting database
    rm ../../data.db
  fi
fi

for trial in $TRIALS
do
  for distance in $DISTANCES
  do
    echo Trial $trial, distance $distance
    ../../waf --run "WiFi-QA1 --distance=$distance --run=run-$distance-$trial"
  done
done

#
#Another SQL command which just collects raw numbers of frames received.
#
#CMD="select Experiments.input,avg(Singletons.value) \
#    from Singletons,Experiments \
#    where Singletons.run = Experiments.run AND \
#          Singletons.name='wifi-rx-frames' \
#    group by Experiments.input \
#    order by abs(Experiments.input) ASC;"

mv ../../DataOfUser1.db .

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

sqlite3 -noheader DataOfUser1.db "$CMD" > wifi-default.data
sed -i "s/|/ /g" wifi-default.temp
awk '{print $1 " " $3*8*1000/20/1000/1000}' wifi-default.temp >throughput.data
awk '{print $1 " " $4/1000000}' wifi-default.temp >delay.data
awk '{print $1 " " ($2-$3)/$2*100}' wifi-default.temp >loss.data
gnuplot wifi-example.gnuplot

echo "Done; data in wifi-default.data, plot in wifi-default.eps"
