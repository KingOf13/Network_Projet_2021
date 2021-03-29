#!/bin/bash
simple="0 0 0 0 0"
delay="0 50 0 0 0"
jitter="0 50 10 0 0"
loss="10 0 0 0 0"
cut="0 0 0 10 0"
error="0 0 0 0 10"
complete="10 50 10 10 10"

rm -f ../datas/simple.csv ../datas/delayed.csv ../datas/jittered.csv ../datas/loss.csv ../datas/cut.csv ../datas/error.csv ../datas/complete.csv
echo "; "Simple"; "Delayed"; "Jittered"; "With loss"; "Cut"; "Error"; "Complete"" > ../datas/full_datas.csv
echo "num;times" > ../datas/simple.csv
echo "num;times" > ../datas/delayed.csv
echo "num;times" > ../datas/jittered.csv
echo "num;times" > ../datas/loss.csv
echo "num;times" > ../datas/cut.csv
echo "num;times" > ../datas/complete.csv
for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50; do
  echo "LOOP N°$i"
  simple_time="$(TIMEFORMAT='%R'; time ( ./param_test.sh $simple >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed (simple)"
    simple_time=Fail
  fi
  echo "$i; $simple_time" >> ../datas/simple.csv
  killall -q -9 link_sim
  echo "Simple transfert n°$i done"
delayed_time="$(TIMEFORMAT='%R'; time ( ./param_test.sh $delay >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed (delay)"
    delayed_time=Fail
  fi
  echo "$i; $delayed_time" >> ../datas/delayed.csv
  killall -q -9 link_sim  
  echo "Delayed transfert n°$i done"
  jittered_time="$(TIMEFORMAT='%R'; time ( ./param_test.sh $jitter >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed (jitter)"
    jittered_time=Fail
  fi
  echo "$i; $jittered_time" >> ../datas/jittered.csv
  killall -q -9 link_sim  
  echo "Jittered transfert n°$i done"
  lost_time="$(TIMEFORMAT='%R'; time ( ./param_test.sh $loss >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed (loss)"
    lost_time=Fail
  fi
  echo "$i; $lost_time" >> ../datas/loss.csv
  killall -q -9 link_sim  
  echo "Transfert with loss n°$i done"
  cut_time="$(TIMEFORMAT='%R'; time ( ./param_test.sh $cut >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed (cut)"
    cut_time=Fail
  fi
  echo "$i; $cut_time" >> ../datas/cut.csv
  killall -q -9 link_sim  
  echo "Cut transfert n°$i done"
  errored_time="$(TIMEFORMAT='%R'; time ( ./param_test.sh $error >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed (error)"
    errored_time=Fail
  fi
  echo "$i; $errored_time" >> ../datas/error.csv
  killall -q -9 link_sim  
  echo "Corrupted transfert n°$i done"
  complete_time="$(TIMEFORMAT='%R'; time ( ./param_test.sh $complete >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed (complete)"
    complete_time=Fail
  fi
  echo "$i; $complete_time" >> ../datas/complete.csv
  killall -q -9 link_sim  
  echo "Complete transfert n°$i done"
  echo "$i; $simple_time; $delayed_time; $jittered_time; $lost_time; $cut_time; $errored_time; $complete_time" >> ../datas/full_datas.csv
done
echo "THE CSV ARE IN DATAS/ "
