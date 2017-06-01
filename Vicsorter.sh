#!/bin/bash

LOCATION0=/home/luna/data/PR251/2016/
LOCATION1=/home/luna/codes/PR251-analysis/sortedfiles/24Mg/Collimator/Corrected/Calibrated/timetest/
#LOCATION1=/home/luna/codes/PR251-analysis/sortedfiles/26Mg/Collimator/Corrected/Calibrated/
#LOCATION1=/home/luna/codes/PR251-analysis/sortedfiles/BaGeL/calibrations/CsCo/quadon/
#LOCATION1=/home/luna/codes/PR251-analysis/sortedfiles/154Sm/Collimator/Corrected/

OVERWRITE=1

#24Mg Collimator
for COUNT in {2227,2233,2238,2243,2250,2256,2272,2281,2290,2307,2320,2321,2322,2328,2336,2356,2362,2370,2376,2383,2398,2403,2408,2412,2426,2431,2435,2442,2446,2450,2455,2459,2464}
#26Mg Collimator
#for COUNT in {2221,2222,2225,2232,2237,2242,2249,2255,2267,2271,2276,2280,2285,2294,2304,2308,2309,2323,2329,2337,2348,2357,2363,2371,2384} #all 
#for COUNT in {2232,2237,2242,2249,2255,2267,2271,2276,2280,2285,2294,2304,2308,2309,2323,2329,2337,2348,2357,2363,2371,2384}
#CsCo calib quad on
#for COUNT in {2083,2310,2311,2312,2313,3016,3092,3093,3094,3095,3096}
#CsCo calib quad off
#for COUNT in {1236,1247,1254}
#154Sm Collimator 4.8mg/cm2 target pulse section off
#for COUNT in {2263,2264,2265,2266,2268,2269,2270,2273,2274,2275,2277,2278,2279,2282,2283,2284,2286,2287,2288,2291,2292,2293,2298,2300,2301,2302,2303,2305,2306,2324,2325,2326,2327,2330,2331,2332,2333,2334,2335,2338,2339,2340,2341,2342,2344,2345,2346,2349,2350,2351,2352,2353,2355,2358,2359,2360,2361,2364,2365,2366,2367,2368,2369,2372,2373,2374,2375,2377,2381,2382,2385,2387,2388,2389,2390,2391,2399,2400,2401,2402,2404,2405,2406,2407,2409,2410,2413,2414,2415,2416,2417,2419,2423,2424,2425,2427,2428,2429,2430,2432,2433,2434,2436,2437,2438,2439,2440,2441,2443,2444,2445,2447,2448,2449,2451,2452,2453,2454,2456,2457,2458,2460,2461,2462,2463}



do

  echo "File number being processed: " $COUNT

  if [ $COUNT -lt 10 ]
  then
      FILE=$LOCATION0"run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE=$LOCATION0"run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE=$LOCATION0"run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE=$LOCATION0"run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE=$LOCATION0"run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2=$LOCATION1"sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2=$LOCATION1"sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2=$LOCATION1"sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2=$LOCATION1"sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2=$LOCATION1"sorted"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo $FILE2
  if [ -e $FILE ]
  then
      if [ ! -e $FILE2 -o $OVERWRITE -eq 1 ]
      then
      ./analyzer -i $FILE
      mv output.root $FILE2   
      fi
  fi
done
echo "Go home"
