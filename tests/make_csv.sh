simple="0 0 0 0 0"
delay="0 200 0 0 0"
jitter="0 200 50 0 0"
loss="10 0 0 0 0"
cut="0 0 0 10 0"
error="0 0 0 0 10"
complete="10 200 50 10 10"

rm -f ../datas/simple.csv ../datas/delayed.csv ../datas/jittered.csv ../datas/loss.csv ../datas/cut.csv ../datas/error.csv ../datas/complete.csv
echo "; "Simple"; "Delayed"; "Jittered"; "With loss"; "Cut"; "Error"; "Complete"" > ../datas/full_datas.csv

for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50; do
  echo "LOOP N°$i"
  simple_time="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $simple >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed (simple)"
    simple_time=Fail
  fi
  echo "$i; $simple_time" >> ../datas/simple.csv
  killall -q -9 link_sim
  delayed_time="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $delay >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed(delay)"
    delayed_time=Fail
  fi
  echo "$i; $delayed_time" >> ../datas/delayed.csv
  killall -q -9 link_sim
  jittered_time="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $jitter >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed (jitter)"
    jittered_time=Fail
  fi
  echo "$i; $jittered_time" >> ../datas/jittered.csv
  killall -q -9 link_sim
  lost_time="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $loss >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed (loss)"
    lost_time=Fail
  fi
  echo "$i; $lost_time" >> ../datas/loss.csv
  killall -q -9 link_sim
  cut_time="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $cut >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed (cut)"
    cut_time=Fail
  fi
  echo "$i; $cut_time" >> ../datas/cut.csv
  killall -q -9 link_sim
  errored_time="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $error >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed (error)"
    errored_time=Fail
  fi
  echo "$i; $errored_time" >> ../datas/error.csv
  killall -q -9 link_sim
  complete_time="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $complete >/dev/null) 2>&1)"
  ret=$?
  if [ ret == 1 ]; then
    echo "Test n°$i failed (complete)"
    complete_time=Fail
  fi
  echo "$i; $complete_time" >> ../datas/complete.csv
  killall -q -9 link_sim
  echo "$i; $simple_time; $delayed_time; $jittered_time; $lost_time; $cut_time; $errored_time; $complete_time" >> ../datas/full_datas.csv
done
echo "THE CSV ARE IN DATAS/ "
