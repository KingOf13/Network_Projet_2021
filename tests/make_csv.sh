simple="0 0 0 0 0"
delay="0 200 0 0 0"
jitter="0 200 50 0 0"
loss="10 0 0 0 0"
cut="0 0 0 10 0"
error="0 0 0 0 10"
complete="10 200 50 10 10"

rm -f ../datas/simple.csv ../datas/delayed.csv ../datas/jittered.csv ../datas/loss.csv ../datas/cut.csv ../datas/error.csv ../datas/complete.csv

for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50; do
  echo "LOOP N°$i"
  mytime="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $simple >/dev/null) 2>&1)"
  echo "$i; $mytime" >> ../datas/simple.csv
  killall -9 link_sim
  mytime="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $delay >/dev/null) 2>&1)"
  echo "$i; $mytime" >> ../datas/delayed.csv
  killall -9 link_sim
  mytime="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $jitter >/dev/null) 2>&1)"
  echo "$i; $mytime" >> ../datas/jittered.csv
  killall -9 link_sim
  mytime="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $loss >/dev/null) 2>&1)"
  echo "$i; $mytime" >> ../datas/loss.csv
  killall -9 link_sim
  mytime="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $cut >/dev/null) 2>&1)"
  echo "$i; $mytime" >> ../datas/cut.csv
  killall -9 link_sim
  mytime="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $error >/dev/null) 2>&1)"
  echo "$i; $mytime" >> ../datas/error.csv
  killall -9 link_sim
  mytime="$(TIMEFORMAT='%lU'; time ( ./param_test.sh $complete >/dev/null) 2>&1)"
  echo "$i; $mytime" >> ../datas/complete.csv
  killall -9 link_sim
done
echo "THE CSV ARE IN DATAS/ "
