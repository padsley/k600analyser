#!/bin/bash

LOCATION0=/home/harshna/Documents/PR251/k600_data/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/24Mg/Colimator/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/24Mg/Colimator/21Aug_Sorted_26Mg_LinCal/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/24Mg/Colimator/22Aug_Sorted_152Eu_Quad_cal/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/24Mg/NoColimator/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/26Mg/Colimator/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/26Mg/NoColimator/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/154Sm/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/152Eu/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/56Co/
LOCATION1=/home/harshna/Documents/PR251/Sorted_data/AmBe/
#LOCATION1=/media/harshna/Harshna&apos;s Passport Ultra/PR251 data /sorted runs/154Sm/

#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/116Sn/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/12C/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/CsCo/quad_on/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/CsCo/quad_off/


OVERWRITE=1

#24Mg Collimator
#for COUNT in {2227,2233,2238,2243,2250,2256,2272,2281,2290,2307,2320,2321,2322,2328,2336,2347,2356,2362,2370,2376,2383,2398,2403,2408,2412,2418,2426,2431,2435,2442,2446,2450,2455,2459,2464}

#24Mg No Collimator
#for COUNT in {2039,2043,2047,2051,2055,2100,2107,2119,2163,2182,2197}

#26Mg Collimator
#for COUNT in {2221,2222,2225,2232,2237,2242,2249,2255,2267,2271,2276,2280,2285,2294,2304,2308,2309,2323,2329,2337,2348,2357,2363,2371,2384}

#26Mg No Collimator
#for COUNT in {2037,2042,2045,2046,2049,2050,2053,2054,2103,2104,2105,2106,2110,2112,2113,2114,2115,2117,2118,2161,2162,2178,2181,2195,2196}

#154Sm Collimator 4.8mg/cm2 target pulse section off
#for COUNT in {2449,2451,2452,2453,2454,2456,2457,2458,2460,2461,2462,2463}


#done
#{2263,2264,2265,2266,2268,2269,2270,2273,2274,2275,2277,2278,2279,2282,2283,2284,2286,2287,2288,2291,2292,2293,2298,2300}
#{2301,2302,2303,2305,2306,2324,2325,2326,2327,2330,2331,2332,2333,2334,2335,2338,2339,2340,2341,2342,2344,2345,2346,2349,2350}
#{2351,2352,2353,2355,2358,2359,2360,2361,2364,2365,2366,2367,2368}
#{2369,2372,2373,2374,2375,2377,2380,2381,2382,2385,2387,2388,2389}
#{2390,2391,2399,2400,2401,2402,2404,2405,2406,2407,2409,2410,2413}
#{2414,2415,2416,2417,2419,2423,2424,2425,2427,2428,2429,2430,2432}
#{2433,2434,2436,2437,2438,2439,2440,2441,2443,2444,2445,2447,2448}
#{2449,2451,2452,2453,2454,2456,2457,2458,2460,2461,2462,2463}


#116Sn 
#for COUNT in {2165,2166,2167,2168,2169,2170,2171,2172,2173,2174,2175,2176,2177,2183,2184,2185,2186,2187,2188,2189,2190,2191,2192,2193,2194,2198,2199,2200,2202,2203,2204,2205,2206}

#12C
#for COUNT in {2040,2044,2048,2052,2056,2075,2076,2102,2109}

#all 
#for COUNT in {2232,2237,2242,2249,2255,2267,2271,2276,2280,2285,2294,2304,2308,2309,2323,2329,2337,2348,2357,2363,2371,2384}

#CsCo calib quad on
#for COUNT in {2083,2310,2311,2312,2313,3016,3092,3093,3094,3095,3096}

#CsCo calib quad off
#for COUNT in {1236,1247,1254}

#152Eu
#for COUNT in {3000..3015}

#56Co
#for COUNT in {1196,1197,1225,2004,2006,2007,3067,3068,3069,3070,3071,3072,3073,3074,3075,3076,3077,3078,3079,3080,3081,3082,3083,3084,3085,3086,3087,3088,3089,3090,3091,3097,3098,3099,3100,3101,3102,3103,3104}

#AmBe-Fe+60Co+137Cs
for COUNT in {3027..3066}
#{1229,1230,1242,1244}
#{3027..3066}

#for COUNT in {2263,2264,2265}

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
