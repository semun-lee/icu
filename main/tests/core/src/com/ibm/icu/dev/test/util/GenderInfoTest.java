/*
 *******************************************************************************
 * Copyright (C) 2003-2012, Google, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
*/
package com.ibm.icu.dev.test.util;

import java.util.Arrays;
import java.util.List;

import com.ibm.icu.dev.test.TestFmwk;
import com.ibm.icu.util.GenderInfo;
import com.ibm.icu.util.GenderInfo.Gender;
import com.ibm.icu.util.ULocale;

public class GenderInfoTest extends TestFmwk {
    public static void main(String[] args) {
        new GenderInfoTest().run(args);
    }

    public static GenderInfo NEUTRAL_LOCALE = GenderInfo.getInstance(ULocale.ENGLISH);
    public static GenderInfo MIXED_NEUTRAL_LOCALE = GenderInfo.getInstance(new ULocale("is"));
    public static GenderInfo MALE_TAINTS_LOCALE = GenderInfo.getInstance(ULocale.FRENCH);

    public void TestEmpty() {
        check(Gender.OTHER, Gender.OTHER);
    }

    public void TestOne() {
        for (Gender g : Gender.values()) {
            check(g, g, g);
        }
    }

    public void TestOther() {
        check(Gender.OTHER, Gender.MALE, Gender.MALE, Gender.FEMALE);
        check(Gender.OTHER, Gender.MALE, Gender.FEMALE, Gender.MALE);
        check(Gender.MALE, Gender.MALE, Gender.MALE, Gender.MALE);
        check(Gender.FEMALE, Gender.FEMALE, Gender.FEMALE, Gender.FEMALE);
        check(Gender.OTHER, Gender.MALE, Gender.FEMALE, Gender.OTHER);
    }

    public void check(Gender mixed, Gender taints, Gender... genders) {
        List<Gender> mixed0 = Arrays.asList(genders);
        assertEquals("neutral " + mixed0, Gender.OTHER, NEUTRAL_LOCALE.getListGender(mixed0));
        assertEquals("mixed neutral " + mixed0, mixed, MIXED_NEUTRAL_LOCALE.getListGender(mixed0));
        assertEquals("male taints " + mixed0, taints, MALE_TAINTS_LOCALE.getListGender(mixed0));
    }
    
    public void TestFallback() {
        assertEquals("Strange locale = root", GenderInfo.getInstance(ULocale.ROOT), GenderInfo.getInstance(new ULocale("xxx")));
        assertEquals("Strange locale = root", GenderInfo.getInstance(ULocale.FRANCE), GenderInfo.getInstance(ULocale.CANADA_FRENCH));
    }
}
