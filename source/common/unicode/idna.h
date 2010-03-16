/*
*******************************************************************************
*   Copyright (C) 2010, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  idna.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010mar05
*   created by: Markus W. Scherer
*/

#ifndef __IDNA_H__
#define __IDNA_H__

/**
 * \file
 * \brief C++ API: New API for IDNA (Internationalizing Domain Names In Applications).
 */

#include "unicode/utypes.h"

#if !UCONFIG_NO_IDNA

#include "unicode/uidna.h"
#include "unicode/unistr.h"

/*
 * IDNA option bit set values.
 */
enum {
    // TODO: Options from old API are mostly usable with the new API as well.
    // All options should be moved to a C header.
    // They are actually still defined in uidna.h right now and thus commented out here.
    // TODO: It should be safe to replace the old #defines with enum constants, right?
    /**
     * Default options value: None of the other options are set.
     * @stable ICU 2.6
     */
    // UIDNA_DEFAULT=0,
    /**
     * Option to allow unassigned code points in domain names and labels.
     * This option is ignored by the UTS46 implementation.
     * @stable ICU 2.6
     */
    // UIDNA_ALLOW_UNASSIGNED=1,
    /**
     * Option to check whether the input conforms to the STD3 ASCII rules,
     * for example the restriction of labels to LDH characters
     * (ASCII Letters, Digits and Hyphen-Minus).
     * @stable ICU 2.6
     */
    // UIDNA_USE_STD3_RULES=2,
    /**
     * IDNA option to check for whether the input conforms to the BiDi rules.
     * @draft ICU 4.6
     */
    UIDNA_CHECK_BIDI=4,
    /**
     * IDNA option to check for whether the input conforms to the CONTEXTJ rules.
     * @draft ICU 4.6
     */
    UIDNA_CHECK_CONTEXTJ=8,
    /**
     * IDNA option for nontransitional processing in ToASCII().
     * By default, ToASCII() uses transitional processing.
     * @draft ICU 4.6
     */
    UIDNA_NONTRANSITIONAL_TO_ASCII=0x10
};

/*
 * IDNA error bit set values.
 * When a domain name or label fails a processing step or does not meet the
 * validity criteria, then one or more of these error bits are set.
 */
enum {
    // TODO: Should we combine the length errors into one single UIDNA_ERROR_LABEL_OR_NAME_LENGTH?
    /**
     * A non-final domain name label (or the whole domain name) is empty.
     * @draft ICU 4.6
     */
    UIDNA_ERROR_EMPTY_LABEL=1,
    /**
     * A domain name label is longer than 63 bytes.
     * (See STD13/RFC1034 3.1. Name space specifications and terminology.)
     * This is only checked in ToASCII operations, and only if the UIDNA_USE_STD3_RULES is set.
     * @draft ICU 4.6
     */
    UIDNA_ERROR_LABEL_TOO_LONG=2,
    /**
     * A domain name is longer than 255 bytes in its storage form.
     * (See STD13/RFC1034 3.1. Name space specifications and terminology.)
     * This is only checked in ToASCII operations, and only if the UIDNA_USE_STD3_RULES is set.
     * @draft ICU 4.6
     */
    UIDNA_ERROR_DOMAIN_NAME_TOO_LONG=4,
    // TODO: Should we combine the hyphen errors into one single UIDNA_ERROR_BAD_HYPHEN?
    /**
     * A label starts with a hyphen-minus ('-').
     * @draft ICU 4.6
     */
    UIDNA_ERROR_LEADING_HYPHEN=8,
    /**
     * A label ends with a hyphen-minus ('-').
     * @draft ICU 4.6
     */
    UIDNA_ERROR_TRAILING_HYPHEN=0x10,
    /**
     * A label contains hyphen-minus ('-') in the third and fourth positions.
     * @draft ICU 4.6
     */
    UIDNA_ERROR_HYPHEN_3_4=0x20,  // TODO: Is this a reasonable name?
    /**
     * A label starts with a combining mark.
     * @draft ICU 4.6
     */
    UIDNA_ERROR_LEADING_COMBINING_MARK=0x40,
    /**
     * A label or domain name contains disallowed characters.
     * @draft ICU 4.6
     */
    UIDNA_ERROR_DISALLOWED=0x80,
    /**
     * A label starts with "xn--" but does not contain valid Punycode.
     * @draft ICU 4.6
     */
    UIDNA_ERROR_PUNYCODE=0x100,
    /**
     * A label contains a dot=full stop.
     * This can occur in an ACE label, and in an input string for a single-label function.
     * @draft ICU 4.6
     */
    UIDNA_ERROR_LABEL_HAS_DOT=0x200,
    /**
     * An ACE label is not valid.
     * It might contain characters that are not allowed in ACE labels,
     * or it might not be normalized, or both.
     * @draft ICU 4.6
     */
    UIDNA_ERROR_INVALID_ACE_LABEL=0x400,
    /**
     * A label does not meet the IDNA BiDi requirements (for right-to-left characters).
     * @draft ICU 4.6
     */
    UIDNA_ERROR_BIDI=0x800,
    /**
     * A label does not meet the IDNA CONTEXTJ requirements.
     * @draft ICU 4.6
     */
    UIDNA_ERROR_CONTEXTJ=0x1000
};

U_NAMESPACE_BEGIN

class U_COMMON_API IDNAErrors;

/**
 * Abstract base class for IDNA processing.
 * See http://www.unicode.org/reports/tr46/
 * and http://www.ietf.org/rfc/rfc3490.txt
 *
 * This newer API currently only implements UTS #46.
 * The older uidna.h C API only implements IDNA2003.
 * @draft ICU 4.6
 */
