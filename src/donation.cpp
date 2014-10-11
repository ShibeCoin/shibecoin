// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Donation code (c) 2014 Dave Fletcher
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "donation.h"

typedef std::map<uint256, CDonation> MapDonations;
MapDonations openDonations, donationCache;
int64 nDonationsTotal;

void CDonationDB::Init(std::string filename)
{
    CDonationDB ddb(filename, "cr+");
    Dbc* pcursor = ddb.GetCursor();
    if (!pcursor)
        throw std::runtime_error("CDonationDB::Init() : cannot create DB cursor");
    unsigned int fFlags = DB_SET_RANGE;
    nDonationsTotal = 0;
    loop
    {
        // Read next record
        CDataStream ssKey(SER_DISK, CLIENT_VERSION);
        if (fFlags == DB_SET_RANGE)
            ssKey << std::make_pair(std::string("source"), uint256(0));
        CDataStream ssValue(SER_DISK, CLIENT_VERSION);
        int ret = ddb.ReadAtCursor(pcursor, ssKey, ssValue, fFlags);
        fFlags = DB_NEXT;
        if (ret == DB_NOTFOUND)
            break;
        else if (ret != 0)
        {
            pcursor->close();
            throw std::runtime_error("CDonationDB::Init() : error scanning DB");
        }

        // Unserialize
        std::string strType;
        ssKey >> strType;
        if (strType == "total") // Obsolete, but it's in a few existing DBs.
            continue;
        uint256 hash;
        CDonation donation;
        if ((strType == "source") || (strType == "payment"))
        {
            ssKey >> hash;
            ssValue >> donation;
            if ((strType == "source") && !donation.IsNull() && !donation.IsPaid())
            {
                openDonations[hash] = donation;
            }
            donationCache[hash] = donation;
            if (strType == "source")
                nDonationsTotal += donation.nAmount;
        }
    }

    pcursor->close();
}

void CDonationDB::Update(CWallet *wallet)
{
    if (wallet->IsLocked() || fWalletUnlockStakingOnly || wallet->strDonationsFile.empty())
        return;

    LOCK(wallet->cs_wallet);

    std::vector<uint256> removals;
    std::vector<CDonation> donations;
    BOOST_FOREACH(MapDonations::value_type& pDonation, openDonations)
    {
        std::map<uint256, CWalletTx>::const_iterator mi = wallet->mapWallet.find(pDonation.first);
        if ((mi != wallet->mapWallet.end()) && (mi->second.GetAvailableCredit() > 0))
        {
            donations.push_back(pDonation.second);
        }
    }

    // This code alters openDonations so iterate twice to avoid potential issues.
    BOOST_FOREACH(CDonation& pDonation, donations)
    {
        CDonationDB ddb(wallet->strDonationsFile);
        const std::string sAddress(fTestNet ? "SS6uWe8TSsqX7bQivadY6DnAsKRfnLcJw1" : "sQ3fFMko2rGjNnVr1SE13foqFHTUdg7acB");
        CBitcoinAddress address(sAddress);
        CWalletTx wtx;
        wtx.mapValue["comment"] = std::string("ShibeCoin team donation");
        wtx.mapValue["to"] = sAddress;
        wtx.BindWallet(wallet);
        wtx.vin.clear();
        wtx.vout.clear();
        wtx.fFromMe = true;
        CScript scriptPubKey;
        scriptPubKey.SetDestination(address.Get());
        wtx.vout.push_back(CTxOut(pDonation.nAmount, scriptPubKey));
        std::map<uint256, CWalletTx>::const_iterator mi = wallet->mapWallet.find(pDonation.stakeTxHash);
        if (mi->second.IsSpent((mi->second.vout.size() == 3) ? 2 : 1))
        {
            ddb.Erase(pDonation.stakeTxHash);
            continue;
        }
        int64 nChange = mi->second.GetAvailableCredit() - pDonation.nAmount;
        CReserveKey reservekey(wallet);
        CPubKey vchPubKey;
        assert(reservekey.GetReservedKey(vchPubKey));
        if (nChange > 0)
        {
            CScript scriptChange;
            scriptChange.SetDestination(vchPubKey.GetID());
            wtx.vout.push_back(CTxOut(nChange, scriptChange));
        }
        wtx.vin.push_back(CTxIn(pDonation.stakeTxHash, (mi->second.vout.size() == 3) ? 2 : 1));
        if (!SignSignature(*wallet, mi->second, wtx, 0))
        {
            printf("ERROR CREATING DONATION TX: Signing transaction failed\n");
            CDonationDB(wallet->strDonationsFile).Delete(pDonation.stakeTxHash);
            continue;
        }
        wtx.AddSupportingTransactions();
        wtx.fTimeReceivedIsTxTime = true;
        if (!wallet->CommitTransaction(wtx, reservekey))
        {
            printf("Error: The donation transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here.\n");
            openDonations[pDonation.stakeTxHash] = pDonation;  // Re-queue
            continue;
        }
        printf("CREATIED DONATION TX: %s\n", wtx.ToString().c_str());
        ddb.Pay(pDonation, wtx.GetHash());
    }
}

