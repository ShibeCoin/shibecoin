Shibecoin integration/staging tree
================================

http://www.Shibecoin.io

Copyright (c) 2009-2014 Bitcoin Developers
Copyright (c) 2011-2014 Litecoin Developers
Copyright (c) 2014 Shibecoin Developers

What is Shibecoin?
----------------

In June 2014 Shibecoin was taken over by a community-backed development team.
A swap to Shibecoin V3 in September 2014 will transition to a customized fork of Proof-of-Stake-Velocity (PoSV)
algorithm which replaced Proof-of-Stake (PoS) (and a 5-day PoW phase prior to PoS).
 - 1 minute block target
 - 3,000,000 Shibecoin mined during PoW phase
 - PoSV will change to 20% per annum stake for the first 6 months, decreasing by 5% every 6 months, until 5% per annum is reached and it will remain there indefinitely
	
Shibecoin first started in May 2014 as a variant of Litecoin using Scrypt as
the Proof-of-Work (PoW) hash algorithm.
 - 1 minute block target
 - 50 coins per block
 - 5 day PoW phase
 - Maturity: 10 blocks
 - Confirms: 4 blocks
 - after 5 days PoW (6300 blocks) transition to 100% PoS at 5% stake earned, initially coded at a minimum maturity age of 8 days, max of 40 days.

For more information, as well as an immediately useable, binary version of
the Shibecoin wallet client, please visit http://www.shibecoin.io or our bitcointalk.org thread at https://bitcointalk.org/index.php?topic=724624.msg8188627#msg8188627

License
-------

Shibecoin is released under the terms of the MIT license. See `COPYING` for more
information or see http://opensource.org/licenses/MIT.

Development process
-------------------

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. 

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test. Please be patient and help out, and
remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write unit tests for new code, and to
submit new unit tests for old code.

Unit tests for the core code are in `src/test/`. To compile and run them:

    cd src; make -f makefile.unix test

Unit tests for the GUI code are in `src/qt/test/`. To compile and run them:

    qmake BITCOIN_QT_TEST=1 -o Makefile.test bitcoin-qt.pro
    make -f Makefile.test
    ./Shibecoin-qt_test