class U_COMMON_API IDNA : public UObject {
public:
    /**
     * Returns an IDNA instance which implements UTS #46.
     * Returns an unmodifiable instance, owned by the caller.
     * Cache it for multiple operations, and delete it when done.
     *
     * UTS #46 defines Unicode IDNA Compatibility Processing,
     * updated to the latest version of Unicode and compatible with both
     * IDNA2003 and IDNA2008.
     *
     * ToASCII operations use transitional processing, including deviation mappings,
     * unless the UIDNA_NONTRANSITIONAL_TO_ASCII is used.
     * ToUnicode operations always use nontransitional processing,
     * passing deviation characters through without change.
     *
     * Disallowed characters are mapped to U+FFFD.
     *
     * For available options see the uidna.h header as well as this header.
     * Operations with the UTS #46 instance do not support the
     * UIDNA_ALLOW_UNASSIGNED option.
     *
     * By default, UTS #46 disallows all ASCII characters other than
     * letters, digits, hyphen (LDH) and dot/full stop.
     * When the UIDNA_USE_STD3_RULES option is used, all ASCII characters are treated as
     * valid or mapped.
     *
     * TODO: Do we need separate toASCIIOptions and toUnicodeOptions?
     *       That is, would users commonly want different options for the
     *       toASCII and toUnicode operations?
     *
     * @param options Bit set to modify the processing and error checking.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return the UTS #46 IDNA instance, if successful
     * @draft ICU 4.6
     */
    static IDNA *
    createUTS46Instance(uint32_t options, UErrorCode &errorCode);

    /**
     * Converts a single domain name label into its ASCII form for DNS lookup.
     * ToASCII can fail if the input label cannot be converted into an ASCII form.
     * In this case, the destination string will be bogus and errors.hasErrors() will be TRUE.
     *
     * The UErrorCode indicates an error only in exceptional cases,
     * such as a U_MEMORY_ALLOCATION_ERROR.
     *
     * @param label Input domain name label
     * @param dest Destination string object
     * @param errors Output container of IDNA processing errors.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return dest
     * @draft ICU 4.6
     */
    virtual UnicodeString &
    labelToASCII(const UnicodeString &label, UnicodeString &dest,
                 IDNAErrors &errors, UErrorCode &errorCode) const = 0;

    /**
     * Converts a single domain name label into its Unicode form for human-readable display.
     * ToUnicode never fails. If any processing step fails, then the input label
     * is returned, possibly with modifications according to the types of errors,
     * and errors.hasErrors() will be TRUE.
     *
     * For available options see the uidna.h header.
     *
     * @param label Input domain name label
     * @param dest Destination string object
     * @param errors Output container of IDNA processing errors.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return dest
     * @draft ICU 4.6
     */
    virtual UnicodeString &
    labelToUnicode(const UnicodeString &label, UnicodeString &dest,
                   IDNAErrors &errors, UErrorCode &errorCode) const = 0;

    /**
     * Converts a whole domain name into its ASCII form for DNS lookup.
     * ToASCII can fail if the input label cannot be converted into an ASCII form.
     * In this case, the destination string will be bogus and errors.hasErrors() will be TRUE.
     *
     * The UErrorCode indicates an error only in exceptional cases,
     * such as a U_MEMORY_ALLOCATION_ERROR.
     *
     * @param label Input domain name label
     * @param dest Destination string object
     * @param errors Output container of IDNA processing errors.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return dest
     * @draft ICU 4.6
     */
    virtual UnicodeString &
    nameToASCII(const UnicodeString &name, UnicodeString &dest,
                IDNAErrors &errors, UErrorCode &errorCode) const = 0;

    /**
     * Converts a whole domain name into its Unicode form for human-readable display.
     * ToUnicode never fails. If any processing step fails, then the input domain name
     * is returned, possibly with modifications according to the types of errors,
     * and errors.hasErrors() will be TRUE.
     *
     * @param label Input domain name label
     * @param dest Destination string object
     * @param errors Output container of IDNA processing errors.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return dest
     * @draft ICU 4.6
     */
    virtual UnicodeString &
    nameToUnicode(const UnicodeString &name, UnicodeString &dest,
                  IDNAErrors &errors, UErrorCode &errorCode) const = 0;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     * @returns a UClassID for this class.
     * @draft ICU 4.6
     */
    static UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     * @return a UClassID for the actual class.
     * @draft ICU 4.6
     */
    virtual UClassID getDynamicClassID() const = 0;
};

class UTS46;

/**
 * Output container for IDNA processing errors.
 * @draft ICU 4.6
 */
class U_COMMON_API IDNAErrors : public UObject {
public:
    /**
     * Constructor for stack allocation.
     * @draft ICU 4.6
     */
    IDNAErrors() : errors(0) {}
    /**
     * Were there IDNA processing errors?
     * @return TRUE if there were processing errors
     * @draft ICU 4.6
     */
    UBool hasErrors() const { return errors!=0; }
    /**
     * Returns a bit set indicating IDNA processing errors.
     * See UIDNA_ERROR_... constants.
     * @return bit set of processing errors
     * @draft ICU 4.6
     */
    uint32_t getErrors() const { return errors; }

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     * @returns a UClassID for this class.
     * @draft ICU 4.6
     */
    static UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     * @return a UClassID for the actual class.
     * @draft ICU 4.6
     */
    virtual UClassID getDynamicClassID() const;

private:
    friend class UTS46;

    IDNAErrors(const IDNAErrors &other);  // no copying
    IDNAErrors &operator=(const IDNAErrors &other);  // no copying

    void reset() { errors=0; }

    uint32_t errors;
};

U_NAMESPACE_END

#endif  // UCONFIG_NO_IDNA
#endif  // __IDNA_H__