bool CDonationDB::Delete(const uint256 &hash)
{
    CDonation donation;
    Get(hash, donation);
    if (donation.IsNull()) return false;
    nDonationsTotal -= donation.nAmount;
    openDonations.erase(donation.stakeTxHash);
    donationCache.erase(donation.stakeTxHash);
    openDonations.erase(donation.donateTxHash);
    donationCache.erase(donation.donateTxHash);
    return Erase(std::make_pair(std::string("source"), donation.stakeTxHash)) && 
           Erase(std::make_pair(std::string("payment"), donation.donateTxHash));
}

bool CDonationDB::Add(const uint256 &stakeTxHash, const CDonation &donation)
{
    const std::string sAddress(fTestNet ? "SS6uWe8TSsqX7bQivadY6DnAsKRfnLcJw1" : "sQ3fFMko2rGjNnVr1SE13foqFHTUdg7acB");
    CBitcoinAddress address(sAddress);
    CScript scriptPubKey;
    scriptPubKey.SetDestination(address.Get());
    if (CTxOut(donation.nAmount, scriptPubKey).IsDust())
        return false;
    openDonations[stakeTxHash] = donation;
    donationCache[stakeTxHash] = donation;
    nDonationsTotal += donation.nAmount;
    return Write(std::make_pair(std::string("source"), stakeTxHash), donation);
}

bool CDonationDB::Pay(CDonation &donation, const uint256 &donateTxHash)
{
    donation.donateTxHash = donateTxHash;
    donationCache[donation.donateTxHash] = donation;
    openDonations.erase(donation.stakeTxHash);
    openDonations.erase(donation.donateTxHash);
    return Write(std::make_pair(std::string("source"), donation.stakeTxHash), donation) &&
           Write(std::make_pair(std::string("payment"), donation.donateTxHash), donation);
}

bool CDonationDB::Get(const uint256 &hash, CDonation &donationOut)
{
    donationOut.SetNull();
    MapDonations::const_iterator i = donationCache.find(hash);
    if (i != donationCache.end())
    {
      donationOut = i->second;
      return true;
    }
    return false;
}

bool CDonationDB::IsPaid(const uint256 &stakeTxHash)
{
    CDonation donation;
    return Get(stakeTxHash, donation) ? donation.IsPaid() : false;
}

bool CDonationDB::IsDonationPayment(const uint256 &donateTxHash)
{
    CDonation donation;
    MapDonations::const_iterator i = donationCache.find(donateTxHash);
    if (i == donationCache.end())
        return false;
    return (i->second.donateTxHash == donateTxHash);
}

bool CDonationDB::IsDonationSource(const uint256 &stakeTxHash)
{
    CDonation donation;
    MapDonations::const_iterator i = donationCache.find(stakeTxHash);
    if (i == donationCache.end())
        return false;
    return (i->second.stakeTxHash == stakeTxHash);
}

int64 CDonationDB::GetTotalDonations(void)
{
    return nDonationsTotal;
}
