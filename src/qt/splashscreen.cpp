// Copyright (c) 2011-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "splashscreen.h"
#include "clientversion.h"
#include "util.h"

#include <QPainter>
#undef loop /* ugh, remove this when the #define loop is gone from util.h */
#include <QApplication>

SplashScreen::SplashScreen(const QPixmap &pixmap, Qt::WindowFlags f) :
    QSplashScreen(pixmap, f)
{

    setContentsMargins(12, 12, 12, 12);

    // set reference point, paddings
    int paddingLeftCol2         = 280;
    int paddingTopCol2          = 414;
    int line1 = 0;
    int line2 = 12;
    int line3 = 24;
    int line4 = 36;
    int line5 = 48;

    float fontFactor            = 1.0;

    // define text to place
    QString titleText       = QString(QApplication::applicationName()).replace(QString("-testnet"), QString(""), Qt::CaseSensitive); // cut of testnet, place it as single object further down
    QString versionText     = QString("Version %1 ").arg(QString::fromStdString(FormatFullVersion()));
    QString copyrightText1   = QChar(0xA9)+QString(" 2009-%1 ").arg(COPYRIGHT_YEAR) + QString(tr("The Bitcoin developers"));
    QString copyrightText2   = QChar(0xA9)+QString(" 2011-%1 ").arg(COPYRIGHT_YEAR) + QString(tr("The Litecoin developers"));
    QString copyrightText3   = QChar(0xA9)+QString(" 2013-%1 ").arg(COPYRIGHT_YEAR) + QString(tr("The Reddcoin developers"));
    QString copyrightText4   = QChar(0xA9)+QString(" 2014-%1 ").arg(COPYRIGHT_YEAR) + QString(tr("The ShibeCoin developers"));

    QString font            = "Arial";

    // load the bitmap for writing some text over it
    QPixmap newPixmap;
    if(GetBoolArg("-testnet")) {
        newPixmap     = QPixmap(":/images/splash_testnet");
    }
    else {
        newPixmap     = QPixmap(":/images/splash");
    }

    QPainter pixPaint(&newPixmap);
    pixPaint.setPen(QColor(0,0,0));

    pixPaint.setFont(QFont(font, 8*fontFactor));
    pixPaint.drawText(paddingLeftCol2,paddingTopCol2+line5,versionText);

    // draw copyright stuff
    pixPaint.drawText(paddingLeftCol2,paddingTopCol2+line1,copyrightText1);
    pixPaint.drawText(paddingLeftCol2,paddingTopCol2+line2,copyrightText2);
    pixPaint.drawText(paddingLeftCol2,paddingTopCol2+line3,copyrightText3);
    pixPaint.drawText(paddingLeftCol2,paddingTopCol2+line4,copyrightText4);

    pixPaint.end();

    this->setPixmap(newPixmap);
}
