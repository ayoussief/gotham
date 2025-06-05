// Copyright (c) 2011-2020 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_QT_GOTHAMADDRESSVALIDATOR_H
#define GOTHAM_QT_GOTHAMADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class GothamAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit GothamAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

/** Gotham address widget validator, checks for a valid gotham address.
 */
class GothamAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit GothamAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

#endif // GOTHAM_QT_GOTHAMADDRESSVALIDATOR_H
