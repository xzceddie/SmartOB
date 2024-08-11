# SmartOB
Smart Order Book maintains a structure `sob::SmartOrderBook` that intelligently updates the market, even when market data streams are not totally synchronised.


## Features
- Resilience. When order stream, which is the source of truth, catches up, it will be overwriting the guesses;
- Performance. Optimized using `boost::circular_buffer` to replace `std::list` for holding the list of orders in each L3 order book level;
- Extensibility. Defining clear callbacks for hooking downstream strategies, one example is the `sob::LoggingStrategy` used in `src/simSOB.cpp`;

## Assumptions
- Messages within the same streams are in-order;
- Messages are not lost;
- Synchronized stream are broadcast in the order [order, snapshot, (trade)] in an interleaved way, where trade message may be missing if there is not any;
- The stream of orders are the source of truth, once each order is known, everything is known;

## Caveates
- Performance is not optimized to its best due to lack of time;
- Not know if the guessing logic is the best;


## Dependencies
### C++

- `Boost`, for `boost::circular_buffer` data structure and "boost::program_options" command line arguments parser. To install: (Linux) `sudo apt install libboost-all-dev`, (MacOs) `brew install boost`;
- `Catch2`, testing framework, already conatined in `thirdparty/`;
- `spdlog`, logging library, produces better logs, already contained in `thirdparty/`;

### Python

- `click`, for command line arguments parsing, to install: `pip3 install click`;

## To clone
```bash
git clone --recursive https://github.com/xzceddie/SmartOB.git
```
**NOTE**: `--recursive` is important.

## To build
```bash
mkdir build/
cd build/
cmake ../
make -j8
```

## To test
```bash
cd build/tests/
ctest
```

## To simulate any input stream
```bash
cd build/src/
```
use the execuatbles `simOB` or `simSOB`; `simOB` is for order stream only, `simSOB` is used to handle mixed streams;
try `./simOB --help` or `./simSOB --help` for printing the help messages;
try `test1.stream`, `test2.stream`, `test3.stream` in `assets/` or `build/assts/` for `simOB` and the rest for `simSOB`;
also try to read the test cases in `tests/` for better understanding of how it works


## Guessing Algorithm and Peformance Considerations
### Guessing Algorithm
Three cases are considered
1. Order stream is faster: this is trivial, because order source is the source of the truth, just apply the orders and use that informantion;
1. Trade stream is faster: Several cases can happen, consdering a buying trade, where the buyer is the liquidity taker:
    - There is a trade with the price in the spread of the lateste L3 Order Book: 
        There has to be asking orders quoting on that price level which has not been received yet. 
        In this case, do not take a guess very quickly. 
        **Only if this level has also been consumed and the trade price level is higher**. Then place some **extra buying orders** on the consumed price level. The amount of the place quotes is equal to **2** times the trade amount on that level. Because we are assuming **30% of the market/aggresive orders are filled, so about twice as much aggressive buying orders are not filled and become buying orders left quoted on the book.** Note that this will move up the price of the instrument.
    - There is a trade-through consumes no less than one ask levels:
        take out all the comsumed levels and the part of the last unconsumed level;
        apply **2 times** the volumn of bidding on the best ask level when this trade has not come in yet;
    - Trade consumes part of the best ask level:
        just take out the consumed part;
    - **So in general, only make a guess when a trade seems to consume some ask level, and put twice the volume on the level after the trade, but ask becomes bid**
1. Snapshot streeam is faster:
    Just go through each level of the snapshot, 
    - if it is not in the current L3 order book, add one level in the L3 book as if there is a new add-liquidity order;
    - if it is within the current L3 order book but the level amount is not the same, then apply some artificial add-liquidity or take-liquidity orders to make the volume correct;
    - if there are levels in the L3 order book but not in the current snapshot, just delete the level in the L3 book;

### Performance considerations
1. `std::list` is too slow because memeory allocations is needed every time you do `push_back` or `push_front`;
2. `boost::circular_buffer` supports constant time `push_front` and `push_back` too, and also avoids lots of memory allocations. But may lead to data corruption when overflow, I create an adaptor class `sob::dBuffer` such that when full, circular buffer will resize twice and copy to the new circular buffer, thus not corrupting data;
3. When using `boost::circular_buffer` it is best if you may have an expectation of how large it is probably going to be to reduce the time you need to resize. **This is relevant in the case when trade mesages are fast and it consumes some level, we need to create some new L3 level. The initial size of the level is set to be about 4 times the volume because we will put 2 times the volume there, but it can fluctuate, so we give it some extra room;**


### Serialised Stream format
**See `assets/` for reference**
order: type{Normal{'N'}, cancel{'C'}, reprice{'R'}}, orderId, isSell {0, 1}, size, price, [oldId], [oldPx], [oldSz]
trade: "T isSell{0, 1} [px0] [vol0] [px1] [vol1] ..." 
snapshot: "S bidDepth askDepth [bid_px] [bid_sz] ... [ask_px] [ask_sz] ..."
