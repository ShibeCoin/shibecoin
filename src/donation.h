// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Donation code (c) 2014 Dave Fletcher
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "serialize.h"
#include "db.h"
#include "wallet.h"

class CDonation
{
public:

    uint256 stakeTxHash, donateTxHash;
    int64 nAmount;
    double nPercent;

    CDonation(uint256 stakeTxHashIn, int64 nAmountIn, double nPercentIn)
    {
        stakeTxHash = stakeTxHashIn;
        nAmount = nAmountIn;
        nPercent = nPercentIn;
        donateTxHash = 0;
    }

    CDonation(void)
    {
        SetNull();
    }

    inline void SetNull(void)
    {
        stakeTxHash = 0;
        donateTxHash = 0;
        nAmount = 0;
        nPercent = 0.0;
    }
    
    inline bool IsNull(void)
    {
        return (stakeTxHash != 0) &&
               (donateTxHash != 0) &&
               (nPercent > 0.0) &&
               (nAmount > 0);
    }

    inline bool IsPaid(void)
    {
        return (donateTxHash != 0);
    }

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(VARINT(nVersion));

        READWRITE(VARINT(nAmount));
        READWRITE(stakeTxHash);
        READWRITE(donateTxHash);
        READWRITE(nPercent);
    )

};

class CDonationDB : public CDB
{
public:

    CDonationDB(std::string strFilename, const char* pszMode="r+") : CDB(strFilename.c_str(), pszMode)
    {
    }

    static void Init(std::string filename);

    static void Update(CWallet *pWallet);

    /** Add a new donation.
     */
    bool Add(const uint256 &stakeTxHash, const CDonation &donation);

    /** Fulfil with a payment transaction.
     */
    bool Pay(CDonation &donation, const uint256 &donateTxHash);

    /** Fetch donation details. This function can look up the donation using
     *  either the staking tx that created it, or the donation payment tx.
     */
    bool Get(const uint256 &hash, CDonation &donationOut);

    /** True if the given transaction is a donation source and it has been
     *  paid.
     */
    bool IsPaid(const uint256 &stakeTxHash);

    /** True if the given transaction is a donation payment.
     */
    bool IsDonationPayment(const uint256 &donateTxHash);

    /** True if the given transaction is the source of a donation, e.g. the
     *  transaction containing stake coinbase that has a donation attached.
     *  If this returns true for a given tx, GetDonation will also return
     *  true.
     */
    bool IsDonationSource(const uint256 &stakeTxHash);

    /** Sum of all donations.
     */
    int64 GetTotalDonations(void);
};
